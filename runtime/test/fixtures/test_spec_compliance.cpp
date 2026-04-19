// Spec-required acceptance tests flagged by doc 08 §9 and doc 15 §9
// that were missing from Stages 5-8 coverage.
//
// Each TEST cites the exact doc line it satisfies. Test helpers reuse
// the regular fixture builders — no hand-rolled CBOR here — so a
// negative result can only come from a runtime rule firing, not from
// producing the wrong bytes.

#include <gtest/gtest.h>

#include <array>
#include <climits>
#include <cstdint>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "binding/package.hpp"
#include "binding/unit.hpp"
#include "cbor/strict.hpp"
#include "envelope/outer.hpp"
#include "registry/registry.hpp"
#include "trust_root.hpp"
#include "vm/domain_labels.hpp"
#include "vm/family_policy.hpp"
#include "vm_stub_entry.hpp"

#include "fixtures/fixture_generator.hpp"

#include "test_blob_builder.hpp"

namespace {

using VMPilot::Common::VM::VmOpcode;
using VMPilot::DomainLabels::FamilyId;
using VMPilot::DomainLabels::PolicyId;
using VMPilot::Runtime::VM_STUB_ARTIFACT_ABI_VERSION;
using VMPilot::Runtime::VmStubArtifactArgs;
using VMPilot::Runtime::Binding::accept_package;
using VMPilot::Runtime::Binding::accept_unit_entry;
using VMPilot::Runtime::Binding::AcceptConfig;
using VMPilot::Runtime::Binding::AcceptedPackage;
using VMPilot::Runtime::Binding::RuntimeEpochState;
using VMPilot::Runtime::Binding::UnitAcceptError;
using VMPilot::Runtime::Envelope::parse_outer_envelope;
using VMPilot::Test::build_test_blob;
using VMPilot::Test::fill_seed;
using VMPilot::Test::make_test_bb;
using VMPilot::Test::TestInstruction;

AcceptConfig default_config() {
    return AcceptConfig{
        {"package-schema-v1"},
        {"canonical-metadata-bytes-v1"},
        RuntimeEpochState{2, 1},
        PolicyId::Debug,
    };
}

struct AcceptedBundle {
    VMPilot::Fixtures::PackageArtifactAssembly art;
    AcceptedPackage pkg;
    VMPilot::Runtime::Envelope::OuterEnvelope env;
};

AcceptedBundle accept_bundle(VMPilot::Fixtures::PackageArtifactAssembly art,
                             const AcceptConfig& cfg = default_config()) {
    auto env = parse_outer_envelope(art.bytes);
    EXPECT_TRUE(env.has_value());
    auto pkg = accept_package(art.bytes.data(), art.bytes.size(),
                              VMPilot::Runtime::trust_root(), *env, cfg);
    EXPECT_TRUE(pkg.has_value());
    return {std::move(art), std::move(*pkg), std::move(*env)};
}

std::array<std::uint8_t, 32> default_unit_identity() {
    return {
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
        0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5,
        0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    };
}

std::vector<std::uint8_t> make_halting_blob(bool debug_mode,
                                            std::uint8_t seed_base = 1) {
    std::uint8_t seed[32];
    fill_seed(seed, seed_base);
    const TestInstruction halt{VmOpcode::HALT, 0, 0, 0, 0};
    auto bb = make_test_bb(1, 0x50, {halt});
    return build_test_blob(seed, {bb}, {}, debug_mode);
}

VMPilot::Fixtures::PackageArtifactAssembly build_vm_payload_artifact(
    std::string_view family_id, std::string_view policy_id,
    const std::vector<std::uint8_t>& payload,
    std::string_view profile_id = "rfp-vm",
    std::string_view runtime_specialization_id = {},
    std::array<std::uint8_t, 32> provider_requirement_hash = {}) {
    using namespace VMPilot::Fixtures;

    const auto unit_identity = default_unit_identity();
    const auto payload_hash = sha256_of(payload);
    const std::string profile_id_s(profile_id);
    const std::string family_id_s(family_id);
    const std::string policy_id_s(policy_id);
    const std::string runtime_spec_id =
        runtime_specialization_id.empty()
            ? family_id_s + "-" + policy_id_s + "-v1"
            : std::string(runtime_specialization_id);

    const auto desc = UnitDescriptorBuilder{}
                          .unit_id("u-happy")
                          .unit_identity_hash(unit_identity)
                          .family_id(VMPilot::DomainLabels::parse_family_id(family_id_s).value())
                          .requested_policy_id(VMPilot::DomainLabels::parse_policy_id(policy_id_s).value())
                          .resolved_family_profile_id(profile_id_s)
                          .unit_binding_record_id("ubr-happy")
                          .payload_sha256(payload_hash)
                          .payload_size(payload.size())
                          .build();
    const auto prof = ResolvedFamilyProfileBuilder{}
                          .profile_id(profile_id_s)
                          .family_id(VMPilot::DomainLabels::parse_family_id(family_id_s).value())
                          .requested_policy_id(VMPilot::DomainLabels::parse_policy_id(policy_id_s).value())
                          .runtime_specialization_id(runtime_spec_id)
                          .build();

    const auto desc_hash = VMPilot::Cbor::domain_hash_sha256(
        VMPilot::DomainLabels::Hash::UnitDescriptor, desc);
    const auto prof_hash = VMPilot::Cbor::domain_hash_sha256(
        VMPilot::DomainLabels::Hash::ResolvedFamilyProfile, prof);

    UnitBindingAuthSpec auth;
    const auto ubr = UnitBindingRecordBuilder{}
                         .unit_binding_record_id("ubr-happy")
                         .unit_identity_hash(unit_identity)
                         .unit_descriptor_hash(desc_hash)
                         .family_id(VMPilot::DomainLabels::parse_family_id(family_id_s).value())
                         .requested_policy_id(VMPilot::DomainLabels::parse_policy_id(policy_id_s).value())
                         .resolved_family_profile_id(profile_id_s)
                         .resolved_family_profile_content_hash(prof_hash)
                         .payload_sha256(payload_hash)
                         .payload_size(payload.size())
                         .anti_downgrade_epoch(1)
                         .binding_auth(auth)
                         .build();

    RegistryEntrySpec e;
    e.runtime_specialization_id = runtime_spec_id;
    e.family_id =
        VMPilot::DomainLabels::parse_family_id(family_id_s).value();
    e.requested_policy_id =
        VMPilot::DomainLabels::parse_policy_id(policy_id_s).value();
    e.profile_revision = "rev1";
    e.provider_requirement_hash = provider_requirement_hash;
    e.accepted_profile_content_hash = prof_hash;
    const auto registry = RuntimeSpecializationRegistryBuilder{}
                              .clear_entries()
                              .add_entry(e)
                              .build();

    return PackageArtifactBuilder{}
        .payload_bytes(payload)
        .unit_binding_table_bytes(build_unit_binding_table_bytes({ubr}))
        .unit_descriptor_table_bytes(
            build_unit_descriptor_table_bytes({{"u-happy", desc}}))
        .resolved_profile_table_bytes(
            build_resolved_profile_table_bytes({{profile_id_s, prof}}))
        .registry_bytes(registry)
        .build();
}

int64_t run_artifact_entry(
    const VMPilot::Fixtures::PackageArtifactAssembly& art,
    const char* unit_id = "u-happy") {
    std::uint8_t stored_seed[32];
    fill_seed(stored_seed, 1);
    std::array<std::uint64_t, 16> regs{};

    VmStubArtifactArgs args{};
    args.version = VM_STUB_ARTIFACT_ABI_VERSION;
    args.num_regs = static_cast<std::uint32_t>(regs.size());
    args.load_base_delta = 0;
    args.artifact_data = art.bytes.data();
    args.artifact_size = art.bytes.size();
    args.unit_id = unit_id;
    args.stored_seed = stored_seed;
    args.initial_regs = regs.data();
    return vm_stub_entry_artifact(&args);
}

::testing::AssertionResult same_public_shape(
    const VMPilot::Runtime::Envelope::OuterEnvelope& lhs,
    const VMPilot::Runtime::Envelope::OuterEnvelope& rhs,
    std::string_view lhs_label, std::string_view rhs_label) {
    if (lhs.section_table_shape_class != rhs.section_table_shape_class) {
        return ::testing::AssertionFailure()
               << lhs_label
               << ".section_table_shape_class=" << lhs.section_table_shape_class
               << " != " << rhs_label << ".section_table_shape_class="
               << rhs.section_table_shape_class;
    }
    if (lhs.package_schema_version != rhs.package_schema_version) {
        return ::testing::AssertionFailure()
               << lhs_label << ".package_schema_version='"
               << lhs.package_schema_version << "' != " << rhs_label
               << ".package_schema_version='" << rhs.package_schema_version
               << "'";
    }
    if (lhs.canonical_encoding_id != rhs.canonical_encoding_id) {
        return ::testing::AssertionFailure()
               << lhs_label << ".canonical_encoding_id='"
               << lhs.canonical_encoding_id << "' != " << rhs_label
               << ".canonical_encoding_id='" << rhs.canonical_encoding_id
               << "'";
    }
    return ::testing::AssertionSuccess();
}

// Build an artifact whose descriptor + UBR + registry entry all agree
// on (family, policy). Tests vary one field at a time to exercise
// mismatch rejects.
VMPilot::Fixtures::PackageArtifactAssembly build_aligned_artifact(
    PolicyId policy_enum, std::string_view policy_text,
    std::string_view profile_id = "rfp-aligned") {
    using namespace VMPilot::Fixtures;

    const std::array<std::uint8_t, 32> unit_identity{
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
        0xDB, 0xDC, 0xDD, 0xDE, 0xDF, 0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5,
        0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    };
    (void)policy_enum;

    const std::vector<std::uint8_t> payload(96, 0xCD);
    const auto sha = VMPilot::Fixtures::sha256_of(payload);

    const auto desc = UnitDescriptorBuilder{}
                          .unit_id("u-happy")
                          .unit_identity_hash(unit_identity)
                          .family_id(VMPilot::DomainLabels::FamilyId::F1)
                          .requested_policy_id(VMPilot::DomainLabels::parse_policy_id(policy_text).value())
                          .resolved_family_profile_id(std::string(profile_id))
                          .unit_binding_record_id("ubr-happy")
                          .payload_sha256(sha)
                          .payload_size(payload.size())
                          .build();
    const auto prof = ResolvedFamilyProfileBuilder{}
                          .profile_id(std::string(profile_id))
                          .family_id(VMPilot::DomainLabels::FamilyId::F1)
                          .requested_policy_id(VMPilot::DomainLabels::parse_policy_id(policy_text).value())
                          .build();

    const auto desc_hash = VMPilot::Cbor::domain_hash_sha256(
        VMPilot::DomainLabels::Hash::UnitDescriptor, desc);
    const auto prof_hash = VMPilot::Cbor::domain_hash_sha256(
        VMPilot::DomainLabels::Hash::ResolvedFamilyProfile, prof);

    UnitBindingAuthSpec auth;
    const auto ubr = UnitBindingRecordBuilder{}
                         .unit_binding_record_id("ubr-happy")
                         .unit_identity_hash(unit_identity)
                         .unit_descriptor_hash(desc_hash)
                         .family_id(VMPilot::DomainLabels::FamilyId::F1)
                         .requested_policy_id(VMPilot::DomainLabels::parse_policy_id(policy_text).value())
                         .resolved_family_profile_id(std::string(profile_id))
                         .resolved_family_profile_content_hash(prof_hash)
                         .payload_sha256(sha)
                         .payload_size(payload.size())
                         .anti_downgrade_epoch(1)
                         .binding_auth(auth)
                         .build();

    // Registry entry matching the profile's content.
    RegistryEntrySpec e;
    e.runtime_specialization_id = "f1-" + std::string(policy_text) + "-v1";
    e.family_id = VMPilot::DomainLabels::FamilyId::F1;
    e.requested_policy_id = policy_enum;
    e.profile_revision = "rev1";
    e.accepted_profile_content_hash = prof_hash;
    const auto registry = RuntimeSpecializationRegistryBuilder{}
                              .clear_entries()
                              .add_entry(e)
                              .build();

    return PackageArtifactBuilder{}
        .payload_bytes(payload)
        .unit_binding_table_bytes(build_unit_binding_table_bytes({ubr}))
        .unit_descriptor_table_bytes(
            build_unit_descriptor_table_bytes({{"u-happy", desc}}))
        .resolved_profile_table_bytes(build_resolved_profile_table_bytes(
            {{std::string(profile_id), prof}}))
        .registry_bytes(registry)
        .build();
}

}  // namespace

// ─── doc 08 §9 #1: standard artifact to highsec unit entry → reject ─────

TEST(SpecCompliance, StandardArtifactRejectedByHighsecRuntime_doc08_9_1) {
    // Build a fully self-consistent standard-policy artifact; then run
    // acceptance under a runtime config whose minimum_policy_floor is
    // HighSec. UBR.requested_policy_id == Standard < floor HighSec → reject.
    auto art = build_aligned_artifact(PolicyId::Standard, "standard");
    AcceptConfig cfg = default_config();
    cfg.minimum_policy_floor = PolicyId::HighSec;

    auto b = accept_bundle(std::move(art), cfg);
    auto r = accept_unit_entry(b.art.bytes.data(), b.art.bytes.size(), b.env,
                               b.pkg, "u-happy", cfg);
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), UnitAcceptError::PolicyBelowRuntimeFloor);
}

// ─── doc 08 §9 #2: debug profile id + highsec payload → reject ──────────

TEST(SpecCompliance, DebugProfileWithHighsecUbrRejected_doc08_9_2) {
    // Construct a fixture where the payload hash is bound into a
    // highsec UBR/descriptor pair, but the resolved profile itself
    // still declares debug. This is the concrete "debug profile id with
    // highsec payload hash" split the spec forbids.
    using namespace VMPilot::Fixtures;

    const auto unit_identity = default_unit_identity();
    const auto payload = make_halting_blob(false);
    const auto payload_hash = VMPilot::Fixtures::sha256_of(payload);

    const auto desc = UnitDescriptorBuilder{}
                          .unit_id("u-happy")
                          .unit_identity_hash(unit_identity)
                          .family_id(VMPilot::DomainLabels::FamilyId::F1)
                          .requested_policy_id(VMPilot::DomainLabels::PolicyId::HighSec)
                          .resolved_family_profile_id("rfp-debug")
                          .unit_binding_record_id("ubr-happy")
                          .payload_sha256(payload_hash)
                          .payload_size(payload.size())
                          .build();
    const auto prof_debug = ResolvedFamilyProfileBuilder{}
                                .profile_id("rfp-debug")
                                .family_id(VMPilot::DomainLabels::FamilyId::F1)
                                .requested_policy_id(VMPilot::DomainLabels::PolicyId::Debug)
                                .runtime_specialization_id("f1-highsec-v1")
                                .build();
    const auto desc_hash = VMPilot::Cbor::domain_hash_sha256(
        VMPilot::DomainLabels::Hash::UnitDescriptor, desc);
    const auto prof_hash = VMPilot::Cbor::domain_hash_sha256(
        VMPilot::DomainLabels::Hash::ResolvedFamilyProfile, prof_debug);

    UnitBindingAuthSpec auth;
    const auto ubr = UnitBindingRecordBuilder{}
                         .unit_binding_record_id("ubr-happy")
                         .unit_identity_hash(unit_identity)
                         .unit_descriptor_hash(desc_hash)
                         .family_id(VMPilot::DomainLabels::FamilyId::F1)
                         .requested_policy_id(VMPilot::DomainLabels::PolicyId::HighSec)
                         .resolved_family_profile_id("rfp-debug")
                         .resolved_family_profile_content_hash(prof_hash)
                         .payload_sha256(payload_hash)
                         .payload_size(payload.size())
                         .anti_downgrade_epoch(1)
                         .binding_auth(auth)
                         .build();

    auto art =
        PackageArtifactBuilder{}
            .payload_bytes(payload)
            .unit_binding_table_bytes(build_unit_binding_table_bytes({ubr}))
            .unit_descriptor_table_bytes(
                build_unit_descriptor_table_bytes({{"u-happy", desc}}))
            .resolved_profile_table_bytes(
                build_resolved_profile_table_bytes({{"rfp-debug", prof_debug}}))
            .build();

    auto b = accept_bundle(std::move(art));
    auto r = accept_unit_entry(b.art.bytes.data(), b.art.bytes.size(), b.env,
                               b.pkg, "u-happy", default_config());
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), UnitAcceptError::ProfilePolicyIdMismatch);
}

// ─── doc 08 §9 #6: blob-flag vs UBR.requested_policy_id ──────────────────

TEST(SpecCompliance, BlobDebugFlagMustMatchUbrPolicy_doc08_9_6) {
    // Build a non-debug VM blob (BLOB_FLAG_DEBUG = false) but have the
    // signed metadata commit to policy=debug. The public runtime entry
    // must fail closed on the mismatch.
    const auto blob_standard = make_halting_blob(false);
    const auto mismatched = build_vm_payload_artifact(
        "f1", "debug", blob_standard, "rfp-debug", "f1-debug-v1");

    EXPECT_EQ(run_artifact_entry(mismatched), INT64_MIN);
}

// ─── doc 15 §9 #4: standard package presented as highsec → reject ───────

TEST(SpecCompliance, StandardPackageRejectedUnderHighsecFloor_doc15_9_4) {
    auto art = build_aligned_artifact(PolicyId::Standard, "standard");
    AcceptConfig cfg = default_config();
    cfg.minimum_policy_floor = PolicyId::HighSec;

    auto b = accept_bundle(std::move(art), cfg);
    auto r = accept_unit_entry(b.art.bytes.data(), b.art.bytes.size(), b.env,
                               b.pkg, "u-happy", cfg);
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), UnitAcceptError::PolicyBelowRuntimeFloor);
}

// ─── doc 15 §9 #7: public errors don't reveal tier / family / provider ──

TEST(SpecCompliance, PublicFailureSurfaceIsTierNeutral_doc15_9_7) {
    // The public artifact entry point does not surface internal reject
    // reasons; debug builds collapse every failure to INT64_MIN.
    // Exercise tier-, family-, and provider-related rejects and confirm
    // the externally visible result is identical.
    const auto standard_blob = make_halting_blob(false);
    constexpr std::array<std::uint8_t, 32> kProviderRequirement{
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A,
        0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25,
        0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    };

    const auto tier_failure = build_vm_payload_artifact(
        "f1", "debug", standard_blob, "rfp-debug", "f1-debug-v1");
    const auto family_failure = build_vm_payload_artifact(
        "f2", "standard", standard_blob, "rfp-f2", "f2-standard-v1");
    const auto provider_failure = build_vm_payload_artifact(
        "f1", "standard", standard_blob, "rfp-provider", "f1-standard-v1",
        kProviderRequirement);

    const auto tier_rc = run_artifact_entry(tier_failure);
    const auto family_rc = run_artifact_entry(family_failure);
    const auto provider_rc = run_artifact_entry(provider_failure);

    EXPECT_EQ(tier_rc, INT64_MIN);
    EXPECT_EQ(family_rc, INT64_MIN);
    EXPECT_EQ(provider_rc, INT64_MIN);
    EXPECT_EQ(tier_rc, family_rc);
    EXPECT_EQ(family_rc, provider_rc);
}

// ─── doc 15 §9 #10: section_table_shape_class identical across tiers ────

TEST(SpecCompliance, SectionTableShapeClassInvariantAcrossTiers_doc15_9_10) {
    // Real debug / standard / highsec artifacts must publish the same
    // outer skeleton shape. The CI guard is this exact invariant.
    const auto debug_art = build_vm_payload_artifact(
        "f1", "debug", make_halting_blob(true), "rfp-debug", "f1-debug-v1");
    const auto standard_art =
        build_vm_payload_artifact("f1", "standard", make_halting_blob(false),
                                  "rfp-standard", "f1-standard-v1");
    const auto highsec_art =
        build_vm_payload_artifact("f1", "highsec", make_halting_blob(false),
                                  "rfp-highsec", "f1-highsec-v1");

    auto env_debug = parse_outer_envelope(debug_art.bytes);
    auto env_standard = parse_outer_envelope(standard_art.bytes);
    auto env_highsec = parse_outer_envelope(highsec_art.bytes);
    ASSERT_TRUE(env_debug.has_value());
    ASSERT_TRUE(env_standard.has_value());
    ASSERT_TRUE(env_highsec.has_value());

    EXPECT_TRUE(
        same_public_shape(*env_debug, *env_standard, "debug", "standard"));
    EXPECT_TRUE(
        same_public_shape(*env_standard, *env_highsec, "standard", "highsec"));
}

TEST(SpecCompliance, SectionShapeDriftWouldTripInvariantCheck_doc15_9_10) {
    // Negative control: if a producer ever emitted a tier-specific shape
    // class, the public-shape guard above would fail. Assert the guard
    // actually detects such drift.
    auto base = VMPilot::Fixtures::OuterEnvelopeBuilder{}.build();
    auto drifted = VMPilot::Fixtures::OuterEnvelopeBuilder{}
                       .section_table_shape_class(2)
                       .build();

    auto env_base = parse_outer_envelope(base.bytes);
    auto env_drifted = parse_outer_envelope(drifted.bytes);
    ASSERT_TRUE(env_base.has_value());
    ASSERT_TRUE(env_drifted.has_value());

    EXPECT_FALSE(
        same_public_shape(*env_base, *env_drifted, "baseline", "drifted-tier"));
}

// ─── doc 15 §9 #5: old standard artifact + highsec entitlement → reject ─

TEST(SpecCompliance,
     OldStandardArtifactPlusSidecarEntitlementStillRejected_doc15_9_5) {
    // Stage 11 has not landed, so there is no entitlement ingestion path.
    // Simulate an attacker / buggy loader appending a token-like sidecar
    // to an old standard artifact and confirm the executable path still
    // rejects under a highsec runtime floor.
    auto art = build_aligned_artifact(PolicyId::Standard, "standard");
    const std::string fake_entitlement =
        "migration-token-v1:allowed_policy_floor=highsec";
    art.bytes.insert(art.bytes.end(), fake_entitlement.begin(),
                     fake_entitlement.end());

    AcceptConfig cfg = default_config();
    cfg.minimum_policy_floor = PolicyId::HighSec;

    auto b = accept_bundle(std::move(art), cfg);
    auto r = accept_unit_entry(b.art.bytes.data(), b.art.bytes.size(), b.env,
                               b.pkg, "u-happy", cfg);
    ASSERT_FALSE(r.has_value());
    EXPECT_EQ(r.error(), UnitAcceptError::PolicyBelowRuntimeFloor);
}
