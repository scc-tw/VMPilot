/// @file test_stack_limits.cpp
/// @brief Tests for stack overflow, underflow, and shadow stack limits.
///
/// Extracted from:
///   - test_push_pop_oram.cpp: StackOverflow, StackUnderflow
///   - test_call_depth.cpp: MaxNestingSucceeds (shadow stack depth 8),
///                          ShadowStackOverflow (9th CALL fails)

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

// ============================================================================
// From test_push_pop_oram.cpp: Stack overflow (ORAM workspace)
// ============================================================================

TEST(StackLimits, StackOverflow) {
    uint8_t seed[32]; fill_seed(seed);

    // Build a minimal program with PUSH
    auto engine = single_bb_engine<DebugPolicy, DirectOram>(seed, 0xA4,
        {{VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
         {VmOpcode::PUSH, rr(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}},
        {{42, 0, 0}});
    ASSERT_TRUE(engine.has_value());

    // Set vm_sp to 0 (stack full) before execution
    engine->execution().vm_sp = 0;

    // Now try to push — should fail with StackOverflow
    // Skip LOAD_CONST by executing one step
    auto step1 = engine->step();
    ASSERT_TRUE(step1.has_value());

    // The next step is PUSH with vm_sp=0 — should fail
    auto step2 = engine->step();
    ASSERT_FALSE(step2.has_value());
    EXPECT_EQ(step2.error(), DiagnosticCode::StackOverflow);
}

// ============================================================================
// From test_push_pop_oram.cpp: Stack underflow (POP on empty stack)
// ============================================================================

TEST(StackLimits, StackUnderflow) {
    uint8_t seed[32]; fill_seed(seed);
    // POP without PUSH → StackUnderflow error
    auto engine = single_bb_engine<DebugPolicy, DirectOram>(seed, 0xA3,
        {{VmOpcode::POP, rr(), 0, 0, 0},
         {VmOpcode::HALT, flags_none(), 0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_FALSE(r.has_value()) << "POP on empty stack should fail";
    EXPECT_EQ(r.error(), DiagnosticCode::StackUnderflow);
}

// ============================================================================
// From test_call_depth.cpp: 8-level nesting (max shadow, should succeed)
// ============================================================================

TEST(StackLimits, ShadowStackMaxDepth8) {
    uint8_t seed[32]; fill_seed(seed);

    // Build 9 BBs: BB1→BB2→...→BB8 (CALL chain, 8 deep), BB9: RET chain
    // BB1: r0=1, CALL→BB2, HALT
    // BB2..BB8: CALL→BB(i+1), RET
    // BB9: RET (deepest)
    std::vector<TestBB> bbs;
    for (int i = 1; i <= 9; ++i) {
        TestBB bb{};
        bb.bb_id = static_cast<uint32_t>(i);
        bb.epoch = 0;
        bb.live_regs_bitmap = 0xFFFF;
        bb.flags = 0;
        fill_epoch(bb.epoch_seed, static_cast<uint8_t>(0xD0 + i));

        if (i == 1) {
            bb.instructions = {
                {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
                {VmOpcode::CALL_VM, flags_none(), 0, 0, 2},
                {VmOpcode::HALT, flags_none(), 0, 0, 0},
            };
        } else if (i <= 8) {
            bb.instructions = {
                {VmOpcode::CALL_VM, flags_none(), 0, 0, static_cast<uint32_t>(i + 1)},
                {VmOpcode::RET_VM, flags_none(), 0, 0, 0},
            };
        } else {
            // BB9: deepest level, just return
            bb.instructions = {
                {VmOpcode::RET_VM, flags_none(), 0, 0, 0},
            };
        }
        bbs.push_back(bb);
    }

    TestPoolEntry p0{77, 0, 0};
    auto blob = build_test_blob_ex(seed, bbs, {p0});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value()) << "8-level nesting should succeed";
    EXPECT_EQ(r->return_value, 77u)
        << "After 8-level CALL/RET, r0 should be restored to original value";
}

// ============================================================================
// From test_call_depth.cpp: Shadow stack overflow (9th CALL exceeds max)
// ============================================================================

TEST(StackLimits, ShadowStackOverflow9) {
    uint8_t seed[32]; fill_seed(seed);

    // Build 9 BBs, each CALL_VM to the next. The 9th CALL should fail.
    // BB1: CALL→BB2
    // BB2: CALL→BB3
    // ...
    // BB9: CALL→BB10 (should fail: shadow_depth=8 >= VM_MAX_NESTING=8)
    // BB10: HALT (never reached)

    std::vector<TestBB> bbs;
    for (int i = 1; i <= 10; ++i) {
        TestBB bb{};
        bb.bb_id = static_cast<uint32_t>(i);
        bb.epoch = 0;
        bb.live_regs_bitmap = 0xFFFF;
        bb.flags = 0;
        fill_epoch(bb.epoch_seed, static_cast<uint8_t>(0xC0 + i));
        if (i <= 9) {
            bb.instructions = {
                {VmOpcode::CALL_VM, flags_none(), 0, 0, static_cast<uint32_t>(i + 1)},
                {VmOpcode::HALT, flags_none(), 0, 0, 0},
            };
        } else {
            bb.instructions = {
                {VmOpcode::HALT, flags_none(), 0, 0, 0},
            };
        }
        bbs.push_back(bb);
    }

    auto blob = build_test_blob(seed, bbs);
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_FALSE(r.has_value()) << "9th CALL should fail with ShadowStackOverflow";
    EXPECT_EQ(r.error(), DiagnosticCode::ShadowStackOverflow);
}
