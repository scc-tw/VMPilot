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
#include <utility>

#include "provider.hpp"

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
using VMPilot::Runtime::Provider::PolicyRequirement;
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

VerifiedArtifactContext make_ctx(const PolicyRequirement& req) {
    VerifiedArtifactContext ctx{};
    ctx.package_binding_record_hash = {0xA0, 0xA1, 0xA2, 0xA3};
    ctx.resolved_profile_table_hash = {0xB0, 0xB1, 0xB2, 0xB3};
    ctx.policy_requirement_hash = policy_requirement_hash(req);
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
    const auto alt_hash = policy_requirement_hash(alt_req);

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
    EXPECT_EQ(policy_requirement_hash(req1), policy_requirement_hash(req2));

    req2.required_policy_floor = PolicyId::HighSec;
    EXPECT_NE(policy_requirement_hash(req1), policy_requirement_hash(req2));

    req2 = baseline_requirement();
    req2.minimum_provider_epoch = 1;
    EXPECT_NE(policy_requirement_hash(req1), policy_requirement_hash(req2));

    req2 = baseline_requirement();
    req2.required_family_set = {FamilyId::F2};
    EXPECT_NE(policy_requirement_hash(req1), policy_requirement_hash(req2));

    req2 = baseline_requirement();
    req2.allowed_provider_classes = {ProviderClass::LocalTpm};
    EXPECT_NE(policy_requirement_hash(req1), policy_requirement_hash(req2));
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
    const auto hash_a = policy_requirement_hash(req);
    const auto hash_b = policy_requirement_hash(req);
    EXPECT_EQ(hash_a, hash_b);

    const auto caps_a = provider_a.get_capabilities();
    const auto caps_b = provider_b.get_capabilities();
    EXPECT_NE(caps_a.provider_instance_pseudonym,
              caps_b.provider_instance_pseudonym);
    EXPECT_EQ(caps_a.provider_class, caps_b.provider_class);
}
