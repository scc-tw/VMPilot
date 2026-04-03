/// @file test_shift_rotate.cpp
/// @brief Tests for shift and rotate opcodes: SHL, SHR, SAR, ROL, ROR.
///
/// Each opcode is tested with:
///   - Basic shift amount
///   - Shift by 0 (identity)
///   - Shift by 63 (maximum for 64-bit)
///
/// Note: shift amounts are masked to 6 bits (& 63) by the handlers.

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

static void fill_seed(uint8_t seed[32]) {
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);
}

static void fill_epoch(uint8_t seed[32], uint8_t base) {
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(base + i);
}

static uint8_t pool_none() {
    return static_cast<uint8_t>((VM_OPERAND_POOL << 6) | (VM_OPERAND_NONE << 4));
}
static uint8_t rr() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4));
}
static uint8_t flags_none() { return 0; }

/// Run: LOAD_CONST r0=val, LOAD_CONST r1=amt, <opcode> r0, r1, HALT
static tl::expected<VmExecResult, DiagnosticCode>
run_shift_test(VmOpcode opcode, uint64_t val, uint64_t amt, uint8_t epoch_base = 0x66) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, epoch_base);

    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},
        {opcode, rr(), 0, 1, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{val, 0, 0};
    TestPoolEntry p1{amt, 0, 1};
    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    if (!engine.has_value())
        return tl::make_unexpected(engine.error());
    return engine->execute();
}

// ============================================================================
// SHL: logical shift left
// ============================================================================

TEST(ShiftRotate, SHL_Basic) {
    auto r = run_shift_test(VmOpcode::SHL, 1, 4);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 16u) << "1 << 4 = 16";
}

TEST(ShiftRotate, SHL_ByZero) {
    auto r = run_shift_test(VmOpcode::SHL, 0xDEADBEEF, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xDEADBEEFu) << "SHL by 0 should be identity";
}

TEST(ShiftRotate, SHL_By63) {
    auto r = run_shift_test(VmOpcode::SHL, 1, 63);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x8000000000000000ull) << "1 << 63 = MSB set";
}

// ============================================================================
// SHR: logical shift right
// ============================================================================

TEST(ShiftRotate, SHR_Basic) {
    auto r = run_shift_test(VmOpcode::SHR, 0x100, 4);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x10u) << "0x100 >> 4 = 0x10";
}

TEST(ShiftRotate, SHR_ByZero) {
    auto r = run_shift_test(VmOpcode::SHR, 0xDEADBEEF, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xDEADBEEFu) << "SHR by 0 should be identity";
}

TEST(ShiftRotate, SHR_By63) {
    auto r = run_shift_test(VmOpcode::SHR, 0x8000000000000000ull, 63);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 1u) << "MSB >> 63 = 1";
}

// ============================================================================
// SAR: arithmetic shift right (sign-extending)
// ============================================================================

TEST(ShiftRotate, SAR_PositiveBasic) {
    auto r = run_shift_test(VmOpcode::SAR, 0x100, 4);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x10u) << "SAR of positive is same as SHR";
}

TEST(ShiftRotate, SAR_ByZero) {
    auto r = run_shift_test(VmOpcode::SAR, 0xFFFFFFFFFFFFFFFFull, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFFFFFFFFFFull) << "SAR by 0 should be identity";
}

TEST(ShiftRotate, SAR_NegativeBy63) {
    // -1 (all ones) >> 63 = -1 (sign-extends)
    auto r = run_shift_test(VmOpcode::SAR, 0xFFFFFFFFFFFFFFFFull, 63);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFFFFFFFFFFull) << "SAR(-1, 63) should be -1";
}

// ============================================================================
// ROL: rotate left
// ============================================================================

TEST(ShiftRotate, ROL_Basic) {
    auto r = run_shift_test(VmOpcode::ROL, 0x8000000000000001ull, 1);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x0000000000000003ull) << "ROL(MSB|LSB, 1) = 3";
}

TEST(ShiftRotate, ROL_ByZero) {
    auto r = run_shift_test(VmOpcode::ROL, 0xDEADBEEF, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xDEADBEEFu) << "ROL by 0 should be identity";
}

TEST(ShiftRotate, ROL_FullRotation) {
    // ROL by 63 then ROL by 1 = identity. Test ROL by 32 (half rotation).
    auto r = run_shift_test(VmOpcode::ROL, 0x00000001FFFFFFFFull, 32);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFF00000001ull) << "ROL by 32 swaps halves";
}

// ============================================================================
// ROR: rotate right
// ============================================================================

TEST(ShiftRotate, ROR_Basic) {
    auto r = run_shift_test(VmOpcode::ROR, 0x0000000000000003ull, 1);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x8000000000000001ull) << "ROR(3, 1) = MSB|LSB";
}

TEST(ShiftRotate, ROR_ByZero) {
    auto r = run_shift_test(VmOpcode::ROR, 0xDEADBEEF, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xDEADBEEFu) << "ROR by 0 should be identity";
}

TEST(ShiftRotate, ROR_FullRotation) {
    auto r = run_shift_test(VmOpcode::ROR, 0xFFFFFFFF00000001ull, 32);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x00000001FFFFFFFFull) << "ROR by 32 swaps halves";
}

// ============================================================================
// UB-free rotate regression tests.
//
// The naive rotate idiom  (x << n) | (x >> (64 - n))  is undefined behavior
// when n == 0, because C++17 §8.8 [expr.shift] says shifting a uint64_t by
// >= 64 is UB.  On x86-64, hardware SHR masks the count to 6 bits so
// (x >> 64) happens to produce (x >> 0), making the bug invisible.  But
// compilers are free to exploit UB for optimization: a future optimizer pass
// could assume n != 0 (because the UB path "can't happen") and delete the
// n == 0 code path entirely.
//
// The fix uses the standard constant-time idiom:
//   ROL: (x << n) | (x >> ((-n) & 63))
//   ROR: (x >> n) | (x << ((-n) & 63))
//
// When n == 0:  (-0) & 63 = 0, so >> 0 → identity.  No UB.
// When n == 1:  (-1) & 63 = 63, so >> 63 correctly picks up the rotated bit.
//
// These tests exercise amt=0 with various bit patterns to catch any
// regression if the handler is ever reverted to the naive form.
// ============================================================================

TEST(ShiftRotate, ROL_ByZero_AllOnes) {
    auto r = run_shift_test(VmOpcode::ROL, 0xFFFFFFFFFFFFFFFFull, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFFFFFFFFFFull)
        << "ROL(all-ones, 0) must be identity (UB-free rotate regression)";
}

TEST(ShiftRotate, ROL_ByZero_MsbOnly) {
    auto r = run_shift_test(VmOpcode::ROL, 0x8000000000000000ull, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x8000000000000000ull)
        << "ROL(MSB-only, 0) must be identity — the naive (x >> 64) UB "
           "would produce 0 on some compilers";
}

TEST(ShiftRotate, ROL_By64_WrapsToZero) {
    // amt = 64 & 63 = 0, so this is effectively ROL by 0 = identity.
    auto r = run_shift_test(VmOpcode::ROL, 0xCAFEBABE12345678ull, 64);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xCAFEBABE12345678ull)
        << "ROL by 64 masks to 0 (amt & 63), must be identity";
}

TEST(ShiftRotate, ROR_ByZero_AllOnes) {
    auto r = run_shift_test(VmOpcode::ROR, 0xFFFFFFFFFFFFFFFFull, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFFFFFFFFFFull)
        << "ROR(all-ones, 0) must be identity (UB-free rotate regression)";
}

TEST(ShiftRotate, ROR_ByZero_LsbOnly) {
    auto r = run_shift_test(VmOpcode::ROR, 0x0000000000000001ull, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x0000000000000001ull)
        << "ROR(LSB-only, 0) must be identity — the naive (x << 64) UB "
           "would produce 0 on some compilers";
}

TEST(ShiftRotate, ROR_By64_WrapsToZero) {
    // amt = 64 & 63 = 0 → identity.
    auto r = run_shift_test(VmOpcode::ROR, 0xCAFEBABE12345678ull, 64);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xCAFEBABE12345678ull)
        << "ROR by 64 masks to 0 (amt & 63), must be identity";
}

// Verify the fix doesn't break non-zero rotations (sanity).
TEST(ShiftRotate, ROL_By1_Regression) {
    auto r = run_shift_test(VmOpcode::ROL, 0x0000000000000001ull, 1);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x0000000000000002ull)
        << "ROL(1, 1) = 2";
}

TEST(ShiftRotate, ROR_By1_Regression) {
    auto r = run_shift_test(VmOpcode::ROR, 0x0000000000000002ull, 1);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x0000000000000001ull)
        << "ROR(2, 1) = 1";
}

TEST(ShiftRotate, ROL_ROR_Roundtrip) {
    // ROL by 17 then ROR by 17 = identity.
    // We can only test one op at a time, so verify ROL(x, 17) matches
    // the known value and ROR(that, 17) returns the original.
    const uint64_t original = 0xDEADC0DECAFE1234ull;
    auto r1 = run_shift_test(VmOpcode::ROL, original, 17);
    ASSERT_TRUE(r1.has_value());
    uint64_t rotated = r1->return_value;
    EXPECT_NE(rotated, original) << "sanity: ROL by 17 should change the value";

    auto r2 = run_shift_test(VmOpcode::ROR, rotated, 17);
    ASSERT_TRUE(r2.has_value());
    EXPECT_EQ(r2->return_value, original)
        << "ROL by 17 then ROR by 17 must round-trip to original";
}
