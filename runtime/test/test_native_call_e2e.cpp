/// @file test_native_call_e2e.cpp
/// @brief End-to-end tests for NATIVE_CALL opcode through the full pipeline.
///
/// Unlike test_native_bridge.cpp (which tests call_native() directly),
/// these tests build blobs containing NATIVE_CALL instructions and
/// execute them through the dispatcher.  This validates the full path:
///   dispatcher → handle_native_call → call_native → native fn → re-encode
///
/// Reentrancy tests validate:
///   vm → native → vm → native → vm  (nested VM invocations)
///   native → vm → native             (VM calls out, native calls back)

#include "test_blob_builder.hpp"

#include <dispatcher.hpp>
#include <decoder.hpp>
#include <encoding.hpp>
#include <vm_entry.hpp>
#include <vm_loader.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <vm/vm_insn.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <cstdint>
#include <vector>

// ---------------------------------------------------------------------------
// Portable large-stack thread helper (ASan inflates frames ~4x; 100-level
// VM reentrancy needs ~64MB under ASan).
// ---------------------------------------------------------------------------
#if defined(_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#endif

struct LargeStackResult { bool ok; std::string error; };

/// Run `fn(arg)` on a thread with `stack_bytes` stack.
/// Blocks until fn completes.  Returns fn's LargeStackResult.
static LargeStackResult run_on_large_stack(
    LargeStackResult (*fn)(void*), void* arg,
    size_t stack_bytes = 64u * 1024 * 1024)
{
    LargeStackResult out{false, "thread launch failed"};
    struct Ctx { LargeStackResult (*fn)(void*); void* arg; LargeStackResult result; };
    Ctx ctx{fn, arg, {}};

#if defined(_WIN32)
    auto trampoline = [](LPVOID p) -> DWORD {
        auto* c = static_cast<Ctx*>(p);
        c->result = c->fn(c->arg);
        return 0;
    };
    HANDLE h = CreateThread(nullptr, stack_bytes, trampoline, &ctx, 0, nullptr);
    if (!h) return out;
    WaitForSingleObject(h, INFINITE);
    CloseHandle(h);
#else
    auto trampoline = [](void* p) -> void* {
        auto* c = static_cast<Ctx*>(p);
        c->result = c->fn(c->arg);
        return nullptr;
    };
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, stack_bytes);
    pthread_t tid;
    if (pthread_create(&tid, &attr, trampoline, &ctx) != 0) {
        pthread_attr_destroy(&attr);
        return out;
    }
    pthread_join(tid, nullptr);
    pthread_attr_destroy(&attr);
#endif

    return ctx.result;
}

using namespace VMPilot::Common;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Runtime;
using namespace VMPilot::Test;

// ---------------------------------------------------------------------------
// Native test functions (C-linkage-compatible, 8-arg signature)
// ---------------------------------------------------------------------------

static uint64_t native_add_two(uint64_t a, uint64_t b,
                               uint64_t, uint64_t, uint64_t, uint64_t,
                               uint64_t, uint64_t) {
    return a + b;
}

static uint64_t native_constant_99(uint64_t, uint64_t, uint64_t, uint64_t,
                                   uint64_t, uint64_t, uint64_t, uint64_t) {
    return 99;
}

static uint64_t native_triple(uint64_t a, uint64_t, uint64_t, uint64_t,
                              uint64_t, uint64_t, uint64_t, uint64_t) {
    return a * 3;
}

static uint64_t native_sum_four(uint64_t a, uint64_t b,
                                uint64_t c, uint64_t d,
                                uint64_t, uint64_t, uint64_t, uint64_t) {
    return a + b + c + d;
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void fill_seed(uint8_t seed[32]) {
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);
}

static void fill_epoch(uint8_t seed[32], uint8_t base) {
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(base + i);
}

static uint8_t flags_rr(uint8_t cond = 0) {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4) | cond);
}

static uint8_t flags_none() {
    return 0;
}

static VmSecurityConfig test_config() {
    VmSecurityConfig cfg{};
    cfg.debug_mode = true;
    return cfg;
}

// ---------------------------------------------------------------------------
// Test 1: Basic NATIVE_CALL — call a 2-arg native function, verify result
// ---------------------------------------------------------------------------

TEST(NativeCallE2E, BasicTwoArgCall) {
    uint8_t seed[32]; fill_seed(seed);

    // Program: load r0=10, r1=20, NATIVE_CALL[0] (add), HALT
    // NATIVE_CALL[0] calls native_add_two(r0, r1) → result in r0
    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xBB);

    TestInstruction nc{};
    nc.opcode = VmOpcode::NATIVE_CALL;
    nc.flags  = flags_none();
    nc.reg_a  = 0; nc.reg_b = 0;
    nc.aux    = 0;  // transition entry index 0

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {nc, halt};

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count    = 2;
    tc.target_addr  = reinterpret_cast<uint64_t>(&native_add_two);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});
    auto loaded = load_blob(blob.data(), blob.size(), seed, test_config());
    ASSERT_TRUE(loaded.has_value());

    auto& ctx = loaded->ctx;
    ctx.encoded_regs[0] = encode_register(ctx, 0, 10);
    ctx.encoded_regs[1] = encode_register(ctx, 1, 20);

    auto result = execute(ctx, test_config());
    ASSERT_TRUE(result.has_value());

    uint64_t r0 = decode_register(ctx, 0, ctx.encoded_regs[0]);
    EXPECT_EQ(r0, 30u);
}

// ---------------------------------------------------------------------------
// Test 2: Zero-arg native call — returns constant
// ---------------------------------------------------------------------------

TEST(NativeCallE2E, ZeroArgCall) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xCC);

    TestInstruction nc{};
    nc.opcode = VmOpcode::NATIVE_CALL;
    nc.flags  = flags_none();
    nc.aux    = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {nc, halt};

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count    = 0;
    tc.target_addr  = reinterpret_cast<uint64_t>(&native_constant_99);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});
    auto loaded = load_blob(blob.data(), blob.size(), seed, test_config());
    ASSERT_TRUE(loaded.has_value());

    auto result = execute(loaded->ctx, test_config());
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(decode_register(loaded->ctx, 0, loaded->ctx.encoded_regs[0]), 99u);
}

// ---------------------------------------------------------------------------
// Test 3: Four-arg native call
// ---------------------------------------------------------------------------

TEST(NativeCallE2E, FourArgCall) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xDD);

    TestInstruction nc{};
    nc.opcode = VmOpcode::NATIVE_CALL;
    nc.flags = flags_none();
    nc.aux   = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {nc, halt};

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count    = 4;
    tc.target_addr  = reinterpret_cast<uint64_t>(&native_sum_four);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});
    auto loaded = load_blob(blob.data(), blob.size(), seed, test_config());
    ASSERT_TRUE(loaded.has_value());

    auto& ctx = loaded->ctx;
    ctx.encoded_regs[0] = encode_register(ctx, 0, 100);
    ctx.encoded_regs[1] = encode_register(ctx, 1, 200);
    ctx.encoded_regs[2] = encode_register(ctx, 2, 300);
    ctx.encoded_regs[3] = encode_register(ctx, 3, 400);

    auto result = execute(ctx, test_config());
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(decode_register(ctx, 0, ctx.encoded_regs[0]), 1000u);
}

// ---------------------------------------------------------------------------
// Test 4: NATIVE_CALL result feeds into subsequent VM operations
//   r0=5 → NATIVE_CALL triple(r0) → r0=15 → ADD r0,r1(=10) → r0=25
// ---------------------------------------------------------------------------

TEST(NativeCallE2E, ResultFeedsIntoVmOps) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xEE);

    TestInstruction nc{};
    nc.opcode = VmOpcode::NATIVE_CALL;
    nc.flags  = flags_none();
    nc.aux    = 0;

    TestInstruction add{};
    add.opcode = VmOpcode::ADD;
    add.flags  = flags_rr();
    add.reg_a  = 0; add.reg_b = 1;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {nc, add, halt};

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count    = 1;
    tc.target_addr  = reinterpret_cast<uint64_t>(&native_triple);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});
    auto loaded = load_blob(blob.data(), blob.size(), seed, test_config());
    ASSERT_TRUE(loaded.has_value());

    auto& ctx = loaded->ctx;
    ctx.encoded_regs[0] = encode_register(ctx, 0, 5);   // tripled → 15
    ctx.encoded_regs[1] = encode_register(ctx, 1, 10);  // added → 25

    auto result = execute(ctx, test_config());
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(decode_register(ctx, 0, ctx.encoded_regs[0]), 25u);
}

// ---------------------------------------------------------------------------
// Test 5: Multiple NATIVE_CALLs in sequence
//   r0=7 → NATIVE_CALL triple(r0) → r0=21
//   r0=21, r1=9 → NATIVE_CALL add(r0,r1) → r0=30
// ---------------------------------------------------------------------------

TEST(NativeCallE2E, MultipleCallsInSequence) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xFF);

    TestInstruction nc0{};
    nc0.opcode = VmOpcode::NATIVE_CALL;
    nc0.flags  = flags_none();
    nc0.aux    = 0;  // transition[0] = triple

    TestInstruction nc1{};
    nc1.opcode = VmOpcode::NATIVE_CALL;
    nc1.flags  = flags_none();
    nc1.aux    = 1;  // transition[1] = add

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {nc0, nc1, halt};

    TestNativeCall tc0{};
    tc0.call_site_ip = 0; tc0.arg_count = 1;
    tc0.target_addr = reinterpret_cast<uint64_t>(&native_triple);

    TestNativeCall tc1{};
    tc1.call_site_ip = 1; tc1.arg_count = 2;
    tc1.target_addr = reinterpret_cast<uint64_t>(&native_add_two);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc0, tc1});
    auto loaded = load_blob(blob.data(), blob.size(), seed, test_config());
    ASSERT_TRUE(loaded.has_value());

    auto& ctx = loaded->ctx;
    ctx.encoded_regs[0] = encode_register(ctx, 0, 7);
    ctx.encoded_regs[1] = encode_register(ctx, 1, 9);

    auto result = execute(ctx, test_config());
    ASSERT_TRUE(result.has_value());

    // 7*3 = 21, then 21+9 = 30
    EXPECT_EQ(decode_register(ctx, 0, ctx.encoded_regs[0]), 30u);
}

// ---------------------------------------------------------------------------
// Test 6: Invalid transition index → error
// ---------------------------------------------------------------------------

TEST(NativeCallE2E, InvalidIndexFails) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xAA);

    TestInstruction nc{};
    nc.opcode = VmOpcode::NATIVE_CALL;
    nc.flags  = flags_none();
    nc.aux    = 99;  // out of bounds — no transition entries

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {nc, halt};

    // No native calls registered → index 99 is invalid
    auto blob = build_test_blob(seed, {bb});
    auto loaded = load_blob(blob.data(), blob.size(), seed, test_config());
    ASSERT_TRUE(loaded.has_value());

    auto result = execute(loaded->ctx, test_config());
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::NativeCallBridgeFailed);
}

// ---------------------------------------------------------------------------
// Test 7: VM → native → VM reentrancy
//
// The native function receives (blob_ptr, blob_size, seed_ptr) as arguments
// and calls vm_execute() internally, creating a nested VM instance.
// This proves vm→native→vm works without corrupting the outer VM's state.
// ---------------------------------------------------------------------------

// Shared test data for reentrancy (must outlive both VM instances)
struct ReentrantTestData {
    std::vector<uint8_t> inner_blob;
    uint8_t seed[32];
};
static ReentrantTestData* g_reentrant_data = nullptr;

static uint64_t native_call_inner_vm(
    uint64_t input_val, uint64_t, uint64_t, uint64_t,
    uint64_t, uint64_t, uint64_t, uint64_t)
{
    // Re-enter the VM with a simple ADD blob
    auto* data = g_reentrant_data;
    VmExecRequest req{};
    req.blob_data   = data->inner_blob.data();
    req.blob_size   = data->inner_blob.size();
    req.stored_seed = data->seed;
    req.config.debug_mode = true;

    // Pass input_val as r0, 100 as r1 → inner VM computes r0 + r1
    uint64_t initial_regs[16] = {};
    initial_regs[0] = input_val;
    initial_regs[1] = 100;
    req.initial_regs = initial_regs;
    req.num_regs     = 2;

    auto result = vm_execute(req);
    if (!result.has_value()) return 0xDEAD;
    return result->return_value;
}

TEST(NativeCallE2E, VmNativeVmReentrancy) {
    uint8_t seed[32]; fill_seed(seed);

    // Build the INNER blob: ADD r0, r1; HALT
    ReentrantTestData data;
    std::memcpy(data.seed, seed, 32);
    {
        TestBB inner_bb{};
        inner_bb.bb_id = 1; inner_bb.epoch = 0;
        inner_bb.live_regs_bitmap = 0xFFFF; inner_bb.flags = 0;
        fill_epoch(inner_bb.epoch_seed, 0x42);

        TestInstruction add{};
        add.opcode = VmOpcode::ADD;
        add.flags  = flags_rr();
        add.reg_a  = 0; add.reg_b = 1;

        TestInstruction halt{};
        halt.opcode = VmOpcode::HALT;
        inner_bb.instructions = {add, halt};

        data.inner_blob = build_test_blob(seed, {inner_bb});
    }
    g_reentrant_data = &data;

    // Build the OUTER blob: NATIVE_CALL[0] (calls native_call_inner_vm), HALT
    TestBB outer_bb{};
    outer_bb.bb_id = 1; outer_bb.epoch = 0;
    outer_bb.live_regs_bitmap = 0xFFFF; outer_bb.flags = 0;
    fill_epoch(outer_bb.epoch_seed, 0x77);

    TestInstruction nc{};
    nc.opcode = VmOpcode::NATIVE_CALL;
    nc.flags  = flags_none();
    nc.aux    = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    outer_bb.instructions = {nc, halt};

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count    = 1;  // pass r0 as input_val
    tc.target_addr  = reinterpret_cast<uint64_t>(&native_call_inner_vm);

    auto outer_blob = build_test_blob(seed, {outer_bb}, {}, false, {tc});
    auto loaded = load_blob(outer_blob.data(), outer_blob.size(),
                            seed, test_config());
    ASSERT_TRUE(loaded.has_value());

    auto& ctx = loaded->ctx;
    ctx.encoded_regs[0] = encode_register(ctx, 0, 50);  // input to inner VM

    auto result = execute(ctx, test_config());
    ASSERT_TRUE(result.has_value());

    // Inner VM: ADD(50, 100) = 150.  Returned to outer VM as r0.
    EXPECT_EQ(decode_register(ctx, 0, ctx.encoded_regs[0]), 150u);

    g_reentrant_data = nullptr;
}

// ---------------------------------------------------------------------------
// Test 8: native → vm → native (vm_execute calls native, which is our entry)
//
// The top-level IS native code (the test itself). It calls vm_execute()
// which runs NATIVE_CALL → calls a native fn → returns to VM → HALT.
// This is the standard production pattern: loader stub calls vm_execute,
// VM calls external functions via NATIVE_CALL, returns to vm_execute.
// ---------------------------------------------------------------------------

TEST(NativeCallE2E, NativeVmNative) {
    uint8_t seed[32]; fill_seed(seed);

    // Build blob: NATIVE_CALL[0] (triple), HALT
    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0x88);

    TestInstruction nc{};
    nc.opcode = VmOpcode::NATIVE_CALL;
    nc.flags  = flags_none();
    nc.aux    = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {nc, halt};

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count    = 1;
    tc.target_addr  = reinterpret_cast<uint64_t>(&native_triple);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});

    // Use the high-level vm_execute API (what the loader stub calls)
    VmExecRequest req{};
    req.blob_data   = blob.data();
    req.blob_size   = blob.size();
    req.stored_seed = seed;
    req.config.debug_mode = true;

    uint64_t initial_regs[16] = {};
    initial_regs[0] = 11;  // tripled → 33
    req.initial_regs = initial_regs;
    req.num_regs     = 1;

    auto result = vm_execute(req);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->return_value, 33u);
}

// ---------------------------------------------------------------------------
// Test 9: Deep reentrancy — 100 levels of vm → native → vm → native → ...
//
// A single blob contains NATIVE_CALL to a recursive native function.
// The native function decrements a counter and re-enters vm_execute()
// with the SAME blob, creating depth N nesting.  At depth 0 it returns
// the accumulated value.
//
// This proves:
//   - No global mutable state corruption across 100 independent VMContexts
//   - C stack handles 100 × (VMContext + LoadedVM) frames
//   - Encoding tables, enc_state, ORAM are fully independent per level
// ---------------------------------------------------------------------------

struct RecursiveTestData {
    std::vector<uint8_t> blob;
    uint8_t seed[32];
};
static RecursiveTestData* g_recursive_data = nullptr;

/// r0 = remaining depth, r1 = accumulator.
/// Each level: acc += depth, then recurse with depth-1.
/// Final result: sum(1..initial_depth) = depth*(depth+1)/2
static uint64_t native_recursive_vm(
    uint64_t depth, uint64_t acc, uint64_t, uint64_t,
    uint64_t, uint64_t, uint64_t, uint64_t)
{
    acc += depth;
    if (depth == 0) return acc;

    auto* data = g_recursive_data;
    VmExecRequest req{};
    req.blob_data   = data->blob.data();
    req.blob_size   = data->blob.size();
    req.stored_seed = data->seed;
    req.config.debug_mode = true;

    uint64_t regs[16] = {};
    regs[0] = depth - 1;  // remaining depth
    regs[1] = acc;        // accumulator
    req.initial_regs = regs;
    req.num_regs     = 2;

    auto result = vm_execute(req);
    if (!result.has_value()) return 0xDEADDEADu;
    return result->return_value;
}

// The 100-level deep reentrancy body — runs on a thread with 64MB stack
// because each vm_execute + ephemeral encoding frame is ~40KB+ under ASan.
static LargeStackResult deep_reentrancy_body(void* arg) {
    auto* data = static_cast<RecursiveTestData*>(arg);
    g_recursive_data = data;

    constexpr uint64_t DEPTH = 100;
    constexpr uint64_t EXPECTED = DEPTH * (DEPTH + 1) / 2;  // 5050

    VmExecRequest req{};
    req.blob_data   = data->blob.data();
    req.blob_size   = data->blob.size();
    req.stored_seed = data->seed;
    req.config.debug_mode = true;

    uint64_t regs[16] = {};
    regs[0] = DEPTH;
    regs[1] = 0;
    req.initial_regs = regs;
    req.num_regs     = 2;

    auto result = vm_execute(req);
    g_recursive_data = nullptr;

    if (!result.has_value())
        return {false, "vm_execute failed at 100 levels"};
    if (result->return_value != EXPECTED)
        return {false, "expected " + std::to_string(EXPECTED)
                       + " got " + std::to_string(result->return_value)};
    return {true, {}};
}

TEST(NativeCallE2E, DeepReentrancy100Levels) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0x33);

    TestInstruction nc{};
    nc.opcode = VmOpcode::NATIVE_CALL;
    nc.flags  = flags_none();
    nc.aux    = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {nc, halt};

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count    = 2;
    tc.target_addr  = reinterpret_cast<uint64_t>(&native_recursive_vm);

    RecursiveTestData data;
    std::memcpy(data.seed, seed, 32);
    data.blob = build_test_blob(seed, {bb}, {}, false, {tc});

    // Run on a 64MB stack thread — ASan inflates each reentrancy frame
    // to ~40KB+ (VMContext + ephemeral LUT tables + red zones).
    auto r = run_on_large_stack(deep_reentrancy_body, &data);
    ASSERT_TRUE(r.ok) << r.error;
}
