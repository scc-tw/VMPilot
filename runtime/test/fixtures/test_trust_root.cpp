// Stage 3 — VendorTrustRoot embedded at build time.
//
// Confirms:
//   • trust_root() returns a well-formed struct
//   • public_key matches the fixture test key (dev default), so fixtures
//     verify end-to-end against the runtime trust root
//   • immutable: multiple calls return the same reference (same address,
//     same contents), proving the struct lives in a single section slot
//   • fields are null-terminated and carry expected usage labels

#include <gtest/gtest.h>

#include <array>
#include <cstring>

#include "trust_root.hpp"

#include "fixtures/test_signing_key.hpp"

TEST(TrustRoot, WellFormed) {
    EXPECT_TRUE(VMPilot::Runtime::trust_root_is_well_formed());
}

TEST(TrustRoot, MagicAndVersion) {
    const auto& r = VMPilot::Runtime::trust_root();
    EXPECT_EQ(r.magic, VMPilot::Runtime::kVendorTrustRootMagic);
    EXPECT_EQ(r.version, VMPilot::Runtime::kVendorTrustRootVersion);
}

TEST(TrustRoot, EpochIsNonZero) {
    EXPECT_GT(VMPilot::Runtime::trust_root().root_epoch, 0u);
}

TEST(TrustRoot, KeyUsageMatchesBindingRoot) {
    const auto& r = VMPilot::Runtime::trust_root();
    EXPECT_STREQ(r.key_usage, VMPilot::Runtime::kTrustRootKeyUsage);
}

TEST(TrustRoot, SignatureAlgIsEd25519Pure) {
    const auto& r = VMPilot::Runtime::trust_root();
    EXPECT_STREQ(r.signature_alg_id, VMPilot::Runtime::kTrustRootSignatureAlgEd25519);
}

TEST(TrustRoot, DevDefaultMatchesFixtureTestKey) {
    // The whole point of the dev default: fixture signatures must verify
    // against the runtime's embedded public key without any extra wiring.
    const auto& r = VMPilot::Runtime::trust_root();
    const auto& expected = VMPilot::Fixtures::TestKey::kPublicKey;
    ASSERT_EQ(sizeof(r.public_key), expected.size());
    EXPECT_EQ(0, std::memcmp(r.public_key, expected.data(), expected.size()));
}

TEST(TrustRoot, MultipleCallsReturnSameReference) {
    const auto* a = &VMPilot::Runtime::trust_root();
    const auto* b = &VMPilot::Runtime::trust_root();
    EXPECT_EQ(a, b);
}

TEST(TrustRoot, StructSizeMatchesContract) {
    // Downstream acceptance logic assumes this layout; any drift shows up
    // here before binding code starts miscomputing section offsets.
    EXPECT_EQ(sizeof(VMPilot::Runtime::VendorTrustRoot),
              4u + 4u + 8u + 32u + 32u + 32u + 32u + 16u);
}
