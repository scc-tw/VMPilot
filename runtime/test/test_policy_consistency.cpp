/// @file test_policy_consistency.cpp
/// @brief Cross-policy verification: same program should produce same result
///        across Debug, Standard, and HighSec policies.

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

template<typename Policy>
tl::expected<VmExecResult, DiagnosticCode>
run_add_program(const uint8_t seed[32], uint64_t a, uint64_t b) {
    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xF5);

    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},
        {VmOpcode::ADD, rr(), 0, 1, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{a, 0, 0};
    TestPoolEntry p1{b, 0, 1};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});
    auto engine = VmEngine<Policy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    if (!engine.has_value())
        return tl::make_unexpected(engine.error());
    return engine->execute();
}

// ============================================================================
// Test 1: Debug vs Standard produce same plaintext result
// ============================================================================

TEST(PolicyConsistency, DebugVsStandard) {
    uint8_t seed[32]; fill_seed(seed);

    auto r_debug = run_add_program<DebugPolicy>(seed, 100, 200);
    auto r_standard = run_add_program<StandardPolicy>(seed, 100, 200);

    ASSERT_TRUE(r_debug.has_value()) << "Debug policy ADD should succeed";
    ASSERT_TRUE(r_standard.has_value()) << "Standard policy ADD should succeed";

    EXPECT_EQ(r_debug->return_value, 300u);
    EXPECT_EQ(r_standard->return_value, 300u);
    EXPECT_EQ(r_debug->return_value, r_standard->return_value)
        << "Debug and Standard policies should produce identical plaintext results";
}

// ============================================================================
// Test 2: Debug vs HighSec produce same plaintext result
// ============================================================================

TEST(PolicyConsistency, DebugVsHighSec) {
    uint8_t seed[32]; fill_seed(seed);

    auto r_debug = run_add_program<DebugPolicy>(seed, 0xDEAD, 0xBEEF);
    auto r_highsec = run_add_program<HighSecPolicy>(seed, 0xDEAD, 0xBEEF);

    ASSERT_TRUE(r_debug.has_value()) << "Debug policy ADD should succeed";
    ASSERT_TRUE(r_highsec.has_value()) << "HighSec policy ADD should succeed";

    uint64_t expected = 0xDEAD + 0xBEEF;
    EXPECT_EQ(r_debug->return_value, expected);
    EXPECT_EQ(r_highsec->return_value, expected);
    EXPECT_EQ(r_debug->return_value, r_highsec->return_value)
        << "Debug and HighSec policies should produce identical plaintext results";
}
