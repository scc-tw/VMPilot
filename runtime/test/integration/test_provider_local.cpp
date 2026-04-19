// Stage 10 — TrustProvider consumer interface coverage.
//
// Verifies doc 14 §10's required behaviour for 1.0:
//   - LocalEmbeddedProvider reports a capability statement that matches
//     its tier-zero nature (no hardware, no remote attestation)
//   - evaluate_policy_requirement() rejects requirements that exceed
//     what the installed provider can prove
//   - evidence bound to mismatched package / profile / requirement
//     hashes is rejected outright
//   - a degraded / not_satisfied appraisal never masquerades as a pass
//   - appraise() refuses evidence whose declared provider_class
//     disagrees with the capability statement

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "provider.hpp"

#include "fixtures/cbor_test_encoder.hpp"
#include "fixtures/fixture_generator.hpp"

namespace {

using VMPilot::DomainLabels::FamilyId;
using VMPilot::DomainLabels::PolicyId;
using VMPilot::Runtime::Provider::appraise;
using VMPilot::Runtime::Provider::AttestationFormat;
using VMPilot::Runtime::Provider::CapabilityStatement;
using VMPilot::Runtime::Provider::CloneResistanceClass;
using VMPilot::Runtime::Provider::evaluate_policy_requirement;
using VMPilot::Runtime::Provider::FreshnessClass;
using VMPilot::Runtime::Provider::KeyCustodyClass;
using VMPilot::Runtime::Provider::LocalEmbeddedProvider;
using VMPilot::Runtime::Provider::parse_policy_requirement;
using VMPilot::Runtime::Provider::PolicyRequirement;
using VMPilot::Runtime::Provider::PolicyRequirementParseError;
using VMPilot::Runtime::Provider::policy_requirement_hash;
using VMPilot::Runtime::Provider::PrivacyModel;
using VMPilot::Runtime::Provider::ProviderClass;
using VMPilot::Runtime::Provider::ProviderError;
using VMPilot::Runtime::Provider::ProviderEvidence;
using VMPilot::Runtime::Provider::ProviderStatus;
using VMPilot::Runtime::Provider::RecoveryModel;
using VMPilot::Runtime::Provider::VerifiedArtifactContext;

PolicyRequirement baseline_requirement() {
    PolicyRequirement req{};
    req.requirement_version = "requirement-v1";
    req.required_policy_floor = PolicyId::Standard;
    req.required_family_set = {FamilyId::F1};
    req.require_hardware_bound = false;
    req.require_non_exportable_key = false;
    req.require_online_freshness = false;
    req.require_remote_attestation = false;
    req.require_recovery_model = RecoveryModel::SelfService;
    req.allowed_provider_classes = {ProviderClass::LocalEmbedded};
    req.minimum_provider_epoch = 0;
    return req;
}

// The runtime does not serialize PolicyRequirement; the producer
// does. For tests that build a struct and then want its canonical
// hash, round-trip the struct through the fixture encoder's
// PolicyRequirementSpec. After the enum-class refactor the two
// types line up field-for-field, so the helper is a direct copy.
VMPilot::Fixtures::PolicyRequirementSpec req_to_spec(
    const PolicyRequirement& r) {
    VMPilot::Fixtures::PolicyRequirementSpec s;
    s.requirement_version        = r.requirement_version;
    s.required_policy_floor      = r.required_policy_floor;
    s.required_family_set        = r.required_family_set;
    s.require_hardware_bound     = r.require_hardware_bound;
    s.require_non_exportable_key = r.require_non_exportable_key;
    s.require_online_freshness   = r.require_online_freshness;
    s.require_remote_attestation = r.require_remote_attestation;
    s.require_recovery_model     = r.require_recovery_model;
    s.allowed_provider_classes   = r.allowed_provider_classes;
    s.minimum_provider_epoch     = r.minimum_provider_epoch;
    return s;
}

std::array<std::uint8_t, 32> hash_of(const PolicyRequirement& r) {
    const auto bytes =
        VMPilot::Fixtures::encode_policy_requirement(req_to_spec(r));
    return policy_requirement_hash(bytes);
}

VerifiedArtifactContext make_ctx(const PolicyRequirement& req) {
    VerifiedArtifactContext ctx{};
    ctx.package_binding_record_hash = {0xA0, 0xA1, 0xA2, 0xA3};
    ctx.resolved_profile_table_hash = {0xB0, 0xB1, 0xB2, 0xB3};
    ctx.policy_requirement_hash = hash_of(req);
    return ctx;
}

}  // namespace

TEST(Provider, LocalEmbeddedCapabilityStatementMatchesTierZero) {
    LocalEmbeddedProvider provider;
    const auto caps = provider.get_capabilities();

    EXPECT_EQ(caps.provider_class, ProviderClass::LocalEmbedded);
    EXPECT_FALSE(caps.hardware_bound);
    EXPECT_FALSE(caps.non_exportable_key);
    EXPECT_FALSE(caps.online_required);
    EXPECT_TRUE(caps.migratable);
    EXPECT_EQ(caps.clone_resistance_class, CloneResistanceClass::None);
    EXPECT_EQ(caps.freshness_class, FreshnessClass::None);
    EXPECT_EQ(caps.attestation_format, AttestationFormat::None);
    EXPECT_EQ(caps.key_custody_class, KeyCustodyClass::Self);
    EXPECT_EQ(caps.recovery_model, RecoveryModel::SelfService);
    EXPECT_EQ(caps.privacy_model, PrivacyModel::Pairwise);
    EXPECT_NE(std::find(caps.supported_policy_floors.begin(),
                        caps.supported_policy_floors.end(),
                        PolicyId::Standard),
              caps.supported_policy_floors.end());
    EXPECT_EQ(std::find(caps.supported_policy_floors.begin(),
                        caps.supported_policy_floors.end(),
                        PolicyId::HighSec),
              caps.supported_policy_floors.end());
}

TEST(Provider, LocalEmbeddedSatisfiesMinimalRequirement_doc14_10_2) {
    LocalEmbeddedProvider provider;
    const auto req = baseline_requirement();
    const auto ctx = make_ctx(req);

    auto result = evaluate_policy_requirement(provider, req, ctx,
                                              ctx.policy_requirement_hash);
    ASSERT_TRUE(result.has_value())
        << "err=" << static_cast<int>(result.error());
    EXPECT_EQ(result->status, ProviderStatus::Satisfied);
    EXPECT_EQ(result->provider_class, ProviderClass::LocalEmbedded);
    EXPECT_EQ(result->nonce, ctx.policy_requirement_hash);
}

TEST(Provider, LocalEmbeddedRejectsHighsecFloor_doc14_10_1) {
    LocalEmbeddedProvider provider;
    auto req = baseline_requirement();
    req.required_policy_floor = PolicyId::HighSec;
    const auto ctx = make_ctx(req);

    auto result = evaluate_policy_requirement(provider, req, ctx,
                                              ctx.policy_requirement_hash);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(),
              ProviderError::ProviderRequirementNotSatisfied);
}

TEST(Provider, LocalEmbeddedRejectsHardwareBoundRequirement) {
    LocalEmbeddedProvider provider;
    auto req = baseline_requirement();
    req.require_hardware_bound = true;
    const auto ctx = make_ctx(req);

    auto result = evaluate_policy_requirement(provider, req, ctx,
                                              ctx.policy_requirement_hash);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(),
              ProviderError::ProviderRequirementNotSatisfied);
}

TEST(Provider, LocalEmbeddedRejectsRemoteAttestationRequirement) {
    LocalEmbeddedProvider provider;
    auto req = baseline_requirement();
    req.require_remote_attestation = true;
    const auto ctx = make_ctx(req);

    auto result = evaluate_policy_requirement(provider, req, ctx,
                                              ctx.policy_requirement_hash);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(),
              ProviderError::ProviderRequirementNotSatisfied);
}

TEST(Provider, LocalEmbeddedRejectsDisallowedProviderClass_doc14_10_3) {
    LocalEmbeddedProvider provider;
    auto req = baseline_requirement();
    req.allowed_provider_classes = {ProviderClass::LocalTpm};
    const auto ctx = make_ctx(req);

    auto result = evaluate_policy_requirement(provider, req, ctx,
                                              ctx.policy_requirement_hash);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(),
              ProviderError::ProviderRequirementNotSatisfied);
}

TEST(Provider, AppraisalRejectsEvidenceBoundToWrongPackage_doc14_10_8) {
    LocalEmbeddedProvider provider;
    const auto caps = provider.get_capabilities();
    const auto req = baseline_requirement();
    const auto ctx = make_ctx(req);

    ProviderEvidence ev{};
    ev.evidence_version = "evidence-v1";
    ev.provider_class = ProviderClass::LocalEmbedded;
    ev.provider_instance_pseudonym = caps.provider_instance_pseudonym;
    ev.nonce = ctx.policy_requirement_hash;
    ev.package_binding_record_hash = {0xFF};  // different package
    ev.resolved_profile_table_hash = ctx.resolved_profile_table_hash;
    ev.policy_requirement_hash = ctx.policy_requirement_hash;

    const auto result = appraise(caps, req, ev, ctx);
    EXPECT_EQ(result.status, ProviderStatus::NotSatisfied);
}

TEST(Provider, AppraisalRejectsEvidenceClaimingDifferentProviderClass_doc14_10_3) {
    LocalEmbeddedProvider provider;
    const auto caps = provider.get_capabilities();
    const auto req = baseline_requirement();
    const auto ctx = make_ctx(req);

    ProviderEvidence ev{};
    ev.evidence_version = "evidence-v1";
    ev.provider_class = ProviderClass::LocalTpm;  // masquerade
    ev.provider_instance_pseudonym = caps.provider_instance_pseudonym;
    ev.nonce = ctx.policy_requirement_hash;
    ev.package_binding_record_hash = ctx.package_binding_record_hash;
    ev.resolved_profile_table_hash = ctx.resolved_profile_table_hash;
    ev.policy_requirement_hash = ctx.policy_requirement_hash;

    const auto result = appraise(caps, req, ev, ctx);
    EXPECT_EQ(result.status, ProviderStatus::NotSatisfied);
}

TEST(Provider, AppraisalRejectsEvidenceWithMismatchedRequirementHash) {
    LocalEmbeddedProvider provider;
    const auto caps = provider.get_capabilities();
    const auto req = baseline_requirement();
    auto ctx = make_ctx(req);

    // Build evidence against a different requirement, so its
    // policy_requirement_hash no longer matches what the ctx commits to.
    auto alt_req = req;
    alt_req.minimum_provider_epoch = 99;
    const auto alt_hash = hash_of(alt_req);

    ProviderEvidence ev{};
    ev.evidence_version = "evidence-v1";
    ev.provider_class = ProviderClass::LocalEmbedded;
    ev.provider_instance_pseudonym = caps.provider_instance_pseudonym;
    ev.nonce = ctx.policy_requirement_hash;
    ev.package_binding_record_hash = ctx.package_binding_record_hash;
    ev.resolved_profile_table_hash = ctx.resolved_profile_table_hash;
    ev.policy_requirement_hash = alt_hash;

    const auto result = appraise(caps, req, ev, ctx);
    EXPECT_EQ(result.status, ProviderStatus::NotSatisfied);
}

TEST(Provider, PolicyRequirementHashIsDeterministicAndDomainSeparated) {
    auto req1 = baseline_requirement();
    auto req2 = baseline_requirement();
    EXPECT_EQ(hash_of(req1), hash_of(req2));

    req2.required_policy_floor = PolicyId::HighSec;
    EXPECT_NE(hash_of(req1), hash_of(req2));

    req2 = baseline_requirement();
    req2.minimum_provider_epoch = 1;
    EXPECT_NE(hash_of(req1), hash_of(req2));

    req2 = baseline_requirement();
    req2.required_family_set = {FamilyId::F2};
    EXPECT_NE(hash_of(req1), hash_of(req2));

    req2 = baseline_requirement();
    req2.allowed_provider_classes = {ProviderClass::LocalTpm};
    EXPECT_NE(hash_of(req1), hash_of(req2));
}

TEST(Provider, ProviderSwitchDoesNotChangeUnitBindingVerification_doc14_10_5) {
    // A provider swap should not influence artifact authenticity; two
    // providers with different pseudonyms produce different
    // CapabilityStatement objects but the PolicyRequirement hash over
    // the same requirement remains identical. Runtimes that cache the
    // requirement hash per-package can therefore swap providers without
    // replaying PBR/UBR verification.
    LocalEmbeddedProvider provider_a("instance-a");
    LocalEmbeddedProvider provider_b("instance-b");

    const auto req = baseline_requirement();
    const auto hash_a = hash_of(req);
    const auto hash_b = hash_of(req);
    EXPECT_EQ(hash_a, hash_b);

    const auto caps_a = provider_a.get_capabilities();
    const auto caps_b = provider_b.get_capabilities();
    EXPECT_NE(caps_a.provider_instance_pseudonym,
              caps_b.provider_instance_pseudonym);
    EXPECT_EQ(caps_a.provider_class, caps_b.provider_class);
}

// ── Parser negative tests (strict CBOR schema enforcement) ────────────────

namespace {

std::vector<std::uint8_t> baseline_requirement_bytes() {
    return VMPilot::Fixtures::encode_policy_requirement(
        VMPilot::Fixtures::PolicyRequirementSpec{});
}

}  // namespace

TEST(PolicyRequirementParse, HappyPathRoundTrips) {
    const auto bytes = baseline_requirement_bytes();
    auto parsed = parse_policy_requirement(bytes);
    ASSERT_TRUE(parsed.has_value())
        << "err=" << static_cast<int>(parsed.error());
    EXPECT_EQ(parsed->requirement_version, "policy-requirement-v1");
    EXPECT_EQ(parsed->required_policy_floor, PolicyId::Standard);
    EXPECT_EQ(parsed->required_family_set,
              (std::vector<FamilyId>{FamilyId::F1}));
    EXPECT_EQ(parsed->allowed_provider_classes,
              (std::vector<ProviderClass>{ProviderClass::LocalEmbedded}));
    EXPECT_EQ(parsed->minimum_provider_epoch, 0u);
}

TEST(PolicyRequirementParse, UnknownRequirementVersionRejected) {
    VMPilot::Fixtures::PolicyRequirementSpec s;
    s.requirement_version = "policy-requirement-v99";
    const auto bytes = VMPilot::Fixtures::encode_policy_requirement(s);
    auto parsed = parse_policy_requirement(bytes);
    ASSERT_FALSE(parsed.has_value());
    EXPECT_EQ(parsed.error(),
              PolicyRequirementParseError::UnsupportedRequirementVersion);
}

TEST(PolicyRequirementParse, TrailingBytesRejected) {
    auto bytes = baseline_requirement_bytes();
    bytes.push_back(0x00);  // trailing garbage
    auto parsed = parse_policy_requirement(bytes);
    ASSERT_FALSE(parsed.has_value());
    EXPECT_EQ(parsed.error(), PolicyRequirementParseError::BadCbor);
}

TEST(PolicyRequirementParse, OversizedFamilySetRejected) {
    // Length 4 exceeds the hard cap (3). Enum-safe; no unknown values.
    VMPilot::Fixtures::PolicyRequirementSpec s;
    s.required_family_set = {FamilyId::F1, FamilyId::F2, FamilyId::F3,
                             FamilyId::F1};
    const auto bytes = VMPilot::Fixtures::encode_policy_requirement(s);
    auto parsed = parse_policy_requirement(bytes);
    ASSERT_FALSE(parsed.has_value());
    EXPECT_EQ(parsed.error(), PolicyRequirementParseError::ArrayTooLong);
}

TEST(PolicyRequirementParse, OversizedAllowedProviderClassesRejected) {
    // Length 9 exceeds the hard cap (8). Enum-safe.
    VMPilot::Fixtures::PolicyRequirementSpec s;
    s.allowed_provider_classes = {
        ProviderClass::LocalEmbedded, ProviderClass::LocalTpm,
        ProviderClass::LocalTee,      ProviderClass::CloudAttestedVm,
        ProviderClass::CloudHsm,      ProviderClass::ExternalKms,
        ProviderClass::LocalEmbedded, ProviderClass::LocalTpm,
        ProviderClass::LocalTee};
    const auto bytes = VMPilot::Fixtures::encode_policy_requirement(s);
    auto parsed = parse_policy_requirement(bytes);
    ASSERT_FALSE(parsed.has_value());
    EXPECT_EQ(parsed.error(), PolicyRequirementParseError::ArrayTooLong);
}

TEST(PolicyRequirementParse, UnknownFamilyIdRejected) {
    // Needs a raw-string injection: the production enum class has no
    // value for "f7", so this negative test hand-builds the CBOR.
    using namespace VMPilot::Fixtures::Cbor;
    std::vector<std::vector<std::uint8_t>> family_items{encode_text("f7")};
    std::vector<std::vector<std::uint8_t>> class_items{
        encode_text("local_embedded")};
    MapBuilder m;
    m.put_uint(1,  encode_text("policy-requirement-v1"));
    m.put_uint(2,  encode_uint(2));
    m.put_uint(3,  encode_array(family_items));
    m.put_uint(4,  encode_uint(0));
    m.put_uint(5,  encode_uint(0));
    m.put_uint(6,  encode_uint(0));
    m.put_uint(7,  encode_uint(0));
    m.put_uint(8,  encode_uint(1));
    m.put_uint(9,  encode_array(class_items));
    m.put_uint(10, encode_uint(0));
    auto parsed = parse_policy_requirement(m.build());
    ASSERT_FALSE(parsed.has_value());
    EXPECT_EQ(parsed.error(), PolicyRequirementParseError::UnknownEnumValue);
}

TEST(PolicyRequirementParse, UnknownProviderClassRejected) {
    // Same raw-string injection idiom for an unknown ProviderClass.
    using namespace VMPilot::Fixtures::Cbor;
    std::vector<std::vector<std::uint8_t>> family_items{encode_text("f1")};
    std::vector<std::vector<std::uint8_t>> class_items{
        encode_text("chewy_hardware_dongle")};
    MapBuilder m;
    m.put_uint(1,  encode_text("policy-requirement-v1"));
    m.put_uint(2,  encode_uint(2));
    m.put_uint(3,  encode_array(family_items));
    m.put_uint(4,  encode_uint(0));
    m.put_uint(5,  encode_uint(0));
    m.put_uint(6,  encode_uint(0));
    m.put_uint(7,  encode_uint(0));
    m.put_uint(8,  encode_uint(1));
    m.put_uint(9,  encode_array(class_items));
    m.put_uint(10, encode_uint(0));
    auto parsed = parse_policy_requirement(m.build());
    ASSERT_FALSE(parsed.has_value());
    EXPECT_EQ(parsed.error(), PolicyRequirementParseError::UnknownEnumValue);
}

TEST(PolicyRequirementParse, UnknownCoreFieldRejected) {
    // Re-encode baseline with an extra key-value pair that's not in
    // the schema. MapBuilder preserves insertion order; we insert a
    // key > all known keys to keep canonical order valid.
    using namespace VMPilot::Fixtures::Cbor;
    auto baseline = baseline_requirement_bytes();
    auto parsed_baseline = parse_policy_requirement(baseline);
    ASSERT_TRUE(parsed_baseline.has_value());

    VMPilot::Fixtures::PolicyRequirementSpec s;
    // Build a map with a stray key 99.
    std::vector<std::vector<std::uint8_t>> family_items{encode_text("f1")};
    std::vector<std::vector<std::uint8_t>> class_items{
        encode_text("local_embedded")};
    MapBuilder m;
    m.put_uint(1,  encode_text("policy-requirement-v1"));
    m.put_uint(2,  encode_uint(2));
    m.put_uint(3,  encode_array(family_items));
    m.put_uint(4,  encode_uint(0));
    m.put_uint(5,  encode_uint(0));
    m.put_uint(6,  encode_uint(0));
    m.put_uint(7,  encode_uint(0));
    m.put_uint(8,  encode_uint(1));
    m.put_uint(9,  encode_array(class_items));
    m.put_uint(10, encode_uint(0));
    m.put_uint(99, encode_uint(0));  // unknown key
    const auto bytes = m.build();
    auto parsed = parse_policy_requirement(bytes);
    ASSERT_FALSE(parsed.has_value());
    EXPECT_EQ(parsed.error(), PolicyRequirementParseError::UnknownCoreField);
}

TEST(PolicyRequirementParse, WrongFieldTypeRejected) {
    using namespace VMPilot::Fixtures::Cbor;
    // required_policy_floor (key 2) must be uint; pass text.
    std::vector<std::vector<std::uint8_t>> family_items{encode_text("f1")};
    std::vector<std::vector<std::uint8_t>> class_items{
        encode_text("local_embedded")};
    MapBuilder m;
    m.put_uint(1,  encode_text("policy-requirement-v1"));
    m.put_uint(2,  encode_text("standard"));  // WRONG: schema wants uint
    m.put_uint(3,  encode_array(family_items));
    m.put_uint(4,  encode_uint(0));
    m.put_uint(5,  encode_uint(0));
    m.put_uint(6,  encode_uint(0));
    m.put_uint(7,  encode_uint(0));
    m.put_uint(8,  encode_uint(1));
    m.put_uint(9,  encode_array(class_items));
    m.put_uint(10, encode_uint(0));
    const auto bytes = m.build();
    auto parsed = parse_policy_requirement(bytes);
    ASSERT_FALSE(parsed.has_value());
    EXPECT_EQ(parsed.error(), PolicyRequirementParseError::WrongFieldType);
}
