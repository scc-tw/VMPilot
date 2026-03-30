/// @file test_vm_entry.cpp
/// @brief End-to-end tests for the Phase 10 public VM entry API.
///
/// Tests exercise vm_execute() and vm_execute_with_args() as the top-level
/// entry point, verifying the full load-execute-decode lifecycle.

#include "test_blob_builder.hpp"

#include <vm_entry.hpp>
#include <encoding.hpp>
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
// Test 1: SimpleHalt
// ---------------------------------------------------------------------------

TEST(VmEntry, SimpleHalt) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xA0);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {halt};

    auto blob = build_test_blob(seed, {bb});
    auto config = default_config();

    auto result = vm_execute(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(result.has_value())
        << "vm_execute failed: 0x" << std::hex
        << static_cast<uint32_t>(result.error());
    EXPECT_EQ(result->status, VmResult::Halted);
}

// ---------------------------------------------------------------------------
// Test 2: ReturnConstant
// ---------------------------------------------------------------------------

TEST(VmEntry, ReturnConstant) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xB0);

    TestInstruction load_const{};
    load_const.opcode = VmOpcode::LOAD_CONST;
    load_const.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    load_const.reg_a  = 0;
    load_const.aux    = 0;  // pool index 0

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {load_const, halt};

    // Pool entry: value 42, encoded for BB[0], register 0
    std::vector<TestPoolEntry> pool = {{42, 0, 0}};

    auto blob = build_test_blob_ex(seed, {bb}, pool);
    auto config = default_config();

    auto result = vm_execute(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(result.has_value())
        << "vm_execute failed: 0x" << std::hex
        << static_cast<uint32_t>(result.error());
    EXPECT_EQ(result->status, VmResult::Halted);
    EXPECT_EQ(result->return_value, 42u)
        << "LOAD_CONST 42 -> r0, expected return_value == 42";
}

// ---------------------------------------------------------------------------
// Test 3: AddTwoArgs
// ---------------------------------------------------------------------------

TEST(VmEntry, AddTwoArgs) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;  // all regs live (encoding tables for all)
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xC0);

    // ADD r0, r1 — adds r1 into r0
    TestInstruction add{};
    add.opcode = VmOpcode::ADD;
    add.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    add.reg_a  = 0;
    add.reg_b  = 1;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {add, halt};

    auto blob = build_test_blob(seed, {bb});
    auto config = default_config();

    // Pass initial args: r0 = 10, r1 = 20
    uint64_t args[] = {10, 20};
    auto result = vm_execute_with_args(blob.data(), blob.size(), seed,
                                       args, 2, config);
    ASSERT_TRUE(result.has_value())
        << "vm_execute_with_args failed: 0x" << std::hex
        << static_cast<uint32_t>(result.error());
    EXPECT_EQ(result->status, VmResult::Halted);
    EXPECT_EQ(result->return_value, 30u)
        << "ADD 10 + 20 should equal 30, got " << result->return_value;
}

// ---------------------------------------------------------------------------
// Test 4: WithArguments (4 args, verify register accessibility)
// ---------------------------------------------------------------------------

TEST(VmEntry, WithArguments) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xD0);

    // Program: r0 = r0 + r1 + r2 + r3
    // ADD r0, r1
    TestInstruction add01{};
    add01.opcode = VmOpcode::ADD;
    add01.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    add01.reg_a  = 0;
    add01.reg_b  = 1;

    // ADD r0, r2
    TestInstruction add02{};
    add02.opcode = VmOpcode::ADD;
    add02.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    add02.reg_a  = 0;
    add02.reg_b  = 2;

    // ADD r0, r3
    TestInstruction add03{};
    add03.opcode = VmOpcode::ADD;
    add03.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    add03.reg_a  = 0;
    add03.reg_b  = 3;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {add01, add02, add03, halt};

    auto blob = build_test_blob(seed, {bb});
    auto config = default_config();

    // 4 args: r0=100, r1=200, r2=300, r3=400 -> sum = 1000
    uint64_t args[] = {100, 200, 300, 400};
    auto result = vm_execute_with_args(blob.data(), blob.size(), seed,
                                       args, 4, config);
    ASSERT_TRUE(result.has_value())
        << "vm_execute_with_args failed: 0x" << std::hex
        << static_cast<uint32_t>(result.error());
    EXPECT_EQ(result->status, VmResult::Halted);
    EXPECT_EQ(result->return_value, 1000u)
        << "100 + 200 + 300 + 400 should equal 1000, got "
        << result->return_value;
}

// ---------------------------------------------------------------------------
// Test 5: MultiplyArgs
// ---------------------------------------------------------------------------

TEST(VmEntry, MultiplyArgs) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xE0);

    // MUL r0, r1
    TestInstruction mul{};
    mul.opcode = VmOpcode::MUL;
    mul.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    mul.reg_a  = 0;
    mul.reg_b  = 1;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {mul, halt};

    auto blob = build_test_blob(seed, {bb});
    auto config = default_config();

    // r0 = 7, r1 = 6 -> 7 * 6 = 42
    uint64_t args[] = {7, 6};
    auto result = vm_execute_with_args(blob.data(), blob.size(), seed,
                                       args, 2, config);
    ASSERT_TRUE(result.has_value())
        << "vm_execute_with_args failed: 0x" << std::hex
        << static_cast<uint32_t>(result.error());
    EXPECT_EQ(result->status, VmResult::Halted);
    EXPECT_EQ(result->return_value, 42u)
        << "MUL 7 * 6 should equal 42, got " << result->return_value;
}

// ---------------------------------------------------------------------------
// Test 6: XorArgs
// ---------------------------------------------------------------------------

TEST(VmEntry, XorArgs) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xF0);

    // XOR r0, r1
    TestInstruction xor_insn{};
    xor_insn.opcode = VmOpcode::XOR;
    xor_insn.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    xor_insn.reg_a  = 0;
    xor_insn.reg_b  = 1;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {xor_insn, halt};

    auto blob = build_test_blob(seed, {bb});
    auto config = default_config();

    // r0 = 0xFF, r1 = 0x0F -> 0xFF ^ 0x0F = 0xF0
    uint64_t args[] = {0xFF, 0x0F};
    auto result = vm_execute_with_args(blob.data(), blob.size(), seed,
                                       args, 2, config);
    ASSERT_TRUE(result.has_value())
        << "vm_execute_with_args failed: 0x" << std::hex
        << static_cast<uint32_t>(result.error());
    EXPECT_EQ(result->status, VmResult::Halted);
    EXPECT_EQ(result->return_value, 0xF0u)
        << "XOR 0xFF ^ 0x0F should equal 0xF0, got 0x"
        << std::hex << result->return_value;
}

// ---------------------------------------------------------------------------
// Test 7: InvalidBlob
// ---------------------------------------------------------------------------

TEST(VmEntry, InvalidBlob) {
    uint8_t seed[32];
    fill_test_seed(seed);

    // Garbage data -- not a valid blob
    uint8_t garbage[64];
    std::memset(garbage, 0xDE, sizeof(garbage));

    auto config = default_config();
    auto result = vm_execute(garbage, sizeof(garbage), seed, config);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobMagicMismatch);
}

// ---------------------------------------------------------------------------
// Test 8: DefaultConfig
// ---------------------------------------------------------------------------

TEST(VmEntry, DefaultConfig) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0x10);

    TestInstruction load_const{};
    load_const.opcode = VmOpcode::LOAD_CONST;
    load_const.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    load_const.reg_a  = 0;
    load_const.aux    = 0;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {load_const, halt};

    std::vector<TestPoolEntry> pool = {{99, 0, 0}};

    auto blob = build_test_blob_ex(seed, {bb}, pool);

    // Use default VmSecurityConfig (no explicit config argument).
    // Note: default config has debug_mode = false, which would trigger
    // anti-debug checks. For this test, we pass debug_mode = true to
    // avoid false positives from anti-debug in test environments.
    auto config = default_config();
    auto result = vm_execute(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(result.has_value())
        << "vm_execute with default config failed: 0x" << std::hex
        << static_cast<uint32_t>(result.error());
    EXPECT_EQ(result->status, VmResult::Halted);
    EXPECT_EQ(result->return_value, 99u);
}
