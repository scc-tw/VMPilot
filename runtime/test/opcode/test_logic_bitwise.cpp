/// @file opcode/test_logic_bitwise.cpp
/// @brief Tests for logic/bitwise opcodes: XOR, AND, OR, NOT, SHL, SHR, SAR, ROL, ROR.
///
/// Combined from:
///   - test_engine_comprehensive.cpp: XOR, AND, OR, NOT tests
///   - test_shift_rotate.cpp: all shift/rotate tests including UB-free regression

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
static uint8_t flags_none() { return 0; }

/// Helper: single-BB engine with given instructions.
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

// ############################################################################
// Logic: XOR, AND, OR, NOT (from test_engine_comprehensive.cpp)
// ############################################################################

TEST(LogicBitwise, XorComputation) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine(seed, 0xCC,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},
         {VmOpcode::XOR, rr(), 0, 1, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{0xFF00FF00FF00FF00ull, 0, 0},
         {0x00FF00FF00FF00FFull, 0, 1}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFFFFFFFFFFull);
}

TEST(LogicBitwise, AndHomomorphic) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine(seed, 0xDD,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},
         {VmOpcode::AND, rr(), 0, 1, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{0xFF00FF00ull, 0, 0}, {0x00FF00FFull, 0, 1}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0u);
}

TEST(LogicBitwise, OrHomomorphic) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine(seed, 0xEE,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},
         {VmOpcode::OR, rr(), 0, 1, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{0xFF000000ull, 0, 0}, {0x00FF0000ull, 0, 1}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFF0000ull);
}

TEST(LogicBitwise, NotBitwise) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine(seed, 0x11,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::NOT, rr(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{0ull, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFFFFFFFFFFull);
}

// ############################################################################
// Shifts & Rotates (from test_shift_rotate.cpp)
// ############################################################################

// ============================================================================
// SHL: logical shift left
// ============================================================================

TEST(LogicBitwise, SHL_Basic) {
    auto r = run_shift_test(VmOpcode::SHL, 1, 4);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 16u) << "1 << 4 = 16";
}

TEST(LogicBitwise, SHL_ByZero) {
    auto r = run_shift_test(VmOpcode::SHL, 0xDEADBEEF, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xDEADBEEFu) << "SHL by 0 should be identity";
}

TEST(LogicBitwise, SHL_By63) {
    auto r = run_shift_test(VmOpcode::SHL, 1, 63);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x8000000000000000ull) << "1 << 63 = MSB set";
}

// ============================================================================
// SHR: logical shift right
// ============================================================================

TEST(LogicBitwise, SHR_Basic) {
    auto r = run_shift_test(VmOpcode::SHR, 0x100, 4);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x10u) << "0x100 >> 4 = 0x10";
}

TEST(LogicBitwise, SHR_ByZero) {
    auto r = run_shift_test(VmOpcode::SHR, 0xDEADBEEF, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xDEADBEEFu) << "SHR by 0 should be identity";
}

TEST(LogicBitwise, SHR_By63) {
    auto r = run_shift_test(VmOpcode::SHR, 0x8000000000000000ull, 63);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 1u) << "MSB >> 63 = 1";
}

// ============================================================================
// SAR: arithmetic shift right (sign-extending)
// ============================================================================

TEST(LogicBitwise, SAR_PositiveBasic) {
    auto r = run_shift_test(VmOpcode::SAR, 0x100, 4);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x10u) << "SAR of positive is same as SHR";
}

TEST(LogicBitwise, SAR_ByZero) {
    auto r = run_shift_test(VmOpcode::SAR, 0xFFFFFFFFFFFFFFFFull, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFFFFFFFFFFull) << "SAR by 0 should be identity";
}

TEST(LogicBitwise, SAR_NegativeBy63) {
    // -1 (all ones) >> 63 = -1 (sign-extends)
    auto r = run_shift_test(VmOpcode::SAR, 0xFFFFFFFFFFFFFFFFull, 63);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFFFFFFFFFFull) << "SAR(-1, 63) should be -1";
}

// ============================================================================
// ROL: rotate left
// ============================================================================

TEST(LogicBitwise, ROL_Basic) {
    auto r = run_shift_test(VmOpcode::ROL, 0x8000000000000001ull, 1);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x0000000000000003ull) << "ROL(MSB|LSB, 1) = 3";
}

TEST(LogicBitwise, ROL_ByZero) {
    auto r = run_shift_test(VmOpcode::ROL, 0xDEADBEEF, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xDEADBEEFu) << "ROL by 0 should be identity";
}

TEST(LogicBitwise, ROL_FullRotation) {
    // ROL by 63 then ROL by 1 = identity. Test ROL by 32 (half rotation).
    auto r = run_shift_test(VmOpcode::ROL, 0x00000001FFFFFFFFull, 32);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFF00000001ull) << "ROL by 32 swaps halves";
}

// ============================================================================
// ROR: rotate right
// ============================================================================

TEST(LogicBitwise, ROR_Basic) {
    auto r = run_shift_test(VmOpcode::ROR, 0x0000000000000003ull, 1);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x8000000000000001ull) << "ROR(3, 1) = MSB|LSB";
}

TEST(LogicBitwise, ROR_ByZero) {
    auto r = run_shift_test(VmOpcode::ROR, 0xDEADBEEF, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xDEADBEEFu) << "ROR by 0 should be identity";
}

TEST(LogicBitwise, ROR_FullRotation) {
    auto r = run_shift_test(VmOpcode::ROR, 0xFFFFFFFF00000001ull, 32);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x00000001FFFFFFFFull) << "ROR by 32 swaps halves";
}

// ============================================================================
// UB-free rotate regression tests.
//
// The naive rotate idiom  (x << n) | (x >> (64 - n))  is undefined behavior
// when n == 0, because C++17 [expr.shift] says shifting a uint64_t by
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
// When n == 0:  (-0) & 63 = 0, so >> 0 -> identity.  No UB.
// When n == 1:  (-1) & 63 = 63, so >> 63 correctly picks up the rotated bit.
//
// These tests exercise amt=0 with various bit patterns to catch any
// regression if the handler is ever reverted to the naive form.
// ============================================================================

TEST(LogicBitwise, ROL_ByZero_AllOnes) {
    auto r = run_shift_test(VmOpcode::ROL, 0xFFFFFFFFFFFFFFFFull, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFFFFFFFFFFull)
        << "ROL(all-ones, 0) must be identity (UB-free rotate regression)";
}

TEST(LogicBitwise, ROL_ByZero_MsbOnly) {
    auto r = run_shift_test(VmOpcode::ROL, 0x8000000000000000ull, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x8000000000000000ull)
        << "ROL(MSB-only, 0) must be identity — the naive (x >> 64) UB "
           "would produce 0 on some compilers";
}

TEST(LogicBitwise, ROL_By64_WrapsToZero) {
    // amt = 64 & 63 = 0, so this is effectively ROL by 0 = identity.
    auto r = run_shift_test(VmOpcode::ROL, 0xCAFEBABE12345678ull, 64);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xCAFEBABE12345678ull)
        << "ROL by 64 masks to 0 (amt & 63), must be identity";
}

TEST(LogicBitwise, ROR_ByZero_AllOnes) {
    auto r = run_shift_test(VmOpcode::ROR, 0xFFFFFFFFFFFFFFFFull, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFFFFFFFFFFull)
        << "ROR(all-ones, 0) must be identity (UB-free rotate regression)";
}

TEST(LogicBitwise, ROR_ByZero_LsbOnly) {
    auto r = run_shift_test(VmOpcode::ROR, 0x0000000000000001ull, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x0000000000000001ull)
        << "ROR(LSB-only, 0) must be identity — the naive (x << 64) UB "
           "would produce 0 on some compilers";
}

TEST(LogicBitwise, ROR_By64_WrapsToZero) {
    // amt = 64 & 63 = 0 -> identity.
    auto r = run_shift_test(VmOpcode::ROR, 0xCAFEBABE12345678ull, 64);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xCAFEBABE12345678ull)
        << "ROR by 64 masks to 0 (amt & 63), must be identity";
}

// Verify the fix doesn't break non-zero rotations (sanity).
TEST(LogicBitwise, ROL_By1_Regression) {
    auto r = run_shift_test(VmOpcode::ROL, 0x0000000000000001ull, 1);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x0000000000000002ull)
        << "ROL(1, 1) = 2";
}

TEST(LogicBitwise, ROR_By1_Regression) {
    auto r = run_shift_test(VmOpcode::ROR, 0x0000000000000002ull, 1);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x0000000000000001ull)
        << "ROR(2, 1) = 1";
}

TEST(LogicBitwise, ROL_ROR_Roundtrip) {
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
