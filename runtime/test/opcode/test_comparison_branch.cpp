/// @file opcode/test_comparison_branch.cpp
/// @brief Tests for comparison and branch opcodes: CMP, JCC.
///
/// Combined from:
///   - test_cmp_edge_cases.cpp: CMP 0,0; CMP MAX,MAX; signed overflow; unsigned borrow
///   - test_jcc_conditions.cpp: all 10 JCC conditions (taken and not-taken paths)

#include "test_blob_builder.hpp"

#include "vm_engine.hpp"
#include "vm_state.hpp"

#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <vector>
#include <climits>

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
static uint8_t none() { return 0; }

// ############################################################################
// CMP edge cases (from test_cmp_edge_cases.cpp)
// ############################################################################

/// Run CMP and then GET_FLAG to read the specified flag bit.
/// Returns the value of the flag (0 or 1).
///
/// Program: LOAD_CONST r0=a, LOAD_CONST r1=b, CMP r0 r1, GET_FLAG<bit> r0, HALT
static tl::expected<VmExecResult, DiagnosticCode>
run_cmp_get_flag(uint64_t a, uint64_t b, uint8_t flag_bit) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xF0);

    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = a
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},  // r1 = b
        {VmOpcode::CMP, rr(), 0, 1, 0},
        // GET_FLAG: condition field selects which flag bit to read
        {VmOpcode::GET_FLAG, static_cast<uint8_t>((VM_OPERAND_NONE << 6) | (flag_bit & 0x03)), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{a, 0, 0};
    TestPoolEntry p1{b, 0, 1};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    if (!engine.has_value())
        return tl::make_unexpected(engine.error());
    return engine->execute();
}

// ============================================================================
// CMP 0, 0 -> ZF=1
// ============================================================================

TEST(ComparisonBranch, ZeroZero_ZF) {
    auto r = run_cmp_get_flag(0, 0, 0);  // ZF = bit 0
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 1u) << "CMP 0,0 should set ZF=1";
}

// ============================================================================
// CMP MAX, MAX -> ZF=1
// ============================================================================

TEST(ComparisonBranch, MaxMax_ZF) {
    auto r = run_cmp_get_flag(UINT64_MAX, UINT64_MAX, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 1u) << "CMP MAX,MAX should set ZF=1";
}

// ============================================================================
// Signed overflow detection (OF bit)
//   CMP(INT64_MIN, 1) -> diff = INT64_MIN - 1 = INT64_MAX (overflow)
//   OF should be set because ((sa ^ sb) & (sa ^ diff)) < 0
//   sa = INT64_MIN, sb = 1, diff = INT64_MIN - 1 = INT64_MAX
//   (sa ^ sb) = INT64_MIN ^ 1 = negative (MSB set)
//   (sa ^ diff) = INT64_MIN ^ INT64_MAX = -1 (all bits set, negative)
//   AND -> negative -> OF=1
// ============================================================================

TEST(ComparisonBranch, SignedOverflow_OF) {
    uint64_t int64_min = static_cast<uint64_t>(INT64_MIN);
    auto r = run_cmp_get_flag(int64_min, 1, 3);  // OF = bit 3
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 1u) << "CMP(INT64_MIN, 1) should set OF=1 (signed overflow)";
}

// ============================================================================
// Unsigned borrow (CF bit)
//   CMP(5, 10) -> 5 < 10 unsigned -> CF=1
//   CF is set when ua < ub (unsigned comparison).
// ============================================================================

TEST(ComparisonBranch, UnsignedBorrow_CF) {
    auto r = run_cmp_get_flag(5, 10, 2);  // CF = bit 2
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 1u) << "CMP(5, 10) should set CF=1 (unsigned borrow)";
}

// ############################################################################
// JCC conditions (from test_jcc_conditions.cpp)
// ############################################################################

/// Build a JCC test program:
///   BB1: LOAD_CONST r0 = a, LOAD_CONST r1 = b, CMP r0 r1, JCC cond -> BB2, HALT (fallthrough)
///   BB2: LOAD_CONST r0 = 99, HALT
///
/// If JCC is taken, return_value = 99. If not taken, return_value = a.
static tl::expected<VmExecResult, DiagnosticCode>
run_jcc_test(uint64_t a, uint64_t b, uint8_t cond) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb1{};
    bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0x99);

    bb1.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = a
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},  // r1 = b
        {VmOpcode::CMP, rr(), 0, 1, 0},
        {VmOpcode::JCC, static_cast<uint8_t>((VM_OPERAND_NONE << 6) | cond), 0, 0, 2},  // JCC cond -> BB2
        {VmOpcode::HALT, none(), 0, 0, 0},              // fallthrough
        {VmOpcode::NOP, none(), 0, 0, 0},               // padding: prevent BB boundary transition after HALT
    };

    TestBB bb2{};
    bb2.bb_id = 2; bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF; bb2.flags = 0;
    fill_epoch(bb2.epoch_seed, 0xAA);

    bb2.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 2},  // r0 = 99
        {VmOpcode::HALT, none(), 0, 0, 0},
    };

    TestPoolEntry p0{a, 0, 0};   // BB0 (index 0), reg 0
    TestPoolEntry p1{b, 0, 1};   // BB0 (index 0), reg 1
    TestPoolEntry p2{99, 1, 0};  // BB1 (index 1), reg 0

    auto blob = build_test_blob_ex(seed, {bb1, bb2}, {p0, p1, p2});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    if (!engine.has_value())
        return tl::make_unexpected(engine.error());
    return engine->execute();
}

// ============================================================================
// Condition 0: EQ (ZF=1) -- tests signed equal
// ============================================================================

TEST(ComparisonBranch, EQ_Taken) {
    // CMP 42, 42 -> ZF=1 -> EQ taken
    auto r = run_jcc_test(42, 42, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 99u) << "EQ should be taken when a == b";
}

TEST(ComparisonBranch, EQ_NotTaken) {
    // CMP 10, 20 -> ZF=0 -> EQ not taken
    auto r = run_jcc_test(10, 20, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 10u) << "EQ should not be taken when a != b";
}

// ============================================================================
// Condition 1: NE (!ZF) -- tests signed not equal
// ============================================================================

TEST(ComparisonBranch, NE_Taken) {
    // CMP 10, 20 -> ZF=0 -> NE taken
    auto r = run_jcc_test(10, 20, 1);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 99u) << "NE should be taken when a != b";
}

TEST(ComparisonBranch, NE_NotTaken) {
    // CMP 42, 42 -> ZF=1 -> NE not taken
    auto r = run_jcc_test(42, 42, 1);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u) << "NE should not be taken when a == b";
}

// ============================================================================
// Condition 2: LT (SF != OF) -- signed less than
// ============================================================================

TEST(ComparisonBranch, LT_Taken) {
    // CMP 10, 20 -> 10 < 20 (signed) -> LT taken
    auto r = run_jcc_test(10, 20, 2);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 99u) << "LT should be taken when a < b (signed)";
}

TEST(ComparisonBranch, LT_NotTaken) {
    // CMP 20, 10 -> 20 >= 10 (signed) -> LT not taken
    auto r = run_jcc_test(20, 10, 2);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 20u) << "LT should not be taken when a >= b (signed)";
}

// ============================================================================
// Condition 3: GE (SF == OF) -- signed greater or equal
// ============================================================================

TEST(ComparisonBranch, GE_Taken) {
    // CMP 20, 10 -> 20 >= 10 (signed) -> GE taken
    auto r = run_jcc_test(20, 10, 3);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 99u) << "GE should be taken when a >= b (signed)";
}

TEST(ComparisonBranch, GE_NotTaken) {
    // CMP 10, 20 -> 10 < 20 (signed) -> GE not taken
    auto r = run_jcc_test(10, 20, 3);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 10u) << "GE should not be taken when a < b (signed)";
}

// ============================================================================
// Condition 4: LE (ZF || SF != OF) -- signed less or equal
// ============================================================================

TEST(ComparisonBranch, LE_Taken_Equal) {
    // CMP 42, 42 -> ZF=1 -> LE taken
    auto r = run_jcc_test(42, 42, 4);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 99u) << "LE should be taken when a == b";
}

TEST(ComparisonBranch, LE_Taken_Less) {
    // CMP 10, 20 -> 10 < 20 -> LE taken
    auto r = run_jcc_test(10, 20, 4);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 99u) << "LE should be taken when a < b (signed)";
}

TEST(ComparisonBranch, LE_NotTaken) {
    // CMP 20, 10 -> 20 > 10 -> LE not taken
    auto r = run_jcc_test(20, 10, 4);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 20u) << "LE should not be taken when a > b (signed)";
}

// ============================================================================
// Condition 5: GT (!ZF && SF == OF) -- signed greater than
// ============================================================================

TEST(ComparisonBranch, GT_Taken) {
    // CMP 20, 10 -> 20 > 10 -> GT taken
    auto r = run_jcc_test(20, 10, 5);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 99u) << "GT should be taken when a > b (signed)";
}

TEST(ComparisonBranch, GT_NotTaken_Equal) {
    // CMP 42, 42 -> ZF=1 -> GT not taken
    auto r = run_jcc_test(42, 42, 5);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u) << "GT should not be taken when a == b";
}

TEST(ComparisonBranch, GT_NotTaken_Less) {
    // CMP 10, 20 -> 10 < 20 -> GT not taken
    auto r = run_jcc_test(10, 20, 5);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 10u) << "GT should not be taken when a < b (signed)";
}

// ============================================================================
// Condition 6: B (CF=1) -- unsigned below
// ============================================================================

TEST(ComparisonBranch, B_Taken) {
    // CMP 10, 20 -> 10 < 20 (unsigned) -> CF=1 -> B taken
    auto r = run_jcc_test(10, 20, 6);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 99u) << "B should be taken when a < b (unsigned)";
}

TEST(ComparisonBranch, B_NotTaken) {
    // CMP 20, 10 -> 20 >= 10 (unsigned) -> CF=0 -> B not taken
    auto r = run_jcc_test(20, 10, 6);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 20u) << "B should not be taken when a >= b (unsigned)";
}

// ============================================================================
// Condition 7: AE (!CF) -- unsigned above or equal
// ============================================================================

TEST(ComparisonBranch, AE_Taken) {
    // CMP 20, 10 -> 20 >= 10 (unsigned) -> !CF -> AE taken
    auto r = run_jcc_test(20, 10, 7);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 99u) << "AE should be taken when a >= b (unsigned)";
}

TEST(ComparisonBranch, AE_NotTaken) {
    // CMP 10, 20 -> 10 < 20 (unsigned) -> CF=1 -> AE not taken
    auto r = run_jcc_test(10, 20, 7);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 10u) << "AE should not be taken when a < b (unsigned)";
}

// ============================================================================
// Condition 8: BE (CF || ZF) -- unsigned below or equal
// ============================================================================

TEST(ComparisonBranch, BE_Taken_Below) {
    // CMP 10, 20 -> CF=1 -> BE taken
    auto r = run_jcc_test(10, 20, 8);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 99u) << "BE should be taken when a < b (unsigned)";
}

TEST(ComparisonBranch, BE_Taken_Equal) {
    // CMP 42, 42 -> ZF=1 -> BE taken
    auto r = run_jcc_test(42, 42, 8);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 99u) << "BE should be taken when a == b";
}

TEST(ComparisonBranch, BE_NotTaken) {
    // CMP 20, 10 -> !CF && !ZF -> BE not taken
    auto r = run_jcc_test(20, 10, 8);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 20u) << "BE should not be taken when a > b (unsigned)";
}

// ============================================================================
// Condition 9: A (!CF && !ZF) -- unsigned above
// ============================================================================

TEST(ComparisonBranch, A_Taken) {
    // CMP 20, 10 -> !CF && !ZF -> A taken
    auto r = run_jcc_test(20, 10, 9);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 99u) << "A should be taken when a > b (unsigned)";
}

TEST(ComparisonBranch, A_NotTaken_Equal) {
    // CMP 42, 42 -> ZF=1 -> A not taken
    auto r = run_jcc_test(42, 42, 9);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u) << "A should not be taken when a == b";
}

TEST(ComparisonBranch, A_NotTaken_Below) {
    // CMP 10, 20 -> CF=1 -> A not taken
    auto r = run_jcc_test(10, 20, 9);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 10u) << "A should not be taken when a < b (unsigned)";
}

// ============================================================================
// Conditions 10-13: S, NS, O, NO -- hit default case (always false)
// These conditions are not yet implemented in evaluate_condition.
// JCC is never taken, so they always fall through.
// ============================================================================

TEST(ComparisonBranch, S_AlwaysFallthrough) {
    // Condition 10 (S) always evaluates to false -> never taken
    auto r = run_jcc_test(static_cast<uint64_t>(-1LL), 0, 10);
    ASSERT_TRUE(r.has_value());
    // Should fall through (not taken) since cond 10 is unimplemented
    EXPECT_NE(r->return_value, 99u) << "S (cond 10) should not be taken (unimplemented)";
}

TEST(ComparisonBranch, NS_AlwaysFallthrough) {
    auto r = run_jcc_test(1, 0, 11);
    ASSERT_TRUE(r.has_value());
    EXPECT_NE(r->return_value, 99u) << "NS (cond 11) should not be taken (unimplemented)";
}

TEST(ComparisonBranch, O_AlwaysFallthrough) {
    // Even with signed overflow, cond 12 is unimplemented
    auto r = run_jcc_test(static_cast<uint64_t>(INT64_MAX), static_cast<uint64_t>(-1LL), 12);
    ASSERT_TRUE(r.has_value());
    EXPECT_NE(r->return_value, 99u) << "O (cond 12) should not be taken (unimplemented)";
}

TEST(ComparisonBranch, NO_AlwaysFallthrough) {
    auto r = run_jcc_test(10, 20, 13);
    ASSERT_TRUE(r.has_value());
    EXPECT_NE(r->return_value, 99u) << "NO (cond 13) should not be taken (unimplemented)";
}

// ############################################################################
// EngineHandlers CMP+JCC integration (from test_engine_comprehensive.cpp)
// ############################################################################

template<typename Policy = DebugPolicy, typename Oram = DirectOram>
auto single_bb_engine_cb(const uint8_t seed[32], uint8_t epoch_base,
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

TEST(ComparisonBranch, CmpAndJcc_Integration) {
    uint8_t seed[32]; fill_seed(seed);
    // Program: r0=10, r1=20, CMP r0,r1, JCC LT -> BB2 (r0=99), HALT
    TestBB bb1{}; bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0x99);
    bb1.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 10
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},  // r1 = 20
        {VmOpcode::CMP, rr(), 0, 1, 0},
        {VmOpcode::JCC, none(), 0, 0, 2},  // JCC cond=2 (LT) -> BB 2
        {VmOpcode::HALT, none(), 0, 0, 0}, // fallthrough if not taken
    };

    TestBB bb2{}; bb2.bb_id = 2; bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF; bb2.flags = 0;
    fill_epoch(bb2.epoch_seed, 0xAA);
    bb2.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 2},  // r0 = 99
        {VmOpcode::HALT, none(), 0, 0, 0},
    };

    // JCC condition=2 means LT (signed less than)
    bb1.instructions[3].flags = static_cast<uint8_t>((VM_OPERAND_NONE << 6) | 2);

    TestPoolEntry p0{10, 0, 0};
    TestPoolEntry p1{20, 0, 1};
    TestPoolEntry p2{99, 1, 0};  // BB2, reg 0

    auto blob = build_test_blob_ex(seed, {bb1, bb2}, {p0, p1, p2});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 99u);  // branch was taken (10 < 20)
}

TEST(ComparisonBranch, SetGetFlag) {
    uint8_t seed[32]; fill_seed(seed);
    // SET_FLAG ZF=1, GET_FLAG ZF -> r0
    auto engine = single_bb_engine_cb(seed, 0xAB,
        {{VmOpcode::SET_FLAG, static_cast<uint8_t>(0 | 0), 0, 0, 1},  // ZF = 1
         {VmOpcode::GET_FLAG, static_cast<uint8_t>(0 | 0), 0, 0, 0},  // r0 = ZF
         {VmOpcode::HALT, none(), 0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 1u);
}
