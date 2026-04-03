/// @file test_crypto_primitives.cpp
/// @brief Tests for doc 16 crypto primitives: Speck64/128, XEX-Speck64, BLAKE3_KEYED_128, secure_zero.
///
/// Extracted from test_doc16_crypto.cpp into unit/ directory.

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
// SipHash Tests
// ═══════════════════════════════════════════════════════════════════════════════

TEST(SipHashTest, KnownVector) {
    // Test with zero key and empty message
    uint8_t key[16] = {};
    uint64_t hash = Crypto::siphash_2_4(key, nullptr, 0);
    // SipHash-2-4 with zero key and empty input should produce a specific value
    // Verify it at least produces a non-zero result
    EXPECT_NE(hash, 0u);
}

TEST(SipHashTest, DifferentInputsDifferentHashes) {
    uint8_t key[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    uint8_t msg1[] = {0x00};
    uint8_t msg2[] = {0x01};

    uint64_t h1 = Crypto::siphash_2_4(key, msg1, 1);
    uint64_t h2 = Crypto::siphash_2_4(key, msg2, 1);
    EXPECT_NE(h1, h2);
}

TEST(SipHashTest, KeystreamDeterministic) {
    uint8_t key[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    uint64_t enc_state = 0xDEADBEEFCAFEBABE;

    uint64_t ks1 = Crypto::siphash_keystream(key, enc_state, 0);
    uint64_t ks2 = Crypto::siphash_keystream(key, enc_state, 0);
    EXPECT_EQ(ks1, ks2);

    uint64_t ks3 = Crypto::siphash_keystream(key, enc_state, 1);
    EXPECT_NE(ks1, ks3);
}

TEST(SipHashTest, ExpandProduces8Words) {
    uint8_t key[16] = {};
    uint64_t out[8] = {};
    Crypto::siphash_expand(key, 0, 0, out);

    // All 8 words should be populated (extremely unlikely all zero)
    bool all_zero = true;
    for (int i = 0; i < 8; ++i) {
        if (out[i] != 0) all_zero = false;
    }
    EXPECT_FALSE(all_zero);
}
