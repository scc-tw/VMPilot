/// @file test_phase3_handlers.cpp
/// @brief Tests for Phase 3: traits-based handler system.
///
/// Validates:
///   1. build_handler_table compiles for all policy × ORAM combinations
///   2. Handler table has non-null entries for all 55 opcodes
///   3. MOVE handler: same-domain register copy with phantom types
///   4. XOR handler: Class A composition table with zero plaintext
///   5. HALT handler: sets halted flag
///   6. NOP handler: writes to trash registers (GSS chaff)
///   7. DecodedInsn carries phantom-typed RegVal operands

#include "handler_impls.hpp"
#include "decoded_insn.hpp"
#include "vm_state.hpp"
#include "vm_policy.hpp"
#include "oram_strategy.hpp"

#include <vm/encoded_value.hpp>
#include <vm/vm_encoding.hpp>
#include <vm/vm_opcode.hpp>

#include <gtest/gtest.h>

#include <cstring>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;

// ============================================================================
// Helpers
// ============================================================================

/// Create a minimal VmImmutable with memory tables derived from seed.
static VmImmutable make_test_immutable(const uint8_t seed[32]) {
    VmImmutable imm{};
    std::memcpy(imm.stored_seed, seed, 32);
    Encoding::derive_memory_tables(seed, imm.mem.encode, imm.mem.decode);
    return imm;
}

/// Create a VmEpoch with tables derived for a test BB.
static void setup_epoch(VmEpoch& epoch, const VmImmutable& imm,
                         const uint8_t epoch_seed[32],
                         uint16_t live_bitmap = 0xFFFF) {
    BBMetadata bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = live_bitmap;
    std::memcpy(bb.epoch_seed, epoch_seed, 32);
    epoch.enter_bb(bb, imm);
}

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

TEST(Phase3Move, CopiesRegValSameDomain) {
    uint8_t seed[32];
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);

    VmImmutable imm = make_test_immutable(seed);
    VmExecution exec{};
    VmEpoch epoch;

    uint8_t eseed[32];
    for (int i = 0; i < 32; ++i) eseed[i] = static_cast<uint8_t>(0xBB + i);
    setup_epoch(epoch, imm, eseed);

    VmOramState oram{};

    // Encode a value into r1
    PlainVal plain(0xCAFEBABE);
    exec.regs[1] = encode_register(epoch.reg.encode_lut(1), plain);

    // MOVE r0 = r1
    DecodedInsn insn{};
    insn.opcode = VmOpcode::MOVE;
    insn.reg_a = 0;
    insn.reg_b = 1;

    auto table = build_handler_table<HighSecPolicy, RollingKeyOram>();
    auto result = table[uint8_t(VmOpcode::MOVE)](exec, epoch, oram, imm, insn);
    ASSERT_TRUE(result.has_value());

    // r0 should now hold the same encoded bits as r1
    EXPECT_EQ(exec.regs[0], exec.regs[1]);

    // The value was encoded with r1's table, so decode with r1's table.
    // (Per-register encoding: each register has independent bijections.
    //  MOVE copies the encoded bits; downstream handlers must track
    //  which register's encoding applies to the value.)
    PlainVal decoded = decode_register(epoch.reg.decode_lut(1), exec.regs[0]);
    EXPECT_EQ(decoded.bits, 0xCAFEBABEu);
}

// ============================================================================
// 3. XOR Handler (Class A — zero plaintext)
// ============================================================================

TEST(Phase3Xor, ClassAZeroPlaintext) {
    uint8_t seed[32];
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 0x42);

    VmImmutable imm = make_test_immutable(seed);
    VmExecution exec{};
    VmEpoch epoch;

    uint8_t eseed[32];
    for (int i = 0; i < 32; ++i) eseed[i] = static_cast<uint8_t>(0xCC + i);
    setup_epoch(epoch, imm, eseed);

    VmOramState oram{};

    // Encode two known values into r0 and r1
    PlainVal a(0x00FF00FF00FF00FFull);
    PlainVal b(0xFF00FF00FF00FF00ull);
    exec.regs[0] = encode_register(epoch.reg.encode_lut(0), a);
    exec.regs[1] = encode_register(epoch.reg.encode_lut(1), b);

    // XOR r0, r1 (r0 = r0 ^ r1)
    DecodedInsn insn{};
    insn.opcode = VmOpcode::XOR;
    insn.reg_a = 0;
    insn.reg_b = 1;

    auto table = build_handler_table<HighSecPolicy, RollingKeyOram>();
    auto result = table[uint8_t(VmOpcode::XOR)](exec, epoch, oram, imm, insn);
    ASSERT_TRUE(result.has_value());

    // Decode r0 — should be a ^ b = 0xFFFFFFFFFFFFFFFF
    PlainVal decoded = decode_register(epoch.reg.decode_lut(0), exec.regs[0]);
    EXPECT_EQ(decoded.bits, 0xFFFFFFFFFFFFFFFFull);
}

TEST(Phase3Xor, SelfXorIsZero) {
    uint8_t seed[32];
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 0x77);

    VmImmutable imm = make_test_immutable(seed);
    VmExecution exec{};
    VmEpoch epoch;

    uint8_t eseed[32];
    for (int i = 0; i < 32; ++i) eseed[i] = static_cast<uint8_t>(0xDD + i);
    setup_epoch(epoch, imm, eseed);

    VmOramState oram{};

    // XOR r0, r0 should produce encoded(0)
    PlainVal val(0xDEADBEEFu);
    exec.regs[0] = encode_register(epoch.reg.encode_lut(0), val);

    DecodedInsn insn{};
    insn.opcode = VmOpcode::XOR;
    insn.reg_a = 0;
    insn.reg_b = 0;  // self-XOR

    auto table = build_handler_table<StandardPolicy, DirectOram>();
    auto result = table[uint8_t(VmOpcode::XOR)](exec, epoch, oram, imm, insn);
    ASSERT_TRUE(result.has_value());

    PlainVal decoded = decode_register(epoch.reg.decode_lut(0), exec.regs[0]);
    EXPECT_EQ(decoded.bits, 0u);
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

    exec.regs[3] = RegVal(0xBEEF);

    DecodedInsn insn{};
    insn.opcode = VmOpcode::NOP;
    insn.reg_a = 2;   // trash destination
    insn.reg_b = 3;   // source

    auto table = build_handler_table<HighSecPolicy, RollingKeyOram>();
    auto result = table[uint8_t(VmOpcode::NOP)](exec, epoch, oram, imm, insn);
    ASSERT_TRUE(result.has_value());

    EXPECT_EQ(exec.trash_regs[2], 0xBEEFu);
}

// ============================================================================
// 6. DecodedInsn Layout
// ============================================================================

TEST(Phase3DecodedInsn, CarriesPhantomTypedOperands) {
    DecodedInsn insn{};
    insn.resolved_a = RegVal(42);
    insn.resolved_b = RegVal(99);

    // Phantom-typed: these are RegVal, not raw uint64_t
    EXPECT_EQ(insn.resolved_a.bits, 42u);
    EXPECT_EQ(insn.resolved_b.bits, 99u);

    // Cannot assign MemVal to resolved_a:
    // insn.resolved_a = MemVal(42);  // COMPILE ERROR — domain mismatch
}

// ============================================================================
// 7. Stub handlers return NotImplemented
// ============================================================================

TEST(Phase3Stubs, UnimplementedReturnError) {
    VmExecution exec{};
    VmEpoch epoch;
    VmOramState oram{};
    VmImmutable imm{};
    DecodedInsn insn{};

    auto table = build_handler_table<DebugPolicy, DirectOram>();

    // ADD is stubbed
    auto result = table[uint8_t(VmOpcode::ADD)](exec, epoch, oram, imm, insn);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::NotImplemented);
}
