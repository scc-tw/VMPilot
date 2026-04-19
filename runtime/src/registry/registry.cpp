#include "registry/registry.hpp"

#include <cstring>
#include <utility>

#include "VMPilot_crypto.hpp"
#include "binding/signed_partition.hpp"
#include "cbor/schema.hpp"
#include "cbor/strict.hpp"
#include "trust_root.hpp"
#include "vm/domain_labels.hpp"
#include "vm/family_policy.hpp"

namespace VMPilot::Cbor {
template <>
struct CborConsumerTraits<VMPilot::Runtime::Registry::ParseError> {
    using E = VMPilot::Runtime::Registry::ParseError;
    static constexpr E missing_field                 = E::MissingField;
    static constexpr E wrong_field_type              = E::WrongFieldType;
    static constexpr E wrong_hash_size               = E::WrongHashSize;
    static constexpr E bad_cbor                      = E::BadCbor;
    static constexpr E not_a_map                     = E::NotAMap;
    static constexpr E partition_malformed           = E::PartitionMalformed;
    static constexpr E auth_kind_unsupported         = E::AuthKindUnsupported;
    static constexpr E auth_key_id_mismatch          = E::AuthKeyIdMismatch;
    static constexpr E auth_signature_alg_mismatch   = E::AuthSignatureAlgMismatch;
    static constexpr E auth_covered_domain_mismatch  = E::AuthCoveredDomainMismatch;
    static constexpr E signature_wrong_size          = E::SignatureWrongSize;
    static constexpr E signature_invalid             = E::SignatureInvalid;
    static constexpr E missing_core_field            = E::MissingField;
};
}  // namespace VMPilot::Cbor

namespace VMPilot::Runtime::Registry {

namespace {

using VMPilot::Cbor::Value;
using VMPilot::Cbor::parse_strict;

// Header field IDs (see registry.hpp top-of-file schema).
constexpr std::uint64_t kHdr_Version          = 1;
constexpr std::uint64_t kHdr_RuntimeBuildId   = 2;
constexpr std::uint64_t kHdr_PackageSchemaV   = 3;
constexpr std::uint64_t kHdr_RegistryEpoch    = 4;
constexpr std::uint64_t kHdr_Entries          = 5;

// Entry field IDs.
constexpr std::uint64_t kEnt_SpecId                       = 1;
constexpr std::uint64_t kEnt_FamilyId                     = 2;
constexpr std::uint64_t kEnt_PolicyId                     = 3;
constexpr std::uint64_t kEnt_ProfileRevision              = 4;
constexpr std::uint64_t kEnt_SemanticContractVersion      = 5;
constexpr std::uint64_t kEnt_ExecutionContractRef         = 6;
constexpr std::uint64_t kEnt_RequiredPrimitivesHash       = 7;
constexpr std::uint64_t kEnt_RequiredHelpersHash          = 8;
constexpr std::uint64_t kEnt_ProviderRequirementHash      = 9;
constexpr std::uint64_t kEnt_AcceptedProfileContentHash        = 10;
constexpr std::uint64_t kEnt_DiagnosticVisibilityClass         = 11;
constexpr std::uint64_t kEnt_EnabledInThisRuntime              = 12;
constexpr std::uint64_t kEnt_ProviderRequirementCanonicalBytes = 13;

inline tl::unexpected<ParseError> err(ParseError e) noexcept {
    return tl::make_unexpected(e);
}
inline tl::unexpected<LookupError> lookup_err(LookupError e) noexcept {
    return tl::make_unexpected(e);
}

tl::expected<SpecializationEntry, ParseError>
parse_entry(const Value& entry_v) noexcept {
    using namespace VMPilot::Cbor::Schema;
    if (entry_v.kind() != Value::Kind::Map) return err(ParseError::WrongFieldType);

    const auto schema = std::make_tuple(
        TextField<SpecializationEntry>{
            kEnt_SpecId, &SpecializationEntry::runtime_specialization_id},
        EnumTextField<SpecializationEntry,
                      VMPilot::DomainLabels::FamilyId, ParseError>{
            kEnt_FamilyId, &SpecializationEntry::family_id,
            ParseError::UnknownFamilyId},
        EnumTextField<SpecializationEntry,
                      VMPilot::DomainLabels::PolicyId, ParseError>{
            kEnt_PolicyId, &SpecializationEntry::requested_policy_id,
            ParseError::UnknownPolicyId},
        TextField<SpecializationEntry>{
            kEnt_ProfileRevision, &SpecializationEntry::profile_revision},
        TextField<SpecializationEntry>{
            kEnt_SemanticContractVersion,
            &SpecializationEntry::semantic_contract_version},
        TextField<SpecializationEntry>{
            kEnt_ExecutionContractRef,
            &SpecializationEntry::execution_contract_ref},
        HashField<SpecializationEntry>{
            kEnt_RequiredPrimitivesHash,
            &SpecializationEntry::required_runtime_primitives_hash},
        HashField<SpecializationEntry>{
            kEnt_RequiredHelpersHash,
            &SpecializationEntry::required_runtime_helpers_hash},
        HashField<SpecializationEntry>{
            kEnt_ProviderRequirementHash,
            &SpecializationEntry::provider_requirement_hash},
        HashField<SpecializationEntry>{
            kEnt_AcceptedProfileContentHash,
            &SpecializationEntry::accepted_profile_content_hash},
        UintField<SpecializationEntry>{
            kEnt_DiagnosticVisibilityClass,
            &SpecializationEntry::diagnostic_visibility_class},
        BytesField<SpecializationEntry>{
            kEnt_ProviderRequirementCanonicalBytes,
            &SpecializationEntry::provider_requirement_canonical_bytes}
    );
    auto parsed = parse_schema<SpecializationEntry, ParseError>(entry_v, schema);
    if (!parsed) return err(parsed.error());

    // enabled_in_this_runtime is a uint with a tighter range than BoolField
    // gives us (registry surfaces EnabledFlagOutOfRange instead of
    // WrongFieldType); parse it by hand to preserve the distinction.
    auto enabled_u = VMPilot::Cbor::require_uint<ParseError>(
        entry_v, kEnt_EnabledInThisRuntime);
    if (!enabled_u) return err(enabled_u.error());
    if (*enabled_u > 1) return err(ParseError::EnabledFlagOutOfRange);
    parsed->enabled_in_this_runtime = (*enabled_u == 1);

    // Provider-requirement commitment sanity: zero-length bytes must
    // pair with an all-zero hash; any other combination is a producer
    // bug and is rejected before lookup runs.
    constexpr std::array<std::uint8_t, 32> kZeroHash{};
    const auto& req_bytes = parsed->provider_requirement_canonical_bytes;
    const bool bytes_empty = req_bytes.empty();
    const bool hash_zero = (parsed->provider_requirement_hash == kZeroHash);
    if (bytes_empty != hash_zero) {
        return err(ParseError::InconsistentRequirementCommitment);
    }
    if (!bytes_empty) {
        const auto recomputed = VMPilot::Cbor::domain_hash_sha256(
            VMPilot::DomainLabels::Hash::PolicyRequirement, req_bytes);
        if (recomputed != parsed->provider_requirement_hash) {
            return err(ParseError::InconsistentRequirementCommitment);
        }
    }

    return parsed;
}

}  // namespace

tl::expected<Registry, ParseError>
parse(const std::uint8_t* data, std::size_t size) noexcept {
    using namespace VMPilot::Cbor::Schema;
    auto tree_or = parse_strict(data, size);
    if (!tree_or) return err(ParseError::BadCbor);
    const Value& tree = *tree_or;

    const auto header_schema = std::make_tuple(
        TextField<Registry>{kHdr_Version, &Registry::registry_version},
        TextField<Registry>{kHdr_RuntimeBuildId, &Registry::runtime_build_id},
        TextField<Registry>{kHdr_PackageSchemaV, &Registry::package_schema_version},
        UintField<Registry>{kHdr_RegistryEpoch, &Registry::registry_epoch}
    );
    auto hdr_or = parse_schema<Registry, ParseError>(tree, header_schema);
    if (!hdr_or) return err(hdr_or.error());

    const Value* entries_v = tree.find_by_uint_key(kHdr_Entries);
    if (entries_v == nullptr) return err(ParseError::MissingField);
    if (entries_v->kind() != Value::Kind::Array) return err(ParseError::WrongFieldType);

    Registry out = std::move(*hdr_or);
    out.entries.reserve(entries_v->as_array().size());
    for (const auto& e : entries_v->as_array()) {
        auto parsed = parse_entry(e);
        if (!parsed) return err(parsed.error());
        out.entries.push_back(std::move(*parsed));
    }

    // Duplicate-tuple check. Two entries that share all four lookup keys
    // would make dispatch ambiguous; reject at parse time rather than
    // hiding the ambiguity behind "first match wins".
    for (std::size_t i = 0; i < out.entries.size(); ++i) {
        for (std::size_t j = i + 1; j < out.entries.size(); ++j) {
            const auto& a = out.entries[i];
            const auto& b = out.entries[j];
            if (a.runtime_specialization_id == b.runtime_specialization_id &&
                a.family_id == b.family_id &&
                a.requested_policy_id == b.requested_policy_id &&
                a.profile_revision == b.profile_revision) {
                return err(ParseError::DuplicateEntry);
            }
        }
    }

    return out;
}

// ─── Registry partition wrapper (doc 08 §3.1) ───────────────────────────

tl::expected<Registry, ParseError>
parse_partition(const std::uint8_t* data, std::size_t size,
                const VMPilot::Runtime::VendorTrustRoot& root) noexcept {
    auto tree_or = parse_strict(data, size);
    if (!tree_or) return err(ParseError::PartitionMalformed);
    auto view_or = VMPilot::Runtime::Binding::verify_signed_partition_view<ParseError>(
        *tree_or, root, VMPilot::DomainLabels::Auth::RuntimeSpecRegistry);
    if (!view_or) return err(view_or.error());
    const auto& canonical_bytes = *view_or->canonical_bytes;
    return parse(canonical_bytes.data(), canonical_bytes.size());
}

tl::expected<const SpecializationEntry*, LookupError>
lookup(const Registry& reg,
       std::string_view spec_id,
       VMPilot::DomainLabels::FamilyId family_id,
       VMPilot::DomainLabels::PolicyId policy_id,
       std::string_view profile_revision) noexcept {
    const SpecializationEntry* disabled_match = nullptr;
    for (const auto& e : reg.entries) {
        if (e.runtime_specialization_id != spec_id) continue;
        if (e.family_id != family_id) continue;
        if (e.requested_policy_id != policy_id) continue;
        if (e.profile_revision != profile_revision) continue;
        if (e.enabled_in_this_runtime) {
            return &e;
        }
        disabled_match = &e;
    }
    if (disabled_match != nullptr) return lookup_err(LookupError::Disabled);
    return lookup_err(LookupError::NotFound);
}

}  // namespace VMPilot::Runtime::Registry
