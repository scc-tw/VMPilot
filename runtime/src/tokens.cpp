#include "tokens.hpp"

#include <cstring>
#include <string>

#include <VMPilot_crypto.hpp>

#include "binding/signed_partition.hpp"
#include "cbor/strict.hpp"
#include "vm/domain_labels.hpp"

namespace VMPilot::Cbor {
template <>
struct RequireErrors<VMPilot::Runtime::Tokens::TokenError> {
    using E = VMPilot::Runtime::Tokens::TokenError;
    static constexpr E missing_field    = E::MissingCoreField;
    static constexpr E wrong_field_type = E::WrongFieldType;
    static constexpr E wrong_hash_size  = E::WrongHashSize;
};
}  // namespace VMPilot::Cbor

namespace VMPilot::Runtime::Binding {
template <>
struct SignedPartitionErrors<VMPilot::Runtime::Tokens::TokenError> {
    using E = VMPilot::Runtime::Tokens::TokenError;
    // Token layer collapses most wrapper-level failures into
    // TokenMalformed to keep the public surface narrow (doc 10 §9.1
    // redaction rule). Key-id mismatch is the one exception — it
    // flags trust-root drift, which has its own code.
    static constexpr E partition_malformed           = E::TokenMalformed;
    static constexpr E auth_kind_unsupported         = E::TokenMalformed;
    static constexpr E auth_key_id_mismatch          = E::TrustRootKeyUsageMismatch;
    static constexpr E auth_signature_alg_mismatch   = E::TokenMalformed;
    static constexpr E auth_covered_domain_mismatch  = E::TokenMalformed;
    static constexpr E signature_wrong_size          = E::SignatureWrongSize;
    static constexpr E signature_invalid             = E::SignatureInvalid;
    static constexpr E missing_core_field            = E::MissingCoreField;
    static constexpr E wrong_field_type              = E::WrongFieldType;
};
}  // namespace VMPilot::Runtime::Binding

namespace VMPilot::Runtime::Tokens {

namespace {

using VMPilot::Cbor::Value;
using VMPilot::Cbor::parse_strict;

// Shared auth-wrapper keys (doc 06 §9.2 pattern).
// ReprovisionToken field keys.
namespace Rep {
    constexpr std::uint64_t kTokenVersion                        = 1;
    constexpr std::uint64_t kOldEnrollmentId                     = 2;
    constexpr std::uint64_t kNewEnrollmentId                     = 3;
    constexpr std::uint64_t kCustomerAccountIdHash               = 4;
    constexpr std::uint64_t kOldAttestedKeyHash                  = 5;
    constexpr std::uint64_t kNewAttestedKeyHash                  = 6;
    constexpr std::uint64_t kNewProviderEvidenceHash             = 7;
    constexpr std::uint64_t kAllowedPackageBindingRecordHash     = 8;
    constexpr std::uint64_t kAllowedPolicyFloor                  = 9;
    constexpr std::uint64_t kAllowedFamilySet                    = 10;
    constexpr std::uint64_t kReasonCode                          = 11;
    constexpr std::uint64_t kSupportCaseId                       = 12;
    constexpr std::uint64_t kApprovalChainHash                   = 13;
    constexpr std::uint64_t kIssuedAt                            = 14;
    constexpr std::uint64_t kExpiresAt                           = 15;
    constexpr std::uint64_t kOneTimeNonce                        = 16;
}  // namespace Rep

// MigrationToken field keys.
namespace Mig {
    constexpr std::uint64_t kTokenVersion                        = 1;
    constexpr std::uint64_t kOldPackageBindingRecordHash         = 2;
    constexpr std::uint64_t kNewPackageBindingRecordHash         = 3;
    constexpr std::uint64_t kCustomerAccountIdHash               = 4;
    constexpr std::uint64_t kAllowedImportOnce                   = 5;
    constexpr std::uint64_t kAllowedPolicyFloor                  = 6;
    constexpr std::uint64_t kExpiresAt                           = 7;
    constexpr std::uint64_t kNonce                               = 8;
}  // namespace Mig

inline tl::unexpected<TokenError> err(TokenError code) noexcept {
    return tl::make_unexpected(code);
}

inline auto require_text(const Value& m, std::uint64_t k) noexcept {
    return VMPilot::Cbor::require_text<TokenError>(m, k);
}
inline auto require_hash32(const Value& m, std::uint64_t k) noexcept {
    return VMPilot::Cbor::require_hash<TokenError, 32>(m, k);
}
inline auto require_uint(const Value& m, std::uint64_t k) noexcept {
    return VMPilot::Cbor::require_uint<TokenError>(m, k);
}
// Strict CBOR subset has no `bool` kind (doc 07 §3). Canonical
// encoding for the single-bit `allowed_import_once` field is a
// uint — 0 → false, 1 → true; any other value is malformed.
inline auto require_bool(const Value& m, std::uint64_t k) noexcept {
    return VMPilot::Cbor::require_uint_bool<TokenError>(m, k);
}

tl::expected<VMPilot::DomainLabels::PolicyId, TokenError> parse_policy_id(
    std::string_view text) noexcept {
    auto parsed = VMPilot::DomainLabels::parse_policy_id(text);
    if (!parsed.has_value()) return err(TokenError::UnknownEnumValue);
    return *parsed;
}

tl::expected<VMPilot::DomainLabels::FamilyId, TokenError> parse_family_id(
    std::string_view text) noexcept {
    auto parsed = VMPilot::DomainLabels::parse_family_id(text);
    if (!parsed.has_value()) return err(TokenError::UnknownEnumValue);
    return *parsed;
}

tl::expected<ReasonCode, TokenError> parse_reason_code(
    std::string_view text) noexcept {
    auto parsed = VMPilot::enum_from_text<ReasonCode>(text);
    if (!parsed.has_value()) return err(TokenError::UnknownEnumValue);
    return *parsed;
}

}  // namespace

// ─── Reprovision ─────────────────────────────────────────────────────────

tl::expected<ReprovisionToken, TokenError>
parse_reprovision_token(const std::uint8_t* data, std::size_t size,
                        const VendorTrustRoot& root) noexcept {
    if (!trust_root_is_well_formed()) {
        return err(TokenError::TrustRootKeyUsageMismatch);
    }
    if (std::strcmp(root.key_usage, VMPilot::Runtime::kTrustRootKeyUsage) != 0) {
        return err(TokenError::TrustRootKeyUsageMismatch);
    }

    auto outer_or = parse_strict(data, size);
    if (!outer_or) return err(TokenError::TokenMalformed);
    auto view_or = VMPilot::Runtime::Binding::verify_signed_partition_view<TokenError>(
        *outer_or, root, VMPilot::DomainLabels::Auth::ReprovisionToken);
    if (!view_or) return err(view_or.error());
    const auto& canonical_bytes = *view_or->canonical_bytes;
    const auto& signature_bytes = *view_or->signature_bytes;

    auto inner_or = parse_strict(canonical_bytes.data(), canonical_bytes.size());
    if (!inner_or) return err(TokenError::TokenMalformed);
    const Value& m = *inner_or;
    if (m.kind() != Value::Kind::Map) return err(TokenError::TokenMalformed);

    ReprovisionToken out;
    auto token_version_or     = require_text(m, Rep::kTokenVersion);
    auto old_enrollment_or    = require_text(m, Rep::kOldEnrollmentId);
    auto new_enrollment_or    = require_text(m, Rep::kNewEnrollmentId);
    auto customer_hash_or     = require_hash32(m, Rep::kCustomerAccountIdHash);
    auto old_key_hash_or      = require_hash32(m, Rep::kOldAttestedKeyHash);
    auto new_key_hash_or      = require_hash32(m, Rep::kNewAttestedKeyHash);
    auto new_evidence_hash_or = require_hash32(m, Rep::kNewProviderEvidenceHash);
    auto allowed_pkg_hash_or  = require_hash32(m, Rep::kAllowedPackageBindingRecordHash);
    auto policy_floor_text_or = require_text(m, Rep::kAllowedPolicyFloor);
    auto reason_text_or       = require_text(m, Rep::kReasonCode);
    auto support_case_or      = require_text(m, Rep::kSupportCaseId);
    auto approval_chain_or    = require_hash32(m, Rep::kApprovalChainHash);
    auto issued_at_or         = require_uint(m, Rep::kIssuedAt);
    auto expires_at_or        = require_uint(m, Rep::kExpiresAt);
    auto one_time_nonce_or    = require_hash32(m, Rep::kOneTimeNonce);

    if (!token_version_or)     return err(token_version_or.error());
    if (!old_enrollment_or)    return err(old_enrollment_or.error());
    if (!new_enrollment_or)    return err(new_enrollment_or.error());
    if (!customer_hash_or)     return err(customer_hash_or.error());
    if (!old_key_hash_or)      return err(old_key_hash_or.error());
    if (!new_key_hash_or)      return err(new_key_hash_or.error());
    if (!new_evidence_hash_or) return err(new_evidence_hash_or.error());
    if (!allowed_pkg_hash_or)  return err(allowed_pkg_hash_or.error());
    if (!policy_floor_text_or) return err(policy_floor_text_or.error());
    if (!reason_text_or)       return err(reason_text_or.error());
    if (!support_case_or)      return err(support_case_or.error());
    if (!approval_chain_or)    return err(approval_chain_or.error());
    if (!issued_at_or)         return err(issued_at_or.error());
    if (!expires_at_or)        return err(expires_at_or.error());
    if (!one_time_nonce_or)    return err(one_time_nonce_or.error());

    auto policy_or = parse_policy_id(*policy_floor_text_or);
    if (!policy_or) return err(policy_or.error());

    // allowed_family_set is an array of text labels.
    const Value* fam_v = m.find_by_uint_key(Rep::kAllowedFamilySet);
    if (fam_v == nullptr) return err(TokenError::MissingCoreField);
    if (fam_v->kind() != Value::Kind::Array) return err(TokenError::WrongFieldType);
    std::vector<VMPilot::DomainLabels::FamilyId> families;
    families.reserve(fam_v->as_array().size());
    for (const Value& entry : fam_v->as_array()) {
        if (entry.kind() != Value::Kind::Text) return err(TokenError::WrongFieldType);
        auto family_or = parse_family_id(entry.as_text());
        if (!family_or) return err(family_or.error());
        families.push_back(*family_or);
    }

    auto reason_or = parse_reason_code(*reason_text_or);
    if (!reason_or) return err(reason_or.error());

    out.token_version                        = std::move(*token_version_or);
    out.old_enrollment_id                    = std::move(*old_enrollment_or);
    out.new_enrollment_id                    = std::move(*new_enrollment_or);
    out.customer_account_id_hash             = *customer_hash_or;
    out.old_attested_key_hash                = *old_key_hash_or;
    out.new_attested_key_hash                = *new_key_hash_or;
    out.new_provider_evidence_hash           = *new_evidence_hash_or;
    out.allowed_package_binding_record_hash  = *allowed_pkg_hash_or;
    out.allowed_policy_floor                 = *policy_or;
    out.allowed_family_set                   = std::move(families);
    out.reason_code                          = *reason_or;
    out.support_case_id                      = std::move(*support_case_or);
    out.approval_chain_hash                  = *approval_chain_or;
    out.issued_at                            = *issued_at_or;
    out.expires_at                           = *expires_at_or;
    out.one_time_nonce                       = *one_time_nonce_or;
    out.vendor_signature                     = signature_bytes;

    if (out.token_version != "reprovision-token-v1") {
        return err(TokenError::TokenVersionUnsupported);
    }
    return out;
}

tl::expected<ReprovisionToken, TokenError>
accept_reprovision_token(const std::uint8_t* data, std::size_t size,
                         const VendorTrustRoot& root,
                         const ReprovisionContext& ctx,
                         NonceStore& store) noexcept {
    auto token_or = parse_reprovision_token(data, size, root);
    if (!token_or) return err(token_or.error());

    const ReprovisionToken& t = *token_or;

    if (t.expires_at <= ctx.now_unix_seconds) {
        return err(TokenError::TokenExpired);
    }
    if (store.is_consumed(t.one_time_nonce)) {
        return err(TokenError::NonceAlreadyConsumed);
    }
    if (!ctx.old_enrollment_is_revoked) {
        return err(TokenError::OldEnrollmentNotRevoked);
    }
    if (t.allowed_package_binding_record_hash !=
        ctx.current_package_binding_record_hash) {
        return err(TokenError::PackageHashMismatch);
    }
    if (static_cast<std::uint8_t>(t.allowed_policy_floor) <
        static_cast<std::uint8_t>(ctx.required_policy_floor)) {
        return err(TokenError::PolicyFloorBelowRequired);
    }
    if (t.new_provider_evidence_hash != ctx.new_provider_evidence_hash) {
        return err(TokenError::NewProviderEvidenceMismatch);
    }

    store.mark_consumed(t.one_time_nonce);
    return token_or;
}

// ─── Migration ───────────────────────────────────────────────────────────

tl::expected<MigrationToken, TokenError>
parse_migration_token(const std::uint8_t* data, std::size_t size,
                      const VendorTrustRoot& root) noexcept {
    if (!trust_root_is_well_formed()) {
        return err(TokenError::TrustRootKeyUsageMismatch);
    }
    if (std::strcmp(root.key_usage, VMPilot::Runtime::kTrustRootKeyUsage) != 0) {
        return err(TokenError::TrustRootKeyUsageMismatch);
    }

    auto outer_or = parse_strict(data, size);
    if (!outer_or) return err(TokenError::TokenMalformed);
    auto view_or = VMPilot::Runtime::Binding::verify_signed_partition_view<TokenError>(
        *outer_or, root, VMPilot::DomainLabels::Auth::MigrationToken);
    if (!view_or) return err(view_or.error());
    const auto& canonical_bytes = *view_or->canonical_bytes;
    const auto& signature_bytes = *view_or->signature_bytes;

    auto inner_or = parse_strict(canonical_bytes.data(), canonical_bytes.size());
    if (!inner_or) return err(TokenError::TokenMalformed);
    const Value& m = *inner_or;
    if (m.kind() != Value::Kind::Map) return err(TokenError::TokenMalformed);

    auto token_version_or   = require_text(m, Mig::kTokenVersion);
    auto old_pkg_hash_or    = require_hash32(m, Mig::kOldPackageBindingRecordHash);
    auto new_pkg_hash_or    = require_hash32(m, Mig::kNewPackageBindingRecordHash);
    auto customer_hash_or   = require_hash32(m, Mig::kCustomerAccountIdHash);
    auto import_once_or     = require_bool(m, Mig::kAllowedImportOnce);
    auto policy_floor_or    = require_text(m, Mig::kAllowedPolicyFloor);
    auto expires_at_or      = require_uint(m, Mig::kExpiresAt);
    auto nonce_or           = require_hash32(m, Mig::kNonce);

    if (!token_version_or)   return err(token_version_or.error());
    if (!old_pkg_hash_or)    return err(old_pkg_hash_or.error());
    if (!new_pkg_hash_or)    return err(new_pkg_hash_or.error());
    if (!customer_hash_or)   return err(customer_hash_or.error());
    if (!import_once_or)     return err(import_once_or.error());
    if (!policy_floor_or)    return err(policy_floor_or.error());
    if (!expires_at_or)      return err(expires_at_or.error());
    if (!nonce_or)           return err(nonce_or.error());

    auto policy_id_or = parse_policy_id(*policy_floor_or);
    if (!policy_id_or) return err(policy_id_or.error());

    if (!*import_once_or) return err(TokenError::AllowedImportOnceNotTrue);

    MigrationToken out;
    out.token_version                         = std::move(*token_version_or);
    out.old_package_binding_record_hash       = *old_pkg_hash_or;
    out.new_package_binding_record_hash       = *new_pkg_hash_or;
    out.customer_account_id_hash              = *customer_hash_or;
    out.allowed_import_once                   = *import_once_or;
    out.allowed_policy_floor                  = *policy_id_or;
    out.expires_at                            = *expires_at_or;
    out.nonce                                 = *nonce_or;
    out.vendor_signature                      = signature_bytes;

    if (out.token_version != "migration-token-v1") {
        return err(TokenError::TokenVersionUnsupported);
    }
    return out;
}

tl::expected<MigrationToken, TokenError>
accept_migration_token(const std::uint8_t* data, std::size_t size,
                       const VendorTrustRoot& root,
                       const MigrationContext& ctx,
                       NonceStore& store) noexcept {
    auto token_or = parse_migration_token(data, size, root);
    if (!token_or) return err(token_or.error());

    const MigrationToken& t = *token_or;

    if (t.expires_at <= ctx.now_unix_seconds) {
        return err(TokenError::TokenExpired);
    }
    if (store.is_consumed(t.nonce)) {
        return err(TokenError::NonceAlreadyConsumed);
    }
    if (t.old_package_binding_record_hash !=
        ctx.current_package_binding_record_hash) {
        return err(TokenError::PackageHashMismatch);
    }
    if (t.new_package_binding_record_hash !=
        ctx.target_package_binding_record_hash) {
        return err(TokenError::PackageHashMismatch);
    }
    if (static_cast<std::uint8_t>(t.allowed_policy_floor) <
        static_cast<std::uint8_t>(ctx.required_policy_floor)) {
        return err(TokenError::PolicyFloorBelowRequired);
    }

    store.mark_consumed(t.nonce);
    return token_or;
}

}  // namespace VMPilot::Runtime::Tokens
