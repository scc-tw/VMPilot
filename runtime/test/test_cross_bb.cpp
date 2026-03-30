/// @file test_cross_bb.cpp
/// @brief Cross-BB encoding consistency tests (Phase 11+).
///
/// Tests that values survive BB transitions correctly, even when the
/// encoding domain changes (epoch change -> RE_TABLE application).
///
/// These tests are critical because the encoding tables are derived
/// per-epoch from epoch_seeds. When execution crosses a BB boundary
/// with a different epoch, all live register values must be re-encoded
/// via RE_TABLE = encode_new . decode_old. Any bug in this pipeline
/// would silently corrupt every register at the transition point.

#include "test_blob_builder.hpp"

#include <dispatcher.hpp>
#include <decoder.hpp>
#include <encoding.hpp>
#include <vm_loader.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_blob.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <cstdint>
#include <vector>

using namespace VMPilot::Common;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Runtime;
using namespace VMPilot::Test;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void fill_test_seed(uint8_t seed[32]) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(i + 1);
}

static void fill_epoch_seed(uint8_t seed[32], uint8_t base) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(base + i);
}

/// Make flags byte from operand types and condition.
static uint8_t make_flags(uint8_t op_a_type, uint8_t op_b_type,
                          uint8_t condition = 0) {
    return static_cast<uint8_t>((op_a_type << 6) | (op_b_type << 4) |
                                (condition & 0x0F));
}

static VmSecurityConfig default_config() {
    VmSecurityConfig cfg;
    cfg.debug_mode = true;  // disable anti-debug for tests
    return cfg;
}

// ---------------------------------------------------------------------------
// Test 1: PushInBB0PopInBB1SameEpoch
//
// Why this matters: PUSH converts register-domain -> memory-domain,
// and POP converts memory-domain -> register-domain. When both BBs
// share the same epoch, the encoding tables are identical, so the
// round-trip should be lossless. This is the baseline correctness
// check for the ORAM stack across BB boundaries.
// ---------------------------------------------------------------------------

TEST(CrossBB, PushInBB0PopInBB1SameEpoch) {
    uint8_t seed[32];
    fill_test_seed(seed);

    // BB0: LOAD_CONST r0, pool[0]; PUSH r0; JMP BB1
    TestBB bb0{};
    bb0.bb_id = 10;
    bb0.epoch = 0;
    bb0.live_regs_bitmap = 0xFFFF;
    bb0.flags = 0;
    fill_epoch_seed(bb0.epoch_seed, 0x10);

    TestInstruction lc{};
    lc.opcode = VmOpcode::LOAD_CONST;
    lc.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc.reg_a  = 0;
    lc.aux    = 0;

    TestInstruction push{};
    push.opcode = VmOpcode::PUSH;
    push.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_NONE);
    push.reg_a  = 0;

    TestInstruction jmp{};
    jmp.opcode = VmOpcode::JMP;
    jmp.aux    = 20;

    bb0.instructions = {lc, push, jmp};

    // BB1: POP r0; HALT (same epoch, same encoding)
    TestBB bb1{};
    bb1.bb_id = 20;
    bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF;
    bb1.flags = 0;
    fill_epoch_seed(bb1.epoch_seed, 0x10);  // same epoch seed

    TestInstruction pop{};
    pop.opcode = VmOpcode::POP;
    pop.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_NONE);
    pop.reg_a  = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb1.instructions = {pop, halt};

    // Pool: 42, encoded for BB0, register 0
    std::vector<TestPoolEntry> pool = {{42, 0, 0}};

    auto blob = build_test_blob_ex(seed, {bb0, bb1}, pool);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value())
        << "load_blob failed: " << static_cast<uint32_t>(loaded.error());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value())
        << "execute failed: " << static_cast<uint32_t>(result.error());
    EXPECT_EQ(*result, VmResult::Halted);

    uint64_t decoded = decode_register(loaded->ctx, 0,
                                        loaded->ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 42u)
        << "PUSH/POP across same-epoch BBs should preserve value 42, got "
        << decoded;
}

// ---------------------------------------------------------------------------
// Test 2: PushInBB0PopInBB1DifferentEpoch
//
// Why this matters: When the epoch changes, register encoding tables
// are re-derived and RE_TABLE is applied. But memory-domain values
// (stored via PUSH) use a GLOBAL encoding (derived from stored_seed,
// not epoch_seed). So PUSH in epoch 0 converts reg_domain_0 -> mem_domain,
// and POP in epoch 1 converts mem_domain -> reg_domain_1. The value
// must survive because the memory domain is global. This test verifies
// that the STORE_TABLE and LOAD_TABLE compositions handle epoch
// transitions correctly.
// ---------------------------------------------------------------------------

TEST(CrossBB, PushInBB0PopInBB1DifferentEpoch) {
    uint8_t seed[32];
    fill_test_seed(seed);

    // BB0 (epoch 0): LOAD_CONST r0, pool[0]; PUSH r0; JMP BB1
    TestBB bb0{};
    bb0.bb_id = 10;
    bb0.epoch = 0;
    bb0.live_regs_bitmap = 0xFFFF;
    bb0.flags = 0;
    fill_epoch_seed(bb0.epoch_seed, 0x10);

    TestInstruction lc{};
    lc.opcode = VmOpcode::LOAD_CONST;
    lc.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc.reg_a  = 0;
    lc.aux    = 0;

    TestInstruction push{};
    push.opcode = VmOpcode::PUSH;
    push.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_NONE);
    push.reg_a  = 0;

    TestInstruction jmp{};
    jmp.opcode = VmOpcode::JMP;
    jmp.aux    = 20;

    bb0.instructions = {lc, push, jmp};

    // BB1 (epoch 1): POP r0; HALT -- different epoch!
    TestBB bb1{};
    bb1.bb_id = 20;
    bb1.epoch = 1;
    bb1.live_regs_bitmap = 0xFFFF;
    bb1.flags = BB_FLAG_EPOCH_CHANGED;
    fill_epoch_seed(bb1.epoch_seed, 0x20);  // different epoch seed!

    TestInstruction pop{};
    pop.opcode = VmOpcode::POP;
    pop.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_NONE);
    pop.reg_a  = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb1.instructions = {pop, halt};

    // Pool: 42, encoded for BB0, register 0
    std::vector<TestPoolEntry> pool = {{42, 0, 0}};

    auto blob = build_test_blob_ex(seed, {bb0, bb1}, pool);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value())
        << "load_blob failed: " << static_cast<uint32_t>(loaded.error());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value())
        << "execute failed: " << static_cast<uint32_t>(result.error());
    EXPECT_EQ(*result, VmResult::Halted);

    uint64_t decoded = decode_register(loaded->ctx, 0,
                                        loaded->ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 42u)
        << "PUSH/POP across epoch boundary should preserve value 42, got "
        << decoded;
}

// ---------------------------------------------------------------------------
// Test 3: RegisterSurvivesEpochChange
//
// Why this matters: When entering a BB with a different epoch,
// enter_basic_block() derives new encoding tables and applies
// RE_TABLE to all live registers. RE_TABLE = encode_new . decode_old
// maps the old-domain encoded value to the new-domain encoded value
// without exposing the plaintext. This test verifies that a register
// value loaded in epoch 0 can be correctly decoded in epoch 1 after
// RE_TABLE application.
// ---------------------------------------------------------------------------

TEST(CrossBB, RegisterSurvivesEpochChange) {
    uint8_t seed[32];
    fill_test_seed(seed);

    // BB0 (epoch 0): LOAD_CONST r0, pool[0]; JMP BB1
    TestBB bb0{};
    bb0.bb_id = 10;
    bb0.epoch = 0;
    bb0.live_regs_bitmap = 0xFFFF;
    bb0.flags = 0;
    fill_epoch_seed(bb0.epoch_seed, 0x10);

    TestInstruction lc{};
    lc.opcode = VmOpcode::LOAD_CONST;
    lc.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc.reg_a  = 0;
    lc.aux    = 0;

    TestInstruction jmp{};
    jmp.opcode = VmOpcode::JMP;
    jmp.aux    = 20;

    bb0.instructions = {lc, jmp};

    // BB1 (epoch 1): HALT -- different epoch triggers RE_TABLE
    TestBB bb1{};
    bb1.bb_id = 20;
    bb1.epoch = 1;
    bb1.live_regs_bitmap = 0xFFFF;
    bb1.flags = BB_FLAG_EPOCH_CHANGED;
    fill_epoch_seed(bb1.epoch_seed, 0x20);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb1.instructions = {halt};

    // Pool: 99, encoded for BB0, register 0
    std::vector<TestPoolEntry> pool = {{99, 0, 0}};

    auto blob = build_test_blob_ex(seed, {bb0, bb1}, pool);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value())
        << "load_blob failed: " << static_cast<uint32_t>(loaded.error());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value())
        << "execute failed: " << static_cast<uint32_t>(result.error());
    EXPECT_EQ(*result, VmResult::Halted);

    // After epoch change, r0 has been re-encoded via RE_TABLE.
    // Decode using BB1's (current) tables should yield the original value.
    uint64_t decoded = decode_register(loaded->ctx, 0,
                                        loaded->ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 99u)
        << "Register value should survive epoch change via RE_TABLE, got "
        << decoded;
}

// ---------------------------------------------------------------------------
// Test 4: LoopReentersSameBB
//
// Why this matters: When a BB branches back to itself (loop), the
// dispatcher calls enter_basic_block with the same bb_id. This must
// reset enc_state and insn_index_in_bb correctly so that instruction
// decryption succeeds on re-entry. Since the epoch doesn't change,
// encoding tables are NOT re-derived -- only the SipHash chain resets.
// A bug in re-entry would cause decryption failures or incorrect
// opcode resolution on the second iteration.
// ---------------------------------------------------------------------------

TEST(CrossBB, LoopReentersSameBB) {
    uint8_t seed[32];
    fill_test_seed(seed);

    // BB0: LOAD_CONST r0, pool[0](=5); LOAD_CONST r1, pool[1](=1);
    //      LOAD_CONST r2, pool[2](=0); JMP BB1
    TestBB bb0{};
    bb0.bb_id = 10;
    bb0.epoch = 0;
    bb0.live_regs_bitmap = 0xFFFF;
    bb0.flags = 0;
    fill_epoch_seed(bb0.epoch_seed, 0x10);

    TestInstruction lc0{};
    lc0.opcode = VmOpcode::LOAD_CONST;
    lc0.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc0.reg_a  = 0;
    lc0.aux    = 0;  // pool[0] = 5

    TestInstruction lc1{};
    lc1.opcode = VmOpcode::LOAD_CONST;
    lc1.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc1.reg_a  = 1;
    lc1.aux    = 1;  // pool[1] = 1

    TestInstruction lc2{};
    lc2.opcode = VmOpcode::LOAD_CONST;
    lc2.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc2.reg_a  = 2;
    lc2.aux    = 2;  // pool[2] = 0

    TestInstruction jmp_to_loop{};
    jmp_to_loop.opcode = VmOpcode::JMP;
    jmp_to_loop.aux    = 20;

    bb0.instructions = {lc0, lc1, lc2, jmp_to_loop};

    // BB1 (loop body): SUB r0, r1; CMP r0, r2; JCC(NE) BB1; JMP BB2
    // Loop: r0 -= 1 until r0 == 0
    TestBB bb1{};
    bb1.bb_id = 20;
    bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF;
    bb1.flags = 0;
    fill_epoch_seed(bb1.epoch_seed, 0x10);  // same epoch

    TestInstruction sub{};
    sub.opcode = VmOpcode::SUB;
    sub.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    sub.reg_a  = 0;
    sub.reg_b  = 1;

    TestInstruction cmp{};
    cmp.opcode = VmOpcode::CMP;
    cmp.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    cmp.reg_a  = 0;
    cmp.reg_b  = 2;

    TestInstruction jcc_ne{};
    jcc_ne.opcode = VmOpcode::JCC;
    jcc_ne.flags  = make_flags(VM_OPERAND_NONE, VM_OPERAND_NONE, 1);  // NE
    jcc_ne.aux    = 20;  // back to BB1 (loop)

    TestInstruction jmp_exit{};
    jmp_exit.opcode = VmOpcode::JMP;
    jmp_exit.aux    = 30;

    bb1.instructions = {sub, cmp, jcc_ne, jmp_exit};

    // BB2: HALT
    TestBB bb2{};
    bb2.bb_id = 30;
    bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF;
    bb2.flags = 0;
    fill_epoch_seed(bb2.epoch_seed, 0x10);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb2.instructions = {halt};

    // Pool: 5 (r0 counter), 1 (r1 decrement), 0 (r2 comparison target)
    std::vector<TestPoolEntry> pool = {
        {5, 0, 0},  // pool[0] -> BB0, r0
        {1, 0, 1},  // pool[1] -> BB0, r1
        {0, 0, 2},  // pool[2] -> BB0, r2
    };

    auto blob = build_test_blob_ex(seed, {bb0, bb1, bb2}, pool);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value())
        << "load_blob failed: " << static_cast<uint32_t>(loaded.error());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value())
        << "execute failed: " << static_cast<uint32_t>(result.error());
    EXPECT_EQ(*result, VmResult::Halted);

    uint64_t decoded = decode_register(loaded->ctx, 0,
                                        loaded->ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 0u)
        << "Loop should count down from 5 to 0, got " << decoded;
}

// ---------------------------------------------------------------------------
// Test 5: DiamondPattern
//
// Why this matters: A diamond CFG (BB0 branches to BB1 or BB2,
// both converge to BB3) tests that the encoding state is consistent
// regardless of which path was taken. This is the most common CFG
// pattern in compiled code (if-else). The join point (BB3) must have
// the same encoding domain for all incoming edges.
// ---------------------------------------------------------------------------

TEST(CrossBB, DiamondPattern) {
    uint8_t seed[32];
    fill_test_seed(seed);

    // BB0: LOAD_CONST r0, pool[0](=10); LOAD_CONST r1, pool[1](=10);
    //      CMP r0, r1; JCC(EQ) BB2; JMP BB1
    TestBB bb0{};
    bb0.bb_id = 100;
    bb0.epoch = 0;
    bb0.live_regs_bitmap = 0xFFFF;
    bb0.flags = 0;
    fill_epoch_seed(bb0.epoch_seed, 0x50);

    TestInstruction lc0{};
    lc0.opcode = VmOpcode::LOAD_CONST;
    lc0.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc0.reg_a  = 0;
    lc0.aux    = 0;  // pool[0] = 10

    TestInstruction lc1{};
    lc1.opcode = VmOpcode::LOAD_CONST;
    lc1.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc1.reg_a  = 1;
    lc1.aux    = 1;  // pool[1] = 10

    TestInstruction cmp{};
    cmp.opcode = VmOpcode::CMP;
    cmp.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    cmp.reg_a  = 0;
    cmp.reg_b  = 1;

    TestInstruction jcc_eq{};
    jcc_eq.opcode = VmOpcode::JCC;
    jcc_eq.flags  = make_flags(VM_OPERAND_NONE, VM_OPERAND_NONE, 0);  // EQ
    jcc_eq.aux    = 300;  // equal path -> BB2

    TestInstruction jmp_ne{};
    jmp_ne.opcode = VmOpcode::JMP;
    jmp_ne.aux    = 200;  // not-equal path -> BB1

    bb0.instructions = {lc0, lc1, cmp, jcc_eq, jmp_ne};

    // BB1 (not-equal path): LOAD_CONST r2, pool[2](=0); JMP BB3
    TestBB bb1{};
    bb1.bb_id = 200;
    bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF;
    bb1.flags = 0;
    fill_epoch_seed(bb1.epoch_seed, 0x50);

    TestInstruction lc_ne{};
    lc_ne.opcode = VmOpcode::LOAD_CONST;
    lc_ne.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc_ne.reg_a  = 2;
    lc_ne.aux    = 2;  // pool[2] = 0

    TestInstruction jmp_merge1{};
    jmp_merge1.opcode = VmOpcode::JMP;
    jmp_merge1.aux    = 400;

    bb1.instructions = {lc_ne, jmp_merge1};

    // BB2 (equal path): LOAD_CONST r2, pool[3](=1); JMP BB3
    TestBB bb2{};
    bb2.bb_id = 300;
    bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF;
    bb2.flags = 0;
    fill_epoch_seed(bb2.epoch_seed, 0x50);

    TestInstruction lc_eq{};
    lc_eq.opcode = VmOpcode::LOAD_CONST;
    lc_eq.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc_eq.reg_a  = 2;
    lc_eq.aux    = 3;  // pool[3] = 1

    TestInstruction jmp_merge2{};
    jmp_merge2.opcode = VmOpcode::JMP;
    jmp_merge2.aux    = 400;

    bb2.instructions = {lc_eq, jmp_merge2};

    // BB3 (merge): HALT
    TestBB bb3{};
    bb3.bb_id = 400;
    bb3.epoch = 0;
    bb3.live_regs_bitmap = 0xFFFF;
    bb3.flags = 0;
    fill_epoch_seed(bb3.epoch_seed, 0x50);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb3.instructions = {halt};

    // Pool: r0=10, r1=10, not-equal result=0, equal result=1
    std::vector<TestPoolEntry> pool = {
        {10, 0, 0},   // pool[0] -> BB0, r0
        {10, 0, 1},   // pool[1] -> BB0, r1
        {0,  1, 2},   // pool[2] -> BB1, r2 (not-equal path)
        {1,  2, 2},   // pool[3] -> BB2, r2 (equal path)
    };

    auto blob = build_test_blob_ex(seed, {bb0, bb1, bb2, bb3}, pool);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value())
        << "load_blob failed: " << static_cast<uint32_t>(loaded.error());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value())
        << "execute failed: " << static_cast<uint32_t>(result.error());
    EXPECT_EQ(*result, VmResult::Halted);

    // 10 == 10, so JCC(EQ) should be taken -> BB2 -> r2 = 1
    uint64_t decoded = decode_register(loaded->ctx, 2,
                                        loaded->ctx.encoded_regs[2]);
    EXPECT_EQ(decoded, 1u)
        << "Diamond: r0==r1, expected r2=1 (equal path), got " << decoded;
}
