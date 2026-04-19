#include "registry/registry.hpp"

#include <cstring>
#include <utility>

#include "VMPilot_crypto.hpp"
#include "cbor/strict.hpp"
#include "trust_root.hpp"
#include "vm/domain_labels.hpp"
#include "vm/family_policy.hpp"

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

tl::expected<std::string, ParseError>
require_text(const Value& m, std::uint64_t key) noexcept {
    const Value* v = m.find_by_uint_key(key);
    if (v == nullptr) return err(ParseError::MissingField);
    if (v->kind() != Value::Kind::Text) return err(ParseError::WrongFieldType);
    return v->as_text();
}

tl::expected<std::uint64_t, ParseError>
require_uint(const Value& m, std::uint64_t key) noexcept {
    const Value* v = m.find_by_uint_key(key);
    if (v == nullptr) return err(ParseError::MissingField);
    if (v->kind() != Value::Kind::Uint) return err(ParseError::WrongFieldType);
    return v->as_uint();
}

tl::expected<std::array<std::uint8_t, 32>, ParseError>
require_hash(const Value& m, std::uint64_t key) noexcept {
    const Value* v = m.find_by_uint_key(key);
    if (v == nullptr) return err(ParseError::MissingField);
    if (v->kind() != Value::Kind::Bytes) return err(ParseError::WrongFieldType);
    if (v->as_bytes().size() != 32) return err(ParseError::WrongHashSize);
    std::array<std::uint8_t, 32> out{};
    std::memcpy(out.data(), v->as_bytes().data(), 32);
    return out;
}

tl::expected<SpecializationEntry, ParseError>
parse_entry(const Value& entry_v) noexcept {
    if (entry_v.kind() != Value::Kind::Map) return err(ParseError::WrongFieldType);

    auto spec_id = require_text(entry_v, kEnt_SpecId);
    if (!spec_id) return err(spec_id.error());
    auto family = require_text(entry_v, kEnt_FamilyId);
    if (!family) return err(family.error());
    auto policy = require_text(entry_v, kEnt_PolicyId);
    if (!policy) return err(policy.error());
    auto fam_enum = VMPilot::DomainLabels::parse_family_id(*family);
    if (!fam_enum) return err(ParseError::UnknownFamilyId);
    auto pol_enum = VMPilot::DomainLabels::parse_policy_id(*policy);
    if (!pol_enum) return err(ParseError::UnknownPolicyId);
    auto revision = require_text(entry_v, kEnt_ProfileRevision);
    if (!revision) return err(revision.error());
    auto semantic = require_text(entry_v, kEnt_SemanticContractVersion);
    if (!semantic) return err(semantic.error());
    auto exec_ref = require_text(entry_v, kEnt_ExecutionContractRef);
    if (!exec_ref) return err(exec_ref.error());
    auto prim_hash = require_hash(entry_v, kEnt_RequiredPrimitivesHash);
    if (!prim_hash) return err(prim_hash.error());
    auto help_hash = require_hash(entry_v, kEnt_RequiredHelpersHash);
    if (!help_hash) return err(help_hash.error());
    auto prov_hash = require_hash(entry_v, kEnt_ProviderRequirementHash);
    if (!prov_hash) return err(prov_hash.error());
    auto prof_hash = require_hash(entry_v, kEnt_AcceptedProfileContentHash);
    if (!prof_hash) return err(prof_hash.error());
    auto diag_cls = require_uint(entry_v, kEnt_DiagnosticVisibilityClass);
    if (!diag_cls) return err(diag_cls.error());
    auto enabled_u = require_uint(entry_v, kEnt_EnabledInThisRuntime);
    if (!enabled_u) return err(enabled_u.error());
    if (*enabled_u > 1) return err(ParseError::EnabledFlagOutOfRange);

    // Field 13 is required in v1 but may carry zero-length bytes to
    // represent "no provider requirement" (paired with an all-zero
    // field 9). Any other bytes↔hash combination is a producer bug
    // and rejected here before lookup runs.
    const Value* req_bytes_v =
        entry_v.find_by_uint_key(kEnt_ProviderRequirementCanonicalBytes);
    if (req_bytes_v == nullptr) return err(ParseError::MissingField);
    if (req_bytes_v->kind() != Value::Kind::Bytes) {
        return err(ParseError::WrongFieldType);
    }
    const auto& req_bytes = req_bytes_v->as_bytes();

    constexpr std::array<std::uint8_t, 32> kZeroHash{};
    const bool bytes_empty = req_bytes.empty();
    const bool hash_zero = (*prov_hash == kZeroHash);
    if (bytes_empty != hash_zero) {
        return err(ParseError::InconsistentRequirementCommitment);
    }
    if (!bytes_empty) {
        const auto recomputed = VMPilot::Cbor::domain_hash_sha256(
            VMPilot::DomainLabels::Hash::PolicyRequirement, req_bytes);
        if (recomputed != *prov_hash) {
            return err(ParseError::InconsistentRequirementCommitment);
        }
    }

    SpecializationEntry out;
    out.runtime_specialization_id              = std::move(*spec_id);
    out.family_id                              = *fam_enum;
    out.requested_policy_id                    = *pol_enum;
    out.profile_revision                       = std::move(*revision);
    out.semantic_contract_version              = std::move(*semantic);
    out.execution_contract_ref                 = std::move(*exec_ref);
    out.required_runtime_primitives_hash       = *prim_hash;
    out.required_runtime_helpers_hash          = *help_hash;
    out.provider_requirement_hash              = *prov_hash;
    out.accepted_profile_content_hash          = *prof_hash;
    out.diagnostic_visibility_class            = *diag_cls;
    out.enabled_in_this_runtime                = (*enabled_u == 1);
    out.provider_requirement_canonical_bytes   = req_bytes;
    return out;
}

}  // namespace

tl::expected<Registry, ParseError>
parse(const std::uint8_t* data, std::size_t size) noexcept {
    auto tree_or = parse_strict(data, size);
    if (!tree_or) return err(ParseError::BadCbor);
    const Value& tree = *tree_or;
    if (tree.kind() != Value::Kind::Map) return err(ParseError::NotAMap);

    auto version_or = require_text(tree, kHdr_Version);
    auto build_or = require_text(tree, kHdr_RuntimeBuildId);
    auto schema_or = require_text(tree, kHdr_PackageSchemaV);
    auto epoch_or = require_uint(tree, kHdr_RegistryEpoch);
    if (!version_or) return err(version_or.error());
    if (!build_or) return err(build_or.error());
    if (!schema_or) return err(schema_or.error());
    if (!epoch_or) return err(epoch_or.error());

    const Value* entries_v = tree.find_by_uint_key(kHdr_Entries);
    if (entries_v == nullptr) return err(ParseError::MissingField);
    if (entries_v->kind() != Value::Kind::Array) return err(ParseError::WrongFieldType);

    Registry out;
    out.registry_version = std::move(*version_or);
    out.runtime_build_id = std::move(*build_or);
    out.package_schema_version = std::move(*schema_or);
    out.registry_epoch = *epoch_or;
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

namespace {

constexpr std::uint64_t kAuth_Kind              = 1;
constexpr std::uint64_t kAuth_KeyId             = 2;
constexpr std::uint64_t kAuth_SignatureAlgId    = 3;
constexpr std::uint64_t kAuth_CoveredDomain     = 4;
constexpr std::uint64_t kAuth_Signature         = 5;

constexpr std::string_view kAuthKindVendorSignatureV1 = "vendor_signature_v1";

tl::expected<std::string, ParseError>
require_text_in_auth(const Value& m, std::uint64_t key) noexcept {
    const Value* v = m.find_by_uint_key(key);
    if (v == nullptr) return err(ParseError::MissingField);
    if (v->kind() != Value::Kind::Text) return err(ParseError::WrongFieldType);
    return v->as_text();
}

}  // namespace

tl::expected<Registry, ParseError>
parse_partition(const std::uint8_t* data, std::size_t size,
                const VMPilot::Runtime::VendorTrustRoot& root) noexcept {
    auto tree_or = parse_strict(data, size);
    if (!tree_or) return err(ParseError::PartitionMalformed);
    const Value& tree = *tree_or;
    if (tree.kind() != Value::Kind::Array) return err(ParseError::PartitionMalformed);
    if (tree.as_array().size() != 2) return err(ParseError::PartitionMalformed);

    const Value& canonical_v = tree.as_array()[0];
    const Value& auth_v      = tree.as_array()[1];
    if (canonical_v.kind() != Value::Kind::Bytes) return err(ParseError::PartitionMalformed);
    if (auth_v.kind() != Value::Kind::Map) return err(ParseError::PartitionMalformed);

    auto kind = require_text_in_auth(auth_v, kAuth_Kind);
    if (!kind) return err(kind.error());
    if (*kind != kAuthKindVendorSignatureV1) return err(ParseError::AuthKindUnsupported);

    auto key_id = require_text_in_auth(auth_v, kAuth_KeyId);
    if (!key_id) return err(key_id.error());
    if (*key_id != root.root_key_id) return err(ParseError::AuthKeyIdMismatch);

    auto alg_id = require_text_in_auth(auth_v, kAuth_SignatureAlgId);
    if (!alg_id) return err(alg_id.error());
    if (*alg_id != root.signature_alg_id) return err(ParseError::AuthSignatureAlgMismatch);

    auto domain = require_text_in_auth(auth_v, kAuth_CoveredDomain);
    if (!domain) return err(domain.error());
    if (*domain != VMPilot::DomainLabels::Auth::RuntimeSpecRegistry) {
        return err(ParseError::AuthCoveredDomainMismatch);
    }

    const Value* sig_v = auth_v.find_by_uint_key(kAuth_Signature);
    if (sig_v == nullptr) return err(ParseError::MissingField);
    if (sig_v->kind() != Value::Kind::Bytes) return err(ParseError::WrongFieldType);
    const auto& signature = sig_v->as_bytes();
    if (signature.size() != 64) return err(ParseError::SignatureWrongSize);

    const std::vector<std::uint8_t> pubkey_vec(
        root.public_key, root.public_key + sizeof(root.public_key));
    const bool ok = VMPilot::Crypto::Verify_Ed25519(
        pubkey_vec, signature,
        std::string(VMPilot::DomainLabels::Auth::RuntimeSpecRegistry),
        canonical_v.as_bytes());
    if (!ok) return err(ParseError::SignatureInvalid);

    return parse(canonical_v.as_bytes().data(), canonical_v.as_bytes().size());
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
