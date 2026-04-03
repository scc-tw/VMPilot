/// @file test_security_properties.cpp
/// @brief Tests for cryptographic and security properties required by
///        Doc 15, 16, 17, 19 that were not covered by existing tests.
///
/// Properties tested:
///   #1  BB MAC verification correctness (Doc 15 §2.3, Doc 16 §5)
///   #2  CFG hijack rejection (Doc 17 §3.2)
///   #3  Per-instruction ORAM scan invariant (Doc 19 Appendix C, Def C.1)
///   #4  Fixed-iteration verify_bb_mac (Doc 19 §4.2 Fix #2)
///   #5  Branchless Phase L — all three cases (Doc 19 §4.2 Fix #1)
///   #6  ExecSnapshot branchless_restore correctness
///   #7  Branchless Phase E — non-writing opcodes (Doc 19 §4.2 Fix #3)
///   #8  Per-execution independence via RDRAND nonce (Doc 16 §12 S4)

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

static void fill_seed(uint8_t seed[32]) {
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);
}

static void fill_epoch(uint8_t seed[32], uint8_t base = 0x50) {
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(base + i);
}

static constexpr uint8_t f_pool_none() {
    return static_cast<uint8_t>((VM_OPERAND_POOL << 6) | (VM_OPERAND_NONE << 4));
}
static constexpr uint8_t f_rr() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4));
}
static constexpr uint8_t f_r() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_NONE << 4));
}
static constexpr uint8_t f_none() { return 0; }

// ============================================================================
// #1: BB MAC verification correctness (Doc 15 §2.3)
//
// BLAKE3_keyed(integrity_key, all_plaintext_insns_in_BB)[0:8] must match
// the stored MAC.  Flipping any bit in any instruction must cause rejection.
// ============================================================================

TEST(SecurityProperties, BbMacRejectsInstructionTamper) {
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

    auto blob = build_test_blob(seed, {bb}, {42});

    // Tamper: flip a bit in the ENCRYPTED instruction section (not the MAC)
    BlobHeader hdr;
    std::memcpy(&hdr, blob.data(), sizeof(hdr));
    uint32_t insn_off = blob_section_insn(hdr);
    // Flip bit in the second instruction's encrypted bytes
    blob[insn_off + 8] ^= 0x01;

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value())
        << "create() succeeds (MAC is checked at BB boundary, not load)";

    auto result = engine->execute();
    ASSERT_FALSE(result.has_value())
        << "Tampered instruction must cause execution failure";
    // May be BBMacVerificationFailed or InstructionDecryptFailed (cascade)
}

TEST(SecurityProperties, BbMacRejectsMacTamper) {
    // Doc 15 §2.3: BB MAC covers all instructions as a unit.
    // Flipping any bit in the stored MAC must cause verification failure.
    // (Instruction reordering also changes plaintext → MAC mismatch,
    //  but we can't test that via execution because garbage decode may
    //  crash before the MAC check runs at BB boundary.)
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed);

    bb.instructions = {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
        {VmOpcode::LOAD_CONST, f_pool_none(), 1, 0, 1},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb}, {10, 20});

    // Flip a bit in the stored BB MAC
    BlobHeader hdr;
    std::memcpy(&hdr, blob.data(), sizeof(hdr));
    uint32_t mac_off = blob_section_mac(hdr);
    blob[mac_off] ^= 0x01;

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value())
        << "create() succeeds (MAC is checked at BB exit)";

    auto result = engine->execute();
    ASSERT_FALSE(result.has_value())
        << "Flipped MAC bit must cause BBMacVerificationFailed";
    EXPECT_EQ(result.error(), DiagnosticCode::BBMacVerificationFailed);
}

// ============================================================================
// #2: CFG hijack rejection (Doc 17 §3.2)
//
// An attacker who forces the program counter to jump to a BB without
// traversing the legitimate path cannot execute that BB's instructions
// correctly — the enc_state will be wrong, causing cascade decryption
// failure.
// ============================================================================

TEST(SecurityProperties, CfgHijackRejection) {
    // Doc 17 §3.2: An attacker who corrupts enc_state without traversing the
    // legitimate decryption chain cannot decrypt instructions correctly.
    //
    // WHY pipeline-level test (not full execution):
    //   Corrupted enc_state → wrong SipHash keystream → garbage plaintext.
    //   Garbage plaintext may contain MEM operand types pointing to invalid
    //   addresses, causing segfaults BEFORE the MAC check runs.  Doc 17 §3.1
    //   explicitly lists segfault as a valid denial outcome.  But we can't
    //   test crashes in unit tests, so we verify the cryptographic property
    //   at the pipeline level: corrupted enc_state produces different
    //   decrypted instructions than the legitimate enc_state.
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed, 0x50);

    bb.instructions = {
        {VmOpcode::LOAD_CONST, f_pool_none(), 0, 0, 0},
        {VmOpcode::ADD, f_rr(), 0, 1, 0},
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb}, {42});

    // Create two engines with identical state
    auto e_good = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    auto e_bad = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(e_good.has_value());
    ASSERT_TRUE(e_bad.has_value());

    // Corrupt enc_state in the "hijacked" engine
    e_bad->execution().enc_state ^= 0xDEADBEEFCAFEBABEull;

    // fetch_decrypt_decode with correct enc_state → valid instruction
    auto good_insn = pipeline::fetch_decrypt_decode(
        *e_good->shared_immutable(), e_good->execution(), e_good->epoch());
    ASSERT_TRUE(good_insn.has_value())
        << "Legitimate decrypt must succeed";
    EXPECT_EQ(good_insn->opcode, VmOpcode::LOAD_CONST)
        << "Legitimate decrypt must produce LOAD_CONST";

    // fetch_decrypt_decode with corrupted enc_state → garbage
    auto bad_insn = pipeline::fetch_decrypt_decode(
        *e_bad->shared_immutable(), e_bad->execution(), e_bad->epoch());

    if (bad_insn.has_value()) {
        // Decryption produced a "valid" alias, but the plaintext is wrong.
        // The full 8-byte plaintext must differ (SipHash keystream changed).
        EXPECT_NE(good_insn->full_plaintext_insn, bad_insn->full_plaintext_insn)
            << "Corrupted enc_state MUST produce different plaintext instruction "
               "(Doc 17 §3.2: SipHash keystream depends on enc_state)";
    } else {
        // InvalidOpcodeAlias — also a valid cascade failure outcome
        SUCCEED() << "Corrupted enc_state produced invalid opcode alias (cascade failure)";
    }

    // Verify that the legitimate path succeeds end-to-end
    auto result = e_good->execute();
    ASSERT_TRUE(result.has_value()) << "Legitimate execution must succeed";
    EXPECT_EQ(result->return_value, 42u);
}

// ============================================================================
// #3: Per-instruction ORAM scan invariant (Doc 19 Appendix C, Def C.1)
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
// #4: Fixed-iteration verify_bb_mac (Doc 19 §4.2 Fix #2)
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
// #5: Branchless Phase L — all three cases (Doc 19 §4.2 Fix #1)
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
// #6: ExecSnapshot branchless_restore correctness
//
// Verify that capture() + branchless_restore(keep_new=false) is a
// perfect identity: exec state returns to the captured values.
// ============================================================================

TEST(SecurityProperties, ExecSnapshotRoundtrip) {
    VmExecution exec{};
    exec.enc_state = 0xDEADBEEFCAFEBABEull;
    exec.insn_index_in_bb = 7;
    exec.vm_ip = 42;
    for (int r = 0; r < 16; ++r)
        exec.regs[r] = RegVal(static_cast<uint64_t>(r * 1111));
    exec.current_bb_id = 5;
    exec.current_bb_index = 3;
    exec.current_epoch = 2;
    std::memset(exec.bb_chain_state, 0xAA, 32);
    std::memset(exec.insn_fpe_key, 0xBB, 16);

    // Capture
    auto snap = ExecSnapshot::capture(exec);

    // Modify exec
    exec.enc_state = 0x1111111111111111ull;
    exec.insn_index_in_bb = 99;
    exec.vm_ip = 999;
    for (int r = 0; r < 16; ++r)
        exec.regs[r] = RegVal(0xFFFFFFFFFFFFFFFFull);
    exec.current_bb_id = 100;
    exec.current_bb_index = 200;
    exec.current_epoch = 300;
    std::memset(exec.bb_chain_state, 0xCC, 32);
    std::memset(exec.insn_fpe_key, 0xDD, 16);

    // Restore with keep_new=false → must restore snapshot
    snap.branchless_restore(exec, false);

    EXPECT_EQ(exec.enc_state, 0xDEADBEEFCAFEBABEull);
    EXPECT_EQ(exec.insn_index_in_bb, 7u);
    EXPECT_EQ(exec.vm_ip, 42u);
    for (int r = 0; r < 16; ++r)
        EXPECT_EQ(exec.regs[r].bits, static_cast<uint64_t>(r * 1111))
            << "reg[" << r << "] not restored";
    EXPECT_EQ(exec.current_bb_id, 5u);
    EXPECT_EQ(exec.current_bb_index, 3u);
    EXPECT_EQ(exec.current_epoch, 2u);

    uint8_t expected_chain[32], expected_key[16];
    std::memset(expected_chain, 0xAA, 32);
    std::memset(expected_key, 0xBB, 16);
    EXPECT_EQ(std::memcmp(exec.bb_chain_state, expected_chain, 32), 0);
    EXPECT_EQ(std::memcmp(exec.insn_fpe_key, expected_key, 16), 0);
}

TEST(SecurityProperties, ExecSnapshotKeepNew) {
    VmExecution exec{};
    exec.enc_state = 0x1111111111111111ull;
    exec.vm_ip = 100;

    auto snap = ExecSnapshot::capture(exec);

    exec.enc_state = 0x2222222222222222ull;
    exec.vm_ip = 200;

    // Restore with keep_new=true → must keep current (modified) values
    snap.branchless_restore(exec, true);

    EXPECT_EQ(exec.enc_state, 0x2222222222222222ull);
    EXPECT_EQ(exec.vm_ip, 200u);
}

// ============================================================================
// #7: Branchless Phase E — non-writing opcodes (Doc 19 §4.2 Fix #3)
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

// ============================================================================
// #8: Per-execution independence via RDRAND nonce (Doc 16 §12 S4)
//
// Two VmEngine::create() calls with identical blob+seed must produce
// different bb_chain_state (seeded by RDRAND).  This ensures each
// execution derives independent FPE keys.
// ============================================================================

TEST(SecurityProperties, PerExecutionIndependence) {
    uint8_t seed[32]; fill_seed(seed);

    TestBB bb{};
    bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
    fill_epoch(bb.epoch_seed);

    bb.instructions = {
        {VmOpcode::HALT, f_none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb}, {});

    auto e1 = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    auto e2 = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    ASSERT_TRUE(e1.has_value());
    ASSERT_TRUE(e2.has_value());

    // bb_chain_state is seeded by RDRAND nonce — must differ between runs
    // (probability of collision: 2^-64)
    EXPECT_NE(
        std::memcmp(e1->execution().bb_chain_state,
                    e2->execution().bb_chain_state, 32), 0)
        << "Two create() calls must produce different bb_chain_state "
           "(RDRAND nonce diversification, Doc 16 §12 S4). "
           "Collision probability 2^-64.";

    // Consequently, insn_fpe_key must also differ (derived from chain_state)
    EXPECT_NE(
        std::memcmp(e1->execution().insn_fpe_key,
                    e2->execution().insn_fpe_key, 16), 0)
        << "Different bb_chain_state must produce different insn_fpe_key "
           "(path-dependent key derivation)";
}

// ============================================================================
// VmEpoch::branchless_select correctness
// ============================================================================

TEST(SecurityProperties, VmEpochBranchlessSelect_Restore) {
    VmEpoch a{};
    for (int i = 0; i < 256; ++i) {
        a.opcode_perm[i] = static_cast<uint8_t>(i);
        a.opcode_perm_inv[i] = static_cast<uint8_t>(255 - i);
    }
    a.bb_id = 10;
    a.epoch = 20;
    a.live_regs_bitmap = 0x1234;

    VmEpoch snapshot = a;

    // Modify a
    for (int i = 0; i < 256; ++i) {
        a.opcode_perm[i] = static_cast<uint8_t>((i + 77) & 0xFF);
        a.opcode_perm_inv[i] = static_cast<uint8_t>((i + 33) & 0xFF);
    }
    a.bb_id = 99;
    a.epoch = 88;
    a.live_regs_bitmap = 0xFFFF;

    // branchless_select(snapshot, keep_new=false) → restore snapshot
    a.branchless_select(snapshot, false);

    for (int i = 0; i < 256; ++i) {
        EXPECT_EQ(a.opcode_perm[i], static_cast<uint8_t>(i))
            << "opcode_perm[" << i << "] not restored";
        EXPECT_EQ(a.opcode_perm_inv[i], static_cast<uint8_t>(255 - i))
            << "opcode_perm_inv[" << i << "] not restored";
    }
    EXPECT_EQ(a.bb_id, 10u);
    EXPECT_EQ(a.epoch, 20u);
    EXPECT_EQ(a.live_regs_bitmap, 0x1234);
}

TEST(SecurityProperties, VmEpochBranchlessSelect_KeepNew) {
    VmEpoch a{};
    for (int i = 0; i < 256; ++i) {
        a.opcode_perm[i] = static_cast<uint8_t>((i + 77) & 0xFF);
        a.opcode_perm_inv[i] = static_cast<uint8_t>((i + 33) & 0xFF);
    }
    a.bb_id = 99;
    a.epoch = 88;
    a.live_regs_bitmap = 0xFFFF;

    VmEpoch snapshot{};
    std::memset(&snapshot, 0, sizeof(snapshot));

    // branchless_select(snapshot, keep_new=true) → keep a's values
    a.branchless_select(snapshot, true);

    EXPECT_EQ(a.opcode_perm[0], static_cast<uint8_t>(77));
    EXPECT_EQ(a.bb_id, 99u);
    EXPECT_EQ(a.live_regs_bitmap, 0xFFFF);
}
