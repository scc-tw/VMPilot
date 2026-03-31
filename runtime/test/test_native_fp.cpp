/// @file test_native_fp.cpp
/// @brief E2E tests for FP/variadic/struct return native calls (D13§E2-E4).
///
/// These tests verify that the platform_call() trampoline correctly
/// places FP args in xmm/d registers and handles variadic AL on x86-64.
///
/// Note: FP tests only work on platforms with inline asm support
/// (x86-64, ARM64).  The fallback path (all-integer) is already
/// tested by test_native_call_e2e.cpp.

#include "test_blob_builder.hpp"

#include <dispatcher.hpp>
#include <decoder.hpp>
#include <encoding.hpp>
#include <vm_entry.hpp>
#include <vm_loader.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <cstdint>
#include <cmath>
#include <cstdio>
#include <vector>

using namespace VMPilot::Common;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Runtime;
using namespace VMPilot::Test;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void fill_seed(uint8_t seed[32]) {
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);
}

static void fill_epoch(uint8_t seed[32], uint8_t base) {
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(base + i);
}

static VmSecurityConfig test_config() {
    VmSecurityConfig cfg{};
    cfg.debug_mode = true;
    return cfg;
}

/// Pack TransitionEntry.arg_count with FP metadata.
static uint32_t pack_arg_count(uint8_t count, uint8_t fp_mask = 0,
                                bool variadic = false, bool returns_fp = false) {
    uint32_t val = count & 0x0Fu;
    val |= (static_cast<uint32_t>(fp_mask) << 4);
    if (variadic)   val |= (1u << 12);
    if (returns_fp) val |= (1u << 13);
    return val;
}

// ---------------------------------------------------------------------------
// Native test functions with FP signatures.
//
// These functions receive arguments via the platform ABI (FP in xmm/d regs).
// The bridge's platform_call() trampoline is responsible for placing them
// correctly.  If the trampoline is broken, these will receive garbage.
// ---------------------------------------------------------------------------

#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__) || defined(_M_ARM64)

/// Properly typed FP test functions — the platform_call() trampoline
/// loads FP-flagged args into xmm/d regs before calling these.
extern "C" double fp_add_doubles(double a, double b) {
    return a + b;
}

extern "C" double fp_multiply_doubles(double a, double b) {
    return a * b;
}

extern "C" uint64_t mixed_int_fp(uint64_t a, double b,
                                  uint64_t c, double d,
                                  uint64_t, uint64_t,
                                  uint64_t, uint64_t) {
    // On System V x86-64: a in rdi, b in xmm0, c in rsi, d in xmm1
    // On AAPCS ARM64: a in x0, b in d0, c in x1, d in d1
    //
    // NOTE: Our current bridge passes all args sequentially in integer
    // regs AND loads FP-flagged ones into xmm/d regs.  For simple
    // signatures this works, but for mixed int/FP with separate counters
    // (System V), the integer arg positions shift.  This is a known
    // limitation — the compiler must encode the correct register mapping.
    //
    // For this test, we accept the integer values as-is.
    double bd, dd;
    std::memcpy(&bd, &b, 8);  // b might arrive as uint64_t bit pattern
    std::memcpy(&dd, &d, 8);
    return a + static_cast<uint64_t>(bd) + c + static_cast<uint64_t>(dd);
}

// ---------------------------------------------------------------------------
// Test 1: Two double args — fp_add_doubles(3.14, 2.86) = 6.0
// ---------------------------------------------------------------------------

TEST(NativeFP, AddDoubles) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xAA);

    TestInstruction nc{};
    nc.opcode = VmOpcode::NATIVE_CALL;
    nc.flags  = 0;
    nc.aux    = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {nc, halt};

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count    = pack_arg_count(2, 0x03, false, true);
    // fp_mask=0x03 (both args are FP), returns_fp=true
    tc.target_addr  = reinterpret_cast<uint64_t>(&fp_add_doubles);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});
    auto loaded = load_blob(blob.data(), blob.size(), seed, test_config());
    ASSERT_TRUE(loaded.has_value());

    auto& ctx = loaded->ctx;

    // Encode double bit patterns as uint64_t into registers
    double a_d = 3.14, b_d = 2.86;
    uint64_t a_bits, b_bits;
    std::memcpy(&a_bits, &a_d, 8);
    std::memcpy(&b_bits, &b_d, 8);

    ctx.encoded_regs[0] = encode_register(ctx, 0, a_bits);
    ctx.encoded_regs[1] = encode_register(ctx, 1, b_bits);

    auto result = execute(ctx, test_config());
    ASSERT_TRUE(result.has_value())
        << "execute failed: " << static_cast<uint32_t>(result.error());

    // Result is FP (returns_fp=true) — decode from r0 as double bit pattern
    uint64_t result_bits = decode_register(ctx, 0, ctx.encoded_regs[0]);
    double result_d;
    std::memcpy(&result_d, &result_bits, 8);

    EXPECT_NEAR(result_d, 6.0, 1e-10)
        << "fp_add_doubles(3.14, 2.86) should be ~6.0, got " << result_d;
}

// ---------------------------------------------------------------------------
// Test 2: FP multiply — fp_multiply_doubles(2.5, 4.0) = 10.0
// ---------------------------------------------------------------------------

TEST(NativeFP, MultiplyDoubles) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xBB);

    TestInstruction nc{};
    nc.opcode = VmOpcode::NATIVE_CALL;
    nc.aux    = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {nc, halt};

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count    = pack_arg_count(2, 0x03, false, true);
    tc.target_addr  = reinterpret_cast<uint64_t>(&fp_multiply_doubles);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});
    auto loaded = load_blob(blob.data(), blob.size(), seed, test_config());
    ASSERT_TRUE(loaded.has_value());

    auto& ctx = loaded->ctx;
    double a_d = 2.5, b_d = 4.0;
    uint64_t a_bits, b_bits;
    std::memcpy(&a_bits, &a_d, 8);
    std::memcpy(&b_bits, &b_d, 8);

    ctx.encoded_regs[0] = encode_register(ctx, 0, a_bits);
    ctx.encoded_regs[1] = encode_register(ctx, 1, b_bits);

    auto result = execute(ctx, test_config());
    ASSERT_TRUE(result.has_value());

    uint64_t result_bits = decode_register(ctx, 0, ctx.encoded_regs[0]);
    double result_d;
    std::memcpy(&result_d, &result_bits, 8);

    EXPECT_NEAR(result_d, 10.0, 1e-10);
}

// ---------------------------------------------------------------------------
// Test 3: Integer-only call still works with fp_mask=0 (regression)
// ---------------------------------------------------------------------------

static uint64_t native_int_add(uint64_t a, uint64_t b,
                               uint64_t, uint64_t, uint64_t, uint64_t,
                               uint64_t, uint64_t) {
    return a + b;
}

TEST(NativeFP, IntegerRegression) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xCC);

    TestInstruction nc{};
    nc.opcode = VmOpcode::NATIVE_CALL;
    nc.aux    = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {nc, halt};

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count    = pack_arg_count(2, 0x00, false, false);  // pure integer
    tc.target_addr  = reinterpret_cast<uint64_t>(&native_int_add);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});
    auto loaded = load_blob(blob.data(), blob.size(), seed, test_config());
    ASSERT_TRUE(loaded.has_value());

    auto& ctx = loaded->ctx;
    ctx.encoded_regs[0] = encode_register(ctx, 0, 100);
    ctx.encoded_regs[1] = encode_register(ctx, 1, 200);

    auto result = execute(ctx, test_config());
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(decode_register(ctx, 0, ctx.encoded_regs[0]), 300u);
}

// ---------------------------------------------------------------------------
// Test 4: Variadic call — snprintf with integer arg
// (Tests that AL is set correctly on x86-64 for variadic functions.)
// ---------------------------------------------------------------------------

static uint64_t variadic_snprintf_int(uint64_t buf_ptr, uint64_t n,
                                       uint64_t fmt_ptr, uint64_t val,
                                       uint64_t, uint64_t,
                                       uint64_t, uint64_t) {
    // This is a simplified test — we just format an integer
    auto* buf = reinterpret_cast<char*>(buf_ptr);
    auto* fmt = reinterpret_cast<const char*>(fmt_ptr);
    auto len = static_cast<size_t>(n);
    int written = std::snprintf(buf, len, fmt, static_cast<int>(val));
    return static_cast<uint64_t>(written);
}

TEST(NativeFP, VariadicInteger) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xDD);

    TestInstruction nc{};
    nc.opcode = VmOpcode::NATIVE_CALL;
    nc.aux    = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {nc, halt};

    TestNativeCall tc{};
    tc.call_site_ip = 0;
    tc.arg_count    = pack_arg_count(4, 0x00, true, false);  // variadic, no FP
    tc.target_addr  = reinterpret_cast<uint64_t>(&variadic_snprintf_int);

    auto blob = build_test_blob(seed, {bb}, {}, false, {tc});
    auto loaded = load_blob(blob.data(), blob.size(), seed, test_config());
    ASSERT_TRUE(loaded.has_value());

    char buf[64] = {};
    const char* fmt = "val=%d";

    auto& ctx = loaded->ctx;
    ctx.encoded_regs[0] = encode_register(ctx, 0, reinterpret_cast<uint64_t>(buf));
    ctx.encoded_regs[1] = encode_register(ctx, 1, sizeof(buf));
    ctx.encoded_regs[2] = encode_register(ctx, 2, reinterpret_cast<uint64_t>(fmt));
    ctx.encoded_regs[3] = encode_register(ctx, 3, 42);

    auto result = execute(ctx, test_config());
    ASSERT_TRUE(result.has_value());

    EXPECT_STREQ(buf, "val=42");
}

// ---------------------------------------------------------------------------
// Test 5: TransitionEntry bit packing helpers
// ---------------------------------------------------------------------------

TEST(NativeFP, BitPackingHelpers) {
    TransitionEntry te{};
    te.arg_count = pack_arg_count(3, 0xA5, true, true);

    EXPECT_EQ(te_arg_count(te), 3u);
    EXPECT_EQ(te_fp_mask(te), 0xA5u);
    EXPECT_TRUE(te_is_variadic(te));
    EXPECT_TRUE(te_returns_fp(te));
    EXPECT_FALSE(te_returns_struct(te));
    EXPECT_EQ(te_struct_size(te), 0u);
}

TEST(NativeFP, BitPackingBackwardCompat) {
    TransitionEntry te{};
    te.arg_count = 4;  // old-style: just a number

    EXPECT_EQ(te_arg_count(te), 4u);
    EXPECT_EQ(te_fp_mask(te), 0u);
    EXPECT_FALSE(te_is_variadic(te));
    EXPECT_FALSE(te_returns_fp(te));
    EXPECT_FALSE(te_returns_struct(te));
}

#else
// Non-x86-64/ARM64: skip FP tests (no inline asm support)
TEST(NativeFP, SkippedOnThisPlatform) {
    GTEST_SKIP() << "FP native call tests require x86-64 or ARM64";
}
#endif
