#include "tokens.hpp"

#include <cstring>
#include <string>

#include <VMPilot_crypto.hpp>

#include "cbor/strict.hpp"
#include "vm/domain_labels.hpp"

namespace VMPilot::Runtime::Tokens {

namespace {

using VMPilot::Cbor::Value;
using VMPilot::Cbor::parse_strict;

// Shared auth-wrapper keys (doc 06 §9.2 pattern).
constexpr std::uint64_t kAuth_Kind            = 1;
constexpr std::uint64_t kAuth_KeyId           = 2;
constexpr std::uint64_t kAuth_CoveredDomain   = 3;
constexpr std::uint64_t kAuth_SignatureAlgId  = 4;
constexpr std::uint64_t kAuth_Signature       = 5;

constexpr std::string_view kAuthKindVendorSignatureV1 = "vendor_signature_v1";

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

tl::expected<std::string, TokenError> require_text(
    const Value& map, std::uint64_t key) noexcept {
    const Value* value = map.find_by_uint_key(key);
    if (value == nullptr) return err(TokenError::MissingCoreField);
    if (value->kind() != Value::Kind::Text) return err(TokenError::WrongFieldType);
    return value->as_text();
}

tl::expected<std::vector<std::uint8_t>, TokenError> require_bytes(
    const Value& map, std::uint64_t key) noexcept {
    const Value* value = map.find_by_uint_key(key);
    if (value == nullptr) return err(TokenError::MissingCoreField);
    if (value->kind() != Value::Kind::Bytes) return err(TokenError::WrongFieldType);
    return value->as_bytes();
}

tl::expected<std::array<std::uint8_t, 32>, TokenError> require_hash32(
    const Value& map, std::uint64_t key) noexcept {
    auto bytes_or = require_bytes(map, key);
    if (!bytes_or) return err(bytes_or.error());
    if (bytes_or->size() != 32) return err(TokenError::WrongHashSize);
    std::array<std::uint8_t, 32> out{};
    std::memcpy(out.data(), bytes_or->data(), 32);
    return out;
}

tl::expected<std::uint64_t, TokenError> require_uint(
    const Value& map, std::uint64_t key) noexcept {
    const Value* value = map.find_by_uint_key(key);
    if (value == nullptr) return err(TokenError::MissingCoreField);
    if (value->kind() != Value::Kind::Uint) return err(TokenError::WrongFieldType);
    return value->as_uint();
}

// Strict CBOR subset has no `bool` kind (doc 07 §3). Canonical
// encoding for the single-bit `allowed_import_once` field is a
// uint — 0 → false, 1 → true; any other value is malformed.
tl::expected<bool, TokenError> require_bool(
    const Value& map, std::uint64_t key) noexcept {
    const Value* value = map.find_by_uint_key(key);
    if (value == nullptr) return err(TokenError::MissingCoreField);
    if (value->kind() != Value::Kind::Uint) return err(TokenError::WrongFieldType);
    const auto v = value->as_uint();
    if (v == 0) return false;
    if (v == 1) return true;
    return err(TokenError::WrongFieldType);
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
    if (text == "tpm_clear")        return ReasonCode::TpmClear;
    if (text == "motherboard_swap") return ReasonCode::MotherboardSwap;
    if (text == "os_reinstall")     return ReasonCode::OsReinstall;
    if (text == "vm_migration")     return ReasonCode::VmMigration;
    if (text == "other")            return ReasonCode::Other;
    return err(TokenError::UnknownEnumValue);
}

// Verify the binding_auth wrapper common to both token classes.
// `expected_domain` is the covered_domain string the caller expects
// (Auth::ReprovisionToken or Auth::MigrationToken). The function
// returns the signature bytes on success so the caller can invoke
// Verify_Ed25519 over the canonical bytes.
tl::expected<std::vector<std::uint8_t>, TokenError> verify_auth_wrapper(
    const Value& auth_map, const VendorTrustRoot& root,
    std::string_view expected_domain,
    const std::vector<std::uint8_t>& canonical_bytes) noexcept {
    auto kind_or = require_text(auth_map, kAuth_Kind);
    if (!kind_or) return err(kind_or.error());
    if (*kind_or != kAuthKindVendorSignatureV1) {
        return err(TokenError::TokenMalformed);
    }

    auto key_id_or = require_text(auth_map, kAuth_KeyId);
    if (!key_id_or) return err(key_id_or.error());
    if (*key_id_or != root.root_key_id) {
        return err(TokenError::TrustRootKeyUsageMismatch);
    }

    auto alg_or = require_text(auth_map, kAuth_SignatureAlgId);
    if (!alg_or) return err(alg_or.error());
    if (*alg_or != root.signature_alg_id) {
        return err(TokenError::TokenMalformed);
    }

    auto domain_or = require_text(auth_map, kAuth_CoveredDomain);
    if (!domain_or) return err(domain_or.error());
    if (*domain_or != expected_domain) return err(TokenError::TokenMalformed);

    auto sig_or = require_bytes(auth_map, kAuth_Signature);
    if (!sig_or) return err(sig_or.error());
    if (sig_or->size() != 64) return err(TokenError::SignatureWrongSize);

    const std::vector<std::uint8_t> pubkey_vec(
        root.public_key, root.public_key + sizeof(root.public_key));
    const bool sig_ok = VMPilot::Crypto::Verify_Ed25519(
        pubkey_vec, *sig_or, std::string(expected_domain), canonical_bytes);
    if (!sig_ok) return err(TokenError::SignatureInvalid);

    return *sig_or;
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
    const Value& outer = *outer_or;
    if (outer.kind() != Value::Kind::Array) return err(TokenError::TokenMalformed);
    if (outer.as_array().size() != 2) return err(TokenError::TokenMalformed);

    const Value& canon_v = outer.as_array()[0];
    const Value& auth_v  = outer.as_array()[1];
    if (canon_v.kind() != Value::Kind::Bytes) return err(TokenError::TokenMalformed);
    if (auth_v.kind()  != Value::Kind::Map)   return err(TokenError::TokenMalformed);

    const auto& canonical_bytes = canon_v.as_bytes();

    auto sig_or = verify_auth_wrapper(
        auth_v, root, VMPilot::DomainLabels::Auth::ReprovisionToken,
        canonical_bytes);
    if (!sig_or) return err(sig_or.error());

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
    out.vendor_signature                     = std::move(*sig_or);

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
    const Value& outer = *outer_or;
    if (outer.kind() != Value::Kind::Array) return err(TokenError::TokenMalformed);
    if (outer.as_array().size() != 2) return err(TokenError::TokenMalformed);

    const Value& canon_v = outer.as_array()[0];
    const Value& auth_v  = outer.as_array()[1];
    if (canon_v.kind() != Value::Kind::Bytes) return err(TokenError::TokenMalformed);
    if (auth_v.kind()  != Value::Kind::Map)   return err(TokenError::TokenMalformed);

    const auto& canonical_bytes = canon_v.as_bytes();
    auto sig_or = verify_auth_wrapper(
        auth_v, root, VMPilot::DomainLabels::Auth::MigrationToken,
        canonical_bytes);
    if (!sig_or) return err(sig_or.error());

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
    out.vendor_signature                      = std::move(*sig_or);

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
