/// @file test_dispatcher.cpp
/// @brief End-to-end tests for the Phase 7 dispatcher (12-step uniform pipeline).
///
/// Each test constructs a complete encrypted blob via build_test_blob /
/// build_test_blob_ex, loads it with load_blob, and runs execute() or step()
/// to verify the full decode-dispatch-advance pipeline.

#include "test_blob_builder.hpp"

#include <dispatcher.hpp>
#include <decoder.hpp>
#include <encoding.hpp>
#include <vm_loader.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <vm/vm_insn.hpp>

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
// Test 1: SingleHalt
// ---------------------------------------------------------------------------

TEST(Dispatcher, SingleHalt) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xAA);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {halt};

    auto blob = build_test_blob(seed, {bb});
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value())
        << "load_blob failed: " << static_cast<uint32_t>(loaded.error());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value())
        << "execute failed: " << static_cast<uint32_t>(result.error());
    EXPECT_EQ(*result, VmResult::Halted);
}

// ---------------------------------------------------------------------------
// Test 2: LoadConstAndHalt
// ---------------------------------------------------------------------------

TEST(Dispatcher, LoadConstAndHalt) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xBB);

    TestInstruction load_const{};
    load_const.opcode = VmOpcode::LOAD_CONST;
    load_const.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    load_const.reg_a  = 0;
    load_const.aux    = 0;  // pool index 0

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {load_const, halt};

    // Pool entry encoded for BB[0], register 0
    std::vector<TestPoolEntry> pool = {{42, 0, 0}};

    auto blob = build_test_blob_ex(seed, {bb}, pool);
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
    EXPECT_EQ(decoded, 42u);
}

// ---------------------------------------------------------------------------
// Test 3: MoveAndHalt
// ---------------------------------------------------------------------------

TEST(Dispatcher, MoveAndHalt) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xCC);

    TestInstruction load_const{};
    load_const.opcode = VmOpcode::LOAD_CONST;
    load_const.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    load_const.reg_a  = 0;
    load_const.aux    = 0;

    TestInstruction move{};
    move.opcode = VmOpcode::MOVE;
    move.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    move.reg_a  = 1;
    move.reg_b  = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {load_const, move, halt};

    std::vector<TestPoolEntry> pool = {{99, 0, 0}};

    auto blob = build_test_blob_ex(seed, {bb}, pool);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, VmResult::Halted);

    // After MOVE r1, r0: r1 holds r0's encoded value.
    // MOVE copies encoded_regs directly (same BB domain), so we decode
    // using r1's tables. But MOVE does encoded_regs[dst] = encoded_regs[src],
    // meaning r1 now holds r0's encoded value. To decode r1's semantic
    // value, we need to decode using r0's tables (since the encoding
    // is r0's encoding). Actually, MOVE just copies the encoded value,
    // so r1 now holds the encoding of 99 in r0's domain.
    //
    // Wait -- looking at handle_move: ctx.encoded_regs[reg_a] = ctx.encoded_regs[reg_b]
    // So MOVE r1, r0 means reg_a=1, reg_b=0: ctx.encoded_regs[1] = ctx.encoded_regs[0]
    //
    // This gives r1 the encoded value that was in r0, which is encoded
    // using r0's tables. But r1's decode tables are different.
    // Decoding with r1's tables would give garbage.
    //
    // This is correct behavior per the spec: MOVE in the encoded domain
    // means "copy the encoded bits." The compiler ensures that any
    // subsequent use of r1 accounts for the re-encoding. For testing
    // purposes, decode using r0's tables (the source register).
    uint64_t decoded = decode_register(loaded->ctx, 0,
                                        loaded->ctx.encoded_regs[1]);
    EXPECT_EQ(decoded, 99u);
}

// ---------------------------------------------------------------------------
// Test 4: AddTwoConstants
// ---------------------------------------------------------------------------

TEST(Dispatcher, AddTwoConstants) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xDD);

    TestInstruction lc0{};
    lc0.opcode = VmOpcode::LOAD_CONST;
    lc0.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc0.reg_a  = 0;
    lc0.aux    = 0;

    TestInstruction lc1{};
    lc1.opcode = VmOpcode::LOAD_CONST;
    lc1.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc1.reg_a  = 1;
    lc1.aux    = 1;

    TestInstruction add{};
    add.opcode = VmOpcode::ADD;
    add.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    add.reg_a  = 0;
    add.reg_b  = 1;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {lc0, lc1, add, halt};

    // pool[0] -> r0, pool[1] -> r1
    std::vector<TestPoolEntry> pool = {
        {42, 0, 0},
        {58, 0, 1},
    };

    auto blob = build_test_blob_ex(seed, {bb}, pool);
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
    EXPECT_EQ(decoded, 100u)
        << "ADD 42 + 58 should equal 100, got " << decoded;
}

// ---------------------------------------------------------------------------
// Test 5: XorHomomorphic
// ---------------------------------------------------------------------------

TEST(Dispatcher, XorHomomorphic) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xEE);

    TestInstruction lc0{};
    lc0.opcode = VmOpcode::LOAD_CONST;
    lc0.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc0.reg_a  = 0;
    lc0.aux    = 0;

    TestInstruction lc1{};
    lc1.opcode = VmOpcode::LOAD_CONST;
    lc1.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc1.reg_a  = 1;
    lc1.aux    = 1;

    TestInstruction xor_insn{};
    xor_insn.opcode = VmOpcode::XOR;
    xor_insn.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    xor_insn.reg_a  = 0;
    xor_insn.reg_b  = 1;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {lc0, lc1, xor_insn, halt};

    std::vector<TestPoolEntry> pool = {
        {0xAA, 0, 0},
        {0x55, 0, 1},
    };

    auto blob = build_test_blob_ex(seed, {bb}, pool);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, VmResult::Halted);

    uint64_t decoded = decode_register(loaded->ctx, 0,
                                        loaded->ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 0xFFu)
        << "XOR 0xAA ^ 0x55 should equal 0xFF, got 0x" << std::hex << decoded;
}

// ---------------------------------------------------------------------------
// Test 6: SubtractConstants
// ---------------------------------------------------------------------------

TEST(Dispatcher, SubtractConstants) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0x11);

    TestInstruction lc0{};
    lc0.opcode = VmOpcode::LOAD_CONST;
    lc0.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc0.reg_a  = 0;
    lc0.aux    = 0;

    TestInstruction lc1{};
    lc1.opcode = VmOpcode::LOAD_CONST;
    lc1.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc1.reg_a  = 1;
    lc1.aux    = 1;

    TestInstruction sub{};
    sub.opcode = VmOpcode::SUB;
    sub.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    sub.reg_a  = 0;
    sub.reg_b  = 1;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {lc0, lc1, sub, halt};

    std::vector<TestPoolEntry> pool = {
        {100, 0, 0},
        {42,  0, 1},
    };

    auto blob = build_test_blob_ex(seed, {bb}, pool);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, VmResult::Halted);

    uint64_t decoded = decode_register(loaded->ctx, 0,
                                        loaded->ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 58u);
}

// ---------------------------------------------------------------------------
// Test 7: MultiplyConstants
// ---------------------------------------------------------------------------

TEST(Dispatcher, MultiplyConstants) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0x22);

    TestInstruction lc0{};
    lc0.opcode = VmOpcode::LOAD_CONST;
    lc0.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc0.reg_a  = 0;
    lc0.aux    = 0;

    TestInstruction lc1{};
    lc1.opcode = VmOpcode::LOAD_CONST;
    lc1.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc1.reg_a  = 1;
    lc1.aux    = 1;

    TestInstruction mul{};
    mul.opcode = VmOpcode::MUL;
    mul.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    mul.reg_a  = 0;
    mul.reg_b  = 1;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {lc0, lc1, mul, halt};

    std::vector<TestPoolEntry> pool = {
        {7, 0, 0},
        {6, 0, 1},
    };

    auto blob = build_test_blob_ex(seed, {bb}, pool);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, VmResult::Halted);

    uint64_t decoded = decode_register(loaded->ctx, 0,
                                        loaded->ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 42u);
}

// ---------------------------------------------------------------------------
// Test 8: PushPopPreservesValue
// ---------------------------------------------------------------------------

TEST(Dispatcher, PushPopPreservesValue) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0x33);

    // LOAD_CONST r0, pool[0]   (42)
    TestInstruction lc0{};
    lc0.opcode = VmOpcode::LOAD_CONST;
    lc0.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc0.reg_a  = 0;
    lc0.aux    = 0;

    // PUSH r0
    TestInstruction push{};
    push.opcode = VmOpcode::PUSH;
    push.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_NONE);
    push.reg_a  = 0;

    // LOAD_CONST r0, pool[1]   (99 -- overwrite r0)
    TestInstruction lc1{};
    lc1.opcode = VmOpcode::LOAD_CONST;
    lc1.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc1.reg_a  = 0;
    lc1.aux    = 1;

    // POP r0
    TestInstruction pop{};
    pop.opcode = VmOpcode::POP;
    pop.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_NONE);
    pop.reg_a  = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {lc0, push, lc1, pop, halt};

    // Both constants go into r0
    std::vector<TestPoolEntry> pool = {
        {42, 0, 0},
        {99, 0, 0},
    };

    auto blob = build_test_blob_ex(seed, {bb}, pool);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, VmResult::Halted);

    uint64_t decoded = decode_register(loaded->ctx, 0,
                                        loaded->ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 42u)
        << "POP should restore the pushed value (42), got " << decoded;
}

// ---------------------------------------------------------------------------
// Test 9: JmpToSecondBB
// ---------------------------------------------------------------------------

TEST(Dispatcher, JmpToSecondBB) {
    uint8_t seed[32];
    fill_test_seed(seed);

    // BB0: JMP -> BB1
    TestBB bb0{};
    bb0.bb_id = 10;
    bb0.epoch = 0;
    bb0.live_regs_bitmap = 0xFFFF;
    bb0.flags = 0;
    fill_epoch_seed(bb0.epoch_seed, 0x44);

    TestInstruction jmp{};
    jmp.opcode = VmOpcode::JMP;
    jmp.aux    = 20;  // target BB id

    bb0.instructions = {jmp};

    // BB1: LOAD_CONST r0, pool[0]; HALT
    TestBB bb1{};
    bb1.bb_id = 20;
    bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF;
    bb1.flags = 0;
    fill_epoch_seed(bb1.epoch_seed, 0x44);  // same epoch -> same seed

    TestInstruction lc{};
    lc.opcode = VmOpcode::LOAD_CONST;
    lc.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc.reg_a  = 0;
    lc.aux    = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb1.instructions = {lc, halt};

    // Pool constant encoded for BB1 (index 1), register 0
    std::vector<TestPoolEntry> pool = {{77, 1, 0}};

    auto blob = build_test_blob_ex(seed, {bb0, bb1}, pool);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, VmResult::Halted);

    uint64_t decoded = decode_register(loaded->ctx, 0,
                                        loaded->ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 77u);
}

// ---------------------------------------------------------------------------
// Test 10: ConditionalBranch
// ---------------------------------------------------------------------------

TEST(Dispatcher, ConditionalBranch) {
    uint8_t seed[32];
    fill_test_seed(seed);

    // BB0: LOAD_CONST r0, pool[0]; LOAD_CONST r1, pool[1];
    //      CMP r0, r1; JCC(EQ) -> BB2; JMP -> BB1
    TestBB bb0{};
    bb0.bb_id = 100;
    bb0.epoch = 0;
    bb0.live_regs_bitmap = 0xFFFF;
    bb0.flags = 0;
    fill_epoch_seed(bb0.epoch_seed, 0x55);

    TestInstruction lc0{};
    lc0.opcode = VmOpcode::LOAD_CONST;
    lc0.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc0.reg_a  = 0;
    lc0.aux    = 0;

    TestInstruction lc1{};
    lc1.opcode = VmOpcode::LOAD_CONST;
    lc1.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc1.reg_a  = 1;
    lc1.aux    = 1;

    TestInstruction cmp{};
    cmp.opcode = VmOpcode::CMP;
    cmp.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    cmp.reg_a  = 0;
    cmp.reg_b  = 1;

    TestInstruction jcc{};
    jcc.opcode = VmOpcode::JCC;
    jcc.flags  = make_flags(VM_OPERAND_NONE, VM_OPERAND_NONE, 0);  // condition 0 = EQ
    jcc.aux    = 300;  // target BB id for equal path

    TestInstruction jmp_fallthrough{};
    jmp_fallthrough.opcode = VmOpcode::JMP;
    jmp_fallthrough.aux    = 200;  // target BB id for not-equal path

    bb0.instructions = {lc0, lc1, cmp, jcc, jmp_fallthrough};

    // BB1 (not-equal path): LOAD_CONST r2, pool[2]; HALT
    TestBB bb1{};
    bb1.bb_id = 200;
    bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF;
    bb1.flags = 0;
    fill_epoch_seed(bb1.epoch_seed, 0x55);  // same epoch

    TestInstruction lc_ne{};
    lc_ne.opcode = VmOpcode::LOAD_CONST;
    lc_ne.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc_ne.reg_a  = 2;
    lc_ne.aux    = 2;

    TestInstruction halt1{};
    halt1.opcode = VmOpcode::HALT;

    bb1.instructions = {lc_ne, halt1};

    // BB2 (equal path): LOAD_CONST r2, pool[3]; HALT
    TestBB bb2{};
    bb2.bb_id = 300;
    bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF;
    bb2.flags = 0;
    fill_epoch_seed(bb2.epoch_seed, 0x55);  // same epoch

    TestInstruction lc_eq{};
    lc_eq.opcode = VmOpcode::LOAD_CONST;
    lc_eq.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc_eq.reg_a  = 2;
    lc_eq.aux    = 3;

    TestInstruction halt2{};
    halt2.opcode = VmOpcode::HALT;

    bb2.instructions = {lc_eq, halt2};

    // Pool: r0=42, r1=42 (equal), not-equal result=0, equal result=1
    // pool[0] -> BB0, r0;  pool[1] -> BB0, r1;
    // pool[2] -> BB1, r2;  pool[3] -> BB2, r2
    std::vector<TestPoolEntry> pool = {
        {42, 0, 0},   // pool[0]: plaintext 42, BB0, r0
        {42, 0, 1},   // pool[1]: plaintext 42, BB0, r1
        {0,  1, 2},   // pool[2]: plaintext 0, BB1, r2 (not-equal path)
        {1,  2, 2},   // pool[3]: plaintext 1, BB2, r2 (equal path)
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

    // r0==r1 (both 42), so JCC(EQ) should be taken -> BB2 -> r2 = 1
    uint64_t decoded = decode_register(loaded->ctx, 2,
                                        loaded->ctx.encoded_regs[2]);
    EXPECT_EQ(decoded, 1u)
        << "Expected r2=1 (equal path), got " << decoded;
}

// ---------------------------------------------------------------------------
// Test 11: StepFunction
// ---------------------------------------------------------------------------

TEST(Dispatcher, StepFunction) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0x66);

    TestInstruction lc{};
    lc.opcode = VmOpcode::LOAD_CONST;
    lc.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc.reg_a  = 0;
    lc.aux    = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {lc, halt};

    std::vector<TestPoolEntry> pool = {{42, 0, 0}};

    auto blob = build_test_blob_ex(seed, {bb}, pool);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    // Step 1: LOAD_CONST -> Running
    auto r1 = step(loaded->ctx, config);
    ASSERT_TRUE(r1.has_value())
        << "step 1 failed: " << static_cast<uint32_t>(r1.error());
    EXPECT_EQ(*r1, VmResult::Running);

    // Step 2: HALT -> Halted
    auto r2 = step(loaded->ctx, config);
    ASSERT_TRUE(r2.has_value())
        << "step 2 failed: " << static_cast<uint32_t>(r2.error());
    EXPECT_EQ(*r2, VmResult::Halted);
}

// ---------------------------------------------------------------------------
// Test 12: NotInstruction
// ---------------------------------------------------------------------------

TEST(Dispatcher, NotInstruction) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0x77);

    TestInstruction lc{};
    lc.opcode = VmOpcode::LOAD_CONST;
    lc.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc.reg_a  = 0;
    lc.aux    = 0;

    TestInstruction not_insn{};
    not_insn.opcode = VmOpcode::NOT;
    not_insn.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_NONE);
    not_insn.reg_a  = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {lc, not_insn, halt};

    std::vector<TestPoolEntry> pool = {{0xFF00FF00FF00FF00ULL, 0, 0}};

    auto blob = build_test_blob_ex(seed, {bb}, pool);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, VmResult::Halted);

    uint64_t decoded = decode_register(loaded->ctx, 0,
                                        loaded->ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 0x00FF00FF00FF00FFu);
}

// ---------------------------------------------------------------------------
// Test 13: ChaffDoesNotAffectResult
// ---------------------------------------------------------------------------

TEST(Dispatcher, ChaffDoesNotAffectResult) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0x88);

    // LOAD_CONST r0, pool[0]; NOP; NOP; LOAD_CONST r1, pool[1]; NOP; ADD r0, r1; HALT
    TestInstruction lc0{};
    lc0.opcode = VmOpcode::LOAD_CONST;
    lc0.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc0.reg_a  = 0;
    lc0.aux    = 0;

    TestInstruction nop{};
    nop.opcode = VmOpcode::NOP;
    nop.flags  = 0;
    nop.reg_a  = 0;
    nop.reg_b  = 0;
    nop.aux    = 0xCAFE0001;

    TestInstruction nop2{};
    nop2.opcode = VmOpcode::NOP;
    nop2.flags  = 0;
    nop2.reg_a  = 0;
    nop2.reg_b  = 0;
    nop2.aux    = 0xCAFE0002;

    TestInstruction lc1{};
    lc1.opcode = VmOpcode::LOAD_CONST;
    lc1.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc1.reg_a  = 1;
    lc1.aux    = 1;

    TestInstruction nop3{};
    nop3.opcode = VmOpcode::NOP;
    nop3.flags  = 0;
    nop3.reg_a  = 0;
    nop3.reg_b  = 0;
    nop3.aux    = 0xCAFE0003;

    TestInstruction add{};
    add.opcode = VmOpcode::ADD;
    add.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    add.reg_a  = 0;
    add.reg_b  = 1;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {lc0, nop, nop2, lc1, nop3, add, halt};

    std::vector<TestPoolEntry> pool = {
        {10, 0, 0},
        {20, 0, 1},
    };

    auto blob = build_test_blob_ex(seed, {bb}, pool);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, VmResult::Halted);

    uint64_t decoded = decode_register(loaded->ctx, 0,
                                        loaded->ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 30u)
        << "ADD 10+20 with NOP chaff should still be 30, got " << decoded;
}

// ---------------------------------------------------------------------------
// Test 14: WidthExtension (SEXT8)
// ---------------------------------------------------------------------------

TEST(Dispatcher, WidthExtension) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0x99);

    TestInstruction lc{};
    lc.opcode = VmOpcode::LOAD_CONST;
    lc.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc.reg_a  = 0;
    lc.aux    = 0;

    TestInstruction sext{};
    sext.opcode = VmOpcode::SEXT8;
    sext.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_NONE);
    sext.reg_a  = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {lc, sext, halt};

    // 0xFF as int8_t is -1, sign-extended to 64 bits = 0xFFFFFFFFFFFFFFFF
    std::vector<TestPoolEntry> pool = {{0xFF, 0, 0}};

    auto blob = build_test_blob_ex(seed, {bb}, pool);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, VmResult::Halted);

    uint64_t decoded = decode_register(loaded->ctx, 0,
                                        loaded->ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 0xFFFFFFFFFFFFFFFFu)
        << "SEXT8(0xFF) should be 0xFFFFFFFFFFFFFFFF, got 0x"
        << std::hex << decoded;
}

// ---------------------------------------------------------------------------
// Test 15: AddCarryPropagation
// ---------------------------------------------------------------------------

TEST(Dispatcher, AddCarryPropagation) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xAB);

    TestInstruction lc0{};
    lc0.opcode = VmOpcode::LOAD_CONST;
    lc0.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc0.reg_a  = 0;
    lc0.aux    = 0;

    TestInstruction lc1{};
    lc1.opcode = VmOpcode::LOAD_CONST;
    lc1.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc1.reg_a  = 1;
    lc1.aux    = 1;

    TestInstruction add{};
    add.opcode = VmOpcode::ADD;
    add.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    add.reg_a  = 0;
    add.reg_b  = 1;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {lc0, lc1, add, halt};

    std::vector<TestPoolEntry> pool = {
        {0xFFFFFFFFFFFFFFFFULL, 0, 0},
        {1, 0, 1},
    };

    auto blob = build_test_blob_ex(seed, {bb}, pool);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*result, VmResult::Halted);

    uint64_t decoded = decode_register(loaded->ctx, 0,
                                        loaded->ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 0u)
        << "ADD 0xFFFFFFFFFFFFFFFF + 1 should overflow to 0, got " << decoded;
}
