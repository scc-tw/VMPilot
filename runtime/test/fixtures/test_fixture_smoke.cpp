// Stage 0 smoke test.
//
// Proves end-to-end that the fixture infrastructure compiles and runs:
//   1. default PackageBindingRecordBuilder produces canonical CBOR bytes
//   2. deterministic byte content across two `build()` calls (reproducibility)
//   3. independent Botan verifier accepts the signature over length-prefixed
//      domain || canonical bytes, using the embedded public key
//   4. any tampering (signature flip, domain swap, public key mismatch,
//      canonical-byte flip) causes verifier rejection

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

#include <botan/ed25519.h>
#include <botan/pubkey.h>

#include "fixtures/fixture_generator.hpp"
#include "fixtures/sign_ed25519_testonly.hpp"
#include "fixtures/test_signing_key.hpp"

#include "vm/domain_labels.hpp"

namespace {

bool botan_verify(const std::array<std::uint8_t, 32>& pubkey,
                  const std::array<std::uint8_t, 64>& sig,
                  std::string_view covered_domain,
                  const std::vector<std::uint8_t>& canonical_bytes) {
    const std::vector<std::uint8_t> pub(pubkey.begin(), pubkey.end());
    Botan::Ed25519_PublicKey bpub{pub};
    Botan::PK_Verifier verifier{bpub, "Pure"};
    const auto prefixed =
        VMPilot::Fixtures::Sign::domain_prefixed_message(covered_domain,
                                                         canonical_bytes);
    verifier.update(prefixed.data(), prefixed.size());
    return verifier.check_signature(sig.data(), sig.size());
}

}  // namespace

TEST(FixtureSmoke, DefaultPackageBindingRecordProducesBytes) {
    const auto art = VMPilot::Fixtures::PackageBindingRecordBuilder{}.build();
    EXPECT_FALSE(art.canonical_bytes.empty());
    EXPECT_EQ(art.covered_domain,
              std::string(VMPilot::DomainLabels::Auth::PackageBinding));
    EXPECT_EQ(art.signer_public_key, VMPilot::Fixtures::TestKey::kPublicKey);
}

TEST(FixtureSmoke, DefaultPackageBindingRecordIsDeterministic) {
    // Canonical bytes must match across identical builds. Signatures may
    // differ because Ed25519 sig is deterministic but a backend that adds a
    // nonce (e.g. wrong mode) would betray itself here — still worth
    // asserting that the wire bytes match.
    const auto a = VMPilot::Fixtures::PackageBindingRecordBuilder{}.build();
    const auto b = VMPilot::Fixtures::PackageBindingRecordBuilder{}.build();
    EXPECT_EQ(a.canonical_bytes, b.canonical_bytes);
    EXPECT_EQ(a.signature, b.signature);
}

TEST(FixtureSmoke, SignatureVerifiesWithBotan) {
    const auto art = VMPilot::Fixtures::PackageBindingRecordBuilder{}.build();
    EXPECT_TRUE(botan_verify(art.signer_public_key, art.signature,
                             art.covered_domain, art.canonical_bytes));
}

TEST(FixtureSmoke, TamperedSignatureFails) {
    auto art = VMPilot::Fixtures::PackageBindingRecordBuilder{}.build();
    art.signature[0] ^= 0x01;
    EXPECT_FALSE(botan_verify(art.signer_public_key, art.signature,
                              art.covered_domain, art.canonical_bytes));
}

TEST(FixtureSmoke, WrongDomainFails) {
    const auto art = VMPilot::Fixtures::PackageBindingRecordBuilder{}.build();
    // Same key, same canonical bytes, but caller claims the signature is
    // over the unit-binding domain. Length-prefix separation must catch it.
    EXPECT_FALSE(botan_verify(
        art.signer_public_key, art.signature,
        VMPilot::DomainLabels::Auth::UnitBinding, art.canonical_bytes));
}

TEST(FixtureSmoke, WrongPublicKeyFails) {
    const auto art = VMPilot::Fixtures::PackageBindingRecordBuilder{}.build();
    auto bogus = art.signer_public_key;
    bogus[0] ^= 0x01;
    EXPECT_FALSE(botan_verify(bogus, art.signature, art.covered_domain,
                              art.canonical_bytes));
}

TEST(FixtureSmoke, TamperedCanonicalBytesFails) {
    auto art = VMPilot::Fixtures::PackageBindingRecordBuilder{}.build();
    ASSERT_FALSE(art.canonical_bytes.empty());
    art.canonical_bytes[0] ^= 0x01;
    EXPECT_FALSE(botan_verify(art.signer_public_key, art.signature,
                              art.covered_domain, art.canonical_bytes));
}

TEST(FixtureSmoke, MutatedFieldChangesCanonicalBytes) {
    const auto base = VMPilot::Fixtures::PackageBindingRecordBuilder{}.build();
    const auto mutated = VMPilot::Fixtures::PackageBindingRecordBuilder{}
                             .anti_downgrade_epoch(2)
                             .build();
    EXPECT_NE(base.canonical_bytes, mutated.canonical_bytes);
    EXPECT_NE(base.signature, mutated.signature);
}
