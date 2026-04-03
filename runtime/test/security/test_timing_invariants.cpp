/// @file test_timing_invariants.cpp
/// @brief Timing invariant and determinism tests for constant-time security.
///
/// Extracted from:
///   test_doc17_doc19_crypto.cpp Group B — Doc 19 Theorem 4.1 isomorphism
///   test_doc17_doc19_crypto.cpp Group D — Enhanced NOP ghost dataflow
///   test_security_properties.cpp — ORAM scan invariant, fixed-iteration MAC,
///                                  branchless Phase L, branchless Phase E

#include "test_blob_builder.hpp"

#include "vm_engine.hpp"
#include "vm_policy.hpp"
#include "vm_state.hpp"
#include "pipeline.hpp"

#include <vm/vm_crypto.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_opcode.hpp>
#include <vm/vm_blob.hpp>

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <vector>

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Common::VM::Crypto;
using namespace VMPilot::Test;

// ============================================================================
// Helpers
// ============================================================================



/// Operand type flag helpers.
static constexpr uint8_t f_pool_none() {
    return static_cast<uint8_t>((VM_OPERAND_POOL << 6) | (VM_OPERAND_NONE << 4));
}
static constexpr uint8_t f_rr() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4));
}
static constexpr uint8_t f_none() { return 0; }

// ============================================================================
// Group B: Doc 19 — dispatch_unit State Isomorphism (Theorem 4.1)
//
// Theorem: dispatch_unit(sigma_0) = A(...A(sigma_0, p_0)..., p_{N-1})
//
// For non-branch instructions within a single BB:
//   N x step()  ==  1 x dispatch_unit()
//
// Compared fields: regs[0..15], insn_fpe_key, enc_state, vm_ip,
//                  insn_index_in_bb, vm_flags.
// ============================================================================

/// Helper: compare mutable exec state of two engines.
static void assert_state_identical(const VmExecution& a, const VmExecution& b,
                                   const char* context) {
    for (int r = 0; r < 16; ++r) {
        EXPECT_EQ(a.regs[r].bits, b.regs[r].bits)
            << context << ": regs[" << r << "] mismatch";
    }
    EXPECT_EQ(std::memcmp(a.insn_fpe_key, b.insn_fpe_key, 16), 0)
        << context << ": insn_fpe_key mismatch";
    EXPECT_EQ(a.enc_state, b.enc_state)
        << context << ": enc_state mismatch";
    EXPECT_EQ(a.vm_ip, b.vm_ip)
        << context << ": vm_ip mismatch";
    EXPECT_EQ(a.insn_index_in_bb, b.insn_index_in_bb)
        << context << ": insn_index_in_bb mismatch";
    EXPECT_EQ(a.vm_flags, b.vm_flags)
        << context << ": vm_flags mismatch";
}

/// B1. Determinism with HighSecPolicy (N=4).
///
/// step() delegates to dispatch_unit() — so 1×step() == 1×dispatch_unit()
/// structurally (same function).  This test verifies that two identically
/// initialized engines produce bit-identical state after one dispatch unit.
TEST(Doc19Isomorphism, HighSec_N4) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed);

    // 5 insns: 4 for the DU + HALT
    bb.instructions = {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},  // r0 = 100
        {VmOpcode::ADD, f_rr(), 0, 1, 0},
        {VmOpcode::SUB, f_rr(), 0, 1, 0},
        {VmOpcode::NOP, f_rr(), 2, 3, 0x12345678},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb}, {100});

    auto parent = VmEngine<HighSecPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(parent.has_value());
    auto imm = parent->shared_immutable();

    auto ea = VmEngine<HighSecPolicy, DirectOram>::create_reentrant(imm);
    auto eb = VmEngine<HighSecPolicy, DirectOram>::create_reentrant(imm);
    ASSERT_TRUE(ea.has_value());
    ASSERT_TRUE(eb.has_value());

    // Engine A: 1 x step() (which IS dispatch_unit)
    auto ra = ea->step();
    ASSERT_TRUE(ra.has_value()) << "step() failed";

    // Engine B: 1 x dispatch_unit()
    auto rb = eb->dispatch_unit();
    ASSERT_TRUE(rb.has_value()) << "dispatch_unit() failed";

    assert_state_identical(ea->execution(), eb->execution(),
                           "HighSec N=4 isomorphism");
}

/// B2. Determinism with StandardPolicy (N=2).
TEST(Doc19Isomorphism, Standard_N2) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed);

    bb.instructions = {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
        {VmOpcode::ADD, f_rr(), 0, 1, 0},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb}, {7});

    auto parent = VmEngine<StandardPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(parent.has_value());
    auto imm = parent->shared_immutable();

    auto ea = VmEngine<StandardPolicy, DirectOram>::create_reentrant(imm);
    auto eb = VmEngine<StandardPolicy, DirectOram>::create_reentrant(imm);
    ASSERT_TRUE(ea.has_value());
    ASSERT_TRUE(eb.has_value());

    auto ra = ea->step();
    ASSERT_TRUE(ra.has_value());
    auto rb = eb->dispatch_unit();
    ASSERT_TRUE(rb.has_value());

    assert_state_identical(ea->execution(), eb->execution(),
                           "Standard N=2 isomorphism");
}

/// B3. Degeneracy: DebugPolicy (N=1).  dispatch_unit == step().
TEST(Doc19Isomorphism, Debug_N1_Degeneracy) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed);

    bb.instructions = {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb}, {99});

    auto parent = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(parent.has_value());
    auto imm = parent->shared_immutable();

    auto ea = VmEngine<DebugPolicy, DirectOram>::create_reentrant(imm);
    auto eb = VmEngine<DebugPolicy, DirectOram>::create_reentrant(imm);
    ASSERT_TRUE(ea.has_value());
    ASSERT_TRUE(eb.has_value());

    auto ra = ea->step();
    ASSERT_TRUE(ra.has_value());
    auto rb = eb->dispatch_unit();
    ASSERT_TRUE(rb.has_value());

    assert_state_identical(ea->execution(), eb->execution(),
                           "Debug N=1 degeneracy");
}

/// B4. Multi-DU cumulative determinism.
///
/// 2 x step()  ==  2 x dispatch_unit()  (HighSecPolicy, N=4).
/// step() delegates to dispatch_unit(), so this verifies deterministic
/// state evolution over multiple dispatch units.
TEST(Doc19Isomorphism, MultiDU_HighSec) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed);

    // 9 insns: 8 for two DUs + HALT
    bb.instructions = {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},  // DU 1
        {VmOpcode::ADD, f_rr(), 0, 1, 0},
        {VmOpcode::SUB, f_rr(), 0, 1, 0},
        {VmOpcode::NOP, f_rr(), 2, 3, 0xAAAAAAAA},
        {VmOpcode::LOAD_CONST, f_pool_none(), 1, 0, 1},  // DU 2
        {VmOpcode::MUL, f_rr(), 0, 1, 0},
        {VmOpcode::AND, f_rr(), 0, 1, 0},
        {VmOpcode::NOP, f_rr(), 3, 2, 0xBBBBBBBB},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb}, {100, 3});

    auto parent = VmEngine<HighSecPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(parent.has_value());
    auto imm = parent->shared_immutable();

    auto ea = VmEngine<HighSecPolicy, DirectOram>::create_reentrant(imm);
    auto eb = VmEngine<HighSecPolicy, DirectOram>::create_reentrant(imm);
    ASSERT_TRUE(ea.has_value());
    ASSERT_TRUE(eb.has_value());

    // Engine A: 2 x step() (each = 1 dispatch_unit of N=4 instructions)
    for (int i = 0; i < 2; ++i) {
        auto r = ea->step();
        ASSERT_TRUE(r.has_value()) << "step() #" << i << " failed";
    }
    for (int du = 0; du < 2; ++du) {
        auto r = eb->dispatch_unit();
        ASSERT_TRUE(r.has_value()) << "dispatch_unit() #" << du << " failed";
    }

    assert_state_identical(ea->execution(), eb->execution(),
                           "Multi-DU cumulative isomorphism");
}

// ============================================================================
// Group D: Enhanced NOP Ghost Dataflow
// ============================================================================

/// D3. Enhanced NOP ghost dataflow under constant_time policy.
///
/// Under HighSecPolicy (constant_time=true), the NOP handler must execute
/// ghost ALU (plain_a + plain_b) and write to trash_regs.  This verifies
/// that the NOP handler's ghost computation actually runs.
TEST(Doc19EdgeCases, EnhancedNopGhostDataflow) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed);

    // NOP with REG,REG operands (r0, r1) so plain_a and plain_b are
    // FPE-decoded register values (non-zero after LOAD_CONST).
    bb.instructions = {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},  // r0 = 0xAA
        {VmOpcode::LOAD_CONST, f_pool_none(), 1, 0, 1},  // r1 = 0xBB
        {VmOpcode::NOP, f_rr(), 0, 1, 0x99999999},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb}, {0xAA, 0xBB});

    // HighSecPolicy has constant_time = true -> enhanced NOP path.
    auto engine = VmEngine<HighSecPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    // Zero trash_regs before execution to detect writes.
    std::memset(engine->execution().trash_regs, 0,
                sizeof(engine->execution().trash_regs));

    auto result = engine->execute();
    ASSERT_TRUE(result.has_value());

    // After NOP with reg_a=0: trash_regs[0] should be non-zero
    // (ghost ALU wrote plain_a + plain_b there).
    EXPECT_NE(engine->execution().trash_regs[0], 0u)
        << "Enhanced NOP (constant_time=true) must write ghost ALU result "
           "to trash_regs[reg_a]";
}

// ============================================================================
// ORAM Scan Invariant (Doc 19 Appendix C, Def C.1)
//
// Every dispatch unit executes exactly 1 ORAM scan per sub-instruction,
// unconditionally.  The nonce must increment once per sub-instruction
// regardless of opcode (NOP, ADD, PUSH all produce the same nonce delta).
// ============================================================================

TEST(SecurityProperties, OramScanPerInstruction_NopOnly) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed);

    // 4 NOPs + HALT (5 insns — DebugPolicy N=1 does 5 dispatch units)
    bb.instructions = {
        {VmOpcode::NOP, f_rr(), 0, 1, 0x11},
        {VmOpcode::NOP, f_rr(), 2, 3, 0x22},
        {VmOpcode::NOP, f_rr(), 4, 5, 0x33},
        {VmOpcode::NOP, f_rr(), 6, 7, 0x44},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    };

    // Test with RollingKeyOram to verify nonce increments
    auto blob = build_test_blob(seed, {bb}, {});
    auto engine = VmEngine<DebugPolicy, RollingKeyOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    uint64_t nonce_before = engine->oram().nonce;
    auto result = engine->execute();
    ASSERT_TRUE(result.has_value());

    // 5 instructions × 1 ORAM scan each = nonce incremented 5 times
    uint64_t nonce_after = engine->oram().nonce;
    EXPECT_EQ(nonce_after - nonce_before, 5u)
        << "Every instruction (including NOP) must produce exactly 1 ORAM scan "
           "(Doc 19 Appendix C, Def C.1)";
}

TEST(SecurityProperties, OramScanPerInstruction_MixedOpcodes) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed);

    // Mix of opcodes: LOAD_CONST, ADD, CMP, NOP, HALT
    bb.instructions = {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
        {VmOpcode::ADD, f_rr(), 0, 1, 0},
        {VmOpcode::CMP, f_rr(), 0, 1, 0},
        {VmOpcode::NOP, f_rr(), 2, 3, 0xAA},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb}, {42});
    auto engine = VmEngine<DebugPolicy, RollingKeyOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    uint64_t nonce_before = engine->oram().nonce;
    auto result = engine->execute();
    ASSERT_TRUE(result.has_value());

    // Same count regardless of opcode mix
    EXPECT_EQ(engine->oram().nonce - nonce_before, 5u)
        << "ORAM nonce must increment once per instruction regardless of opcode";
}

// ============================================================================
// Fixed-iteration verify_bb_mac (Doc 19 §4.2 Fix #2)
//
// verify_bb_mac must iterate max_bb_insn_count times, not the actual BB
// length.  With two BBs of different sizes, verify that max_bb_insn_count
// is set to the larger one.
// ============================================================================

TEST(SecurityProperties, FixedIterationMac_MaxBbInsnCount) {
    uint8_t seed[32]; fill_seed(seed);

    // BB0: 2 instructions, BB1: 5 instructions
    TestBB bb0{};
    bb0.bb_id = 1; bb0.epoch = 0;
    bb0.live_regs_bitmap = 0xFFFF; bb0.flags = 0;
    fill_epoch(bb0.epoch_seed, 0x50);
    bb0.instructions = {
        {VmOpcode::JMP, f_none(), 0, 0, 2},
        {VmOpcode::NOP, f_none(), 0, 0, 0},  // padding (never reached)
    };

    TestBB bb1{};
    bb1.bb_id = 2; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0x70);
    bb1.instructions = {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
        {VmOpcode::NOP, f_rr(), 2, 3, 0x11},
        {VmOpcode::NOP, f_rr(), 4, 5, 0x22},
        {VmOpcode::NOP, f_rr(), 6, 7, 0x33},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb0, bb1}, {42});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    // max_bb_insn_count must be max(2, 5) = 5
    EXPECT_EQ(engine->shared_immutable()->max_bb_insn_count, 5u)
        << "max_bb_insn_count must be the maximum BB size across all BBs "
           "(Doc 19 §4.2 Fix #2)";

    // Execution must succeed (MAC verification uses fixed iteration count)
    auto result = engine->execute();
    ASSERT_TRUE(result.has_value())
        << "Fixed-iteration MAC must correctly verify BBs of different sizes";
    EXPECT_EQ(result->return_value, 42u);
}

// ============================================================================
// Branchless Phase L — all three cases (Doc 19 §4.2 Fix #1)
//
// Phase L always executes verify_bb_mac + enter_basic_block + MUX.
// Three cases:
//   (a) branch_taken → commit new state
//   (b) at_bb_end && !branch_taken → commit (fallthrough)
//   (c) mid-BB && !branch_taken → restore snapshot (no-op transition)
// ============================================================================

TEST(SecurityProperties, PhaseLBranchless_BranchTaken) {
    // Case (a): JMP sets branch_taken → Phase L commits transition
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb0{};
    bb0.bb_id = 1; bb0.epoch = 0;
    bb0.live_regs_bitmap = 0xFFFF; bb0.flags = 0;
    fill_epoch(bb0.epoch_seed, 0x50);
    bb0.instructions = {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
        {VmOpcode::JMP, f_none(), 0, 0, 2},
    };

    TestBB bb1{};
    bb1.bb_id = 2; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0x70);
    bb1.instructions = {
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb0, bb1}, {77});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    auto result = engine->execute();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->return_value, 77u)
        << "Phase L case (a): branch_taken must transition to target BB";
}

TEST(SecurityProperties, PhaseLBranchless_Fallthrough) {
    // Case (b): at BB end, no branch → fallthrough to next sequential BB
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb0{};
    bb0.bb_id = 1; bb0.epoch = 0;
    bb0.live_regs_bitmap = 0xFFFF; bb0.flags = 0;
    fill_epoch(bb0.epoch_seed, 0x50);
    bb0.instructions = {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
    };

    TestBB bb1{};
    bb1.bb_id = 2; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0x70);
    bb1.instructions = {
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb0, bb1}, {88});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    auto result = engine->execute();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->return_value, 88u)
        << "Phase L case (b): at BB end must fallthrough to next BB";
}

TEST(SecurityProperties, PhaseLBranchless_MidBbNoTransition) {
    // Case (c): mid-BB, no branch → snapshot restored, execution continues
    // Use HighSec (N=4) with a 5-instruction BB: the first dispatch_unit
    // processes insns 0-3 (mid-BB), Phase L must NOT transition.
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed);

    bb.instructions = {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},  // DU1: slot 0
        {VmOpcode::NOP, f_rr(), 2, 3, 0x11},              // DU1: slot 1
        {VmOpcode::NOP, f_rr(), 4, 5, 0x22},              // DU1: slot 2
        {VmOpcode::NOP, f_rr(), 6, 7, 0x33},              // DU1: slot 3
        {VmOpcode::HALT, f_none(), 0, 0, 0},              // DU2: slot 0
    };

    auto blob = build_test_blob(seed, {bb}, {55});
    auto engine = VmEngine<HighSecPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    // After first dispatch_unit (4 insns), vm_ip=4, still in BB (5 insns).
    // Phase L case (c): identity transition, snapshot restored.
    auto r1 = engine->dispatch_unit();
    ASSERT_TRUE(r1.has_value());
    EXPECT_EQ(*r1, VmResult::Stepped)
        << "Mid-BB Phase L must NOT halt";
    EXPECT_EQ(engine->execution().current_bb_id, 1u)
        << "Phase L case (c): must stay in same BB";

    // Second dispatch_unit executes HALT
    auto r2 = engine->dispatch_unit();
    ASSERT_TRUE(r2.has_value());
    EXPECT_EQ(*r2, VmResult::Halted);
}

// ============================================================================
// Branchless Phase E — non-writing opcodes (Doc 19 §4.2 Fix #3)
//
// For non-writing opcodes (NOP, CMP, JMP), FPE_Encode is always computed
// but the result is discarded via bitmask MUX.  The register value must
// remain unchanged (the FPE-encoded value from the previous instruction).
// ============================================================================

TEST(SecurityProperties, BranchlessPhaseE_NonWritingOpcodePreservesReg) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed);

    // LOAD_CONST r0 = 42, then CMP r0,r1 (non-writing), then HALT
    // After CMP, r0 must still hold a value that decodes to 42
    bb.instructions = {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
        {VmOpcode::CMP, f_rr(), 0, 1, 0},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb}, {42});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    auto result = engine->execute();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->return_value, 42u)
        << "CMP (non-writing opcode) must not corrupt r0 — branchless Phase E "
           "MUX must preserve the register value (Doc 19 §4.2 Fix #3)";
}

// ############################################################################
// Doc19EdgeCases: HALT mid-DU and branch at last slot
// (from test_doc17_doc19_crypto.cpp)
// ############################################################################

TEST(Doc19EdgeCases, HaltMidDispatchUnit) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed);

    // HALT at slot 1 of a 4-wide DU (2 real insns, 2 padding)
    bb.instructions = {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},  // r0 = 42
        {VmOpcode::HALT, f_none(), 0, 0, 0},
        {VmOpcode::NOP, f_none(), 0, 0, 0},  // should not execute
        {VmOpcode::NOP, f_none(), 0, 0, 0},  // should not execute
    };

    auto blob = build_test_blob(seed, {bb}, {42});
    auto engine = VmEngine<HighSecPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto result = engine->execute();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->status, VmResult::Halted);
    EXPECT_EQ(result->return_value, 42u)
        << "HALT mid-DU must correctly return r0 value";
}

TEST(Doc19EdgeCases, BranchAtLastSlot) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb0{};
    bb0.bb_id = 1; bb0.epoch = 0;
    bb0.live_regs_bitmap = 0xFFFF; bb0.flags = 0;
    fill_epoch(bb0.epoch_seed, 0x50);

    // N=4 DU: [LOAD_CONST, NOP, NOP, JMP bb1]
    bb0.instructions = {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
        {VmOpcode::NOP, f_rr(), 2, 3, 0x11111111},
        {VmOpcode::NOP, f_rr(), 4, 5, 0x22222222},
        {VmOpcode::JMP, f_none(), 0, 0, 2},  // aux = bb_id of BB1
    };

    TestBB bb1{};
    bb1.bb_id = 2; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    fill_epoch(bb1.epoch_seed, 0x70);

    bb1.instructions = {
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb0, bb1}, {7});
    auto engine = VmEngine<HighSecPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());
    auto result = engine->execute();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->status, VmResult::Halted);
    EXPECT_EQ(result->return_value, 7u)
        << "JMP at last DU slot must transition to BB1 and return r0=7";
}
