/// @file opcode/test_data_movement.cpp
/// @brief Tests for data movement opcodes: LOAD/STORE, PUSH/POP, LOAD_CTX/STORE_CTX.
///
/// Combined from:
///   - test_load_store.cpp: STORE/LOAD roundtrip, multiple addresses, large values, overwrite, zero
///   - test_push_pop_oram.cpp: basic roundtrip, LIFO, multiple values, large value, RollingKey LIFO
///   - test_ctx_ops.cpp: LOAD_CTX vm_sp, LOAD_CTX vm_ip, STORE_CTX roundtrip

#include "test_blob_builder.hpp"

#include "vm_engine.hpp"
#include "vm_state.hpp"

#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <vector>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Test;

// ============================================================================
// Helpers
// ============================================================================

static uint8_t pool_none() {
    return static_cast<uint8_t>((VM_OPERAND_POOL << 6) | (VM_OPERAND_NONE << 4));
}
static uint8_t rr() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4));
}
static uint8_t reg_none() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_NONE << 4));
}
static uint8_t flags_none() { return 0; }

template<typename Policy = DebugPolicy, typename Oram = DirectOram>
auto single_bb_engine(const uint8_t seed[32], uint8_t epoch_base,
                      const std::vector<TestInstruction>& insns,
                      const std::vector<TestPoolEntry>& pool = {}) {
    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, epoch_base);
    bb.instructions = insns;
    auto blob = pool.empty()
        ? build_test_blob(seed, {bb})
        : build_test_blob_ex(seed, {bb}, pool);
    return VmEngine<Policy, Oram>::create(
        blob.data(), blob.size(), seed);
}

// ############################################################################
// LOAD / STORE (from test_load_store.cpp)
// ############################################################################

// ============================================================================
// Test 1: STORE then LOAD roundtrip
// ============================================================================

TEST(DataMovement, StoreLoadRoundtrip) {
    uint8_t seed[32]; fill_seed(seed);

    alignas(8) uint64_t guest_mem = 0;

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xA0);

    // r0 = 42, STORE r0 → guest_mem, LOAD_CONST r0=0, LOAD r0 ← guest_mem, HALT
    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 42
        {VmOpcode::STORE, rr(), 0, 0, 0},               // STORE r0 at aux=0
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 0 (clear)
        {VmOpcode::LOAD, rr(), 0, 0, 0},                // LOAD r0 from aux=0
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{42, 0, 0};
    TestPoolEntry p1{0, 0, 0};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed,
        static_cast<int64_t>(reinterpret_cast<uintptr_t>(&guest_mem)));
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u) << "STORE then LOAD should roundtrip";
}

// ============================================================================
// Test 2: Multiple addresses (store at different offsets)
// ============================================================================

TEST(DataMovement, MultipleAddresses) {
    uint8_t seed[32]; fill_seed(seed);

    alignas(8) uint64_t guest_mem[2] = {0, 0};

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xA1);

    // Store 10 at offset 0, store 20 at offset 8, load from offset 0
    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 10
        {VmOpcode::STORE, rr(), 0, 0, 0},               // STORE at offset 0
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 20
        {VmOpcode::STORE, rr(), 0, 0, 8},               // STORE at offset 8
        {VmOpcode::LOAD, rr(), 0, 0, 0},                // LOAD from offset 0 → r0=10
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{10, 0, 0};
    TestPoolEntry p1{20, 0, 0};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed,
        static_cast<int64_t>(reinterpret_cast<uintptr_t>(&guest_mem[0])));
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 10u) << "LOAD from offset 0 should read first stored value";
    EXPECT_NE(guest_mem[1], 0u) << "STORE at offset 8 should have written encoded(20)";
}

// ============================================================================
// Test 3: Load from second offset
// ============================================================================

TEST(DataMovement, LoadFromSecondOffset) {
    uint8_t seed[32]; fill_seed(seed);

    alignas(8) uint64_t guest_mem[2] = {0, 0};

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xA2);

    // Store 10 at offset 0, store 20 at offset 8, load from offset 8
    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 10
        {VmOpcode::STORE, rr(), 0, 0, 0},               // STORE at offset 0
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 20
        {VmOpcode::STORE, rr(), 0, 0, 8},               // STORE at offset 8
        {VmOpcode::LOAD, rr(), 0, 0, 8},                // LOAD from offset 8 → r0=20
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{10, 0, 0};
    TestPoolEntry p1{20, 0, 0};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed,
        static_cast<int64_t>(reinterpret_cast<uintptr_t>(&guest_mem[0])));
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 20u) << "LOAD from offset 8 should read second value";
}

// ============================================================================
// Test 4: Large value roundtrip
// ============================================================================

TEST(DataMovement, LargeValueRoundtrip_LoadStore) {
    uint8_t seed[32]; fill_seed(seed);

    alignas(8) uint64_t guest_mem = 0;
    uint64_t big = 0xDEADBEEFCAFEBABEull;

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xA3);

    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
        {VmOpcode::STORE, rr(), 0, 0, 0},
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},
        {VmOpcode::LOAD, rr(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{big, 0, 0};
    TestPoolEntry p1{0, 0, 0};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed,
        static_cast<int64_t>(reinterpret_cast<uintptr_t>(&guest_mem)));
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, big);
}

// ============================================================================
// Test 5: STORE overwrites previous value
// ============================================================================

TEST(DataMovement, StoreOverwrite) {
    uint8_t seed[32]; fill_seed(seed);

    alignas(8) uint64_t guest_mem = 0;

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xA4);

    // Store 10, then overwrite with 20, then load (should get 20)
    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 10
        {VmOpcode::STORE, rr(), 0, 0, 0},
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 20
        {VmOpcode::STORE, rr(), 0, 0, 0},               // overwrite
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 2},  // r0 = 0
        {VmOpcode::LOAD, rr(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{10, 0, 0};
    TestPoolEntry p1{20, 0, 0};
    TestPoolEntry p2{0, 0, 0};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1, p2});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed,
        static_cast<int64_t>(reinterpret_cast<uintptr_t>(&guest_mem)));
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 20u) << "Second STORE should overwrite first";
}

// ============================================================================
// Test 6: Zero value roundtrip
// ============================================================================

TEST(DataMovement, ZeroValueRoundtrip_LoadStore) {
    uint8_t seed[32]; fill_seed(seed);

    alignas(8) uint64_t guest_mem = 0xFFFFFFFFFFFFFFFFull;  // non-zero initial

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xA5);

    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 0
        {VmOpcode::STORE, rr(), 0, 0, 0},
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 99 (clear)
        {VmOpcode::LOAD, rr(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{0, 0, 0};
    TestPoolEntry p1{99, 0, 0};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed,
        static_cast<int64_t>(reinterpret_cast<uintptr_t>(&guest_mem)));
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0u) << "Zero should roundtrip through STORE/LOAD";
}

// ############################################################################
// PUSH / POP (from test_push_pop_oram.cpp — excluding StackOverflow/Underflow)
// ############################################################################

// ============================================================================
// Basic PUSH/POP roundtrip
// ============================================================================

TEST(DataMovement, BasicRoundtrip_DirectOram) {
    uint8_t seed[32]; fill_seed(seed);
    // r0=42, PUSH r0, LOAD_CONST r0=0, POP r0, HALT
    auto engine = single_bb_engine<DebugPolicy, DirectOram>(seed, 0xA0,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 0
         {VmOpcode::POP, rr(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{42, 0, 0}, {0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u);
}

TEST(DataMovement, BasicRoundtrip_RollingKeyOram) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine<DebugPolicy, RollingKeyOram>(seed, 0xA0,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},
         {VmOpcode::POP, rr(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{42, 0, 0}, {0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u);
}

// ============================================================================
// LIFO order: push A, push B, pop → B, pop → A
// ============================================================================

TEST(DataMovement, LIFOOrder) {
    uint8_t seed[32]; fill_seed(seed);
    // r0=10 (A), PUSH r0, LOAD r0=20 (B), PUSH r0, POP r0 → should be 20 (B)
    // Then POP r1 → should be 10 (A), but we verify via r0.
    // Strategy: push 10, push 20, pop r0 (=20), HALT
    auto engine = single_bb_engine<DebugPolicy, DirectOram>(seed, 0xA1,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 10
         {VmOpcode::PUSH, rr(), 0, 0, 0},                // push 10
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},   // r0 = 20
         {VmOpcode::PUSH, rr(), 0, 0, 0},                // push 20
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 2},   // r0 = 0 (clear)
         {VmOpcode::POP, rr(), 0, 0, 0},                 // pop → r0 = 20 (LIFO)
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{10, 0, 0}, {20, 0, 0}, {0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 20u) << "LIFO: first pop should return last pushed";
}

// ============================================================================
// Multiple values: push 3 values, pop them in reverse order
// ============================================================================

TEST(DataMovement, MultipleValues_PushPop) {
    uint8_t seed[32]; fill_seed(seed);
    // Push 10, 20, 30. Pop three times. Last pop (third) gives 10 in r0.
    // Sequence: push 10, push 20, push 30, pop (30→r0), pop (20→r0), pop (10→r0)
    auto engine = single_bb_engine<DebugPolicy, DirectOram>(seed, 0xA2,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 10
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 20
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 2},  // r0 = 30
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::POP, rr(), 0, 0, 0},                // r0 = 30
         {VmOpcode::POP, rr(), 0, 0, 0},                // r0 = 20
         {VmOpcode::POP, rr(), 0, 0, 0},                // r0 = 10
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{10, 0, 0}, {20, 0, 0}, {30, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 10u) << "After 3 pushes and 3 pops, r0 should be first pushed value";
}

// ============================================================================
// Large value roundtrip (64-bit)
// ============================================================================

TEST(DataMovement, LargeValueRoundtrip_PushPop) {
    uint8_t seed[32]; fill_seed(seed);
    uint64_t big = 0xDEADBEEFCAFEBABEull;
    auto engine = single_bb_engine<DebugPolicy, DirectOram>(seed, 0xA5,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},
         {VmOpcode::POP, rr(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{big, 0, 0}, {0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, big);
}

// ============================================================================
// RollingKeyOram: LIFO order preserved under re-encryption
// ============================================================================

TEST(DataMovement, RollingKey_LIFOOrder) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine<DebugPolicy, RollingKeyOram>(seed, 0xA6,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 10
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},   // r0 = 20
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 2},   // r0 = 0
         {VmOpcode::POP, rr(), 0, 0, 0},                 // r0 = 20
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{10, 0, 0}, {20, 0, 0}, {0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 20u);
}

// ############################################################################
// LOAD_CTX / STORE_CTX (from test_ctx_ops.cpp)
// ############################################################################

// ============================================================================
// LOAD_CTX vm_sp (aux=1)
// vm_sp starts at VM_OBLIVIOUS_SIZE (4096). After LOAD_CTX, r0 should be 4096.
// ============================================================================

TEST(DataMovement, LoadCtxSp) {
    uint8_t seed[32]; fill_seed(seed);

    // LOAD_CTX r0, vm_sp (aux=1), HALT
    auto engine = single_bb_engine(seed, 0xA0,
        {{VmOpcode::LOAD_CTX, flags_none(), 0, 0, 1},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, VM_OBLIVIOUS_SIZE)
        << "LOAD_CTX vm_sp should return initial stack pointer (4096)";
}

// ============================================================================
// LOAD_CTX vm_ip (aux=0)
// vm_ip at the LOAD_CTX instruction depends on where it is in the program.
// It's the first instruction (ip=0).
// ============================================================================

TEST(DataMovement, LoadCtxIp) {
    uint8_t seed[32]; fill_seed(seed);

    // LOAD_CTX r0, vm_ip (aux=0), HALT
    auto engine = single_bb_engine(seed, 0xA1,
        {{VmOpcode::LOAD_CTX, flags_none(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    // vm_ip at the first instruction should be 0 (entry_ip of first BB)
    EXPECT_EQ(r->return_value, 0u)
        << "LOAD_CTX vm_ip at first instruction should be 0";
}

// ============================================================================
// STORE_CTX + LOAD_CTX roundtrip on vm_sp
// Set vm_sp to 2048 via STORE_CTX, then read it back via LOAD_CTX.
// ============================================================================

TEST(DataMovement, StoreCtxLoadCtxRoundtrip) {
    uint8_t seed[32]; fill_seed(seed);

    // LOAD_CONST r0=2048, STORE_CTX r0 → vm_sp, LOAD_CTX r0 → vm_sp, HALT
    auto engine = single_bb_engine(seed, 0xA2,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 2048
         {VmOpcode::STORE_CTX, reg_none(), 0, 0, 1},     // vm_sp = r0 (operand A = REG)
         {VmOpcode::LOAD_CTX, flags_none(), 0, 0, 1},    // r0 = vm_sp
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{2048, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 2048u)
        << "STORE_CTX then LOAD_CTX should roundtrip vm_sp = 2048";
}

// ############################################################################
// EngineHandlers data-movement tests (from test_engine_comprehensive.cpp)
// ############################################################################

TEST(DataMovement, EngineHandlers_LoadConstAndHalt) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine(seed, 0xBB,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{42, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u);
}

TEST(DataMovement, EngineHandlers_MovePreservesValue) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine(seed, 0xBB,
        {{VmOpcode::LOAD_CONST, pool_none(), 1, 0, 0},  // r1 = pool[0]
         {VmOpcode::MOVE, rr(), 0, 1, 0},               // r0 = r1
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{99, 0, 1}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 99u);
}

// ############################################################################
// EngineOram PUSH/POP (from test_engine_comprehensive.cpp)
// ############################################################################

TEST(DataMovement, EngineOram_PushPopRoundtrip) {
    uint8_t seed[32]; fill_seed(seed);
    // r0=42, PUSH r0, LOAD_CONST r0=0, POP r0, HALT
    auto engine = single_bb_engine<DebugPolicy, DirectOram>(seed, 0xA0,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 0
         {VmOpcode::POP, rr(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{42, 0, 0}, {0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u);
}

TEST(DataMovement, EngineOram_PushPopWithRollingKeystream) {
    uint8_t seed[32]; fill_seed(seed);
    // Same test but with RollingKeyOram -- should produce identical result
    auto engine = single_bb_engine<DebugPolicy, RollingKeyOram>(seed, 0xA0,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},
         {VmOpcode::POP, rr(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{42, 0, 0}, {0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u);
}

// ############################################################################
// EngineAtomic tests (from test_engine_comprehensive.cpp)
// ############################################################################

#include "pipeline.hpp"
#include "handler_impls.hpp"

#include <vm/encoded_value.hpp>
#include <vm/vm_context.hpp>

#include <cassert>

namespace {

/// Helper: set up state for direct atomic handler invocation.
struct AtomicTestFixture {
    uint8_t seed[32];
    std::shared_ptr<const VmImmutable> imm;
    VmEpoch epoch;
    VmExecution exec{};
    VmOramState oram{};
    HandlerTable table;

    AtomicTestFixture() {
        fill_seed(seed);
        auto engine = single_bb_engine(seed, 0xF7,
            {{VmOpcode::HALT, flags_none(), 0, 0, 0}});
        assert(engine.has_value());
        imm = engine->shared_immutable();

        BBMetadata bb{};
        bb.bb_id = 1;
        bb.epoch = 0;
        bb.live_regs_bitmap = 0xFFFF;
        fill_epoch(bb.epoch_seed, 0xF7);
        epoch.enter_bb(bb);

        oram.init(*imm);
        table = build_handler_table<DebugPolicy, DirectOram>();
    }

    void set_reg(uint8_t reg, uint64_t plain) {
        exec.regs[reg] = RegVal(plain);
    }

    uint64_t get_reg(uint8_t reg) const {
        return exec.regs[reg].bits;
    }

    DecodedInsn make_insn(VmOpcode op, uint8_t ra, uint8_t rb = 0) const {
        DecodedInsn insn{};
        insn.opcode = op;
        insn.reg_a = ra;
        insn.reg_b = rb;
        insn.aux = 0;
        insn.operand_a_type = 0;
        insn.operand_b_type = 0;
        insn.condition = 0;
        insn.plaintext_opcode = 0;
        insn.plain_a = exec.regs[ra].bits;
        insn.plain_b = exec.regs[rb].bits;
        return insn;
    }

    HandlerResult call(const DecodedInsn& insn) {
        return table[static_cast<uint8_t>(insn.opcode)](
            exec, epoch, oram, *imm, insn);
    }
};

}  // anonymous namespace

TEST(DataMovement, Atomic_LockAdd) {
    AtomicTestFixture f;

    alignas(8) uint64_t target = 100;
    f.exec.load_base_delta = static_cast<int64_t>(
        reinterpret_cast<uintptr_t>(&target));

    f.set_reg(0, 50);

    auto insn = f.make_insn(VmOpcode::LOCK_ADD, /*reg_a=*/0);
    auto r = f.call(insn);
    ASSERT_TRUE(r.has_value());

    EXPECT_EQ(target, 150u);
    EXPECT_EQ(f.get_reg(0), 100u);
}

TEST(DataMovement, Atomic_LockAddZero) {
    AtomicTestFixture f;

    alignas(8) uint64_t target = 42;
    f.exec.load_base_delta = static_cast<int64_t>(
        reinterpret_cast<uintptr_t>(&target));

    f.set_reg(0, 0);

    auto insn = f.make_insn(VmOpcode::LOCK_ADD, 0);
    auto r = f.call(insn);
    ASSERT_TRUE(r.has_value());

    EXPECT_EQ(target, 42u);
    EXPECT_EQ(f.get_reg(0), 42u);
}

TEST(DataMovement, Atomic_Xchg) {
    AtomicTestFixture f;

    alignas(8) uint64_t target = 0xDEADBEEF;
    f.exec.load_base_delta = static_cast<int64_t>(
        reinterpret_cast<uintptr_t>(&target));

    f.set_reg(0, 0xCAFEBABE);

    auto insn = f.make_insn(VmOpcode::XCHG, /*reg_a=*/0);
    auto r = f.call(insn);
    ASSERT_TRUE(r.has_value());

    EXPECT_EQ(target, 0xCAFEBABEu);
    EXPECT_EQ(f.get_reg(0), 0xDEADBEEFu);
}

TEST(DataMovement, Atomic_XchgSameValue) {
    AtomicTestFixture f;

    alignas(8) uint64_t target = 77;
    f.exec.load_base_delta = static_cast<int64_t>(
        reinterpret_cast<uintptr_t>(&target));

    f.set_reg(0, 77);

    auto insn = f.make_insn(VmOpcode::XCHG, 0);
    auto r = f.call(insn);
    ASSERT_TRUE(r.has_value());

    EXPECT_EQ(target, 77u);
    EXPECT_EQ(f.get_reg(0), 77u);
}

TEST(DataMovement, Atomic_CmpxchgSuccess) {
    AtomicTestFixture f;

    alignas(8) uint64_t target = 100;
    f.exec.load_base_delta = static_cast<int64_t>(
        reinterpret_cast<uintptr_t>(&target));

    f.set_reg(0, 100);  // expected
    f.set_reg(1, 200);  // desired

    auto insn = f.make_insn(VmOpcode::CMPXCHG, /*reg_a=*/0, /*reg_b=*/1);
    auto r = f.call(insn);
    ASSERT_TRUE(r.has_value());

    EXPECT_EQ(target, 200u);
    EXPECT_EQ(f.get_reg(0), 100u);
    EXPECT_EQ(f.exec.vm_flags & 0x01, 0x01u);
}

TEST(DataMovement, Atomic_CmpxchgFailure) {
    AtomicTestFixture f;

    alignas(8) uint64_t target = 100;
    f.exec.load_base_delta = static_cast<int64_t>(
        reinterpret_cast<uintptr_t>(&target));

    f.set_reg(0, 999);  // wrong expected
    f.set_reg(1, 200);  // desired (should NOT be written)

    auto insn = f.make_insn(VmOpcode::CMPXCHG, 0, 1);
    auto r = f.call(insn);
    ASSERT_TRUE(r.has_value());

    EXPECT_EQ(target, 100u);
    EXPECT_EQ(f.get_reg(0), 100u);
    EXPECT_EQ(f.exec.vm_flags & 0x01, 0x00u);
}

TEST(DataMovement, Atomic_CmpxchgSuccessThenFailure) {
    AtomicTestFixture f;

    alignas(8) uint64_t target = 10;
    f.exec.load_base_delta = static_cast<int64_t>(
        reinterpret_cast<uintptr_t>(&target));

    // First: succeed (expect 10, swap in 20)
    f.set_reg(0, 10);
    f.set_reg(1, 20);

    auto insn = f.make_insn(VmOpcode::CMPXCHG, 0, 1);
    auto r1 = f.call(insn);
    ASSERT_TRUE(r1.has_value());
    EXPECT_EQ(target, 20u);
    EXPECT_EQ(f.exec.vm_flags & 0x01, 0x01u);

    // Second: fail (expect 10, but memory is now 20)
    f.set_reg(0, 10);  // stale expected
    f.set_reg(1, 30);

    auto insn2 = f.make_insn(VmOpcode::CMPXCHG, 0, 1);
    auto r2 = f.call(insn2);
    ASSERT_TRUE(r2.has_value());
    EXPECT_EQ(target, 20u);  // unchanged
    EXPECT_EQ(f.get_reg(0), 20u);  // actual value loaded
    EXPECT_EQ(f.exec.vm_flags & 0x01, 0x00u);
}

TEST(DataMovement, Atomic_AtomicLoad) {
    AtomicTestFixture f;

    alignas(8) uint64_t target = 0x123456789ABCDEF0ull;
    f.exec.load_base_delta = static_cast<int64_t>(
        reinterpret_cast<uintptr_t>(&target));

    auto insn = f.make_insn(VmOpcode::ATOMIC_LOAD, /*reg_a=*/0);
    auto r = f.call(insn);
    ASSERT_TRUE(r.has_value());

    EXPECT_EQ(f.get_reg(0), 0x123456789ABCDEF0ull);
}

TEST(DataMovement, Atomic_AtomicLoadZero) {
    AtomicTestFixture f;

    alignas(8) uint64_t target = 0;
    f.exec.load_base_delta = static_cast<int64_t>(
        reinterpret_cast<uintptr_t>(&target));

    auto insn = f.make_insn(VmOpcode::ATOMIC_LOAD, 0);
    auto r = f.call(insn);
    ASSERT_TRUE(r.has_value());

    EXPECT_EQ(f.get_reg(0), 0u);
}

TEST(DataMovement, Atomic_AtomicLoadDifferentRegisters) {
    AtomicTestFixture f;

    alignas(8) uint64_t target = 42;
    f.exec.load_base_delta = static_cast<int64_t>(
        reinterpret_cast<uintptr_t>(&target));

    auto insn = f.make_insn(VmOpcode::ATOMIC_LOAD, /*reg_a=*/3);
    auto r = f.call(insn);
    ASSERT_TRUE(r.has_value());

    EXPECT_EQ(f.get_reg(3), 42u);
}

TEST(DataMovement, Atomic_LockAddOverflow) {
    AtomicTestFixture f;

    alignas(8) uint64_t target = UINT64_MAX;
    f.exec.load_base_delta = static_cast<int64_t>(
        reinterpret_cast<uintptr_t>(&target));

    f.set_reg(0, 1);

    auto insn = f.make_insn(VmOpcode::LOCK_ADD, 0);
    auto r = f.call(insn);
    ASSERT_TRUE(r.has_value());

    EXPECT_EQ(target, 0u);  // wraps around
    EXPECT_EQ(f.get_reg(0), UINT64_MAX);  // old value
}

// ============================================================================
// FENCE: full memory barrier — should not crash, no register side effects.
// ============================================================================

TEST(DataMovement, Atomic_FenceDoesNotCrash) {
    AtomicTestFixture f;

    auto insn = f.make_insn(VmOpcode::FENCE, /*reg_a=*/0);
    auto r = f.call(insn);
    ASSERT_TRUE(r.has_value())
        << "FENCE handler should succeed without crashing";
}

TEST(DataMovement, FenceViaEngine) {
    // Full-engine test: FENCE followed by HALT should execute cleanly.
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine(seed, 0xAD,
        {{VmOpcode::FENCE, flags_none(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
}
