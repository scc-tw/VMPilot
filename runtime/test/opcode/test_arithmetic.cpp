/// @file opcode/test_arithmetic.cpp
/// @brief Tests for arithmetic opcodes: ADD, SUB, MUL, DIV, NEG.
///
/// Extracted from test_engine_comprehensive.cpp (ADD, SUB, MUL, DIV tests)
/// plus new edge-case tests for wrap/overflow/identity behavior.

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
static uint8_t none() { return 0; }

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

/// Run a binary arithmetic test: LOAD_CONST r0=a, LOAD_CONST r1=b, <op> r0 r1, HALT
static tl::expected<VmExecResult, DiagnosticCode>
run_binop_test(VmOpcode opcode, uint64_t a, uint64_t b, uint8_t epoch_base = 0x22) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine(seed, epoch_base,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},
         {opcode, rr(), 0, 1, 0},
         {VmOpcode::HALT, none(), 0, 0, 0}},
        {{a, 0, 0}, {b, 0, 1}});
    if (!engine.has_value())
        return tl::make_unexpected(engine.error());
    return engine->execute();
}

/// Run a unary arithmetic test: LOAD_CONST r0=a, <op> r0, HALT
static tl::expected<VmExecResult, DiagnosticCode>
run_unaryop_test(VmOpcode opcode, uint64_t a, uint8_t epoch_base = 0x22) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine(seed, epoch_base,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {opcode, rr(), 0, 0, 0},
         {VmOpcode::HALT, none(), 0, 0, 0}},
        {{a, 0, 0}});
    if (!engine.has_value())
        return tl::make_unexpected(engine.error());
    return engine->execute();
}

// ############################################################################
// Existing tests extracted from test_engine_comprehensive.cpp
// ############################################################################

// ============================================================================
// ADD
// ============================================================================

TEST(Arithmetic, AddDebugPolicy) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine<DebugPolicy>(seed, 0x22,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},
         {VmOpcode::ADD, rr(), 0, 1, 0},
         {VmOpcode::HALT, none(), 0, 0, 0}},
        {{30, 0, 0}, {12, 0, 1}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u);
}

// ============================================================================
// SUB
// ============================================================================

TEST(Arithmetic, SubDebugPolicy) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine<DebugPolicy>(seed, 0x33,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},
         {VmOpcode::SUB, rr(), 0, 1, 0},
         {VmOpcode::HALT, none(), 0, 0, 0}},
        {{100, 0, 0}, {58, 0, 1}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u);
}

// ============================================================================
// MUL
// ============================================================================

TEST(Arithmetic, MulClassC) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine(seed, 0x44,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},
         {VmOpcode::MUL, rr(), 0, 1, 0},
         {VmOpcode::HALT, none(), 0, 0, 0}},
        {{6, 0, 0}, {7, 0, 1}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u);
}

// ============================================================================
// DIV (by zero returns 0)
// ============================================================================

TEST(Arithmetic, DivByZero) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine(seed, 0x55,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},
         {VmOpcode::DIV, rr(), 0, 1, 0},
         {VmOpcode::HALT, none(), 0, 0, 0}},
        {{42, 0, 0}, {0, 0, 1}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0u);  // div by zero returns 0
}

// ############################################################################
// New edge-case tests
// ############################################################################

// ============================================================================
// ADD edge cases
// ============================================================================

TEST(Arithmetic, AddWrap_MaxPlusOne) {
    // UINT64_MAX + 1 wraps to 0
    auto r = run_binop_test(VmOpcode::ADD, UINT64_MAX, 1, 0x30);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0u) << "ADD(UINT64_MAX, 1) should wrap to 0";
}

TEST(Arithmetic, AddZeroZero) {
    // 0 + 0 = 0
    auto r = run_binop_test(VmOpcode::ADD, 0, 0, 0x31);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0u) << "ADD(0, 0) should be 0";
}

// ============================================================================
// SUB edge cases
// ============================================================================

TEST(Arithmetic, SubWrap_ZeroMinusOne) {
    // 0 - 1 wraps to UINT64_MAX
    auto r = run_binop_test(VmOpcode::SUB, 0, 1, 0x32);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, UINT64_MAX) << "SUB(0, 1) should wrap to UINT64_MAX";
}

// ============================================================================
// MUL edge cases
// ============================================================================

TEST(Arithmetic, MulMaxTimesZero) {
    // UINT64_MAX * 0 = 0
    auto r = run_binop_test(VmOpcode::MUL, UINT64_MAX, 0, 0x34);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0u) << "MUL(UINT64_MAX, 0) should be 0";
}

// ============================================================================
// DIV edge cases
// ============================================================================

TEST(Arithmetic, DivZeroByOne) {
    // 0 / 1 = 0
    auto r = run_binop_test(VmOpcode::DIV, 0, 1, 0x35);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0u) << "DIV(0, 1) should be 0";
}

// ============================================================================
// NEG edge cases
// ============================================================================

TEST(Arithmetic, NegZero) {
    // NEG(0) = 0 (two's complement: -0 = 0)
    auto r = run_unaryop_test(VmOpcode::NEG, 0, 0x36);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0u) << "NEG(0) should be 0";
}

TEST(Arithmetic, NegOne) {
    // NEG(1) = UINT64_MAX (two's complement: -1 = 0xFFFFFFFFFFFFFFFF)
    auto r = run_unaryop_test(VmOpcode::NEG, 1, 0x37);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, UINT64_MAX) << "NEG(1) should be UINT64_MAX";
}

// ############################################################################
// IMUL / IDIV / MOD tests
// ############################################################################

// ============================================================================
// IMUL (signed multiply)
// ============================================================================

TEST(Arithmetic, ImulBasic) {
    // IMUL(6, 7) = 42 (signed)
    auto r = run_binop_test(VmOpcode::IMUL, 6, 7, 0x40);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u);
}

TEST(Arithmetic, ImulNegative) {
    // IMUL(-3, 7) = -21 → two's complement 0xFFFFFFFFFFFFFFEB
    auto r = run_binop_test(VmOpcode::IMUL,
        static_cast<uint64_t>(static_cast<int64_t>(-3)),  // 0xFFFFFFFFFFFFFFFD
        7, 0x41);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, static_cast<uint64_t>(static_cast<int64_t>(-21)))
        << "IMUL(-3, 7) should be -21 in two's complement";
}

TEST(Arithmetic, ImulOverflow) {
    // IMUL(INT64_MAX, 2) wraps — signed overflow is technically UB but
    // handler casts through int64_t multiply; on two's complement hardware
    // the expected bit pattern is 0xFFFFFFFFFFFFFFFE.
    auto r = run_binop_test(VmOpcode::IMUL,
        static_cast<uint64_t>(INT64_MAX), 2, 0x42);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, static_cast<uint64_t>(-2))
        << "IMUL(INT64_MAX, 2) should wrap to 0xFFFFFFFFFFFFFFFE";
}

// ============================================================================
// IDIV (signed divide, zero-safe)
// ============================================================================

TEST(Arithmetic, IdivBasic) {
    // IDIV(42, 7) = 6 (signed)
    auto r = run_binop_test(VmOpcode::IDIV, 42, 7, 0x43);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 6u);
}

TEST(Arithmetic, IdivNegative) {
    // IDIV(-42, 7) = -6 → two's complement 0xFFFFFFFFFFFFFFFA
    auto r = run_binop_test(VmOpcode::IDIV,
        static_cast<uint64_t>(static_cast<int64_t>(-42)), 7, 0x44);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, static_cast<uint64_t>(static_cast<int64_t>(-6)))
        << "IDIV(-42, 7) should be -6 in two's complement";
}

TEST(Arithmetic, IdivByZero) {
    // IDIV(42, 0) = 0 (ct_idiv zero-safe)
    auto r = run_binop_test(VmOpcode::IDIV, 42, 0, 0x45);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0u) << "IDIV by zero should return 0 (zero-safe)";
}

// IDIV(INT64_MIN, -1) — signed division overflow, handled by ct_idiv.
// Returns INT64_MIN (two's complement wrap), avoids x86 #DE trap and C++ UB.
TEST(Arithmetic, IdivMinByNegOne) {
    auto r = run_binop_test(VmOpcode::IDIV,
        static_cast<uint64_t>(INT64_MIN),
        static_cast<uint64_t>(static_cast<int64_t>(-1)), 0x46);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, static_cast<uint64_t>(INT64_MIN))
        << "IDIV(INT64_MIN, -1) must return INT64_MIN (two's complement wrap)";
}

// ============================================================================
// MOD (unsigned modulus, zero-safe)
// ============================================================================

TEST(Arithmetic, ModBasic) {
    // MOD(42, 5) = 2
    auto r = run_binop_test(VmOpcode::MOD, 42, 5, 0x47);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 2u);
}

TEST(Arithmetic, ModByZero) {
    // MOD(42, 0) = 0 (ct_mod zero-safe)
    auto r = run_binop_test(VmOpcode::MOD, 42, 0, 0x48);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0u) << "MOD by zero should return 0 (zero-safe)";
}

TEST(Arithmetic, ModIdentity) {
    // MOD(42, 43) = 42 (when a < b, a % b = a)
    auto r = run_binop_test(VmOpcode::MOD, 42, 43, 0x49);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u) << "MOD(42, 43) should be 42";
}

// ============================================================================
// Additional MUL / DIV edge cases
// ============================================================================

TEST(Arithmetic, MulZero) {
    // MUL(anything, 0) = 0 (unsigned)
    auto r = run_binop_test(VmOpcode::MUL, 12345, 0, 0x4A);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0u) << "MUL(anything, 0) should be 0";
}

TEST(Arithmetic, DivSelf) {
    // DIV(42, 42) = 1
    auto r = run_binop_test(VmOpcode::DIV, 42, 42, 0x4B);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 1u) << "DIV(42, 42) should be 1";
}
