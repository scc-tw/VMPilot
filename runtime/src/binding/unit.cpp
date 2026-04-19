#include "binding/unit.hpp"

#include <algorithm>
#include <cstring>

#include "VMPilot_crypto.hpp"
#include "binding/inner_partition.hpp"
#include "binding/resolved_profile.hpp"
#include "cbor/strict.hpp"
#include "eh_guard.hpp"
#include "vm/domain_labels.hpp"
#include "vm/family_policy.hpp"

namespace VMPilot::Cbor {
template <>
struct RequireErrors<VMPilot::Runtime::Binding::UnitAcceptError> {
    using E = VMPilot::Runtime::Binding::UnitAcceptError;
    static constexpr E missing_field    = E::MissingCoreField;
    static constexpr E wrong_field_type = E::WrongFieldType;
    static constexpr E wrong_hash_size  = E::WrongHashSize;
};
}  // namespace VMPilot::Cbor

namespace VMPilot::Runtime::Binding {

namespace {

using VMPilot::Cbor::Value;
using VMPilot::Cbor::parse_strict;
using VMPilot::Cbor::domain_hash_sha256;

// ─── UnitDescriptor field IDs (doc 03 §7) ────────────────────────────────
constexpr std::uint64_t kUd_DescriptorVersion           = 1;
constexpr std::uint64_t kUd_UnitId                       = 2;
constexpr std::uint64_t kUd_UnitIdentityHash             = 3;
constexpr std::uint64_t kUd_FamilyId                     = 4;
constexpr std::uint64_t kUd_RequestedPolicyId            = 5;
constexpr std::uint64_t kUd_ResolvedFamilyProfileId      = 6;
constexpr std::uint64_t kUd_PayloadIdentity              = 7;
constexpr std::uint64_t kUd_UnitBindingRecordId          = 8;

// ─── UBR field IDs (doc 06 §5.1) ────────────────────────────────────────
//
// The UBR canonical bytes carry only fields 1..9. `binding_auth` is the
// second element of the wrapping CBOR array — see doc 06 §9.3. Separating
// them avoids the self-referential commitment problem where
// binding_auth.record_hash would have to commit to bytes that include
// itself.
constexpr std::uint64_t kUbr_UnitBindingRecordId                  = 1;
constexpr std::uint64_t kUbr_UnitIdentityHash                     = 2;
constexpr std::uint64_t kUbr_UnitDescriptorHash                   = 3;
constexpr std::uint64_t kUbr_FamilyId                             = 4;
constexpr std::uint64_t kUbr_RequestedPolicyId                    = 5;
constexpr std::uint64_t kUbr_ResolvedFamilyProfileId              = 6;
constexpr std::uint64_t kUbr_ResolvedFamilyProfileContentHash     = 7;
constexpr std::uint64_t kUbr_PayloadIdentity                      = 8;
constexpr std::uint64_t kUbr_AntiDowngradeEpoch                   = 9;

// ─── PayloadIdentity sub-map field IDs (doc 06 §3.1) ────────────────────
constexpr std::uint64_t kPi_Sha256Digest  = 1;
constexpr std::uint64_t kPi_PayloadSize   = 2;

// ─── UnitBindingAuth field IDs (doc 06 §9.3) ────────────────────────────
constexpr std::uint64_t kAuth_Kind                    = 1;
constexpr std::uint64_t kAuth_UnitBindingTableHash    = 2;
constexpr std::uint64_t kAuth_InclusionIndex          = 3;
constexpr std::uint64_t kAuth_RecordHash              = 4;

constexpr std::string_view kAuthKindPackageSignedUnitInclusionV1 =
    "package_signed_unit_inclusion_v1";

inline tl::unexpected<UnitAcceptError> err(UnitAcceptError e) noexcept {
    return tl::make_unexpected(e);
}

template <typename E>
tl::unexpected<UnitAcceptError> err_as(UnitAcceptError e, const E&) noexcept {
    return tl::make_unexpected(e);
}

bool copy32(std::array<std::uint8_t, 32>& out,
            const std::vector<std::uint8_t>& src) noexcept {
    if (src.size() != 32) return false;
    std::memcpy(out.data(), src.data(), 32);
    return true;
}

bool hash_equals(const std::array<std::uint8_t, 32>& a,
                 const std::array<std::uint8_t, 32>& b) noexcept {
    return std::memcmp(a.data(), b.data(), 32) == 0;
}

UnitAcceptError map_eh_contract_error(
    VMPilot::Runtime::EH::ContractVerifyError error) noexcept {
    using VMPilot::Runtime::EH::ContractVerifyError;

    switch (error) {
        case ContractVerifyError::MalformedContract:
            return UnitAcceptError::ExceptionUnwindContractMalformed;
        case ContractVerifyError::ExecutableEhStatusNotReservedDisabled:
            return UnitAcceptError::ExecutableEhStatusNotReservedDisabled;
        case ContractVerifyError::CrossProtectedFrameUnwindPermitted:
            return UnitAcceptError::CrossProtectedFrameUnwindPermitted;
        case ContractVerifyError::NativeBoundaryBehaviorNotFailClosed:
            return UnitAcceptError::NativeBoundaryUnwindBehaviorNotFailClosed;
        case ContractVerifyError::HandlerTableNotReservedEmpty:
            return UnitAcceptError::HandlerTableNotReservedEmpty;
        case ContractVerifyError::CleanupTableNotReservedEmpty:
            return UnitAcceptError::CleanupTableNotReservedEmpty;
        case ContractVerifyError::UnknownCriticalExtension:
            return UnitAcceptError::UnknownEhCriticalExtension;
        case ContractVerifyError::FamilySpecificUnwindSurfaceMismatch:
            return UnitAcceptError::FamilySpecificUnwindSurfaceMismatch;
    }
    return UnitAcceptError::ExceptionUnwindContractMalformed;
}

// ─── Field extraction helpers ───────────────────────────────────────────
//
// Pre-bind the common CBOR templates to UnitAcceptError so call sites
// stay tidy. Every previous RequireCtx usage emitted the same
// {MissingCoreField, WrongFieldType, WrongHashSize} triple, so the
// per-call error customisation point was dead weight.

inline auto require_text(const Value& m, std::uint64_t k) noexcept {
    return VMPilot::Cbor::require_text<UnitAcceptError>(m, k);
}
inline auto require_uint(const Value& m, std::uint64_t k) noexcept {
    return VMPilot::Cbor::require_uint<UnitAcceptError>(m, k);
}
inline auto require_hash32(const Value& m, std::uint64_t k) noexcept {
    return VMPilot::Cbor::require_hash<UnitAcceptError, 32>(m, k);
}

// ─── PayloadIdentity ────────────────────────────────────────────────────

tl::expected<PayloadIdentity, UnitAcceptError>
parse_payload_identity(const Value& m, UnitAcceptError malformed_err) noexcept {
    if (m.kind() != Value::Kind::Map) return err(malformed_err);
auto digest = require_hash32(m, kPi_Sha256Digest);
    auto size_or = require_uint(m, kPi_PayloadSize);
    if (!digest) return err(digest.error());
    if (!size_or) return err(size_or.error());
    return PayloadIdentity{*digest, *size_or};
}

// ─── UnitDescriptor ─────────────────────────────────────────────────────

tl::expected<UnitDescriptor, UnitAcceptError>
parse_unit_descriptor_bytes(const std::vector<std::uint8_t>& bytes) noexcept {
    auto tree_or = parse_strict(bytes.data(), bytes.size());
    if (!tree_or) return err(UnitAcceptError::UnitDescriptorMalformed);
    const Value& tree = *tree_or;
    if (tree.kind() != Value::Kind::Map) return err(UnitAcceptError::UnitDescriptorMalformed);

    VMPILOT_TRY_ASSIGN(ver,  require_text(tree, kUd_DescriptorVersion));
    VMPILOT_TRY_ASSIGN(uid,  require_text(tree, kUd_UnitId));
    VMPILOT_TRY_ASSIGN(uih,  require_hash32(tree, kUd_UnitIdentityHash));
    VMPILOT_TRY_ASSIGN(fam,  require_text(tree, kUd_FamilyId));
    VMPILOT_TRY_ASSIGN(pol,  require_text(tree, kUd_RequestedPolicyId));
    VMPILOT_TRY_ASSIGN(prof, require_text(tree, kUd_ResolvedFamilyProfileId));
    VMPILOT_TRY_ASSIGN(ubr,  require_text(tree, kUd_UnitBindingRecordId));

    auto fam_enum = VMPilot::DomainLabels::parse_family_id(fam);
    if (!fam_enum) return err(UnitAcceptError::UnknownFamilyId);
    auto pol_enum = VMPilot::DomainLabels::parse_policy_id(pol);
    if (!pol_enum) return err(UnitAcceptError::UnknownPolicyId);

    const Value* pid_v = tree.find_by_uint_key(kUd_PayloadIdentity);
    if (pid_v == nullptr) return err(UnitAcceptError::MissingCoreField);
    VMPILOT_TRY_ASSIGN(pid,
        parse_payload_identity(*pid_v, UnitAcceptError::UnitDescriptorMalformed));

    UnitDescriptor out;
    out.descriptor_version              = std::string(ver);
    out.unit_id                          = std::string(uid);
    out.unit_identity_hash               = uih;
    out.family_id                        = *fam_enum;
    out.requested_policy_id              = *pol_enum;
    out.resolved_family_profile_id       = std::string(prof);
    out.payload_identity                 = pid;
    out.unit_binding_record_id           = std::string(ubr);
    return out;
}

// ─── UnitBindingAuth + UBR ──────────────────────────────────────────────

tl::expected<UnitBindingAuth, UnitAcceptError>
parse_binding_auth(const Value& auth_v) noexcept {
    if (auth_v.kind() != Value::Kind::Map) return err(UnitAcceptError::UnitBindingAuthMalformed);

auto kind = require_text(auth_v, kAuth_Kind);
    auto ubt_hash = require_hash32(auth_v, kAuth_UnitBindingTableHash);
    auto inclusion = require_uint(auth_v, kAuth_InclusionIndex);
    auto rec_hash = require_hash32(auth_v, kAuth_RecordHash);
    if (!kind) return err(kind.error());
    if (!ubt_hash) return err(ubt_hash.error());
    if (!inclusion) return err(inclusion.error());
    if (!rec_hash) return err(rec_hash.error());

    if (*kind != kAuthKindPackageSignedUnitInclusionV1) {
        return err(UnitAcceptError::UnitBindingAuthMalformed);
    }

    UnitBindingAuth out;
    out.kind                         = std::move(*kind);
    out.unit_binding_table_hash      = *ubt_hash;
    out.inclusion_index              = *inclusion;
    out.record_hash                  = *rec_hash;
    return out;
}

// Parse a single UBT entry (CBOR array[2] = [canonical_without_auth_bytes,
// UnitBindingAuth_map]) and verify the record_hash inside the auth object
// matches the domain-hashed canonical bytes. This is where doc 06 §9.3's
// "record_hash commits to UBR content minus auth" rule actually fires.
tl::expected<UnitBindingRecord, UnitAcceptError>
parse_unit_binding_record(const Value& wrapper_v) noexcept {
    if (wrapper_v.kind() != Value::Kind::Array) {
        return err(UnitAcceptError::UnitBindingRecordMalformed);
    }
    if (wrapper_v.as_array().size() != 2) {
        return err(UnitAcceptError::UnitBindingRecordMalformed);
    }
    const Value& canonical_v = wrapper_v.as_array()[0];
    const Value& auth_v      = wrapper_v.as_array()[1];
    if (canonical_v.kind() != Value::Kind::Bytes) {
        return err(UnitAcceptError::UnitBindingRecordMalformed);
    }
    if (auth_v.kind() != Value::Kind::Map) {
        return err(UnitAcceptError::UnitBindingAuthMalformed);
    }

    const auto& canonical_bytes = canonical_v.as_bytes();
    auto inner_or = parse_strict(canonical_bytes.data(), canonical_bytes.size());
    if (!inner_or) return err(UnitAcceptError::UnitBindingRecordMalformed);
    const Value& ubr_v = *inner_or;
    if (ubr_v.kind() != Value::Kind::Map) {
        return err(UnitAcceptError::UnitBindingRecordMalformed);
    }

auto id = require_text(ubr_v, kUbr_UnitBindingRecordId);
    auto uih = require_hash32(ubr_v, kUbr_UnitIdentityHash);
    auto udh = require_hash32(ubr_v, kUbr_UnitDescriptorHash);
    auto fam = require_text(ubr_v, kUbr_FamilyId);
    auto pol = require_text(ubr_v, kUbr_RequestedPolicyId);
    auto prof = require_text(ubr_v, kUbr_ResolvedFamilyProfileId);
    auto pch = require_hash32(ubr_v, kUbr_ResolvedFamilyProfileContentHash);
    auto epoch = require_uint(ubr_v, kUbr_AntiDowngradeEpoch);
    if (!id) return err(id.error());
    if (!uih) return err(uih.error());
    if (!udh) return err(udh.error());
    if (!fam) return err(fam.error());
    if (!pol) return err(pol.error());
    if (!prof) return err(prof.error());
    if (!pch) return err(pch.error());
    if (!epoch) return err(epoch.error());

    auto fam_enum = VMPilot::DomainLabels::parse_family_id(*fam);
    if (!fam_enum) return err(UnitAcceptError::UnknownFamilyId);
    auto pol_enum = VMPilot::DomainLabels::parse_policy_id(*pol);
    if (!pol_enum) return err(UnitAcceptError::UnknownPolicyId);

    const Value* pid_v = ubr_v.find_by_uint_key(kUbr_PayloadIdentity);
    if (pid_v == nullptr) return err(UnitAcceptError::MissingCoreField);
    auto pid = parse_payload_identity(*pid_v, UnitAcceptError::UnitBindingRecordMalformed);
    if (!pid) return err(pid.error());

    auto auth = parse_binding_auth(auth_v);
    if (!auth) return err(auth.error());

    // Record hash must commit to the exact canonical bytes we just parsed.
    // Without this the wrapper's element [0] could be swapped for any
    // other UBR's canonical bytes and acceptance would continue on
    // stale content.
    const auto computed_record_hash = domain_hash_sha256(
        VMPilot::DomainLabels::Hash::UnitBindingRecord,
        canonical_bytes.data(), canonical_bytes.size());
    if (!hash_equals(computed_record_hash, auth->record_hash)) {
        return err(UnitAcceptError::UnitBindingRecordRecordHashMismatch);
    }

    UnitBindingRecord out;
    out.unit_binding_record_id                    = std::move(*id);
    out.unit_identity_hash                         = *uih;
    out.unit_descriptor_hash                       = *udh;
    out.family_id                                  = *fam_enum;
    out.requested_policy_id                        = *pol_enum;
    out.resolved_family_profile_id                 = std::move(*prof);
    out.resolved_family_profile_content_hash       = *pch;
    out.payload_identity                           = *pid;
    out.anti_downgrade_epoch                       = *epoch;
    out.binding_auth                               = std::move(*auth);
    return out;
}

// ─── Table lookups ──────────────────────────────────────────────────────

// UnitDescriptorTable: CBOR map { unit_id (text) : descriptor_bytes }.
tl::expected<std::vector<std::uint8_t>, UnitAcceptError>
lookup_descriptor_bytes(const std::vector<std::uint8_t>& table_bytes,
                        std::string_view unit_id) noexcept {
    auto tree_or = parse_strict(table_bytes.data(), table_bytes.size());
    if (!tree_or) return err(UnitAcceptError::UnitDescriptorTableMalformed);
    const Value& tree = *tree_or;
    if (tree.kind() != Value::Kind::Map) return err(UnitAcceptError::UnitDescriptorTableMalformed);

    const Value* v = tree.find_by_text_key(unit_id);
    if (v == nullptr) return err(UnitAcceptError::UnitDescriptorNotFound);
    if (v->kind() != Value::Kind::Bytes) return err(UnitAcceptError::UnitDescriptorTableMalformed);
    return v->as_bytes();
}

// UnitBindingTable: CBOR array of [canonical_bytes, auth_map] wrappers.
// Linear scan — fully parse each wrapper, match by record_id, and return
// the parsed UBR with its inclusion_index cross-checked against its
// position in the array. Any structural anomaly surfaces UbtMalformed
// before content mismatches so fuzzed or truncated tables don't reach
// downstream acceptance logic with half-initialised state.
tl::expected<UnitBindingRecord, UnitAcceptError>
lookup_ubr(const std::vector<std::uint8_t>& table_bytes,
           std::string_view record_id) noexcept {
    auto tree_or = parse_strict(table_bytes.data(), table_bytes.size());
    if (!tree_or) return err(UnitAcceptError::UnitBindingTableMalformed);
    const Value& tree = *tree_or;
    if (tree.kind() != Value::Kind::Array) return err(UnitAcceptError::UnitBindingTableMalformed);

    const auto& entries = tree.as_array();
    for (std::size_t i = 0; i < entries.size(); ++i) {
        const Value& entry = entries[i];
        // Fast filter: peek at element [0] canonical bytes for the
        // record id before paying for full parse + hash verification.
        if (entry.kind() != Value::Kind::Array) {
            return err(UnitAcceptError::UnitBindingTableMalformed);
        }
        if (entry.as_array().size() != 2) {
            return err(UnitAcceptError::UnitBindingTableMalformed);
        }
        const Value& canonical_v = entry.as_array()[0];
        if (canonical_v.kind() != Value::Kind::Bytes) {
            return err(UnitAcceptError::UnitBindingTableMalformed);
        }
        auto peek_or = parse_strict(canonical_v.as_bytes().data(),
                                    canonical_v.as_bytes().size());
        if (!peek_or) continue;
        const Value* id_v = peek_or->find_by_uint_key(kUbr_UnitBindingRecordId);
        if (id_v == nullptr || id_v->kind() != Value::Kind::Text) continue;
        if (id_v->as_text() != record_id) continue;

        // Match: parse fully (verifies record_hash) and cross-check the
        // auth's inclusion_index against the UBR's real position.
        auto parsed = parse_unit_binding_record(entry);
        if (!parsed) return err(parsed.error());
        if (parsed->binding_auth.inclusion_index != i) {
            return err(UnitAcceptError::UnitBindingAuthMalformed);
        }
        return parsed;
    }
    return err(UnitAcceptError::UnitBindingRecordNotFound);
}

// ResolvedFamilyProfileTable: CBOR map { profile_id (text) : profile_bytes }.
tl::expected<std::vector<std::uint8_t>, UnitAcceptError>
lookup_profile_bytes(const std::vector<std::uint8_t>& table_bytes,
                     std::string_view profile_id) noexcept {
    auto tree_or = parse_strict(table_bytes.data(), table_bytes.size());
    if (!tree_or) return err(UnitAcceptError::ResolvedProfileTableMalformed);
    const Value& tree = *tree_or;
    if (tree.kind() != Value::Kind::Map) return err(UnitAcceptError::ResolvedProfileTableMalformed);

    const Value* v = tree.find_by_text_key(profile_id);
    if (v == nullptr) return err(UnitAcceptError::ResolvedProfileNotFound);
    if (v->kind() != Value::Kind::Bytes) return err(UnitAcceptError::ResolvedProfileTableMalformed);
    return v->as_bytes();
}

}  // namespace

tl::expected<AcceptedUnit, UnitAcceptError>
accept_unit_entry(const std::uint8_t* artifact_data,
                  std::size_t artifact_size,
                  const Envelope::OuterEnvelope& env,
                  const AcceptedPackage& accepted_pkg,
                  std::string_view unit_id,
                  const AcceptConfig& config) noexcept {
    // Carve the inner partition + payload partition out of the buffer.
    if (env.inner_metadata_partition.offset +
            env.inner_metadata_partition.length > artifact_size) {
        return err(UnitAcceptError::InnerPartitionMalformed);
    }
    if (env.payload_partition.offset +
            env.payload_partition.length > artifact_size) {
        return err(UnitAcceptError::InnerPartitionMalformed);
    }
    auto inner_or = parse_inner_partition(
        artifact_data + env.inner_metadata_partition.offset,
        env.inner_metadata_partition.length);
    if (!inner_or) return err(UnitAcceptError::InnerPartitionMalformed);
    const InnerPartition& inner = *inner_or;

    // 1. Locate the descriptor for this unit. Both descriptor and UBR
    //    need to exist before we can start cross-checking.
    auto desc_bytes_or = lookup_descriptor_bytes(inner.unit_descriptor_table, unit_id);
    if (!desc_bytes_or) return err(desc_bytes_or.error());
    const std::vector<std::uint8_t>& desc_bytes = *desc_bytes_or;

    auto desc_or = parse_unit_descriptor_bytes(desc_bytes);
    if (!desc_or) return err(desc_or.error());
    UnitDescriptor desc = std::move(*desc_or);

    // 2. Use the descriptor's binding-record id to fetch the UBR.
    auto ubr_or = lookup_ubr(inner.unit_binding_table, desc.unit_binding_record_id);
    if (!ubr_or) return err(ubr_or.error());
    UnitBindingRecord ubr = std::move(*ubr_or);

    // 3. Inclusion is already proven by construction: Stage 5 verified the
    //    inner partition's unit_binding_table bytes against the signed PBR
    //    commitment, and we just pulled this UBR out of those verified
    //    bytes. The `binding_auth.unit_binding_table_hash` field is an
    //    informational pointer (hash of the UBT under a non-cyclic
    //    definition chosen by the producer); it is deliberately not
    //    re-verified here because doing so would require a cyclic
    //    definition (the field would have to commit to bytes that include
    //    itself). PackageUbtHashMismatch is retained as a reserved error
    //    for a future tighter Merkle-style inclusion proof.
    (void)accepted_pkg.unit_binding_table_hash;  // asserted-by-Stage-5

    // 4. Descriptor hash: the UBR commits to the canonical descriptor
    //    bytes; re-hash and compare.
    const auto desc_hash = domain_hash_sha256(
        VMPilot::DomainLabels::Hash::UnitDescriptor,
        desc_bytes.data(), desc_bytes.size());
    if (!hash_equals(desc_hash, ubr.unit_descriptor_hash)) {
        return err(UnitAcceptError::UnitDescriptorHashMismatch);
    }

    // 5. Descriptor fields must agree with the UBR's commitments. These
    //    are cheap but important cross-checks — a forger that swaps the
    //    descriptor without updating UBR trips here even if the hash
    //    check (step 4) happens to collide.
    if (desc.family_id != ubr.family_id) {
        return err(UnitAcceptError::DescriptorFamilyIdMismatch);
    }
    if (desc.requested_policy_id != ubr.requested_policy_id) {
        return err(UnitAcceptError::DescriptorPolicyIdMismatch);
    }
    if (desc.resolved_family_profile_id != ubr.resolved_family_profile_id) {
        return err(UnitAcceptError::DescriptorProfileIdMismatch);
    }
    if (!hash_equals(desc.unit_identity_hash, ubr.unit_identity_hash)) {
        return err(UnitAcceptError::DescriptorUnitIdentityMismatch);
    }
    if (desc.unit_binding_record_id != ubr.unit_binding_record_id) {
        return err(UnitAcceptError::UnitDescriptorBindingRecordIdMismatch);
    }

    // 6. Profile: hash-verify. Full profile parsing waits for Stage 8.
    auto profile_bytes_or = lookup_profile_bytes(
        inner.resolved_profile_table, desc.resolved_family_profile_id);
    if (!profile_bytes_or) return err(profile_bytes_or.error());
    std::vector<std::uint8_t> profile_bytes = std::move(*profile_bytes_or);

    const auto profile_hash = domain_hash_sha256(
        VMPilot::DomainLabels::Hash::ResolvedFamilyProfile,
        profile_bytes.data(), profile_bytes.size());
    if (!hash_equals(profile_hash, ubr.resolved_family_profile_content_hash)) {
        return err(UnitAcceptError::ResolvedProfileContentHashMismatch);
    }

    // 6b. Profile ↔ UBR cross-check (doc 08 §9 #2). The profile's own
    //     embedded (family_id / policy_id / profile_id) must agree with
    //     the UBR that referenced it. Otherwise a producer could slide a
    //     debug-policy profile in under a highsec-policy UBR: every
    //     hash still matches (UBR committed to this exact profile), but
    //     the profile's declared policy contradicts the UBR's claim and
    //     the runtime would dispatch on the wrong tier.
    auto profile_header = parse_resolved_family_profile_header(profile_bytes);
    if (!profile_header) {
        return err(UnitAcceptError::ResolvedProfileTableMalformed);
    }
    if (profile_header->family_id != ubr.family_id) {
        return err(UnitAcceptError::ProfileFamilyIdMismatch);
    }
    if (profile_header->requested_policy_id != ubr.requested_policy_id) {
        return err(UnitAcceptError::ProfilePolicyIdMismatch);
    }
    if (profile_header->profile_id != ubr.resolved_family_profile_id) {
        return err(UnitAcceptError::ProfileIdMismatch);
    }

    // 6c. Stage 9 — 1.0 reserved exception/unwind surface verifier.
    //     The profile must carry the typed contract, but 1.0 runtime only
    //     accepts the strictly reserved posture: executable EH disabled,
    //     empty handler/cleanup tables, cross-frame unwind forbidden, and
    //     native boundary unwind translated to trap/fail-closed.
    auto eh_contract =
        VMPilot::Runtime::EH::verify_reserved_exception_unwind_contract(
            profile_bytes, ubr.family_id);
    if (!eh_contract) {
        return err(map_eh_contract_error(eh_contract.error()));
    }

    // 7. Payload identity: single-unit packaging for now — the whole
    //    payload partition belongs to this unit. Multi-unit payload
    //    slicing is a future stage.
    const std::size_t payload_off = static_cast<std::size_t>(env.payload_partition.offset);
    const std::size_t payload_len = static_cast<std::size_t>(env.payload_partition.length);
    if (payload_len != ubr.payload_identity.payload_size) {
        return err(UnitAcceptError::PayloadSizeMismatch);
    }
    // Hash payload bytes with plain SHA-256 (no domain separation —
    // payload_identity.sha256_digest is a straight SHA-256 per doc 06
    // §3.1, not a domain-hashed digest).
    std::vector<std::uint8_t> payload_copy(
        artifact_data + payload_off, artifact_data + payload_off + payload_len);
    const auto sha_vec = VMPilot::Crypto::SHA256(payload_copy, /*salt=*/{});
    std::array<std::uint8_t, 32> sha{};
    if (sha_vec.size() != 32) return err(UnitAcceptError::PayloadSha256Mismatch);
    std::memcpy(sha.data(), sha_vec.data(), 32);
    if (!hash_equals(sha, ubr.payload_identity.sha256_digest)) {
        return err(UnitAcceptError::PayloadSha256Mismatch);
    }

    // 8. Anti-downgrade gates.
    //    8a. UBR vs runtime floor.
    if (ubr.anti_downgrade_epoch < config.epoch.minimum_accepted_epoch) {
        return err(UnitAcceptError::AntiDowngradeEpochTooOld);
    }
    //    8b. Package must monotonically cover the unit (doc 06 §10):
    //         package anti-downgrade epoch >= unit anti-downgrade epoch.
    //         Otherwise a PBR from an older ship could paper over a newer
    //         UBR, which is exactly the downgrade scenario the lattice
    //         exists to prevent.
    if (accepted_pkg.anti_downgrade_epoch < ubr.anti_downgrade_epoch) {
        return err(UnitAcceptError::PackageEpochBelowUnitEpoch);
    }

    //    8c. Runtime policy floor (doc 15 §9 #4). The enum ordering is
    //         Debug(1) < Standard(2) < HighSec(3) — numerically
    //         comparable. A standard-tier package cannot unlock a
    //         runtime that requires highsec.
    if (static_cast<std::uint8_t>(ubr.requested_policy_id) <
        static_cast<std::uint8_t>(config.minimum_policy_floor)) {
        return err(UnitAcceptError::PolicyBelowRuntimeFloor);
    }

    AcceptedUnit out;
    out.descriptor               = std::move(desc);
    out.ubr                      = std::move(ubr);
    out.resolved_profile_bytes   = std::move(profile_bytes);
    out.payload_offset           = payload_off;
    out.payload_length           = payload_len;
    return out;
}

}  // namespace VMPilot::Runtime::Binding
