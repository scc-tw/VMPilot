/// @file test_width_extension.cpp
/// @brief Tests for width/extension opcodes: SEXT8/16/32, ZEXT8/16/32, TRUNC8/16.
///
/// Each opcode is tested with a basic value and an edge case.

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

/// Run a single-BB program: LOAD_CONST r0=val, <opcode> r0, HALT
/// Returns the decoded return_value (plaintext of r0).
static tl::expected<VmExecResult, DiagnosticCode>
run_width_test(VmOpcode opcode, uint64_t input_val, uint8_t epoch_base = 0x77) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, epoch_base);

    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
        {opcode, rr(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry pool{input_val, 0, 0};
    auto blob = build_test_blob_ex(seed, {bb}, {pool});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    if (!engine.has_value())
        return tl::make_unexpected(engine.error());
    return engine->execute();
}

// ============================================================================
// SEXT8: sign-extend byte to 64-bit
// ============================================================================

TEST(WidthExtension, SEXT8_Positive) {
    auto r = run_width_test(VmOpcode::SEXT8, 0x7F);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x7Fu) << "SEXT8(0x7F) should be 0x7F (positive)";
}

TEST(WidthExtension, SEXT8_Negative) {
    auto r = run_width_test(VmOpcode::SEXT8, 0x80);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFFFFFFFF80ull)
        << "SEXT8(0x80) should sign-extend to 0xFFFFFFFFFFFFFF80";
}

// ============================================================================
// SEXT16: sign-extend 16-bit to 64-bit
// ============================================================================

TEST(WidthExtension, SEXT16_Positive) {
    auto r = run_width_test(VmOpcode::SEXT16, 0x7FFF);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x7FFFu) << "SEXT16(0x7FFF) should be 0x7FFF";
}

TEST(WidthExtension, SEXT16_Negative) {
    auto r = run_width_test(VmOpcode::SEXT16, 0x8000);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFFFFFF8000ull)
        << "SEXT16(0x8000) should sign-extend to 0xFFFFFFFFFFFF8000";
}

// ============================================================================
// SEXT32: sign-extend 32-bit to 64-bit
// ============================================================================

TEST(WidthExtension, SEXT32_Positive) {
    auto r = run_width_test(VmOpcode::SEXT32, 0x7FFFFFFF);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x7FFFFFFFu) << "SEXT32(0x7FFFFFFF) should be 0x7FFFFFFF";
}

TEST(WidthExtension, SEXT32_Negative) {
    auto r = run_width_test(VmOpcode::SEXT32, 0x80000000);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFF80000000ull)
        << "SEXT32(0x80000000) should sign-extend to 0xFFFFFFFF80000000";
}

// ============================================================================
// ZEXT8: zero-extend byte (mask to 8 bits)
// ============================================================================

TEST(WidthExtension, ZEXT8_Basic) {
    auto r = run_width_test(VmOpcode::ZEXT8, 0xDEADBEEFCAFE00FFull);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFu)
        << "ZEXT8 should mask to low 8 bits";
}

TEST(WidthExtension, ZEXT8_Zero) {
    auto r = run_width_test(VmOpcode::ZEXT8, 0xFF00FF00FF00FF00ull);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0u)
        << "ZEXT8 should mask to low 8 bits (all zero)";
}

// ============================================================================
// ZEXT16: zero-extend 16-bit (mask to 16 bits)
// ============================================================================

TEST(WidthExtension, ZEXT16_Basic) {
    auto r = run_width_test(VmOpcode::ZEXT16, 0xDEADBEEFCAFEBABEull);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xBABEu)
        << "ZEXT16 should mask to low 16 bits";
}

TEST(WidthExtension, ZEXT16_HighBits) {
    auto r = run_width_test(VmOpcode::ZEXT16, 0xFFFFFFFFFFFF1234ull);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x1234u)
        << "ZEXT16 should mask to low 16 bits";
}

// ============================================================================
// ZEXT32: zero-extend 32-bit (mask to 32 bits)
// ============================================================================

TEST(WidthExtension, ZEXT32_Basic) {
    auto r = run_width_test(VmOpcode::ZEXT32, 0xDEADBEEF12345678ull);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x12345678u)
        << "ZEXT32 should mask to low 32 bits";
}

TEST(WidthExtension, ZEXT32_AllOnes) {
    auto r = run_width_test(VmOpcode::ZEXT32, 0xFFFFFFFFFFFFFFFFull);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFFFFFu)
        << "ZEXT32 of all-ones should be 0xFFFFFFFF";
}

// ============================================================================
// TRUNC8: truncate to 8 bits (same as ZEXT8 in implementation)
// ============================================================================

TEST(WidthExtension, TRUNC8_Basic) {
    auto r = run_width_test(VmOpcode::TRUNC8, 0xABCDEF0123456789ull);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x89u)
        << "TRUNC8 should keep only low 8 bits";
}

TEST(WidthExtension, TRUNC8_MaxByte) {
    auto r = run_width_test(VmOpcode::TRUNC8, 0x00000000000000FFull);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFu)
        << "TRUNC8(0xFF) should be 0xFF";
}

// ============================================================================
// TRUNC16: truncate to 16 bits (same as ZEXT16 in implementation)
// ============================================================================

TEST(WidthExtension, TRUNC16_Basic) {
    auto r = run_width_test(VmOpcode::TRUNC16, 0xABCDEF0123456789ull);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0x6789u)
        << "TRUNC16 should keep only low 16 bits";
}

TEST(WidthExtension, TRUNC16_MaxShort) {
    auto r = run_width_test(VmOpcode::TRUNC16, 0x000000000000FFFFull);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0xFFFFu)
        << "TRUNC16(0xFFFF) should be 0xFFFF";
}
