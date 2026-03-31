/// @file test_phase4_engine.cpp
/// @brief Integration tests for VmEngine<Policy, Oram>.
///
/// Validates the full create→execute pipeline:
///   1. VmEngine::create() from test blob → success
///   2. Simple HALT program → VmResult::Halted
///   3. LOAD_CONST + HALT → correct return value
///   4. XOR program → correct computation
///   5. create_reentrant() shares VmImmutable
///   6. All 3 policies produce identical results for same blob

#include "test_blob_builder.hpp"

#include "vm_engine.hpp"

#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_context.hpp>

#include <gtest/gtest.h>

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

static uint8_t flags_pool_none(uint8_t cond = 0) {
    return static_cast<uint8_t>((VM_OPERAND_POOL << 6) | (VM_OPERAND_NONE << 4) | cond);
}

static uint8_t flags_rr(uint8_t cond = 0) {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4) | cond);
}

static uint8_t flags_none() { return 0; }

// ============================================================================
// Test 1: HALT-only program
// ============================================================================

TEST(Phase4Engine, HaltOnly) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xAA);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags = flags_none();
    bb.instructions = {halt};

    auto blob = build_test_blob(seed, {bb});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value()) << "create() failed";

    auto result = engine->execute();
    ASSERT_TRUE(result.has_value()) << "execute() failed";
    EXPECT_EQ(result->status, VmResult::Halted);
}

// ============================================================================
// Test 2: LOAD_CONST r0, 42; HALT → return value == 42
// ============================================================================

TEST(Phase4Engine, LoadConstAndHalt) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xBB);

    // LOAD_CONST r0, pool[0]  (pool[0] = 42, encoded for r0 in BB 0)
    TestInstruction lc{};
    lc.opcode = VmOpcode::LOAD_CONST;
    lc.flags = flags_pool_none();
    lc.reg_a = 0; lc.reg_b = 0;
    lc.aux = 0;  // pool index 0

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags = flags_none();
    bb.instructions = {lc, halt};

    // Pool entry: value 42, encoded for register 0 in BB index 0
    TestPoolEntry pool_entry{};
    pool_entry.plaintext = 42;
    pool_entry.target_bb_index = 0;
    pool_entry.target_reg = 0;

    auto blob = build_test_blob_ex(seed, {bb}, {pool_entry});

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value()) << "create() failed";

    auto result = engine->execute();
    ASSERT_TRUE(result.has_value()) << "execute() failed";
    EXPECT_EQ(result->status, VmResult::Halted);
    EXPECT_EQ(result->return_value, 42u);
}

// ============================================================================
// Test 3: XOR computation
//   LOAD_CONST r0, 0xFF00FF00FF00FF00
//   LOAD_CONST r1, 0x00FF00FF00FF00FF
//   XOR r0, r1   → r0 = 0xFFFFFFFFFFFFFFFF
//   HALT
// ============================================================================

TEST(Phase4Engine, XorComputation) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xCC);

    TestInstruction lc0{};
    lc0.opcode = VmOpcode::LOAD_CONST;
    lc0.flags = flags_pool_none();
    lc0.reg_a = 0; lc0.reg_b = 0;
    lc0.aux = 0;

    TestInstruction lc1{};
    lc1.opcode = VmOpcode::LOAD_CONST;
    lc1.flags = flags_pool_none();
    lc1.reg_a = 1; lc1.reg_b = 0;
    lc1.aux = 1;

    TestInstruction xor_insn{};
    xor_insn.opcode = VmOpcode::XOR;
    xor_insn.flags = flags_rr();
    xor_insn.reg_a = 0; xor_insn.reg_b = 1;
    xor_insn.aux = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags = flags_none();

    bb.instructions = {lc0, lc1, xor_insn, halt};

    TestPoolEntry p0{0xFF00FF00FF00FF00ull, 0, 0};
    TestPoolEntry p1{0x00FF00FF00FF00FFull, 0, 1};

    auto blob = build_test_blob_ex(seed, {bb}, {p0, p1});

    auto engine = VmEngine<HighSecPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value()) << "create() failed";

    auto result = engine->execute();
    ASSERT_TRUE(result.has_value()) << "execute() failed";
    EXPECT_EQ(result->status, VmResult::Halted);
    EXPECT_EQ(result->return_value, 0xFFFFFFFFFFFFFFFFull);
}

// ============================================================================
// Test 4: create_reentrant shares VmImmutable
// ============================================================================

TEST(Phase4Engine, ReentrantSharesImmutable) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xDD);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags = flags_none();
    bb.instructions = {halt};

    auto blob = build_test_blob(seed, {bb});

    auto engine = VmEngine<StandardPolicy>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    auto shared_imm = engine->shared_immutable();
    EXPECT_EQ(shared_imm.use_count(), 2);  // engine + our copy

    auto inner = VmEngine<StandardPolicy>::create_reentrant(shared_imm);
    ASSERT_TRUE(inner.has_value());

    EXPECT_EQ(shared_imm.use_count(), 3);  // engine + our copy + inner

    // Both should execute successfully
    auto r1 = engine->execute();
    ASSERT_TRUE(r1.has_value());
    EXPECT_EQ(r1->status, VmResult::Halted);

    auto r2 = inner->execute();
    ASSERT_TRUE(r2.has_value());
    EXPECT_EQ(r2->status, VmResult::Halted);
}

// ============================================================================
// Test 5: All 3 policies produce same result for same blob
// ============================================================================

TEST(Phase4Engine, CrossPolicyConsistency) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0xEE);

    TestInstruction lc{};
    lc.opcode = VmOpcode::LOAD_CONST;
    lc.flags = flags_pool_none();
    lc.reg_a = 0; lc.reg_b = 0;
    lc.aux = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags = flags_none();
    bb.instructions = {lc, halt};

    TestPoolEntry pe{12345u, 0, 0};
    auto blob = build_test_blob_ex(seed, {bb}, {pe});

    // Execute with all 3 policies
    auto r1 = VmEngine<HighSecPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed)->execute();
    auto r2 = VmEngine<StandardPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed)->execute();
    auto r3 = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed)->execute();

    ASSERT_TRUE(r1.has_value());
    ASSERT_TRUE(r2.has_value());
    ASSERT_TRUE(r3.has_value());

    // All must produce the same return value
    EXPECT_EQ(r1->return_value, 12345u);
    EXPECT_EQ(r2->return_value, 12345u);
    EXPECT_EQ(r3->return_value, 12345u);
}
