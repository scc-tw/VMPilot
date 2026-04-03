/// @file test_mac_integrity.cpp
/// @brief MAC integrity verification and CFG hijack rejection tests.
///
/// Extracted from:
///   test_security_properties.cpp — BbMacRejectsInstructionTamper,
///                                  BbMacRejectsMacTamper, CfgHijackRejection
///   test_engine_comprehensive.cpp — CorruptedMacRejected

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

static constexpr uint8_t f_pool_none() {
    return static_cast<uint8_t>((VM_OPERAND_POOL << 6) | (VM_OPERAND_NONE << 4));
}
static constexpr uint8_t f_rr() {
    return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4));
}
static constexpr uint8_t f_none() { return 0; }
static uint8_t none() { return 0; }

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
// Anti-tamper: corrupted blob rejected by BlobView
// (from test_engine_comprehensive.cpp)
// ============================================================================

TEST(EngineAntiTamper, CorruptedMacRejected) {
    uint8_t seed[32]; fill_seed(seed);
    TestBB bb{}; bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    fill_epoch(bb.epoch_seed, 0xD0);
    // Use 2 instructions so execution reaches BB boundary (MAC check)
    bb.instructions = {
        {VmOpcode::NOP, none(), 0, 0, 0},
        {VmOpcode::HALT, none(), 0, 0, 0},
    };

    auto blob = build_test_blob(seed, {bb});

    // Corrupt the MAC section (after instructions + pool + metadata).
    // This causes BB MAC verification to fail without corrupting the
    // instruction ciphertext (which would produce garbage operand types).
    auto& hdr = *reinterpret_cast<BlobHeader*>(blob.data());
    uint32_t mac_off = blob_section_mac(hdr);
    if (mac_off < blob.size()) blob[mac_off] ^= 0xFF;

    auto engine = VmEngine<DebugPolicy>::create(blob.data(), blob.size(), seed);
    ASSERT_TRUE(engine.has_value());

    auto r = engine->execute();
    // BB MAC should fail at BB boundary (step 11)
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), DiagnosticCode::BBMacVerificationFailed);
}
