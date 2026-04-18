// Stage 2 — adaptor-layer Ed25519 verifier.
//
// Exercises VMPilot::Crypto::Verify_Ed25519 against signatures produced by
// the test-only Botan signer from Stage 0. Confirms that producer and
// consumer agree on the length-prefixed covered_domain rule and that every
// tamper / boundary condition fails closed.
//
// CRYPTO_LIB chooses which backend implementation runs; the interface is
// identical regardless.

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "VMPilot_crypto.hpp"

#include "fixtures/fixture_generator.hpp"
#include "fixtures/sign_ed25519_testonly.hpp"
#include "fixtures/test_signing_key.hpp"

#include "vm/domain_labels.hpp"

namespace {

std::vector<std::uint8_t> pub_key_vec() {
    return std::vector<std::uint8_t>(VMPilot::Fixtures::TestKey::kPublicKey.begin(),
                                     VMPilot::Fixtures::TestKey::kPublicKey.end());
}
std::vector<std::uint8_t> sig_vec(const std::array<std::uint8_t, 64>& s) {
    return std::vector<std::uint8_t>(s.begin(), s.end());
}

}  // namespace

TEST(VerifyEd25519, HappyPathAccepted) {
    const std::vector<std::uint8_t> msg{'h', 'e', 'l', 'l', 'o'};
    const auto sig = VMPilot::Fixtures::Sign::sign_ed25519(
        VMPilot::Fixtures::TestKey::kPrivateSeed,
        VMPilot::DomainLabels::Auth::PackageBinding, msg);

    EXPECT_TRUE(VMPilot::Crypto::Verify_Ed25519(
        pub_key_vec(), sig_vec(sig),
        std::string(VMPilot::DomainLabels::Auth::PackageBinding), msg));
}

TEST(VerifyEd25519, EmptyMessageAccepted) {
    const std::vector<std::uint8_t> msg;
    const auto sig = VMPilot::Fixtures::Sign::sign_ed25519(
        VMPilot::Fixtures::TestKey::kPrivateSeed,
        VMPilot::DomainLabels::Auth::UnitBinding, msg);

    EXPECT_TRUE(VMPilot::Crypto::Verify_Ed25519(
        pub_key_vec(), sig_vec(sig),
        std::string(VMPilot::DomainLabels::Auth::UnitBinding), msg));
}

TEST(VerifyEd25519, TamperedSignatureRejected) {
    const std::vector<std::uint8_t> msg{1, 2, 3};
    auto sig = VMPilot::Fixtures::Sign::sign_ed25519(
        VMPilot::Fixtures::TestKey::kPrivateSeed,
        VMPilot::DomainLabels::Auth::PackageBinding, msg);
    sig[0] ^= 0x01;
    EXPECT_FALSE(VMPilot::Crypto::Verify_Ed25519(
        pub_key_vec(), sig_vec(sig),
        std::string(VMPilot::DomainLabels::Auth::PackageBinding), msg));
}

TEST(VerifyEd25519, TamperedMessageRejected) {
    const std::vector<std::uint8_t> msg{1, 2, 3};
    const auto sig = VMPilot::Fixtures::Sign::sign_ed25519(
        VMPilot::Fixtures::TestKey::kPrivateSeed,
        VMPilot::DomainLabels::Auth::PackageBinding, msg);
    std::vector<std::uint8_t> flipped = msg;
    flipped[0] ^= 0x01;
    EXPECT_FALSE(VMPilot::Crypto::Verify_Ed25519(
        pub_key_vec(), sig_vec(sig),
        std::string(VMPilot::DomainLabels::Auth::PackageBinding), flipped));
}

TEST(VerifyEd25519, WrongPublicKeyRejected) {
    const std::vector<std::uint8_t> msg{7, 8, 9};
    const auto sig = VMPilot::Fixtures::Sign::sign_ed25519(
        VMPilot::Fixtures::TestKey::kPrivateSeed,
        VMPilot::DomainLabels::Auth::PackageBinding, msg);
    std::vector<std::uint8_t> bogus = pub_key_vec();
    bogus[0] ^= 0x01;
    EXPECT_FALSE(VMPilot::Crypto::Verify_Ed25519(
        bogus, sig_vec(sig),
        std::string(VMPilot::DomainLabels::Auth::PackageBinding), msg));
}

TEST(VerifyEd25519, WrongDomainRejected) {
    // Sign under PackageBinding, then attempt to verify claiming UnitBinding.
    // Length-prefixed domain separation guarantees the message digest
    // differs and the signature fails.
    const std::vector<std::uint8_t> msg{'x', 'y'};
    const auto sig = VMPilot::Fixtures::Sign::sign_ed25519(
        VMPilot::Fixtures::TestKey::kPrivateSeed,
        VMPilot::DomainLabels::Auth::PackageBinding, msg);
    EXPECT_FALSE(VMPilot::Crypto::Verify_Ed25519(
        pub_key_vec(), sig_vec(sig),
        std::string(VMPilot::DomainLabels::Auth::UnitBinding), msg));
}

TEST(VerifyEd25519, WrongPublicKeySizeRejected) {
    const std::vector<std::uint8_t> msg{1, 2};
    const auto sig = VMPilot::Fixtures::Sign::sign_ed25519(
        VMPilot::Fixtures::TestKey::kPrivateSeed,
        VMPilot::DomainLabels::Auth::PackageBinding, msg);

    std::vector<std::uint8_t> short_key(31, 0);
    EXPECT_FALSE(VMPilot::Crypto::Verify_Ed25519(
        short_key, sig_vec(sig),
        std::string(VMPilot::DomainLabels::Auth::PackageBinding), msg));

    std::vector<std::uint8_t> long_key(33, 0);
    EXPECT_FALSE(VMPilot::Crypto::Verify_Ed25519(
        long_key, sig_vec(sig),
        std::string(VMPilot::DomainLabels::Auth::PackageBinding), msg));
}

TEST(VerifyEd25519, WrongSignatureSizeRejected) {
    const std::vector<std::uint8_t> msg{4, 5};
    auto sig = VMPilot::Fixtures::Sign::sign_ed25519(
        VMPilot::Fixtures::TestKey::kPrivateSeed,
        VMPilot::DomainLabels::Auth::PackageBinding, msg);

    std::vector<std::uint8_t> short_sig(sig.begin(), sig.begin() + 63);
    EXPECT_FALSE(VMPilot::Crypto::Verify_Ed25519(
        pub_key_vec(), short_sig,
        std::string(VMPilot::DomainLabels::Auth::PackageBinding), msg));

    std::vector<std::uint8_t> long_sig(sig.begin(), sig.end());
    long_sig.push_back(0x00);
    EXPECT_FALSE(VMPilot::Crypto::Verify_Ed25519(
        pub_key_vec(), long_sig,
        std::string(VMPilot::DomainLabels::Auth::PackageBinding), msg));
}

TEST(VerifyEd25519, EmptyDomainRejected) {
    const std::vector<std::uint8_t> msg{6, 7};
    const auto sig = VMPilot::Fixtures::Sign::sign_ed25519(
        VMPilot::Fixtures::TestKey::kPrivateSeed,
        VMPilot::DomainLabels::Auth::PackageBinding, msg);
    EXPECT_FALSE(VMPilot::Crypto::Verify_Ed25519(
        pub_key_vec(), sig_vec(sig), std::string{}, msg));
}

TEST(VerifyEd25519, DomainLengthBoundary) {
    // A 255-byte domain is accepted; a 256-byte domain is rejected because
    // the length prefix is a single byte. No signature needed for the
    // reject case — the verifier short-circuits on size.
    const std::string just_too_long(256, 'd');
    const std::vector<std::uint8_t> msg{9};
    std::vector<std::uint8_t> dummy_sig(64, 0);
    EXPECT_FALSE(VMPilot::Crypto::Verify_Ed25519(
        pub_key_vec(), dummy_sig, just_too_long, msg));
}

TEST(VerifyEd25519, FixturePackageBindingRecordVerifies) {
    // End-to-end: drive a real PackageBindingRecord through the Stage 0
    // builder, then hand signature + canonical bytes to the adaptor.
    // This is the real producer/consumer contract test — anything the
    // fixture generator emits must be accepted by the runtime verifier.
    const auto art = VMPilot::Fixtures::PackageBindingRecordBuilder{}.build();

    const std::vector<std::uint8_t> pubkey(art.signer_public_key.begin(),
                                           art.signer_public_key.end());
    const std::vector<std::uint8_t> sig(art.signature.begin(),
                                        art.signature.end());
    EXPECT_TRUE(VMPilot::Crypto::Verify_Ed25519(
        pubkey, sig, art.covered_domain, art.canonical_bytes));
}
