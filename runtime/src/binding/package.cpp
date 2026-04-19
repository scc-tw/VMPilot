#include "binding/package.hpp"

#include <algorithm>
#include <cstring>

#include "VMPilot_crypto.hpp"
#include "binding/inner_partition.hpp"
#include "binding/signed_partition.hpp"
#include "cbor/schema.hpp"
#include "cbor/strict.hpp"
#include "vm/domain_labels.hpp"

// Wire AcceptError into the unified CBOR consumer trait (covers
// require_* templates + SignedPartition verification).
namespace VMPilot::Cbor {
template <>
struct CborConsumerTraits<VMPilot::Runtime::Binding::AcceptError> {
    using E = VMPilot::Runtime::Binding::AcceptError;
    static constexpr E missing_field                 = E::MissingCoreField;
    static constexpr E wrong_field_type              = E::WrongFieldType;
    static constexpr E wrong_hash_size               = E::WrongHashSize;
    static constexpr E bad_cbor                      = E::PbrCanonicalDecodeFailed;
    static constexpr E not_a_map                     = E::PbrCanonicalDecodeFailed;
    static constexpr E unknown_core_field            = E::UnknownCoreField;
    static constexpr E partition_malformed           = E::PbrPartitionMalformed;
    static constexpr E auth_kind_unsupported         = E::AuthKindUnsupported;
    static constexpr E auth_key_id_mismatch          = E::AuthKeyIdMismatch;
    static constexpr E auth_signature_alg_mismatch   = E::AuthSignatureAlgMismatch;
    static constexpr E auth_covered_domain_mismatch  = E::AuthCoveredDomainMismatch;
    static constexpr E signature_wrong_size          = E::SignatureWrongSize;
    static constexpr E signature_invalid             = E::SignatureInvalid;
    static constexpr E missing_core_field            = E::MissingCoreField;
};
}  // namespace VMPilot::Cbor

namespace VMPilot::Runtime::Binding {

namespace {

using VMPilot::Cbor::Value;
using VMPilot::Cbor::parse_strict;
using VMPilot::Cbor::domain_hash_sha256;

// Field IDs inside the canonical PBR map. Keep in sync with the fixture
// builder in runtime/test/fixtures/fixture_generator.cpp and with
constexpr std::uint64_t kField_PbrId                      = 1;
constexpr std::uint64_t kField_PackageSchemaVersion       = 2;
constexpr std::uint64_t kField_CanonicalEncodingId        = 3;
constexpr std::uint64_t kField_UnitBindingTableHash       = 4;
constexpr std::uint64_t kField_ResolvedProfileTableHash   = 5;
constexpr std::uint64_t kField_RuntimeSpecRegistryHash    = 6;
constexpr std::uint64_t kField_ArtifactLayoutHash         = 7;
constexpr std::uint64_t kField_AntiDowngradeEpoch         = 8;
constexpr std::uint64_t kField_MinimumRuntimeEpoch        = 9;

// Field IDs inside PackageBindingAuth (doc 06 §9.2).
inline tl::unexpected<AcceptError> err(AcceptError e) noexcept {
    return tl::make_unexpected(e);
}

bool is_in_list(const std::vector<std::string>& list, std::string_view v) noexcept {
    for (const auto& x : list) if (x == v) return true;
    return false;
}

// Constant-time-ish equality for hashes. std::memcmp is fine here because
// we're comparing digests; a timing side channel on equal-vs-not equal of
// a commitment hash doesn't leak anything useful.
bool hash_equals(const std::array<std::uint8_t, 32>& a,
                 const std::array<std::uint8_t, 32>& b) noexcept {
    return std::memcmp(a.data(), b.data(), 32) == 0;
}

}  // namespace

tl::expected<AcceptedPackage, AcceptError>
accept_package(const std::uint8_t* artifact_data,
               std::size_t artifact_size,
               const VendorTrustRoot& root,
               const Envelope::OuterEnvelope& env,
               const AcceptConfig& config) noexcept {
    // 1. Carve the PBR partition out of the artifact. Locator bounds were
    //    checked by Stage 4 but we re-assert here so accept_package stays
    //    independent of its caller.
    const std::uint64_t pbr_end =
        env.package_binding_record.offset + env.package_binding_record.length;
    if (env.package_binding_record.length == 0 ||
        pbr_end > artifact_size ||
        pbr_end < env.package_binding_record.offset) {
        return err(AcceptError::PbrLocatorOutOfBounds);
    }
    const std::uint8_t* pbr_bytes = artifact_data + env.package_binding_record.offset;

    // 1a. Trust root sanity. trust_root_is_well_formed() guards magic,
    //     version, usage label, zero-key, and string termination — any
    //     tamper of the pinned .vmpltr section fails here before a
    //     single signature byte is touched.
    if (!trust_root_is_well_formed()) {
        return err(AcceptError::TrustRootMalformed);
    }
    // Enforce the `key_usage` label separately. trust_root_is_well_formed
    // already rejects a wrong usage string, but making the check explicit
    // at every verification site means future key_usage types (e.g.
    // entitlement signing) can't accidentally satisfy artifact verification.
    if (std::strcmp(root.key_usage,
                    VMPilot::Runtime::kTrustRootKeyUsage) != 0) {
        return err(AcceptError::TrustRootKeyUsageMismatch);
    }

    // 2. Open the PBR partition via the shared signed-partition
    //    template. This does the [canonical_bytes, auth_map] +
    //    Ed25519 verify pipeline in one place (see
    //    runtime/include/binding/signed_partition.hpp).
    auto partition_or = parse_strict(pbr_bytes, env.package_binding_record.length);
    if (!partition_or) return err(AcceptError::PbrPartitionMalformed);
    auto view_or = verify_signed_partition_view<AcceptError>(
        *partition_or, root, VMPilot::DomainLabels::Auth::PackageBinding);
    if (!view_or) return err(view_or.error());
    const std::vector<std::uint8_t>& canonical_bytes = *view_or->canonical_bytes;

    // 5. Parse the PBR canonical content. If this fails after the signature
    //    verified, an implementation bug snuck by — the signer produced
    //    noncanonical bytes and the pre-verify decoder didn't reject them.
    //    Either way, fail-closed.
    auto pbr_or = parse_strict(canonical_bytes);
    if (!pbr_or) return err(AcceptError::PbrCanonicalDecodeFailed);

    using namespace VMPilot::Cbor::Schema;
    // Reject any PBR map key outside the v1 schema before field extraction
    // — doc 07's strict-subset ethos: unknown producer fields must not slip
    // past a signed record.
    {
        auto unknown_or = reject_unknown_keys<AcceptError>(
            *pbr_or,
            {kField_PbrId, kField_PackageSchemaVersion, kField_CanonicalEncodingId,
             kField_UnitBindingTableHash, kField_ResolvedProfileTableHash,
             kField_RuntimeSpecRegistryHash, kField_ArtifactLayoutHash,
             kField_AntiDowngradeEpoch, kField_MinimumRuntimeEpoch});
        if (!unknown_or) return err(unknown_or.error());
    }
    const auto schema = std::make_tuple(
        TextField<AcceptedPackage>{kField_PbrId,
                                   &AcceptedPackage::package_binding_record_id},
        TextField<AcceptedPackage>{kField_PackageSchemaVersion,
                                   &AcceptedPackage::package_schema_version},
        TextField<AcceptedPackage>{kField_CanonicalEncodingId,
                                   &AcceptedPackage::canonical_encoding_id},
        HashField<AcceptedPackage>{kField_UnitBindingTableHash,
                                   &AcceptedPackage::unit_binding_table_hash},
        HashField<AcceptedPackage>{kField_ResolvedProfileTableHash,
                                   &AcceptedPackage::resolved_profile_table_hash},
        HashField<AcceptedPackage>{kField_RuntimeSpecRegistryHash,
                                   &AcceptedPackage::runtime_specialization_registry_hash},
        HashField<AcceptedPackage>{kField_ArtifactLayoutHash,
                                   &AcceptedPackage::artifact_layout_hash},
        UintField<AcceptedPackage>{kField_AntiDowngradeEpoch,
                                   &AcceptedPackage::anti_downgrade_epoch},
        UintField<AcceptedPackage>{kField_MinimumRuntimeEpoch,
                                   &AcceptedPackage::minimum_runtime_epoch}
    );
    auto result_or = parse_schema<AcceptedPackage, AcceptError>(*pbr_or, schema);
    if (!result_or) return err(result_or.error());
    AcceptedPackage result = std::move(*result_or);

    // 6. Acceptance policy gates.
    if (!is_in_list(config.supported_schema_versions, result.package_schema_version)) {
        return err(AcceptError::UnsupportedPackageSchemaVersion);
    }
    if (!is_in_list(config.supported_canonical_encodings, result.canonical_encoding_id)) {
        return err(AcceptError::UnsupportedCanonicalEncodingId);
    }
    if (result.minimum_runtime_epoch > config.epoch.runtime_epoch) {
        return err(AcceptError::RuntimeEpochTooOld);
    }
    if (result.anti_downgrade_epoch < config.epoch.minimum_accepted_epoch) {
        return err(AcceptError::AntiDowngradeEpochTooOld);
    }

    // 7. Whole-package binding. Envelope body first: the layout hash commits
    //    to the fixed header + canonical CBOR metadata, so any locator
    //    tamper that parses cleanly still fails here.
    const std::size_t envelope_body_len = env.metadata_offset + env.metadata_length;
    if (envelope_body_len > artifact_size) {
        return err(AcceptError::PbrLocatorOutOfBounds);
    }
    const auto layout_computed = domain_hash_sha256(
        VMPilot::DomainLabels::Hash::ArtifactLayout,
        artifact_data,
        envelope_body_len);
    if (!hash_equals(layout_computed, result.artifact_layout_hash)) {
        return err(AcceptError::ArtifactLayoutHashMismatch);
    }

    // 8. Inner partition: parse as a strict-CBOR map and hash each
    //    sub-table's payload independently. Distinct payloads + distinct
    //    domain labels mean the three PBR commitments commit to three
    //    distinct byte ranges, not the same bytes three times.
    if (env.inner_metadata_partition.offset + env.inner_metadata_partition.length >
            artifact_size) {
        return err(AcceptError::PbrLocatorOutOfBounds);
    }
    const std::uint8_t* inner_bytes =
        artifact_data + env.inner_metadata_partition.offset;
    const std::size_t inner_len = static_cast<std::size_t>(env.inner_metadata_partition.length);

    auto inner_or = parse_inner_partition(inner_bytes, inner_len);
    if (!inner_or) return err(AcceptError::PbrPartitionMalformed);
    const InnerPartition& inner = *inner_or;

    const auto ubt_computed = domain_hash_sha256(
        VMPilot::DomainLabels::Hash::UnitBindingTable,
        inner.unit_binding_table.data(), inner.unit_binding_table.size());
    if (!hash_equals(ubt_computed, result.unit_binding_table_hash)) {
        return err(AcceptError::UnitBindingTableHashMismatch);
    }

    const auto rpt_computed = domain_hash_sha256(
        VMPilot::DomainLabels::Hash::ResolvedProfileTable,
        inner.resolved_profile_table.data(), inner.resolved_profile_table.size());
    if (!hash_equals(rpt_computed, result.resolved_profile_table_hash)) {
        return err(AcceptError::ResolvedProfileTableHashMismatch);
    }

    const auto reg_computed = domain_hash_sha256(
        VMPilot::DomainLabels::Hash::RuntimeSpecRegistry,
        inner.runtime_specialization_registry.data(),
        inner.runtime_specialization_registry.size());
    if (!hash_equals(reg_computed, result.runtime_specialization_registry_hash)) {
        return err(AcceptError::RuntimeSpecializationRegistryHashMismatch);
    }

    // 8. Retain the canonical bytes so Stage 7 can cross-check without re-parsing.
    result.pbr_canonical_bytes = canonical_bytes;
    return result;
}

}  // namespace VMPilot::Runtime::Binding
