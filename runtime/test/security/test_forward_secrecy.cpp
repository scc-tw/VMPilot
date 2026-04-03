/// @file test_forward_secrecy.cpp
/// @brief Forward-secrecy property tests: key ratchet, BB chain state,
///        FPE key derivation, dead register sanitization, FPE roundtrip.
///
/// Extracted from test_doc16_crypto.cpp (Doc16ForwardSecrecy group)
/// and test_phase2_state.cpp (FpeEncodeDecodeRoundtrip).

#include <vm/speck64.hpp>
#include <vm/xex_speck64.hpp>
#include <vm/vm_crypto.hpp>
#include <vm/secure_zero.hpp>

#include <gtest/gtest.h>

#include <array>
#include <cstring>
#include <random>
#include <set>

using namespace VMPilot::Common::VM::Crypto;
using namespace VMPilot::Common::VM;

// ═══════════════════════════════════════════════════════════════════════════════
// Doc 16 Forward-Secrecy Tests
//
// These tests validate the key ratchet, BB chain state evolution, and
// dead register sanitization properties described in doc 16 rev.8.
// ═══════════════════════════════════════════════════════════════════════════════

/// Key ratchet is irreversible: knowing next_key reveals nothing about prev_key.
///
/// SECURITY PROPERTY (hash preimage resistance):
///   Given K_{j+1} = BLAKE3_KEYED_128(K_j, msg), recovering K_j requires
///   inverting BLAKE3 which has 2^128 preimage resistance.  We verify this
///   operationally by checking that ratcheted keys are unpredictable — they
///   don't have simple arithmetic relationships to the input key.
TEST(Doc16ForwardSecrecy, RatchetIsIrreversible) {
    uint8_t key[16];
    for (int i = 0; i < 16; i++) key[i] = static_cast<uint8_t>(i);

    uint64_t regs[16] = {};
    regs[0] = 42;

    uint8_t fp[16];
    blake3_keyed_fingerprint(key, regs, fp);

    uint8_t msg[22];
    std::memcpy(msg, fp, 16);
    uint16_t opcode = 0; uint32_t aux = 0;
    std::memcpy(msg + 16, &opcode, 2);
    std::memcpy(msg + 18, &aux, 4);

    uint8_t next_key[16];
    blake3_keyed_128(key, msg, 22, next_key, 16);

    // next_key should differ from key in at least half the bytes (avalanche)
    int diff_bytes = 0;
    for (int i = 0; i < 16; i++)
        if (key[i] != next_key[i]) diff_bytes++;
    EXPECT_GE(diff_bytes, 8)
        << "Ratcheted key should differ from input in at least half the bytes (avalanche)";
}

/// Key ratchet: BLAKE3_KEYED_128(key, fingerprint||opcode||aux) must produce
/// a DIFFERENT key for different fingerprints (execution-dependent keys).
///
/// SECURITY PROPERTY: Two instructions with different register states (but
/// same opcode/aux) produce different next keys.  An attacker who knows the
/// current key and observes one instruction's effect cannot predict the key
/// after a different instruction's effect.
TEST(Doc16ForwardSecrecy, KeyRatchetDependsOnFingerprint) {
    uint8_t key[16] = {0x42};

    // Two different register states → two different fingerprints
    uint64_t regs_a[16] = {}; regs_a[0] = 100;
    uint64_t regs_b[16] = {}; regs_b[0] = 200;

    uint8_t fp_a[16], fp_b[16];
    blake3_keyed_fingerprint(key, regs_a, fp_a);
    blake3_keyed_fingerprint(key, regs_b, fp_b);
    ASSERT_NE(std::memcmp(fp_a, fp_b, 16), 0);

    // Ratchet: next_key = BLAKE3_KEYED_128(key, fp || opcode || aux)
    uint16_t opcode = 1;
    uint32_t aux = 0;
    uint8_t msg_a[22], msg_b[22];
    std::memcpy(msg_a, fp_a, 16);
    std::memcpy(msg_a + 16, &opcode, 2);
    std::memcpy(msg_a + 18, &aux, 4);
    std::memcpy(msg_b, fp_b, 16);
    std::memcpy(msg_b + 16, &opcode, 2);
    std::memcpy(msg_b + 18, &aux, 4);

    uint8_t next_key_a[16], next_key_b[16];
    blake3_keyed_128(key, msg_a, 22, next_key_a, 16);
    blake3_keyed_128(key, msg_b, 22, next_key_b, 16);

    EXPECT_NE(std::memcmp(next_key_a, next_key_b, 16), 0)
        << "Different register states must produce different ratcheted keys";
}

/// BB chain state evolution is path-dependent:
/// entering the same BB from different histories produces different chain states.
///
/// SECURITY PROPERTY: The chain state is a hash chain over all previously
/// visited BBs.  Two executions that visit the same BB through different
/// paths get different chain states → different FPE keys → different encodings.
/// This prevents an attacker from learning the FPE key by observing a
/// repeated BB execution.
TEST(Doc16ForwardSecrecy, BbChainStateIsPathDependent) {
    uint8_t seed[32];
    for (int i = 0; i < 32; i++) seed[i] = static_cast<uint8_t>(i + 1);

    // Path 1: start → BB 1
    uint8_t chain1[32] = {};  // initial chain state
    uint8_t evolve1[36];
    std::memcpy(evolve1, chain1, 32);
    uint32_t bb1_id = 1;
    std::memcpy(evolve1 + 32, &bb1_id, 4);
    uint8_t chain1_after[32];
    blake3_keyed_hash(seed, evolve1, 36, chain1_after, 32);

    // Path 2: start → BB 2 → BB 1
    uint8_t chain2[32] = {};  // same initial
    uint8_t evolve2a[36];
    std::memcpy(evolve2a, chain2, 32);
    uint32_t bb2_id = 2;
    std::memcpy(evolve2a + 32, &bb2_id, 4);
    uint8_t chain2_after_bb2[32];
    blake3_keyed_hash(seed, evolve2a, 36, chain2_after_bb2, 32);

    uint8_t evolve2b[36];
    std::memcpy(evolve2b, chain2_after_bb2, 32);
    std::memcpy(evolve2b + 32, &bb1_id, 4);
    uint8_t chain2_after_bb1[32];
    blake3_keyed_hash(seed, evolve2b, 36, chain2_after_bb1, 32);

    // Both visited BB 1, but through different paths → different chain states
    EXPECT_NE(std::memcmp(chain1_after, chain2_after_bb1, 32), 0)
        << "Same BB reached via different paths must have different chain states";
}

/// FPE key derivation: BLAKE3_KEYED(epoch_seed, chain_state) produces
/// different keys for different chain states (same epoch_seed).
TEST(Doc16ForwardSecrecy, FpeKeyDependsOnChainState) {
    uint8_t epoch_seed[32];
    for (int i = 0; i < 32; i++) epoch_seed[i] = static_cast<uint8_t>(0xAA + i);

    uint8_t chain_a[32] = {};
    uint8_t chain_b[32] = {}; chain_b[0] = 1;

    uint8_t key_a[16], key_b[16];
    blake3_keyed_hash(epoch_seed, chain_a, 32, key_a, 16);
    blake3_keyed_hash(epoch_seed, chain_b, 32, key_b, 16);

    EXPECT_NE(std::memcmp(key_a, key_b, 16), 0)
        << "Different chain states must produce different FPE keys";
}

/// Dead register sanitization: FPE_Encode(key, reg, 0) for dead registers
/// produces a deterministic non-zero encoded value.  Two dead registers
/// get DIFFERENT encoded values (XEX tweak).
TEST(Doc16ForwardSecrecy, DeadRegisterSanitization) {
    uint8_t key[16] = {0x55};
    Speck64_RoundKeys rk;
    Speck64_KeySchedule(key, rk);
    XEX_Tweaks tw;
    XEX_ComputeTweaks(rk, tw);

    // Encode zero for all 16 registers — all should be distinct (XEX tweaks)
    std::set<uint64_t> encoded_zeros;
    for (uint8_t r = 0; r < 16; ++r) {
        uint64_t enc = FPE_Encode(rk, tw, r, 0);
        encoded_zeros.insert(enc);
        // Encoded zero should not be zero (cipher is not identity)
        EXPECT_NE(enc, 0u) << "Sanitized dead register should not be zero";
    }
    EXPECT_EQ(encoded_zeros.size(), 16u)
        << "Dead register sanitization must produce distinct values per register";
}

// ═══════════════════════════════════════════════════════════════════════════════
// FPE Roundtrip and Per-Register Independence
//
// From test_phase2_state.cpp — validates Speck-FPE encode/decode roundtrip
// and per-register independence using the xex_speck64.hpp API.
// ═══════════════════════════════════════════════════════════════════════════════

TEST(Phase2Epoch, FpeEncodeDecodeRoundtrip) {
    // Doc 16: register encoding uses Speck-FPE (XEX mode over Speck64/128)
    // instead of per-BB RegTables LUTs.  Verify roundtrip and per-register
    // independence using the xex_speck64.hpp API.

    using namespace VMPilot::Common::VM::Crypto;

    // Set up a test key (128 bits)
    uint8_t key[16];
    for (int i = 0; i < 16; ++i) key[i] = static_cast<uint8_t>(0xA0 + i);

    Speck64_RoundKeys rk;
    Speck64_KeySchedule(key, rk);

    XEX_Tweaks tweaks;
    XEX_ComputeTweaks(rk, tweaks);

    // Verify encode→decode roundtrip for several values across registers
    const uint64_t test_values[] = {0, 1, 0x42, 0xDEADBEEF, 0xCAFEBABE12345678ull};
    for (uint8_t reg = 0; reg < 4; ++reg) {
        for (uint64_t plain : test_values) {
            uint64_t encoded = FPE_Encode(rk, tweaks, reg, plain);
            uint64_t decoded = FPE_Decode(rk, tweaks, reg, encoded);
            EXPECT_EQ(decoded, plain)
                << "FPE roundtrip failed for reg=" << int(reg)
                << ", plain=0x" << std::hex << plain;
        }
    }

    // Verify per-register independence: same plaintext in different registers
    // must produce different ciphertext (XEX tweak guarantees this).
    uint64_t plain = 0x42;
    uint64_t enc_r0 = FPE_Encode(rk, tweaks, 0, plain);
    uint64_t enc_r1 = FPE_Encode(rk, tweaks, 1, plain);
    EXPECT_NE(enc_r0, enc_r1)
        << "Same plaintext in different registers must produce different ciphertext";

    // Encoding is not identity (ciphertext != plaintext)
    EXPECT_NE(enc_r0, plain) << "FPE_Encode should not return plaintext";
}
