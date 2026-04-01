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
