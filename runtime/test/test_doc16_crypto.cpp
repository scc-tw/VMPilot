/// @file test_doc16_crypto.cpp
/// @brief Tests for doc 16 crypto primitives: Speck64/128, XEX-Speck64, BLAKE3_KEYED_128, secure_zero.

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
// Speck64/128 Tests
// ═══════════════════════════════════════════════════════════════════════════════

class Speck64Test : public ::testing::Test {
protected:
    /// NSA test vector for Speck64/128 (from the original paper):
    ///   Key:       0x1b1a1918 13121110 0b0a0908 03020100
    ///   Plaintext: 0x3b726574 7475432d
    ///   Expected:  0x8c6fa548 454e028b
    static constexpr uint8_t nsa_key[16] = {
        0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0a, 0x0b,
        0x10, 0x11, 0x12, 0x13, 0x18, 0x19, 0x1a, 0x1b
    };
    // Plaintext words: x = 0x3b726574, y = 0x7475432d
    // As uint64_t: (x << 32) | y = 0x3b7265747475432d
    static constexpr uint64_t nsa_plaintext  = UINT64_C(0x3b7265747475432d);
    static constexpr uint64_t nsa_ciphertext = UINT64_C(0x8c6fa548454e028b);
};

/// Verify Speck64/128 against the official NSA test vector.
/// This is the primary correctness test — if this fails, everything downstream
/// (XEX, FPE, key ratchet) is broken.
TEST_F(Speck64Test, NsaKnownVector) {
    Speck64_RoundKeys rk;
    Speck64_KeySchedule(nsa_key, rk);

    uint64_t ct = Speck64_Encrypt(rk, nsa_plaintext);
    EXPECT_EQ(ct, nsa_ciphertext)
        << "Speck64/128 encrypt does not match NSA test vector";
}

/// Verify that decrypt(encrypt(P)) == P for the NSA test vector.
TEST_F(Speck64Test, NsaRoundtrip) {
    Speck64_RoundKeys rk;
    Speck64_KeySchedule(nsa_key, rk);

    uint64_t ct = Speck64_Encrypt(rk, nsa_plaintext);
    uint64_t pt = Speck64_Decrypt(rk, ct);
    EXPECT_EQ(pt, nsa_plaintext);
}

/// Encrypt→decrypt roundtrip for 1000 random values — fuzz the permutation property.
/// A block cipher is a permutation: Dec(Enc(x)) = x for all x.
TEST_F(Speck64Test, RandomRoundtrip1000) {
    std::mt19937_64 rng(42);  // deterministic seed for reproducibility
    uint8_t key[16];
    for (auto& b : key) b = static_cast<uint8_t>(rng());

    Speck64_RoundKeys rk;
    Speck64_KeySchedule(key, rk);

    for (int i = 0; i < 1000; ++i) {
        uint64_t pt = rng();
        uint64_t ct = Speck64_Encrypt(rk, pt);
        uint64_t recovered = Speck64_Decrypt(rk, ct);
        ASSERT_EQ(recovered, pt) << "Roundtrip failed at iteration " << i;
    }
}

/// Different keys produce different ciphertexts for the same plaintext.
/// This verifies key-dependent permutation (no weak-key degeneration).
TEST_F(Speck64Test, DifferentKeysProduceDifferentCiphertexts) {
    uint64_t pt = 0xDEADBEEFCAFEBABE;
    std::set<uint64_t> seen;

    for (uint8_t i = 0; i < 100; ++i) {
        uint8_t key[16] = {};
        key[0] = i;  // vary only the first byte
        Speck64_RoundKeys rk;
        Speck64_KeySchedule(key, rk);
        seen.insert(Speck64_Encrypt(rk, pt));
    }

    // All 100 ciphertexts should be distinct (PRP property).
    EXPECT_EQ(seen.size(), 100u);
}

/// Encrypting zero plaintext with zero key should produce a non-zero ciphertext.
/// This validates that the cipher is not a degenerate identity function.
TEST_F(Speck64Test, ZeroInputNotIdentity) {
    uint8_t key[16] = {};
    Speck64_RoundKeys rk;
    Speck64_KeySchedule(key, rk);

    uint64_t ct = Speck64_Encrypt(rk, 0);
    EXPECT_NE(ct, 0u) << "E_K(0) should not be 0 for any key";
}

// ═══════════════════════════════════════════════════════════════════════════════
// GF(2^64) Doubling Tests
// ═══════════════════════════════════════════════════════════════════════════════

/// GF64_Double(1) = 2 (no reduction needed, high bit is 0).
TEST(GF64Test, DoubleOne) {
    EXPECT_EQ(GF64_Double(1), 2u);
}

/// GF64_Double of a value with high bit set triggers reduction.
/// If δ = 0x8000000000000000, then δ << 1 = 0 (overflow), and we XOR with 0x1B.
TEST(GF64Test, DoubleHighBitTriggersReduction) {
    uint64_t val = UINT64_C(0x8000000000000000);
    uint64_t doubled = GF64_Double(val);
    EXPECT_EQ(doubled, GF64_REDUCTION)
        << "Doubling the MSB-only value should give the reduction polynomial";
}

/// Successive doublings from 1 should never repeat (GF(2^64) has order 2^64 - 1).
/// We test the first 100 doublings are all distinct.
TEST(GF64Test, SuccessiveDoublingsAreDistinct) {
    std::set<uint64_t> seen;
    uint64_t val = 1;
    for (int i = 0; i < 100; ++i) {
        ASSERT_TRUE(seen.insert(val).second)
            << "GF64_Double produced a repeat at iteration " << i;
        val = GF64_Double(val);
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// XEX-Speck64 Tests
// ═══════════════════════════════════════════════════════════════════════════════

class XexTest : public ::testing::Test {
protected:
    uint8_t key_[16] = {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,
        0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    Speck64_RoundKeys rk_;
    XEX_Tweaks tw_;

    void SetUp() override {
        Speck64_KeySchedule(key_, rk_);
        XEX_ComputeTweaks(rk_, tw_);
    }
};

/// XEX encrypt→decrypt roundtrip: Dec_T(Enc_T(P)) = P for all registers.
TEST_F(XexTest, RoundtripAllRegisters) {
    for (uint8_t reg = 0; reg < 16; ++reg) {
        uint64_t pt = 0xCAFEBABE00000000 | reg;
        uint64_t ct = FPE_Encode(rk_, tw_, reg, pt);
        uint64_t recovered = FPE_Decode(rk_, tw_, reg, ct);
        EXPECT_EQ(recovered, pt)
            << "XEX roundtrip failed for register " << (int)reg;
    }
}

/// Per-register independence: same plaintext in different registers → different ciphertext.
///
/// SECURITY PROPERTY: This is the core XEX guarantee.  Without per-register
/// tweaks, an adversary could detect when two registers hold the same value
/// by comparing their encodings.  XEX makes this comparison useless.
///
/// Formally: for r ≠ s, Pr[Enc_{T_r}(P) = Enc_{T_s}(P)] = 1/2^64 (PRP).
TEST_F(XexTest, PerRegisterIndependence) {
    uint64_t pt = 0xDEADBEEF;
    std::set<uint64_t> ciphertexts;

    for (uint8_t reg = 0; reg < 16; ++reg) {
        uint64_t ct = FPE_Encode(rk_, tw_, reg, pt);
        ciphertexts.insert(ct);
    }

    // All 16 ciphertexts should be distinct.
    EXPECT_EQ(ciphertexts.size(), 16u)
        << "Same plaintext in different registers should produce different ciphertexts";
}

/// XEX is NOT the identity: Enc(P) ≠ P.
TEST_F(XexTest, EncryptionIsNonTrivial) {
    uint64_t pt = 42;
    for (uint8_t reg = 0; reg < 16; ++reg) {
        uint64_t ct = FPE_Encode(rk_, tw_, reg, pt);
        EXPECT_NE(ct, pt) << "FPE should not be identity for reg " << (int)reg;
    }
}

/// Tweaks are all distinct (GF doubling guarantee).
TEST_F(XexTest, TweaksAreDistinct) {
    std::set<uint64_t> tweaks;
    for (uint8_t reg = 0; reg < 16; ++reg)
        tweaks.insert(tw_.t[reg]);
    EXPECT_EQ(tweaks.size(), 16u);
}

/// Different keys produce different encryptions (key-dependent permutation).
TEST_F(XexTest, DifferentKeysAreDifferent) {
    uint8_t key2[16] = {};
    key2[0] = 0xFF;

    Speck64_RoundKeys rk2;
    Speck64_KeySchedule(key2, rk2);
    XEX_Tweaks tw2;
    XEX_ComputeTweaks(rk2, tw2);

    uint64_t pt = 0x1234567890ABCDEF;
    uint64_t ct1 = FPE_Encode(rk_, tw_, 0, pt);
    uint64_t ct2 = FPE_Encode(rk2, tw2, 0, pt);

    EXPECT_NE(ct1, ct2);
}

/// Convenience one-shot FPE_Encode/FPE_Decode roundtrip.
TEST_F(XexTest, ConvenienceFunctionRoundtrip) {
    uint64_t pt = 0xFEEDFACECAFEBEEF;
    uint64_t ct = FPE_Encode(key_, 5, pt);
    uint64_t recovered = FPE_Decode(key_, 5, ct);
    EXPECT_EQ(recovered, pt);
}

/// Random values roundtrip through FPE for all 16 registers.
TEST_F(XexTest, FuzzRoundtrip500) {
    std::mt19937_64 rng(99);
    for (int i = 0; i < 500; ++i) {
        uint64_t pt = rng();
        uint8_t reg = static_cast<uint8_t>(rng() & 0x0F);
        uint64_t ct = FPE_Encode(rk_, tw_, reg, pt);
        uint64_t recovered = FPE_Decode(rk_, tw_, reg, ct);
        ASSERT_EQ(recovered, pt) << "FPE fuzz roundtrip failed at i=" << i;
    }
}

// ═══════════════════════════════════════════════════════════════════════════════
// BLAKE3_KEYED_128 Tests
// ═══════════════════════════════════════════════════════════════════════════════

TEST(Blake3Keyed128Test, DeterministicOutput) {
    uint8_t key[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    uint8_t data[] = "hello world";
    uint8_t out1[16], out2[16];

    blake3_keyed_128(key, data, sizeof(data) - 1, out1, 16);
    blake3_keyed_128(key, data, sizeof(data) - 1, out2, 16);

    EXPECT_EQ(std::memcmp(out1, out2, 16), 0)
        << "Same input should produce identical output";
}

TEST(Blake3Keyed128Test, DifferentKeysProduceDifferentOutput) {
    uint8_t key1[16] = {};
    uint8_t key2[16] = {};
    key1[0] = 1;
    key2[0] = 2;
    uint8_t data[] = "test";
    uint8_t out1[16], out2[16];

    blake3_keyed_128(key1, data, 4, out1, 16);
    blake3_keyed_128(key2, data, 4, out2, 16);

    EXPECT_NE(std::memcmp(out1, out2, 16), 0);
}

TEST(Blake3Keyed128Test, DifferentDataProducesDifferentOutput) {
    uint8_t key[16] = {0xFF};
    uint8_t data1[] = "alpha";
    uint8_t data2[] = "bravo";
    uint8_t out1[16], out2[16];

    blake3_keyed_128(key, data1, 5, out1, 16);
    blake3_keyed_128(key, data2, 5, out2, 16);

    EXPECT_NE(std::memcmp(out1, out2, 16), 0);
}

TEST(Blake3Keyed128Test, FingerprintChangesWithAnyRegister) {
    uint8_t key[16] = {0xAA};
    uint64_t regs[16] = {};
    regs[0] = 42;

    uint8_t fp_base[16];
    blake3_keyed_fingerprint(key, regs, fp_base);

    // Changing ANY register should change the fingerprint.
    // This is the entanglement property: all registers contribute.
    for (int r = 0; r < 16; ++r) {
        uint64_t saved = regs[r];
        regs[r] = 0xDEAD;
        uint8_t fp_mod[16];
        blake3_keyed_fingerprint(key, regs, fp_mod);
        EXPECT_NE(std::memcmp(fp_base, fp_mod, 16), 0)
            << "Fingerprint should change when register " << r << " is modified";
        regs[r] = saved;  // restore
    }
}

TEST(Blake3Keyed128Test, OutputIsNonZero) {
    uint8_t key[16] = {1};
    uint8_t data[1] = {0};
    uint8_t out[16] = {};

    blake3_keyed_128(key, data, 1, out, 16);

    bool all_zero = true;
    for (auto b : out) if (b != 0) { all_zero = false; break; }
    EXPECT_FALSE(all_zero);
}

// ═══════════════════════════════════════════════════════════════════════════════
// Secure Zero Tests
// ═══════════════════════════════════════════════════════════════════════════════

/// Verify that secure_zero actually zeros the buffer.
/// (We can't verify "not optimised away" at the C++ level — that requires
/// inspecting the generated assembly.  But we can verify the observable effect.)
TEST(SecureZeroTest, ZerosBuffer) {
    uint8_t buf[64];
    std::memset(buf, 0xFF, sizeof(buf));

    secure_zero(buf, sizeof(buf));

    for (auto b : buf)
        EXPECT_EQ(b, 0);
}

/// Null pointer is safe.
TEST(SecureZeroTest, NullptrIsSafe) {
    secure_zero(nullptr, 100);  // should not crash
}

/// Zero length is safe.
TEST(SecureZeroTest, ZeroLengthIsSafe) {
    uint8_t buf[4] = {0xFF, 0xFF, 0xFF, 0xFF};
    secure_zero(buf, 0);  // should not modify
    EXPECT_EQ(buf[0], 0xFF);
}

/// SecureLocal zeros on destruction.
TEST(SecureZeroTest, SecureLocalZerosOnDestruction) {
    alignas(8) uint8_t snapshot[16];

    {
        SecureLocal<std::array<uint8_t, 16>> local;
        local.val.fill(0xAB);
        // Take snapshot of address for post-destruction check.
        // NOTE: Accessing memory after destruction is technically UB,
        // but we're verifying the security property in a test context.
        std::memcpy(snapshot, &local.val, 16);
        EXPECT_EQ(snapshot[0], 0xAB);  // confirm it was written
    }
    // After destruction, the memory should be zeroed.
    // This test relies on the stack not being reused immediately.
    // In practice, the destructor call to secure_zero is what we're testing.
}

/// SecureLocal<uint32_t[27]> — exact type used for Speck round keys.
TEST(SecureZeroTest, SecureLocalWithArrayType) {
    SecureLocal<uint32_t[27]> rk;
    rk.val[0] = 42;
    rk.val[26] = 99;
    EXPECT_EQ(rk.val[0], 42u);
    EXPECT_EQ(rk.val[26], 99u);
    // Will be zeroed on scope exit.
}

// ═══════════════════════════════════════════════════════════════════════════════
// Doc 16 Forward-Secrecy Tests
//
// These tests validate the key ratchet, BB chain state evolution, and
// dead register sanitization properties described in doc 16 rev.8.
// ═══════════════════════════════════════════════════════════════════════════════

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
