/// @file test_cmp_edge_cases.cpp
/// @brief Tests for CMP flag correctness on edge cases.
///
/// CMP sets flags: ZF (bit 0), SF (bit 1), CF (bit 2), OF (bit 3).
/// Tests: CMP 0,0 (ZF=1), CMP MAX,MAX (ZF=1), signed overflow (OF=1),
/// unsigned borrow (CF=1).

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
// Test 1: CMP 0, 0 → ZF=1
// ============================================================================

TEST(CmpEdgeCases, ZeroZero_ZF) {
    auto r = run_cmp_get_flag(0, 0, 0);  // ZF = bit 0
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 1u) << "CMP 0,0 should set ZF=1";
}

// ============================================================================
// Test 2: CMP MAX, MAX → ZF=1
// ============================================================================

TEST(CmpEdgeCases, MaxMax_ZF) {
    auto r = run_cmp_get_flag(UINT64_MAX, UINT64_MAX, 0);
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 1u) << "CMP MAX,MAX should set ZF=1";
}

// ============================================================================
// Test 3: Signed overflow detection (OF bit)
//   CMP(INT64_MIN, 1) → diff = INT64_MIN - 1 = INT64_MAX (overflow)
//   OF should be set because (INT64_MIN ^ 1) has MSB set and
//   (INT64_MIN ^ diff) also has MSB... let's check the formula.
//
//   CMP sets OF when: ((sa ^ sb) & (sa ^ diff)) < 0
//   sa = INT64_MIN, sb = 1, diff = INT64_MIN - 1 = INT64_MAX
//   (sa ^ sb) = INT64_MIN ^ 1 = negative (MSB set)
//   (sa ^ diff) = INT64_MIN ^ INT64_MAX = -1 (all bits set, negative)
//   AND → negative → OF=1
// ============================================================================

TEST(CmpEdgeCases, SignedOverflow_OF) {
    uint64_t int64_min = static_cast<uint64_t>(INT64_MIN);
    auto r = run_cmp_get_flag(int64_min, 1, 3);  // OF = bit 3
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 1u) << "CMP(INT64_MIN, 1) should set OF=1 (signed overflow)";
}

// ============================================================================
// Test 4: Unsigned borrow (CF bit)
//   CMP(5, 10) → 5 < 10 unsigned → CF=1
//   CF is set when ua < ub (unsigned comparison).
// ============================================================================

TEST(CmpEdgeCases, UnsignedBorrow_CF) {
    auto r = run_cmp_get_flag(5, 10, 2);  // CF = bit 2
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 1u) << "CMP(5, 10) should set CF=1 (unsigned borrow)";
}
