/// @file test_cfg_patterns.cpp
/// @brief Tests for multi-BB CFG patterns.
///
/// Tests: JMP backward (loop), diamond (if-else merge), switch (multi-way),
/// nested loop, linear chain, JMP forward, and unconditional JMP.

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

// ============================================================================
// Test 1: Unconditional JMP forward (BB1 → BB3, skipping BB2)
// ============================================================================

TEST(CfgPatterns, UnconditionalJmpForward) {
    uint8_t seed[32]; fill_seed(seed);

    // BB1: r0=42, JMP → BB3
    TestBB bb1{};
    bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0xB0);
    bb1.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 42
        {VmOpcode::JMP, flags_none(), 0, 0, 3},         // JMP → BB3
    };

    // BB2: r0=99 (should be skipped)
    TestBB bb2{};
    bb2.bb_id = 2; bb2.epoch = 1;
    bb2.live_regs_bitmap = 0xFFFF; bb2.flags = 0;
    fill_epoch(bb2.epoch_seed, 0xB1);
    bb2.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 1},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    // BB3: HALT (r0 should still be 42)
    TestBB bb3{};
    bb3.bb_id = 3; bb3.epoch = 2;
    bb3.live_regs_bitmap = 0xFFFF; bb3.flags = 0;
    fill_epoch(bb3.epoch_seed, 0xB2);
    bb3.instructions = {
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{42, 0, 0};  // BB index 0, reg 0
    TestPoolEntry p1{99, 1, 0};  // BB index 1, reg 0

    auto blob = build_test_blob_ex(seed, {bb1, bb2, bb3}, {p0, p1});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 42u) << "JMP should skip BB2, r0 stays 42";
}

// ============================================================================
// Test 2: Diamond pattern (if-else merge)
//   BB1: CMP r0, 0; JCC NE → BB3; fallthrough → BB2
//   BB2: r0 = 100; JMP → BB4
//   BB3: r0 = 200; JMP → BB4
//   BB4: HALT
// ============================================================================

TEST(CfgPatterns, DiamondIfElse) {
    uint8_t seed[32]; fill_seed(seed);

    // BB1: r0 = 5 (non-zero), r1 = 0, CMP r0 r1, JCC NE → BB3
    TestBB bb1{};
    bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0xC0);
    bb1.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 5
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},  // r1 = 0
        {VmOpcode::CMP, rr(), 0, 1, 0},
        {VmOpcode::JCC, static_cast<uint8_t>((VM_OPERAND_NONE << 6) | 1), 0, 0, 3},  // NE → BB3
        {VmOpcode::JMP, flags_none(), 0, 0, 2},         // fallthrough → BB2
    };

    // BB2: r0 = 100, JMP → BB4 (else branch)
    TestBB bb2{};
    bb2.bb_id = 2; bb2.epoch = 1;
    bb2.live_regs_bitmap = 0xFFFF; bb2.flags = 0;
    fill_epoch(bb2.epoch_seed, 0xC1);
    bb2.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 2},
        {VmOpcode::JMP, flags_none(), 0, 0, 4},
    };

    // BB3: r0 = 200, JMP → BB4 (then branch)
    TestBB bb3{};
    bb3.bb_id = 3; bb3.epoch = 2;
    bb3.live_regs_bitmap = 0xFFFF; bb3.flags = 0;
    fill_epoch(bb3.epoch_seed, 0xC2);
    bb3.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 3},
        {VmOpcode::JMP, flags_none(), 0, 0, 4},
    };

    // BB4: HALT
    TestBB bb4{};
    bb4.bb_id = 4; bb4.epoch = 3;
    bb4.live_regs_bitmap = 0xFFFF; bb4.flags = 0;
    fill_epoch(bb4.epoch_seed, 0xC3);
    bb4.instructions = {
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{5, 0, 0};    // BB0, r0
    TestPoolEntry p1{0, 0, 1};    // BB0, r1
    TestPoolEntry p2{100, 1, 0};  // BB1, r0
    TestPoolEntry p3{200, 2, 0};  // BB2, r0

    auto blob = build_test_blob_ex(seed, {bb1, bb2, bb3, bb4}, {p0, p1, p2, p3});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    // 5 != 0 → NE taken → BB3 → r0=200
    EXPECT_EQ(r->return_value, 200u) << "Diamond: NE branch should be taken";
}

// ============================================================================
// Test 3: Loop pattern (decrement counter, loop until zero)
//   BB1: r0 = 3, r1 = 1, JMP → BB2
//   BB2: SUB r0, r1; CMP r0, r2(=0); JCC NE → BB2; fallthrough → BB3
//   BB3: HALT (r0 = 0)
// ============================================================================

TEST(CfgPatterns, LoopCountdown) {
    uint8_t seed[32]; fill_seed(seed);

    // BB1: r0 = 3, r1 = 1, r2 = 0, JMP → BB2
    TestBB bb1{};
    bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0xD0);
    bb1.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 3
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},  // r1 = 1
        {VmOpcode::LOAD_CONST, pool_none(), 2, 0, 2},  // r2 = 0
        {VmOpcode::JMP, flags_none(), 0, 0, 2},         // JMP → BB2
    };

    // BB2: SUB r0 r1, CMP r0 r2, JCC NE → BB2 (loop), fallthrough → BB3
    TestBB bb2{};
    bb2.bb_id = 2; bb2.epoch = 1;
    bb2.live_regs_bitmap = 0xFFFF; bb2.flags = 0;
    fill_epoch(bb2.epoch_seed, 0xD1);
    bb2.instructions = {
        {VmOpcode::SUB, rr(), 0, 1, 0},
        {VmOpcode::CMP, rr(), 0, 2, 0},
        {VmOpcode::JCC, static_cast<uint8_t>((VM_OPERAND_NONE << 6) | 1), 0, 0, 2},  // NE → BB2
        {VmOpcode::JMP, flags_none(), 0, 0, 3},  // fallthrough → BB3
    };

    // BB3: HALT
    TestBB bb3{};
    bb3.bb_id = 3; bb3.epoch = 2;
    bb3.live_regs_bitmap = 0xFFFF; bb3.flags = 0;
    fill_epoch(bb3.epoch_seed, 0xD2);
    bb3.instructions = {
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{3, 0, 0};  // BB0, r0
    TestPoolEntry p1{1, 0, 1};  // BB0, r1
    TestPoolEntry p2{0, 0, 2};  // BB0, r2

    auto blob = build_test_blob_ex(seed, {bb1, bb2, bb3}, {p0, p1, p2});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 0u) << "Loop should count down 3→2→1→0";
}

// ============================================================================
// Test 4: Switch (multi-way branch via chained JCC)
//   BB1: r0 = 2, r1 = 1, r2 = 2
//         CMP r0 r1, JCC EQ → BB2
//         CMP r0 r2, JCC EQ → BB3
//         JMP → BB4 (default)
//   BB2: r0 = 10, HALT
//   BB3: r0 = 20, HALT
//   BB4: r0 = 30, HALT (default)
// ============================================================================

TEST(CfgPatterns, SwitchMultiWay) {
    uint8_t seed[32]; fill_seed(seed);

    // BB1: r0=2, r1=1, r2=2, CMP r0 r1, JCC EQ→BB2, CMP r0 r2, JCC EQ→BB3, JMP→BB4
    TestBB bb1{};
    bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0xE0);
    bb1.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 2
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},  // r1 = 1
        {VmOpcode::LOAD_CONST, pool_none(), 2, 0, 2},  // r2 = 2
        {VmOpcode::CMP, rr(), 0, 1, 0},
        {VmOpcode::JCC, static_cast<uint8_t>((VM_OPERAND_NONE << 6) | 0), 0, 0, 2},  // EQ → BB2
        {VmOpcode::CMP, rr(), 0, 2, 0},
        {VmOpcode::JCC, static_cast<uint8_t>((VM_OPERAND_NONE << 6) | 0), 0, 0, 3},  // EQ → BB3
        {VmOpcode::JMP, flags_none(), 0, 0, 4},  // default → BB4
    };

    TestBB bb2{};
    bb2.bb_id = 2; bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF; bb2.flags = 0;
    fill_epoch(bb2.epoch_seed, 0xE1);
    bb2.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 3},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestBB bb3{};
    bb3.bb_id = 3; bb3.epoch = 0;
    bb3.live_regs_bitmap = 0xFFFF; bb3.flags = 0;
    fill_epoch(bb3.epoch_seed, 0xE2);
    bb3.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 4},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestBB bb4{};
    bb4.bb_id = 4; bb4.epoch = 0;
    bb4.live_regs_bitmap = 0xFFFF; bb4.flags = 0;
    fill_epoch(bb4.epoch_seed, 0xE3);
    bb4.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 5},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{2, 0, 0};   // BB0, r0
    TestPoolEntry p1{1, 0, 1};   // BB0, r1
    TestPoolEntry p2{2, 0, 2};   // BB0, r2
    TestPoolEntry p3{10, 1, 0};  // BB1, r0
    TestPoolEntry p4{20, 2, 0};  // BB2, r0
    TestPoolEntry p5{30, 3, 0};  // BB3, r0

    auto blob = build_test_blob_ex(seed, {bb1, bb2, bb3, bb4},
                                   {p0, p1, p2, p3, p4, p5});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    // r0=2, r2=2: first CMP(2,1) → NE, second CMP(2,2) → EQ → BB3 → r0=20
    EXPECT_EQ(r->return_value, 20u) << "Switch: r0=2 should match case 2 → BB3 → 20";
}

// ============================================================================
// Test 5: Linear chain (BB1 → BB2 → BB3 via sequential fallthrough)
// ============================================================================

TEST(CfgPatterns, LinearFallthrough) {
    uint8_t seed[32]; fill_seed(seed);

    // BB1: r0=10, JMP→BB2
    TestBB bb1{};
    bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0xF0);
    bb1.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
        {VmOpcode::JMP, flags_none(), 0, 0, 2},
    };

    // BB2: add 5 to r0, JMP→BB3
    TestBB bb2{};
    bb2.bb_id = 2; bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF; bb2.flags = 0;
    fill_epoch(bb2.epoch_seed, 0xF1);
    bb2.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},  // r1 = 5
        {VmOpcode::ADD, rr(), 0, 1, 0},                 // r0 += r1
        {VmOpcode::JMP, flags_none(), 0, 0, 3},
    };

    // BB3: add 3 to r0, HALT → expect 10+5+3 = 18
    TestBB bb3{};
    bb3.bb_id = 3; bb3.epoch = 0;
    bb3.live_regs_bitmap = 0xFFFF; bb3.flags = 0;
    fill_epoch(bb3.epoch_seed, 0xF2);
    bb3.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 2},  // r1 = 3
        {VmOpcode::ADD, rr(), 0, 1, 0},                 // r0 += r1
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{10, 0, 0};  // BB0, r0
    TestPoolEntry p1{5, 1, 1};   // BB1, r1
    TestPoolEntry p2{3, 2, 1};   // BB2, r1

    auto blob = build_test_blob_ex(seed, {bb1, bb2, bb3}, {p0, p1, p2});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 18u) << "Linear chain: 10 + 5 + 3 = 18";
}

// ============================================================================
// Test 6: Accumulation loop (r0 += r1 in loop, 5 iterations)
//   BB1: r0=0, r1=7, r2=5, r3=1, r4=0
//   BB2: ADD r0 r1, SUB r2 r3, CMP r2 r4, JCC NE→BB2, JMP→BB3
//   BB3: HALT (r0 = 5 * 7 = 35)
// ============================================================================

TEST(CfgPatterns, AccumulationLoop) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb1{};
    bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0xD5);
    bb1.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 0 (accumulator)
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},  // r1 = 7 (addend)
        {VmOpcode::LOAD_CONST, pool_none(), 2, 0, 2},  // r2 = 5 (counter)
        {VmOpcode::LOAD_CONST, pool_none(), 3, 0, 3},  // r3 = 1 (decrement)
        {VmOpcode::LOAD_CONST, pool_none(), 4, 0, 4},  // r4 = 0 (zero)
        {VmOpcode::JMP, flags_none(), 0, 0, 2},
    };

    TestBB bb2{};
    bb2.bb_id = 2; bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF; bb2.flags = 0;
    fill_epoch(bb2.epoch_seed, 0xD6);
    bb2.instructions = {
        {VmOpcode::ADD, rr(), 0, 1, 0},                 // r0 += r1
        {VmOpcode::SUB, rr(), 2, 3, 0},                 // r2 -= r3
        {VmOpcode::CMP, rr(), 2, 4, 0},
        {VmOpcode::JCC, static_cast<uint8_t>((VM_OPERAND_NONE << 6) | 1), 0, 0, 2},  // NE → BB2
        {VmOpcode::JMP, flags_none(), 0, 0, 3},
    };

    TestBB bb3{};
    bb3.bb_id = 3; bb3.epoch = 0;
    bb3.live_regs_bitmap = 0xFFFF; bb3.flags = 0;
    fill_epoch(bb3.epoch_seed, 0xD7);
    bb3.instructions = {
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{0, 0, 0};  // r0 = 0
    TestPoolEntry p1{7, 0, 1};  // r1 = 7
    TestPoolEntry p2{5, 0, 2};  // r2 = 5
    TestPoolEntry p3{1, 0, 3};  // r3 = 1
    TestPoolEntry p4{0, 0, 4};  // r4 = 0

    auto blob = build_test_blob_ex(seed, {bb1, bb2, bb3},
                                   {p0, p1, p2, p3, p4});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 35u) << "Accumulation: 5 * 7 = 35";
}

// ============================================================================
// Test 7: JMP backward (explicit backward jump)
// ============================================================================

TEST(CfgPatterns, JmpBackward) {
    uint8_t seed[32]; fill_seed(seed);

    // BB1: r0=0, r1=1, JMP→BB2
    TestBB bb1{};
    bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0xD8);
    bb1.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},  // r0 = 0
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},  // r1 = 1
        {VmOpcode::JMP, flags_none(), 0, 0, 2},
    };

    // BB2: ADD r0 r1 (r0++), CMP r0 with 3, JCC LT → BB2 (backward), JMP→BB3
    TestBB bb2{};
    bb2.bb_id = 2; bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF; bb2.flags = 0;
    fill_epoch(bb2.epoch_seed, 0xD9);
    bb2.instructions = {
        {VmOpcode::ADD, rr(), 0, 1, 0},
        {VmOpcode::LOAD_CONST, pool_none(), 2, 0, 2},  // r2 = 3
        {VmOpcode::CMP, rr(), 0, 2, 0},
        {VmOpcode::JCC, static_cast<uint8_t>((VM_OPERAND_NONE << 6) | 2), 0, 0, 2},  // LT → BB2
        {VmOpcode::JMP, flags_none(), 0, 0, 3},
    };

    // BB3: HALT
    TestBB bb3{};
    bb3.bb_id = 3; bb3.epoch = 0;
    bb3.live_regs_bitmap = 0xFFFF; bb3.flags = 0;
    fill_epoch(bb3.epoch_seed, 0xDA);
    bb3.instructions = {
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{0, 0, 0};  // r0 = 0
    TestPoolEntry p1{1, 0, 1};  // r1 = 1
    TestPoolEntry p2{3, 1, 2};  // r2 = 3 (in BB2)

    auto blob = build_test_blob_ex(seed, {bb1, bb2, bb3}, {p0, p1, p2});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    // Loop: 0+1=1 (LT 3→loop), 1+1=2 (LT 3→loop), 2+1=3 (not LT 3→exit)
    EXPECT_EQ(r->return_value, 3u) << "Backward JMP loop: 0→1→2→3 (exit)";
}

// ============================================================================
// Test 8: Self-JMP (JMP to own BB - degenerate loop, need exit condition)
//   Same BB but with a counter that eventually breaks out
// ============================================================================

TEST(CfgPatterns, SelfJmpWithExit) {
    uint8_t seed[32]; fill_seed(seed);

    // BB1: r0=0, r1=1, r2=2, JMP→BB2
    TestBB bb1{};
    bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0xDB);
    bb1.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
        {VmOpcode::LOAD_CONST, pool_none(), 1, 0, 1},
        {VmOpcode::LOAD_CONST, pool_none(), 2, 0, 2},
        {VmOpcode::JMP, flags_none(), 0, 0, 2},
    };

    // BB2: r0 += r1, CMP r0 r2, JCC LT → BB2 (self), JMP→BB3
    TestBB bb2{};
    bb2.bb_id = 2; bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF; bb2.flags = 0;
    fill_epoch(bb2.epoch_seed, 0xDC);
    bb2.instructions = {
        {VmOpcode::ADD, rr(), 0, 1, 0},
        {VmOpcode::CMP, rr(), 0, 2, 0},
        {VmOpcode::JCC, static_cast<uint8_t>((VM_OPERAND_NONE << 6) | 2), 0, 0, 2},
        {VmOpcode::JMP, flags_none(), 0, 0, 3},
    };

    TestBB bb3{};
    bb3.bb_id = 3; bb3.epoch = 0;
    bb3.live_regs_bitmap = 0xFFFF; bb3.flags = 0;
    fill_epoch(bb3.epoch_seed, 0xDD);
    bb3.instructions = {
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry p0{0, 0, 0};
    TestPoolEntry p1{1, 0, 1};
    TestPoolEntry p2{2, 0, 2};

    auto blob = build_test_blob_ex(seed, {bb1, bb2, bb3}, {p0, p1, p2});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    // 0+1=1 (LT 2→self), 1+1=2 (not LT 2→exit)
    EXPECT_EQ(r->return_value, 2u);
}

// ############################################################################
// EngineCrossBB: JMP to second BB (from test_engine_comprehensive.cpp)
// ############################################################################

TEST(CfgPatterns, EngineCrossBB_JmpToSecondBB) {
    uint8_t seed[32]; fill_seed(seed);

    // BB1: JMP -> BB2.  BB2: LOAD_CONST r0=77, HALT
    TestBB bb1{}; bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0xA1);
    bb1.instructions = {
        {VmOpcode::JMP, flags_none(), 0, 0, 2},   // JMP -> bb_id=2
    };

    TestBB bb2{}; bb2.bb_id = 2; bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF; bb2.flags = 0;
    fill_epoch(bb2.epoch_seed, 0xA2);
    bb2.instructions = {
        {VmOpcode::LOAD_CONST, pool_none(), 0, 0, 0},
        {VmOpcode::HALT, flags_none(), 0, 0, 0},
    };

    TestPoolEntry pe{77, 1, 0};  // BB2 index=1, reg 0
    auto blob = build_test_blob_ex(seed, {bb1, bb2}, {pe});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto r = engine->execute();
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->return_value, 77u);
}
