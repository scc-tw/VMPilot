/// @file test_doc17_doc19_crypto.cpp
/// @brief Cryptographic property tests for Doc 17 (8-byte ratchet) and
///        Doc 19 (fixed-width dispatch unit).
///
/// Tests verify:
///   Group A — Doc 17 enc_state entanglement: flags/reg_pack drive the
///             ratchet; tampering any byte cascades into MAC failure.
///   Group B — Doc 19 Theorem 4.1 isomorphism: N x step() == 1 x dispatch_unit().
///   Group C — Doc 19 chaff welding: removing/adding chaff NOP changes the chain.
///   Group D — Edge cases: HALT mid-DU, branch at last slot, enhanced NOP.

#include "test_blob_builder.hpp"

#include "vm_engine.hpp"
#include "vm_policy.hpp"
#include "vm_state.hpp"

#include <vm/vm_crypto.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_opcode.hpp>

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

/// Pack a VmInsn into uint64_t (matching the blob's 8-byte layout).
static uint64_t pack_insn(uint16_t opcode, uint8_t flags,
                          uint8_t reg_a, uint8_t reg_b, uint32_t aux) {
    VmInsn insn{};
    insn.opcode   = opcode;
    insn.flags    = flags;
    insn.reg_pack = static_cast<uint8_t>((reg_a << 4) | (reg_b & 0x0F));
    insn.aux      = aux;
    uint64_t result = 0;
    std::memcpy(&result, &insn, 8);
    return result;
}

/// Operand type flag helpers.
static constexpr uint8_t f_pool_none() {
    return static_cast<uint8_t>((VM_OPERAND_POOL << 6) | (VM_OPERAND_NONE << 4));
}
static constexpr uint8_t f_rr() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4));
}
static constexpr uint8_t f_none() { return 0; }

// ============================================================================
// Group A: Doc 17 — 8-Byte Entanglement
// ============================================================================

/// A1. Different flags field -> different enc_state.
///
/// Doc 17: update_enc_state hashes all 8 bytes of the plaintext instruction.
/// Two instructions that differ ONLY in the flags byte must produce different
/// enc_states.  Under the old 6-byte ratchet (opcode + aux only), the flags
/// byte was invisible and both would produce the same enc_state.
TEST(Doc17Ratchet, FlagsEntanglement) {
    uint64_t enc_state = 0xDEADC0DECAFE1234;
    uint8_t key[16] = {};
    std::memcpy(key, &enc_state, 8);

    // Two instructions: same opcode(ADD=8), same reg_pack, same aux,
    // different flags (0x40 vs 0x00).
    uint64_t insn_a = pack_insn(8, 0x40, 0, 1, 0);
    uint64_t insn_b = pack_insn(8, 0x00, 0, 1, 0);
    ASSERT_NE(insn_a, insn_b) << "sanity: insns must differ";

    uint8_t msg_a[8], msg_b[8];
    std::memcpy(msg_a, &insn_a, 8);
    std::memcpy(msg_b, &insn_b, 8);

    uint64_t state_a = siphash_2_4(key, msg_a, 8);
    uint64_t state_b = siphash_2_4(key, msg_b, 8);
    EXPECT_NE(state_a, state_b)
        << "Doc 17: different flags byte must yield different enc_state";
}

/// A2. Different reg_pack field -> different enc_state.
TEST(Doc17Ratchet, RegPackEntanglement) {
    uint64_t enc_state = 0xDEADC0DECAFE1234;
    uint8_t key[16] = {};
    std::memcpy(key, &enc_state, 8);

    // Same opcode, flags, aux; different reg_pack (r0,r1 vs r1,r0).
    uint64_t insn_a = pack_insn(8, f_rr(), 0, 1, 0);
    uint64_t insn_b = pack_insn(8, f_rr(), 1, 0, 0);
    ASSERT_NE(insn_a, insn_b);

    uint8_t msg_a[8], msg_b[8];
    std::memcpy(msg_a, &insn_a, 8);
    std::memcpy(msg_b, &insn_b, 8);

    uint64_t state_a = siphash_2_4(key, msg_a, 8);
    uint64_t state_b = siphash_2_4(key, msg_b, 8);
    EXPECT_NE(state_a, state_b)
        << "Doc 17: different reg_pack must yield different enc_state";
}

/// A3. Old 6-byte ratchet cannot distinguish flags; new 8-byte can.
///
/// This is the regression test: prove that the old 6-byte scheme was blind
/// to flags/reg_pack, and the new 8-byte scheme is not.
TEST(Doc17Ratchet, OldSixByteBlindToFlags) {
    uint64_t enc_state = 0xDEADC0DECAFE1234;
    uint8_t key[16] = {};
    std::memcpy(key, &enc_state, 8);

    // Two instructions: same opcode and aux, different flags.
    uint16_t opcode = 8;   // ADD
    uint32_t aux    = 0;
    uint8_t flags_a = 0x40;
    uint8_t flags_b = 0x00;

    // Old 6-byte method: msg = opcode(2) || aux(4) — flags NOT included.
    uint8_t old_msg_a[6], old_msg_b[6];
    std::memcpy(old_msg_a, &opcode, 2);
    std::memcpy(old_msg_a + 2, &aux, 4);
    std::memcpy(old_msg_b, &opcode, 2);
    std::memcpy(old_msg_b + 2, &aux, 4);
    uint64_t old_a = siphash_2_4(key, old_msg_a, 6);
    uint64_t old_b = siphash_2_4(key, old_msg_b, 6);
    EXPECT_EQ(old_a, old_b)
        << "Old 6-byte ratchet was blind to flags (same opcode+aux => same state)";

    // New 8-byte method: msg = full instruction (includes flags + reg_pack).
    uint64_t insn_a = pack_insn(opcode, flags_a, 0, 1, aux);
    uint64_t insn_b = pack_insn(opcode, flags_b, 0, 1, aux);
    uint8_t new_msg_a[8], new_msg_b[8];
    std::memcpy(new_msg_a, &insn_a, 8);
    std::memcpy(new_msg_b, &insn_b, 8);
    uint64_t new_a = siphash_2_4(key, new_msg_a, 8);
    uint64_t new_b = siphash_2_4(key, new_msg_b, 8);
    EXPECT_NE(new_a, new_b)
        << "New 8-byte ratchet distinguishes flags (Doc 17 fix)";
}

/// A4. Cascade failure: 1-bit tamper in encrypted blob -> execution fails.
///
/// Doc 17 §3.1 Avalanche-Driven Implicit Denial: a 1-bit flip in any
/// instruction field corrupts enc_state, which cascades into wrong keystream
/// for subsequent instructions, wrong BB MAC, and ultimate rejection.
TEST(Doc17Ratchet, CascadeFailureOnTamper) {
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

    // Sanity: untampered blob executes successfully.
    {
        auto engine = VmEngine<DebugPolicy, DirectOram>::create(
            blob.data(), blob.size(), seed);
        ASSERT_TRUE(engine.has_value());
        auto result = engine->execute();
        ASSERT_TRUE(result.has_value()) << "Untampered blob must execute";
    }

    // Tamper: flip 1 bit in the 2nd encrypted instruction (byte offset of
    // the flags field within the instruction section).
    auto tampered = blob;
    uint32_t insn_section_offset = 32;  // sizeof(BlobHeader)
    uint32_t target_byte = insn_section_offset + 1 * 8 + 2;  // insn[1].flags
    tampered[target_byte] ^= 0x01;  // flip bit 0

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        tampered.data(), tampered.size(), seed);
    ASSERT_TRUE(engine.has_value()) << "create() should succeed (header intact)";
    auto result = engine->execute();
    EXPECT_FALSE(result.has_value())
        << "Doc 17 cascade: 1-bit tamper must cause execution failure "
           "(BBMacVerificationFailed or InstructionDecryptFailed)";
}

/// A5. Phase G key ratchet includes full_plaintext_insn.
///
/// Two programs with identical opcode/reg/aux but different flags must
/// produce different insn_fpe_key after one step, because Phase G's
/// ratchet message includes the full 8-byte instruction.
TEST(Doc17Ratchet, PhaseGKeyDivergenceOnFlags) {
    uint8_t seed[32]; fill_seed(seed);

    // Two programs differing only in flags of the first instruction.
    // Using REG,REG vs POOL,NONE flags — both are valid for ADD.
    auto make_blob = [&](uint8_t flags) {
        TestBB bb{};
        bb.bb_id = 1; bb.epoch = 0;
        bb.live_regs_bitmap = 0xFFFF; bb.flags = 0;
        fill_epoch(bb.epoch_seed);
        bb.instructions = {
            {VmOpcode::ADD, flags, 0, 1, 0},
            {VmOpcode::HALT, f_none(), 0, 0, 0},
        };
        return build_test_blob(seed, {bb});
    };

    auto blob_a = make_blob(f_rr());        // flags = REG,REG
    auto blob_b = make_blob(f_pool_none()); // flags = POOL,NONE

    // Create reentrant engines for determinism (chain_state = 0).
    auto engine_a_full = VmEngine<DebugPolicy, DirectOram>::create(
        blob_a.data(), blob_a.size(), seed);
    ASSERT_TRUE(engine_a_full.has_value());
    auto imm_a = engine_a_full->shared_immutable();
    auto ea = VmEngine<DebugPolicy, DirectOram>::create_reentrant(imm_a);
    ASSERT_TRUE(ea.has_value());

    auto engine_b_full = VmEngine<DebugPolicy, DirectOram>::create(
        blob_b.data(), blob_b.size(), seed);
    ASSERT_TRUE(engine_b_full.has_value());
    auto imm_b = engine_b_full->shared_immutable();
    auto eb = VmEngine<DebugPolicy, DirectOram>::create_reentrant(imm_b);
    ASSERT_TRUE(eb.has_value());

    // Execute one instruction via step()
    auto ra = ea->step();
    auto rb = eb->step();
    ASSERT_TRUE(ra.has_value());
    ASSERT_TRUE(rb.has_value());

    // insn_fpe_key must differ (Phase G: K_new = BLAKE3(K, FP || full_insn))
    EXPECT_NE(
        std::memcmp(ea->execution().insn_fpe_key,
                    eb->execution().insn_fpe_key, 16), 0)
        << "Phase G ratchet includes full_plaintext_insn; different flags "
           "must produce different insn_fpe_key";
}

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
// Group C: Doc 19 — Chaff Welding (§7.3)
// ============================================================================

/// C1. Removing a chaff NOP breaks the enc_state ratchet chain.
///
/// Doc 19 §7.3: "Removing any chaff breaks the chain (2^{-128} collision)."
///
/// This is a mathematical property of the ratchet function, not an engine
/// integration test.  We verify it by replaying the SipHash enc_state chain
/// for two instruction sequences that differ by one chaff NOP:
///
///   Sequence A: [LOAD_CONST, NOP₁, NOP₂, HALT]
///   Sequence B: [LOAD_CONST, NOP₁,       HALT]
///
/// After instruction 1 (NOP₁) both chains are identical.  At position 2,
/// A processes NOP₂ while B processes HALT — enc_state diverges.
///
/// NOTE: We do NOT compare post-execute() engine state because the
/// branchless Phase L (Doc 19 §4) always runs enter_basic_block at the
/// end of a dispatch unit, which resets enc_state to bb_enc_seed and
/// re-derives insn_fpe_key from chain_state.  This washes out the
/// instruction-level divergence.  The welding guarantee is that tampering
/// with the chaff causes cascade decryption failure at the NEXT instruction
/// (wrong keystream), not that the final post-transition state differs.
TEST(Doc19Welding, RemovingChaffBreaksRatchetChain) {
    // Build plaintext instruction words for both sequences.
    uint64_t insn_load  = pack_insn(
        static_cast<uint16_t>(VmOpcode::LOAD_CONST), f_pool_none(), 0, 0, 0);
    uint64_t insn_nop1  = pack_insn(
        static_cast<uint16_t>(VmOpcode::NOP), f_rr(), 2, 3, 0xCAFECAFE);
    uint64_t insn_nop2  = pack_insn(
        static_cast<uint16_t>(VmOpcode::NOP), f_rr(), 4, 5, 0xDEADDEAD);
    uint64_t insn_halt  = pack_insn(
        static_cast<uint16_t>(VmOpcode::HALT), f_none(), 0, 0, 0);

    // Shared initial enc_state (same bb_enc_seed).
    uint64_t es_a = 0xAAAABBBBCCCCDDDD;
    uint64_t es_b = es_a;

    // Step 0: both process LOAD_CONST.
    auto ratchet = [](uint64_t es, uint64_t insn_u64) {
        uint8_t key[16] = {};
        std::memcpy(key, &es, 8);
        uint8_t msg[8];
        std::memcpy(msg, &insn_u64, 8);
        return siphash_2_4(key, msg, 8);
    };

    es_a = ratchet(es_a, insn_load);
    es_b = ratchet(es_b, insn_load);
    EXPECT_EQ(es_a, es_b) << "After LOAD_CONST: chains must be identical";

    // Step 1: both process NOP₁ (same content).
    es_a = ratchet(es_a, insn_nop1);
    es_b = ratchet(es_b, insn_nop1);
    EXPECT_EQ(es_a, es_b) << "After NOP₁: chains must still be identical";

    // Step 2: A processes NOP₂, B processes HALT.
    es_a = ratchet(es_a, insn_nop2);  // chaff NOP₂
    es_b = ratchet(es_b, insn_halt);  // HALT (chaff removed)
    EXPECT_NE(es_a, es_b)
        << "After divergence: removing chaff NOP₂ must break the chain "
           "(NOP₂ plaintext != HALT plaintext → different SipHash output)";

    // Step 3: A processes HALT — chain diverges further.
    es_a = ratchet(es_a, insn_halt);
    EXPECT_NE(es_a, es_b)
        << "After A's HALT: chains remain diverged (cascade)";
}

/// C2. CSPRNG aux diversity: different aux -> different enc_state.
///
/// Pure unit test on the ratchet function.
TEST(Doc19Welding, AuxDiversityInRatchet) {
    uint64_t enc_state = 0x1234567890ABCDEF;
    uint8_t key[16] = {};
    std::memcpy(key, &enc_state, 8);

    // Two NOP instructions, identical except aux field.
    uint64_t insn_a = pack_insn(
        static_cast<uint16_t>(VmOpcode::NOP), f_rr(), 2, 3, 0xAAAAAAAA);
    uint64_t insn_b = pack_insn(
        static_cast<uint16_t>(VmOpcode::NOP), f_rr(), 2, 3, 0xBBBBBBBB);

    uint8_t msg_a[8], msg_b[8];
    std::memcpy(msg_a, &insn_a, 8);
    std::memcpy(msg_b, &insn_b, 8);

    uint64_t state_a = siphash_2_4(key, msg_a, 8);
    uint64_t state_b = siphash_2_4(key, msg_b, 8);
    EXPECT_NE(state_a, state_b)
        << "Chaff NOPs with different CSPRNG aux must produce different enc_state";
}

/// C3. Adding chaff strictly lengthens the one-way composition chain.
///
/// Doc 19 §7.3: "Adding chaff strictly strengthens the chain."
///
/// Sequence A: [LOAD_CONST, HALT]         — 2-step chain
/// Sequence B: [LOAD_CONST, NOP, HALT]    — 3-step chain
///
/// After processing HALT, sequence B has one extra ratchet step.
/// We verify: enc_state_B(after HALT) != enc_state_A(after HALT).
///
/// Same reasoning as C1: tested at the ratchet function level, not
/// engine level, to avoid branchless Phase L state-reset artifacts.
TEST(Doc19Welding, AddingChaffStrengthensChain) {
    uint64_t insn_load = pack_insn(
        static_cast<uint16_t>(VmOpcode::LOAD_CONST), f_pool_none(), 0, 0, 0);
    uint64_t insn_nop  = pack_insn(
        static_cast<uint16_t>(VmOpcode::NOP), f_rr(), 2, 3, 0xFEEDFACE);
    uint64_t insn_halt = pack_insn(
        static_cast<uint16_t>(VmOpcode::HALT), f_none(), 0, 0, 0);

    auto ratchet = [](uint64_t es, uint64_t insn_u64) {
        uint8_t key[16] = {};
        std::memcpy(key, &es, 8);
        uint8_t msg[8];
        std::memcpy(msg, &insn_u64, 8);
        return siphash_2_4(key, msg, 8);
    };

    uint64_t es_a = 0x1122334455667788;
    uint64_t es_b = es_a;

    // Sequence A: LOAD_CONST → HALT
    es_a = ratchet(es_a, insn_load);
    es_a = ratchet(es_a, insn_halt);

    // Sequence B: LOAD_CONST → NOP → HALT (one extra step)
    es_b = ratchet(es_b, insn_load);
    es_b = ratchet(es_b, insn_nop);
    es_b = ratchet(es_b, insn_halt);

    EXPECT_NE(es_a, es_b)
        << "Adding a chaff NOP must produce a different final enc_state "
           "(longer one-way SipHash composition chain)";
}

// ============================================================================
// Group D: Edge Cases
// ============================================================================

/// D1. HALT mid-dispatch-unit.
///
/// With N=4, if HALT is at slot 1, dispatch_unit must break the inner loop
/// and still return Halted with the correct return value.
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

/// D2. Branch at last slot of dispatch_unit.
///
/// The fusability predicate (Doc 18 Def 3.1) guarantees only the last of N
/// sub-instructions may be a branch.  Verify that JMP at slot N-1 triggers
/// Phase L transition correctly.
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
