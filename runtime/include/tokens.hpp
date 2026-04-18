#ifndef VMPILOT_RUNTIME_TOKENS_HPP
#define VMPILOT_RUNTIME_TOKENS_HPP

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include <tl/expected.hpp>

#include "trust_root.hpp"
#include "vm/family_policy.hpp"

// Stage 11 — signed lifecycle tokens.
//
// Two disjoint token classes, each with its own covered_domain:
//   - ReprovisionToken  (doc 10 §6)   — re-establishes highsec identity
//                                       after TPM clear / motherboard
//                                       swap / OS reinstall / VM
//                                       migration.
//   - MigrationToken    (doc 15 §7.2) — authorises the import-only
//                                       path between two packages;
//                                       never grants execution as
//                                       highsec.
//
// Both are bytes-in-memory verify-only surfaces. The runtime parses
// them, checks the vendor signature against VendorTrustRoot using the
// label that matches the token class, cross-checks field bindings
// (package hashes, policy floor, expiry), and asks a NonceStore if
// the token's one-time nonce has been consumed before. Tokens are
// never mutated or re-signed by the runtime.

namespace VMPilot::Runtime::Tokens {

enum class ReasonCode : std::uint8_t {
    TpmClear = 1,
    MotherboardSwap,
    OsReinstall,
    VmMigration,
    Other,
};

// doc 10 §6.1 — on-wire shape of the signed reprovision token.
struct ReprovisionToken {
    std::string token_version;
    std::string old_enrollment_id;
    std::string new_enrollment_id;
    std::array<std::uint8_t, 32> customer_account_id_hash;
    std::array<std::uint8_t, 32> old_attested_key_hash;
    std::array<std::uint8_t, 32> new_attested_key_hash;
    std::array<std::uint8_t, 32> new_provider_evidence_hash;
    std::array<std::uint8_t, 32> allowed_package_binding_record_hash;
    VMPilot::DomainLabels::PolicyId allowed_policy_floor;
    std::vector<VMPilot::DomainLabels::FamilyId> allowed_family_set;
    ReasonCode reason_code;
    std::string support_case_id;
    std::array<std::uint8_t, 32> approval_chain_hash;
    std::uint64_t issued_at;
    std::uint64_t expires_at;
    std::array<std::uint8_t, 32> one_time_nonce;
    std::vector<std::uint8_t> vendor_signature;  // Ed25519, 64 bytes
};

// doc 15 §7.2 — on-wire shape of the signed migration token.
struct MigrationToken {
    std::string token_version;
    std::array<std::uint8_t, 32> old_package_binding_record_hash;
    std::array<std::uint8_t, 32> new_package_binding_record_hash;
    std::array<std::uint8_t, 32> customer_account_id_hash;
    bool allowed_import_once;  // must be true per doc 15 §7.2
    VMPilot::DomainLabels::PolicyId allowed_policy_floor;
    std::uint64_t expires_at;
    std::array<std::uint8_t, 32> nonce;
    std::vector<std::uint8_t> vendor_signature;
};

enum class TokenError : std::uint8_t {
    // Parse / structural.
    TokenMalformed = 1,
    WrongFieldType,
    MissingCoreField,
    WrongHashSize,
    SignatureWrongSize,
    UnknownEnumValue,

    // Cross-field.
    TokenVersionUnsupported,
    AllowedImportOnceNotTrue,

    // Authenticity.
    SignatureInvalid,
    TrustRootKeyUsageMismatch,

    // Acceptance.
    TokenExpired,
    NonceAlreadyConsumed,
    PackageHashMismatch,       // token.allowed_*_package_hash != current
    PolicyFloorBelowRequired,
    OldEnrollmentNotRevoked,
    NewProviderEvidenceMismatch,
    AllowedFamilyMismatch,
};

// Persisted set of consumed one-time nonces. Stage 12 will extend this
// with on-disk persistence; for now the runtime supplies an in-memory
// implementation so acceptance logic is testable in isolation.
class NonceStore {
public:
    virtual ~NonceStore() = default;
    virtual bool is_consumed(
        const std::array<std::uint8_t, 32>& nonce) const noexcept = 0;
    virtual void mark_consumed(
        const std::array<std::uint8_t, 32>& nonce) noexcept = 0;
};

// Context the runtime binds incoming tokens to. All hashes here have
// already been verified by earlier stages.
struct ReprovisionContext {
    std::array<std::uint8_t, 32> current_package_binding_record_hash;
    VMPilot::DomainLabels::PolicyId required_policy_floor;
    std::uint64_t now_unix_seconds;
    bool old_enrollment_is_revoked;
    std::array<std::uint8_t, 32> new_provider_evidence_hash;
};

struct MigrationContext {
    std::array<std::uint8_t, 32> current_package_binding_record_hash;
    std::array<std::uint8_t, 32> target_package_binding_record_hash;
    VMPilot::DomainLabels::PolicyId required_policy_floor;
    std::uint64_t now_unix_seconds;
};

// Decode + signature verify the reprovision token. Does NOT mark the
// nonce consumed — that is the caller's responsibility after the
// downstream import/activation step succeeds.
tl::expected<ReprovisionToken, TokenError>
parse_reprovision_token(const std::uint8_t* data, std::size_t size,
                        const VendorTrustRoot& root) noexcept;

tl::expected<MigrationToken, TokenError>
parse_migration_token(const std::uint8_t* data, std::size_t size,
                      const VendorTrustRoot& root) noexcept;

// Full doc 10 §6.2 acceptance — parse, verify signature, check
// expiry, check nonce not yet consumed, check bindings. On success
// marks the nonce consumed in `store` as the final step so a later
// replay fails closed (doc 10 §6.3).
tl::expected<ReprovisionToken, TokenError>
accept_reprovision_token(const std::uint8_t* data, std::size_t size,
                         const VendorTrustRoot& root,
                         const ReprovisionContext& ctx,
                         NonceStore& store) noexcept;

// Full doc 15 §7.3 acceptance — parse, verify signature, check
// expiry, check nonce not yet consumed, check current+target package
// bindings, enforce allowed_import_once==true + floor-satisfied.
// Marks the nonce consumed on success.
tl::expected<MigrationToken, TokenError>
accept_migration_token(const std::uint8_t* data, std::size_t size,
                       const VendorTrustRoot& root,
                       const MigrationContext& ctx,
                       NonceStore& store) noexcept;

}  // namespace VMPilot::Runtime::Tokens

#endif  // VMPILOT_RUNTIME_TOKENS_HPP
