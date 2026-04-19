#include "tokens.hpp"

#include <cstring>
#include <string>

#include <VMPilot_crypto.hpp>

#include "binding/signed_partition.hpp"
#include "cbor/schema.hpp"
#include "cbor/strict.hpp"
#include "vm/domain_labels.hpp"

namespace VMPilot::Cbor {
template <>
struct CborConsumerTraits<VMPilot::Runtime::Tokens::TokenError> {
    using E = VMPilot::Runtime::Tokens::TokenError;
    static constexpr E missing_field                 = E::MissingCoreField;
    static constexpr E wrong_field_type              = E::WrongFieldType;
    static constexpr E wrong_hash_size               = E::WrongHashSize;
    static constexpr E bad_cbor                      = E::TokenMalformed;
    static constexpr E not_a_map                     = E::TokenMalformed;
    static constexpr E unknown_enum_value            = E::UnknownEnumValue;
    static constexpr E array_too_long                = E::WrongFieldType;
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
};
}  // namespace VMPilot::Cbor

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

    using namespace VMPilot::Cbor::Schema;
    const auto schema = std::make_tuple(
        TextField<ReprovisionToken>{Rep::kTokenVersion,
                                    &ReprovisionToken::token_version},
        TextField<ReprovisionToken>{Rep::kOldEnrollmentId,
                                    &ReprovisionToken::old_enrollment_id},
        TextField<ReprovisionToken>{Rep::kNewEnrollmentId,
                                    &ReprovisionToken::new_enrollment_id},
        HashField<ReprovisionToken>{Rep::kCustomerAccountIdHash,
                                    &ReprovisionToken::customer_account_id_hash},
        HashField<ReprovisionToken>{Rep::kOldAttestedKeyHash,
                                    &ReprovisionToken::old_attested_key_hash},
        HashField<ReprovisionToken>{Rep::kNewAttestedKeyHash,
                                    &ReprovisionToken::new_attested_key_hash},
        HashField<ReprovisionToken>{Rep::kNewProviderEvidenceHash,
                                    &ReprovisionToken::new_provider_evidence_hash},
        HashField<ReprovisionToken>{Rep::kAllowedPackageBindingRecordHash,
                                    &ReprovisionToken::allowed_package_binding_record_hash},
        EnumTextField<ReprovisionToken,
                      VMPilot::DomainLabels::PolicyId, TokenError>{
            Rep::kAllowedPolicyFloor, &ReprovisionToken::allowed_policy_floor,
            TokenError::UnknownEnumValue},
        EnumTextArrayField<ReprovisionToken,
                           VMPilot::DomainLabels::FamilyId>{
            Rep::kAllowedFamilySet, &ReprovisionToken::allowed_family_set,
            /*min_length=*/0, /*max_length=*/64},
        EnumTextField<ReprovisionToken, ReasonCode, TokenError>{
            Rep::kReasonCode, &ReprovisionToken::reason_code,
            TokenError::UnknownEnumValue},
        TextField<ReprovisionToken>{Rep::kSupportCaseId,
                                    &ReprovisionToken::support_case_id},
        HashField<ReprovisionToken>{Rep::kApprovalChainHash,
                                    &ReprovisionToken::approval_chain_hash},
        UintField<ReprovisionToken>{Rep::kIssuedAt,
                                    &ReprovisionToken::issued_at},
        UintField<ReprovisionToken>{Rep::kExpiresAt,
                                    &ReprovisionToken::expires_at},
        HashField<ReprovisionToken>{Rep::kOneTimeNonce,
                                    &ReprovisionToken::one_time_nonce}
    );
    auto parsed = parse_schema<ReprovisionToken, TokenError>(m, schema);
    if (!parsed) return err(parsed.error());

    if (parsed->token_version != "reprovision-token-v1") {
        return err(TokenError::TokenVersionUnsupported);
    }
    parsed->vendor_signature = signature_bytes;
    return parsed;
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

    using namespace VMPilot::Cbor::Schema;
    const auto schema = std::make_tuple(
        TextField<MigrationToken>{Mig::kTokenVersion,
                                  &MigrationToken::token_version},
        HashField<MigrationToken>{Mig::kOldPackageBindingRecordHash,
                                  &MigrationToken::old_package_binding_record_hash},
        HashField<MigrationToken>{Mig::kNewPackageBindingRecordHash,
                                  &MigrationToken::new_package_binding_record_hash},
        HashField<MigrationToken>{Mig::kCustomerAccountIdHash,
                                  &MigrationToken::customer_account_id_hash},
        BoolField<MigrationToken>{Mig::kAllowedImportOnce,
                                  &MigrationToken::allowed_import_once},
        EnumTextField<MigrationToken,
                      VMPilot::DomainLabels::PolicyId, TokenError>{
            Mig::kAllowedPolicyFloor, &MigrationToken::allowed_policy_floor,
            TokenError::UnknownEnumValue},
        UintField<MigrationToken>{Mig::kExpiresAt,
                                  &MigrationToken::expires_at},
        HashField<MigrationToken>{Mig::kNonce, &MigrationToken::nonce}
    );
    auto parsed = parse_schema<MigrationToken, TokenError>(m, schema);
    if (!parsed) return err(parsed.error());

    if (!parsed->allowed_import_once) return err(TokenError::AllowedImportOnceNotTrue);
    if (parsed->token_version != "migration-token-v1") {
        return err(TokenError::TokenVersionUnsupported);
    }
    parsed->vendor_signature = signature_bytes;
    return parsed;
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
