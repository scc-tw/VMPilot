/// @file opcode/test_control_flow.cpp
/// @brief Tests for control flow opcodes: CALL_VM, RET_VM, JMP (implicit), HALT.
///
/// Combined from:
///   - test_call_depth.cpp: 2-level, 3-level, overflow, max depth, register preservation
///   - test_engine_comprehensive.cpp: HALT-only, LOAD_CONST+HALT

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
static uint8_t none() { return 0; }

// ############################################################################
// Basic HALT and LOAD_CONST+HALT (from test_engine_comprehensive.cpp)
// ############################################################################

// ============================================================================
// HALT-only program (minimal valid program)
// ============================================================================

TEST(ControlFlow, HaltOnly) {
    uint8_t seed[32]; fill_seed(seed);
    TestBB bb{}; bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    fill_epoch(bb.epoch_seed, 0xAA);
    bb.instructions = {{VmOpcode::HALT, none(), 0, 0, 0}};

    auto blob = build_test_blob(seed, {bb});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
}

// ============================================================================
// LOAD_CONST + HALT
// ============================================================================

TEST(ControlFlow, LoadConstAndHalt) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xBB);

    bb.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
        {VmOpcode::HALT, none(), 0, 0, 0},
    };

    TestPoolEntry p0{42, 0, 0};
    auto blob = build_test_blob_ex(seed, {bb}, {p0});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u);
}

// ############################################################################
// CALL_VM / RET_VM (from test_call_depth.cpp)
// ############################################################################

// ============================================================================
// Simple 2-level call
//   BB1: r0=42, CALL_VM->BB2, HALT (after return, r0 restored to 42)
//   BB2: r0=99, RET_VM
// ============================================================================

TEST(ControlFlow, TwoLevelCall) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb1{};
    bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0xC0);
    bb1.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 42
        {VmOpcode::CALL_VM, flags_none(), 0, 0, 2},     // CALL -> BB2
        {VmOpcode::HALT, flags_none(), 0, 0, 0},        // after RET, r0=42
    };

    TestBB bb2{};
    bb2.bb_id = 2; bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF; bb2.flags = 0;
    fill_epoch(bb2.epoch_seed, 0xC1);
    bb2.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 99 (overwrite)
        {VmOpcode::RET_VM, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{42, 0, 0};  // BB0, r0
    TestPoolEntry p1{99, 1, 0};  // BB1, r0

    auto blob = build_test_blob_ex(seed, {bb1, bb2}, {p0, p1});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    // After RET_VM, register snapshot is restored -> r0 = 42
    EXPECT_EQ(r->return_value, 42u)
        << "After CALL+RET, r0 should be restored to pre-CALL value";
}

// ============================================================================
// 3-level nested call
//   BB1: r0=10, CALL->BB2, HALT
//   BB2: r0=20, CALL->BB3, RET
//   BB3: r0=30, RET
// ============================================================================

TEST(ControlFlow, ThreeLevelNested) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb1{};
    bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0xC2);
    bb1.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 10
        {VmOpcode::CALL_VM, flags_none(), 0, 0, 2},     // CALL -> BB2
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestBB bb2{};
    bb2.bb_id = 2; bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF; bb2.flags = 0;
    fill_epoch(bb2.epoch_seed, 0xC3);
    bb2.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 20
        {VmOpcode::CALL_VM, flags_none(), 0, 0, 3},     // CALL -> BB3
        {VmOpcode::RET_VM, flags_none(), 0, 0, 0},
    };

    TestBB bb3{};
    bb3.bb_id = 3; bb3.epoch = 0;
    bb3.live_regs_bitmap = 0xFFFF; bb3.flags = 0;
    fill_epoch(bb3.epoch_seed, 0xC4);
    bb3.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 2},  // r0 = 30
        {VmOpcode::RET_VM, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{10, 0, 0};
    TestPoolEntry p1{20, 1, 0};
    TestPoolEntry p2{30, 2, 0};

    auto blob = build_test_blob_ex(seed, {bb1, bb2, bb3}, {p0, p1, p2});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    // BB3 sets r0=30, RET->BB2 restores r0=20, RET->BB1 restores r0=10
    EXPECT_EQ(r->return_value, 10u)
        << "After 3-level nesting, r0 restored to outermost value (10)";
}

// ============================================================================
// Shadow stack overflow (9th CALL exceeds VM_MAX_NESTING=8)
// ============================================================================

TEST(ControlFlow, ShadowStackOverflow) {
    uint8_t seed[32]; fill_seed(seed);

    // Build 9 BBs, each CALL_VM to the next. The 9th CALL should fail.
    // BB1: CALL->BB2
    // BB2: CALL->BB3
    // ...
    // BB9: CALL->BB10 (should fail: shadow_depth=8 >= VM_MAX_NESTING=8)
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

// ============================================================================
// 8-level nesting (max shadow, should succeed)
// ============================================================================

TEST(ControlFlow, MaxNestingSucceeds) {
    uint8_t seed[32]; fill_seed(seed);

    // Build 9 BBs: BB1->BB2->...->BB8 (CALL chain, 8 deep), BB9: RET chain
    // BB1: r0=1, CALL->BB2, HALT
    // BB2..BB8: CALL->BB(i+1), RET
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
// Register preservation across CALL_VM
// ============================================================================

TEST(ControlFlow, RegisterPreservation) {
    uint8_t seed[32]; fill_seed(seed);

    // BB1: r0=10, r1=20, CALL->BB2, r0 should be 10 after return, HALT
    TestBB bb1{};
    bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0xE0);
    bb1.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 10
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},  // r1 = 20
        {VmOpcode::CALL_VM, flags_none(), 0, 0, 2},
        // After RET_VM: r0=10 (restored), r1=20 (restored)
        // Verify by adding r1 to r0: 10+20=30
        {VmOpcode::ADD, rr(), 0, 1, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    // BB2: clobber r0 and r1, then RET
    TestBB bb2{};
    bb2.bb_id = 2; bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF; bb2.flags = 0;
    fill_epoch(bb2.epoch_seed, 0xE1);
    bb2.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 2},  // r0 = 999
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 3},  // r1 = 888
        {VmOpcode::RET_VM, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{10, 0, 0};   // BB0, r0
    TestPoolEntry p1{20, 0, 1};   // BB0, r1
    TestPoolEntry p2{999, 1, 0};  // BB1, r0
    TestPoolEntry p3{888, 1, 1};  // BB1, r1

    auto blob = build_test_blob_ex(seed, {bb1, bb2}, {p0, p1, p2, p3});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    // After RET: r0=10, r1=20, ADD r0 r1 -> r0=30
    EXPECT_EQ(r->return_value, 30u)
        << "Registers should be preserved across CALL_VM/RET_VM";
}

// ############################################################################
// EngineHandlers: NOP and FENCE (from test_engine_comprehensive.cpp)
// ############################################################################

template<typename Policy = DebugPolicy, typename Oram = DirectOram>
auto single_bb_engine_cf(const uint8_t seed[32], uint8_t epoch_base,
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

TEST(ControlFlow, NopWritesTrash) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine_cf(seed, 0xAC,
        {{VmOpcode::LOAD_CONST, pool_none(), 3, 0, 0},
         {VmOpcode::NOP, 0x10, 2, 3, 0},
         {VmOpcode::HALT, none(), 0, 0, 0}},
        {{0xBEEF, 0, 3}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(engine->execution().trash_regs[2], 0xBEEFu);
}

TEST(ControlFlow, FenceDoesNotCrash) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = single_bb_engine_cf(seed, 0xAD,
        {{VmOpcode::FENCE, none(), 0, 0, 0},
         {VmOpcode::HALT, none(), 0, 0, 0}});
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
}

// ############################################################################
// EngineCfg: CALL_VM / RET_VM shadow stack (from test_engine_comprehensive.cpp)
// ############################################################################

TEST(ControlFlow, CallVmRetVm) {
    uint8_t seed[32]; fill_seed(seed);

    // BB1: LOAD_CONST r0=10, CALL_VM -> BB2
    // BB2: LOAD_CONST r0=20, RET_VM (returns to BB1)
    // BB1 continues: HALT

    TestBB bb1{}; bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF;
    fill_epoch(bb1.epoch_seed, 0xE0);
    bb1.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 10
        {VmOpcode::CALL_VM, none(), 0, 0, 2},           // CALL -> bb_id=2
        {VmOpcode::HALT, none(), 0, 0, 0},              // resume after RET
    };

    TestBB bb2{}; bb2.bb_id = 2; bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF;
    fill_epoch(bb2.epoch_seed, 0xE1);
    bb2.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},  // r0 = 20
        {VmOpcode::RET_VM, none(), 0, 0, 0},            // return to BB1
    };

    TestPoolEntry p0{10, 0, 0};   // BB1, reg 0
    TestPoolEntry p1{20, 1, 0};   // BB2, reg 0

    auto blob = build_test_blob_ex(seed, {bb1, bb2}, {p0, p1});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->status, VmResult::Halted);
}
