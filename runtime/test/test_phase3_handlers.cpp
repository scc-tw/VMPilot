/// @file test_phase3_handlers.cpp
/// @brief Tests for Phase 3: traits-based handler system (doc 16).
///
/// Validates:
///   1. build_handler_table compiles for all policy x ORAM combinations
///   2. Handler table has non-null entries for all 55 opcodes
///   3. MOVE handler: plain_b -> regs[dst]
///   4. XOR handler: plain_a ^ plain_b -> regs[dst]
///   5. HALT handler: sets halted flag
///   6. NOP handler: writes plain_b to trash registers (GSS chaff)
///   7. DecodedInsn carries plain_a/plain_b fields
///   8. NATIVE_CALL returns NativeCallBridgeFailed when no entries exist

#include "handler_impls.hpp"
#include "decoded_insn.hpp"
#include "vm_state.hpp"
#include "vm_policy.hpp"
#include "oram_strategy.hpp"

#include <vm/encoded_value.hpp>
#include <vm/vm_opcode.hpp>

#include <gtest/gtest.h>

#include <cstring>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;

// ============================================================================
// 1. Table Build Tests
// ============================================================================

TEST(Phase3Table, BuildsForAllCombinations) {
    // If this compiles, all 55 HandlerTraits specializations exist
    auto t1 = build_handler_table<HighSecPolicy, RollingKeyOram>();
    auto t2 = build_handler_table<StandardPolicy, RollingKeyOram>();
    auto t3 = build_handler_table<DebugPolicy, DirectOram>();
    auto t4 = build_handler_table<HighSecPolicy, DirectOram>();
    auto t5 = build_handler_table<DebugPolicy, RollingKeyOram>();

    // Every entry must be non-null
    for (size_t i = 0; i < VM_OPCODE_COUNT; ++i) {
        EXPECT_NE(t1[i], nullptr) << "Null handler at opcode " << i << " (HighSec+Rolling)";
        EXPECT_NE(t2[i], nullptr) << "Null handler at opcode " << i << " (Standard+Rolling)";
        EXPECT_NE(t3[i], nullptr) << "Null handler at opcode " << i << " (Debug+Direct)";
        EXPECT_NE(t4[i], nullptr) << "Null handler at opcode " << i << " (HighSec+Direct)";
        EXPECT_NE(t5[i], nullptr) << "Null handler at opcode " << i << " (Debug+Rolling)";
    }
}

TEST(Phase3Table, OpcodeCountMatches) {
    auto table = build_handler_table<HighSecPolicy, RollingKeyOram>();
    EXPECT_EQ(table.size(), static_cast<size_t>(VM_OPCODE_COUNT));
    EXPECT_EQ(table.size(), 55u);
}

// ============================================================================
// 2. MOVE Handler
// ============================================================================

TEST(Phase3Move, CopiesPlaintextToDst) {
    VmExecution exec{};
    VmEpoch epoch;
    VmOramState oram{};
    VmImmutable imm{};

    // MOVE r0 = plain_b
    DecodedInsn insn{};
    insn.opcode = VmOpcode::MOVE;
    insn.reg_a = 0;
    insn.reg_b = 1;
    insn.plain_b = 0xCAFEBABE;

    auto table = build_handler_table<HighSecPolicy, RollingKeyOram>();
    auto result = table[uint8_t(VmOpcode::MOVE)](exec, epoch, oram, imm, insn);
    ASSERT_TRUE(result.has_value());

    // Handler stores plain_b directly into regs[reg_a]
    EXPECT_EQ(exec.regs[0].bits, 0xCAFEBABEu);
}

// ============================================================================
// 3. XOR Handler
// ============================================================================

TEST(Phase3Xor, PlaintextXor) {
    VmExecution exec{};
    VmEpoch epoch;
    VmOramState oram{};
    VmImmutable imm{};

    DecodedInsn insn{};
    insn.opcode = VmOpcode::XOR;
    insn.reg_a = 0;
    insn.reg_b = 1;
    insn.plain_a = 0x00FF00FF00FF00FFull;
    insn.plain_b = 0xFF00FF00FF00FF00ull;

    auto table = build_handler_table<HighSecPolicy, RollingKeyOram>();
    auto result = table[uint8_t(VmOpcode::XOR)](exec, epoch, oram, imm, insn);
    ASSERT_TRUE(result.has_value());

    // plain_a ^ plain_b = 0xFFFFFFFFFFFFFFFF
    EXPECT_EQ(exec.regs[0].bits, 0xFFFFFFFFFFFFFFFFull);
}

TEST(Phase3Xor, SelfXorIsZero) {
    VmExecution exec{};
    VmEpoch epoch;
    VmOramState oram{};
    VmImmutable imm{};

    DecodedInsn insn{};
    insn.opcode = VmOpcode::XOR;
    insn.reg_a = 0;
    insn.reg_b = 0;  // self-XOR
    insn.plain_a = 0xDEADBEEFu;
    insn.plain_b = 0xDEADBEEFu;

    auto table = build_handler_table<StandardPolicy, DirectOram>();
    auto result = table[uint8_t(VmOpcode::XOR)](exec, epoch, oram, imm, insn);
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(exec.regs[0].bits, 0u);
}

// ============================================================================
// 4. HALT Handler
// ============================================================================

TEST(Phase3Halt, SetsHaltedFlag) {
    VmExecution exec{};
    VmEpoch epoch;
    VmOramState oram{};
    VmImmutable imm{};
    DecodedInsn insn{};
    insn.opcode = VmOpcode::HALT;

    EXPECT_FALSE(exec.halted);

    auto table = build_handler_table<DebugPolicy, DirectOram>();
    auto result = table[uint8_t(VmOpcode::HALT)](exec, epoch, oram, imm, insn);
    ASSERT_TRUE(result.has_value());

    EXPECT_TRUE(exec.halted);
}

// ============================================================================
// 5. NOP Handler (GSS chaff)
// ============================================================================

TEST(Phase3Nop, WritesToTrashRegs) {
    VmExecution exec{};
    VmEpoch epoch;
    VmOramState oram{};
    VmImmutable imm{};

    DecodedInsn insn{};
    insn.opcode = VmOpcode::NOP;
    insn.reg_a = 2;        // trash destination
    insn.plain_b = 0xBEEF; // plaintext value from pipeline

    auto table = build_handler_table<HighSecPolicy, RollingKeyOram>();
    auto result = table[uint8_t(VmOpcode::NOP)](exec, epoch, oram, imm, insn);
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(exec.trash_regs[2], 0xBEEFu);
}

// ============================================================================
// 6. DecodedInsn Layout
// ============================================================================

TEST(Phase3DecodedInsn, CarriesPlaintextOperands) {
    DecodedInsn insn{};
    insn.plain_a = 42;
    insn.plain_b = 99;

    // Doc 16: plain_a/plain_b are uint64_t plaintext values
    EXPECT_EQ(insn.plain_a, 42u);
    EXPECT_EQ(insn.plain_b, 99u);

    // resolved_a/resolved_b still exist (FPE-encoded, used by pipeline)
    insn.resolved_a = RegVal(0xFF);
    insn.resolved_b = RegVal(0xAA);
    EXPECT_EQ(insn.resolved_a.bits, 0xFFu);
    EXPECT_EQ(insn.resolved_b.bits, 0xAAu);
}

// ============================================================================
// 7. NATIVE_CALL returns error when no entries exist
// ============================================================================

TEST(Phase3Stubs, NativeCallReturnsNotImplemented) {
    VmExecution exec{};
    VmEpoch epoch;
    VmOramState oram{};
    VmImmutable imm{};
    DecodedInsn insn{};

    // aux=0 but native_calls is empty -> NativeCallBridgeFailed
    insn.aux = 0;

    auto table = build_handler_table<DebugPolicy, DirectOram>();
    auto result = table[uint8_t(VmOpcode::NATIVE_CALL)](exec, epoch, oram, imm, insn);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::NativeCallBridgeFailed);
}
