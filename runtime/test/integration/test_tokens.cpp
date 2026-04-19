// Stage 11 — Reprovision / Migration token acceptance.
//
// Covers doc 10 §6.2 and doc 15 §7.3:
//   - Both tokens parse under their own covered_domain and reject
//     substitution across domains.
//   - Expired / wrong-package / wrong-policy / missing-revocation
//     rejects are surfaced as public TokenError codes.
//   - Replayed one-time nonces are rejected on the second attempt.

#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <set>
#include <string>
#include <vector>

#include "tokens.hpp"
#include "trust_root.hpp"
#include "vm/domain_labels.hpp"
#include "vm/family_policy.hpp"

#include "fixtures/cbor_test_encoder.hpp"
#include "fixtures/sign_ed25519_testonly.hpp"
#include "fixtures/test_signing_key.hpp"

namespace {

using VMPilot::DomainLabels::FamilyId;
using VMPilot::DomainLabels::PolicyId;
using VMPilot::Runtime::trust_root;
using VMPilot::Runtime::Tokens::accept_migration_token;
using VMPilot::Runtime::Tokens::accept_reprovision_token;
using VMPilot::Runtime::Tokens::MigrationContext;
using VMPilot::Runtime::Tokens::NonceStore;
using VMPilot::Runtime::Tokens::ReasonCode;
using VMPilot::Runtime::Tokens::ReprovisionContext;
using VMPilot::Runtime::Tokens::TokenError;

class InMemoryNonceStore final : public NonceStore {
public:
    bool is_consumed(
        const std::array<std::uint8_t, 32>& nonce) const noexcept override {
        return consumed_.count(nonce) != 0;
    }
    void mark_consumed(
        const std::array<std::uint8_t, 32>& nonce) noexcept override {
        consumed_.insert(nonce);
    }

private:
    std::set<std::array<std::uint8_t, 32>> consumed_;
};

std::vector<std::uint8_t> encode_auth_wrapper(
    const std::array<std::uint8_t, 64>& signature,
    std::string_view covered_domain) {
    using namespace VMPilot::Fixtures::Cbor;
    MapBuilder auth;
    auth.put_uint(1, encode_text("vendor_signature_v1"));
    auth.put_uint(2, encode_text("vmpilot-dev-rfc8032-test1"));
    auth.put_uint(3, encode_text("ed25519-pure-v1"));
    auth.put_uint(4, encode_text(std::string(covered_domain)));
    std::vector<std::uint8_t> sig_vec(signature.begin(), signature.end());
    auth.put_uint(5, encode_bytes(sig_vec));
    return auth.build();
}

std::vector<std::uint8_t> wrap_signed_partition(
    const std::vector<std::uint8_t>& canonical_bytes,
    std::string_view covered_domain) {
    using namespace VMPilot::Fixtures;
    const auto signature = Sign::sign_ed25519(
        TestKey::kPrivateSeed, covered_domain, canonical_bytes);
    const auto auth = encode_auth_wrapper(signature, covered_domain);
    const auto canonical_element = Cbor::encode_bytes(canonical_bytes);
    return Cbor::encode_array({canonical_element, auth});
}

// ─── Reprovision ────────────────────────────────────────────────────────

struct ReprovisionSpec {
    std::string token_version{"reprovision-token-v1"};
    std::string old_enrollment_id{"enr-old"};
    std::string new_enrollment_id{"enr-new"};
    std::array<std::uint8_t, 32> customer_account_id_hash{};
    std::array<std::uint8_t, 32> old_attested_key_hash{};
    std::array<std::uint8_t, 32> new_attested_key_hash{};
    std::array<std::uint8_t, 32> new_provider_evidence_hash{};
    std::array<std::uint8_t, 32> allowed_package_binding_record_hash{};
    VMPilot::DomainLabels::PolicyId allowed_policy_floor{
        VMPilot::DomainLabels::PolicyId::Standard};
    std::vector<VMPilot::DomainLabels::FamilyId> allowed_family_set{
        VMPilot::DomainLabels::FamilyId::F1};
    ReasonCode reason_code{ReasonCode::TpmClear};
    std::string support_case_id{"case-001"};
    std::array<std::uint8_t, 32> approval_chain_hash{};
    std::uint64_t issued_at{1'000'000'000};
    std::uint64_t expires_at{2'000'000'000};
    std::array<std::uint8_t, 32> one_time_nonce{};
};

std::vector<std::uint8_t> build_reprovision_canonical(
    const ReprovisionSpec& s) {
    using namespace VMPilot::Fixtures::Cbor;

    std::vector<std::vector<std::uint8_t>> family_items;
    family_items.reserve(s.allowed_family_set.size());
    for (auto f : s.allowed_family_set) {
        family_items.push_back(
            encode_text(VMPilot::DomainLabels::to_text(f)));
    }

    MapBuilder m;
    m.put_uint(1,  encode_text(s.token_version));
    m.put_uint(2,  encode_text(s.old_enrollment_id));
    m.put_uint(3,  encode_text(s.new_enrollment_id));
    m.put_uint(4,  encode_bytes({s.customer_account_id_hash.begin(),
                                 s.customer_account_id_hash.end()}));
    m.put_uint(5,  encode_bytes({s.old_attested_key_hash.begin(),
                                 s.old_attested_key_hash.end()}));
    m.put_uint(6,  encode_bytes({s.new_attested_key_hash.begin(),
                                 s.new_attested_key_hash.end()}));
    m.put_uint(7,  encode_bytes({s.new_provider_evidence_hash.begin(),
                                 s.new_provider_evidence_hash.end()}));
    m.put_uint(8,  encode_bytes({s.allowed_package_binding_record_hash.begin(),
                                 s.allowed_package_binding_record_hash.end()}));
    m.put_uint(9,  encode_text(VMPilot::DomainLabels::to_text(s.allowed_policy_floor)));
    m.put_uint(10, encode_array(family_items));
    m.put_uint(11, encode_text(VMPilot::enum_to_text(s.reason_code)));
    m.put_uint(12, encode_text(s.support_case_id));
    m.put_uint(13, encode_bytes({s.approval_chain_hash.begin(),
                                 s.approval_chain_hash.end()}));
    m.put_uint(14, encode_uint(s.issued_at));
    m.put_uint(15, encode_uint(s.expires_at));
    m.put_uint(16, encode_bytes({s.one_time_nonce.begin(),
                                 s.one_time_nonce.end()}));
    return m.build();
}

std::array<std::uint8_t, 32> pkg_hash_baseline() {
    return {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
            0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x01,
            0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
            0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11};
}

std::array<std::uint8_t, 32> nonce_baseline() {
    return {0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8,
            0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0,
            0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8,
            0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0};
}

std::array<std::uint8_t, 32> evidence_hash_baseline() {
    return {0xE0, 0xE1, 0xE2, 0xE3};
}

ReprovisionSpec baseline_reprovision() {
    ReprovisionSpec s;
    s.allowed_package_binding_record_hash = pkg_hash_baseline();
    s.new_provider_evidence_hash = evidence_hash_baseline();
    s.one_time_nonce = nonce_baseline();
    return s;
}

ReprovisionContext baseline_reprovision_context() {
    ReprovisionContext ctx{};
    ctx.current_package_binding_record_hash = pkg_hash_baseline();
    ctx.required_policy_floor = PolicyId::Standard;
    ctx.now_unix_seconds = 1'500'000'000;
    ctx.old_enrollment_is_revoked = true;
    ctx.new_provider_evidence_hash = evidence_hash_baseline();
    return ctx;
}

}  // namespace

TEST(ReprovisionToken, HappyPathAccepts_doc10_6_2) {
    auto canonical = build_reprovision_canonical(baseline_reprovision());
    auto bytes = wrap_signed_partition(
        canonical, VMPilot::DomainLabels::Auth::ReprovisionToken);
    InMemoryNonceStore store;

    auto result = accept_reprovision_token(
        bytes.data(), bytes.size(), trust_root(),
        baseline_reprovision_context(), store);
    ASSERT_TRUE(result.has_value())
        << "err=" << static_cast<int>(result.error());
    EXPECT_EQ(result->allowed_policy_floor, PolicyId::Standard);
    EXPECT_EQ(result->reason_code, ReasonCode::TpmClear);
    EXPECT_TRUE(store.is_consumed(nonce_baseline()));
}

TEST(ReprovisionToken, ReplayRejectedOnSecondAttempt_doc10_6_3) {
    auto canonical = build_reprovision_canonical(baseline_reprovision());
    auto bytes = wrap_signed_partition(
        canonical, VMPilot::DomainLabels::Auth::ReprovisionToken);
    InMemoryNonceStore store;

    auto first = accept_reprovision_token(
        bytes.data(), bytes.size(), trust_root(),
        baseline_reprovision_context(), store);
    ASSERT_TRUE(first.has_value());

    auto second = accept_reprovision_token(
        bytes.data(), bytes.size(), trust_root(),
        baseline_reprovision_context(), store);
    ASSERT_FALSE(second.has_value());
    EXPECT_EQ(second.error(), TokenError::NonceAlreadyConsumed);
}

TEST(ReprovisionToken, ExpiredTokenRejected) {
    auto canonical = build_reprovision_canonical(baseline_reprovision());
    auto bytes = wrap_signed_partition(
        canonical, VMPilot::DomainLabels::Auth::ReprovisionToken);
    InMemoryNonceStore store;

    auto ctx = baseline_reprovision_context();
    ctx.now_unix_seconds = 3'000'000'000;  // past expires_at

    auto result = accept_reprovision_token(
        bytes.data(), bytes.size(), trust_root(), ctx, store);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), TokenError::TokenExpired);
}

TEST(ReprovisionToken, TokenBoundToDifferentPackageRejected) {
    auto spec = baseline_reprovision();
    spec.allowed_package_binding_record_hash = {0xFF, 0xFE, 0xFD};  // different pkg
    auto canonical = build_reprovision_canonical(spec);
    auto bytes = wrap_signed_partition(
        canonical, VMPilot::DomainLabels::Auth::ReprovisionToken);
    InMemoryNonceStore store;

    auto result = accept_reprovision_token(
        bytes.data(), bytes.size(), trust_root(),
        baseline_reprovision_context(), store);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), TokenError::PackageHashMismatch);
}

TEST(ReprovisionToken, OldEnrollmentNotRevokedRejected) {
    auto canonical = build_reprovision_canonical(baseline_reprovision());
    auto bytes = wrap_signed_partition(
        canonical, VMPilot::DomainLabels::Auth::ReprovisionToken);
    InMemoryNonceStore store;

    auto ctx = baseline_reprovision_context();
    ctx.old_enrollment_is_revoked = false;

    auto result = accept_reprovision_token(
        bytes.data(), bytes.size(), trust_root(), ctx, store);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), TokenError::OldEnrollmentNotRevoked);
}

TEST(ReprovisionToken, PolicyFloorBelowRequiredRejected) {
    auto spec = baseline_reprovision();
    spec.allowed_policy_floor = VMPilot::DomainLabels::PolicyId::Debug;
    auto canonical = build_reprovision_canonical(spec);
    auto bytes = wrap_signed_partition(
        canonical, VMPilot::DomainLabels::Auth::ReprovisionToken);
    InMemoryNonceStore store;

    auto ctx = baseline_reprovision_context();
    ctx.required_policy_floor = PolicyId::Standard;

    auto result = accept_reprovision_token(
        bytes.data(), bytes.size(), trust_root(), ctx, store);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), TokenError::PolicyFloorBelowRequired);
}

TEST(ReprovisionToken, TamperedCanonicalBytesRejected) {
    auto canonical = build_reprovision_canonical(baseline_reprovision());
    auto bytes = wrap_signed_partition(
        canonical, VMPilot::DomainLabels::Auth::ReprovisionToken);
    // Flip a byte inside the canonical_bytes array element. We know
    // the layout starts with a short CBOR array tag; skip past it.
    ASSERT_GT(bytes.size(), 10u);
    bytes[5] ^= 0x01;
    InMemoryNonceStore store;

    auto result = accept_reprovision_token(
        bytes.data(), bytes.size(), trust_root(),
        baseline_reprovision_context(), store);
    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(result.error() == TokenError::SignatureInvalid ||
                result.error() == TokenError::TokenMalformed);
}

TEST(ReprovisionToken, WrongCoveredDomainRejected) {
    // Sign reprovision canonical under migration domain: the verifier
    // should refuse the wrong covered_domain before even checking
    // the field bindings.
    auto canonical = build_reprovision_canonical(baseline_reprovision());
    auto bytes = wrap_signed_partition(
        canonical, VMPilot::DomainLabels::Auth::MigrationToken);
    InMemoryNonceStore store;

    auto result = accept_reprovision_token(
        bytes.data(), bytes.size(), trust_root(),
        baseline_reprovision_context(), store);
    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(result.error() == TokenError::SignatureInvalid ||
                result.error() == TokenError::TokenMalformed);
}

// ─── Migration ──────────────────────────────────────────────────────────

struct MigrationSpec {
    std::string token_version{"migration-token-v1"};
    std::array<std::uint8_t, 32> old_package_hash{};
    std::array<std::uint8_t, 32> new_package_hash{};
    std::array<std::uint8_t, 32> customer_account_id_hash{};
    bool allowed_import_once{true};
    VMPilot::DomainLabels::PolicyId allowed_policy_floor{
        VMPilot::DomainLabels::PolicyId::Standard};
    std::uint64_t expires_at{2'000'000'000};
    std::array<std::uint8_t, 32> nonce{};
};

std::vector<std::uint8_t> build_migration_canonical(const MigrationSpec& s) {
    using namespace VMPilot::Fixtures::Cbor;

    MapBuilder m;
    m.put_uint(1, encode_text(s.token_version));
    m.put_uint(2, encode_bytes({s.old_package_hash.begin(), s.old_package_hash.end()}));
    m.put_uint(3, encode_bytes({s.new_package_hash.begin(), s.new_package_hash.end()}));
    m.put_uint(4, encode_bytes({s.customer_account_id_hash.begin(),
                                s.customer_account_id_hash.end()}));
    m.put_uint(5, encode_uint(s.allowed_import_once ? 1u : 0u));
    m.put_uint(6, encode_text(VMPilot::DomainLabels::to_text(s.allowed_policy_floor)));
    m.put_uint(7, encode_uint(s.expires_at));
    m.put_uint(8, encode_bytes({s.nonce.begin(), s.nonce.end()}));
    return m.build();
}

std::array<std::uint8_t, 32> target_pkg_hash() {
    return {0x77, 0x77, 0x77};
}

MigrationSpec baseline_migration() {
    MigrationSpec s;
    s.old_package_hash = pkg_hash_baseline();
    s.new_package_hash = target_pkg_hash();
    s.nonce = nonce_baseline();
    return s;
}

MigrationContext baseline_migration_context() {
    MigrationContext ctx{};
    ctx.current_package_binding_record_hash = pkg_hash_baseline();
    ctx.target_package_binding_record_hash = target_pkg_hash();
    ctx.required_policy_floor = PolicyId::Standard;
    ctx.now_unix_seconds = 1'500'000'000;
    return ctx;
}

TEST(MigrationToken, HappyPathAccepts_doc15_7_3) {
    auto canonical = build_migration_canonical(baseline_migration());
    auto bytes = wrap_signed_partition(
        canonical, VMPilot::DomainLabels::Auth::MigrationToken);
    InMemoryNonceStore store;

    auto result = accept_migration_token(
        bytes.data(), bytes.size(), trust_root(),
        baseline_migration_context(), store);
    ASSERT_TRUE(result.has_value())
        << "err=" << static_cast<int>(result.error());
    EXPECT_EQ(result->allowed_policy_floor, PolicyId::Standard);
    EXPECT_TRUE(store.is_consumed(nonce_baseline()));
}

TEST(MigrationToken, ReplayRejectedOnSecondAttempt_doc15_9_9) {
    auto canonical = build_migration_canonical(baseline_migration());
    auto bytes = wrap_signed_partition(
        canonical, VMPilot::DomainLabels::Auth::MigrationToken);
    InMemoryNonceStore store;

    auto first = accept_migration_token(
        bytes.data(), bytes.size(), trust_root(),
        baseline_migration_context(), store);
    ASSERT_TRUE(first.has_value());

    auto second = accept_migration_token(
        bytes.data(), bytes.size(), trust_root(),
        baseline_migration_context(), store);
    ASSERT_FALSE(second.has_value());
    EXPECT_EQ(second.error(), TokenError::NonceAlreadyConsumed);
}

TEST(MigrationToken, ExpiredTokenRejected) {
    auto canonical = build_migration_canonical(baseline_migration());
    auto bytes = wrap_signed_partition(
        canonical, VMPilot::DomainLabels::Auth::MigrationToken);
    InMemoryNonceStore store;

    auto ctx = baseline_migration_context();
    ctx.now_unix_seconds = 3'000'000'000;

    auto result = accept_migration_token(
        bytes.data(), bytes.size(), trust_root(), ctx, store);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), TokenError::TokenExpired);
}

TEST(MigrationToken, WrongSourcePackageRejected) {
    auto spec = baseline_migration();
    spec.old_package_hash = {0xAB, 0xCD};
    auto canonical = build_migration_canonical(spec);
    auto bytes = wrap_signed_partition(
        canonical, VMPilot::DomainLabels::Auth::MigrationToken);
    InMemoryNonceStore store;

    auto result = accept_migration_token(
        bytes.data(), bytes.size(), trust_root(),
        baseline_migration_context(), store);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), TokenError::PackageHashMismatch);
}

TEST(MigrationToken, WrongTargetPackageRejected) {
    auto spec = baseline_migration();
    spec.new_package_hash = {0xAB, 0xCD};
    auto canonical = build_migration_canonical(spec);
    auto bytes = wrap_signed_partition(
        canonical, VMPilot::DomainLabels::Auth::MigrationToken);
    InMemoryNonceStore store;

    auto result = accept_migration_token(
        bytes.data(), bytes.size(), trust_root(),
        baseline_migration_context(), store);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), TokenError::PackageHashMismatch);
}

TEST(MigrationToken, ImportOnceFalseRejected) {
    auto spec = baseline_migration();
    spec.allowed_import_once = false;
    auto canonical = build_migration_canonical(spec);
    auto bytes = wrap_signed_partition(
        canonical, VMPilot::DomainLabels::Auth::MigrationToken);
    InMemoryNonceStore store;

    auto result = accept_migration_token(
        bytes.data(), bytes.size(), trust_root(),
        baseline_migration_context(), store);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), TokenError::AllowedImportOnceNotTrue);
}

TEST(MigrationToken, PolicyFloorBelowRequiredRejected) {
    auto spec = baseline_migration();
    spec.allowed_policy_floor = VMPilot::DomainLabels::PolicyId::Debug;
    auto canonical = build_migration_canonical(spec);
    auto bytes = wrap_signed_partition(
        canonical, VMPilot::DomainLabels::Auth::MigrationToken);
    InMemoryNonceStore store;

    auto ctx = baseline_migration_context();
    ctx.required_policy_floor = PolicyId::Standard;

    auto result = accept_migration_token(
        bytes.data(), bytes.size(), trust_root(), ctx, store);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), TokenError::PolicyFloorBelowRequired);
}

TEST(MigrationToken, TokenSignedUnderReprovisionDomainRejected_doc10_9) {
    // Migration and Reprovision tokens must never satisfy each other:
    // a token signed under reprovision-token-v1 should be rejected when
    // fed to the migration verifier.
    auto canonical = build_migration_canonical(baseline_migration());
    auto bytes = wrap_signed_partition(
        canonical, VMPilot::DomainLabels::Auth::ReprovisionToken);
    InMemoryNonceStore store;

    auto result = accept_migration_token(
        bytes.data(), bytes.size(), trust_root(),
        baseline_migration_context(), store);
    ASSERT_FALSE(result.has_value());
}
