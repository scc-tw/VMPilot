#ifndef VMPILOT_RUNTIME_PROVIDER_HPP
#define VMPILOT_RUNTIME_PROVIDER_HPP

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <tl/expected.hpp>

#include "vm/enum_text.hpp"
#include "vm/family_policy.hpp"

// TrustProvider contract (doc 14 §3–§7). A TrustProvider is the
// abstraction behind provider_requirement_hash lookups on the
// RuntimeSpecializationRegistry. Baseline runtime ships one provider,
// `local_embedded` (doc 10 §4), which carries no hardware root; richer
// providers (`local_tpm`, `local_tee`, `cloud_attested_vm`,
// `cloud_hsm`, `external_kms`) land in future iterations.
//
// Design constraints doc 14 bakes in:
//   - public surface never exposes tier / family / provider-class strings
//   - status=degraded never satisfies highsec
//   - evidence must be bound to the current package + profile table +
//     policy requirement hash; provider MAC does NOT replace the
//     vendor signature over the PackageBindingRecord

namespace VMPilot::Runtime::Provider {

enum class ProviderClass : std::uint8_t {
    LocalEmbedded = 1,
    LocalTpm,
    LocalTee,
    CloudAttestedVm,
    CloudHsm,
    ExternalKms,
};

// Canonical on-wire text for ProviderClass — delegates into
// VMPilot::EnumTextTraits<ProviderClass> (specialization lives below
// outside the nested namespace so it binds in ::VMPilot scope).
[[nodiscard]] constexpr std::string_view to_text(ProviderClass c) noexcept;

enum class CloneResistanceClass : std::uint8_t {
    None = 1,
    Soft,
    Hardware,
};

enum class FreshnessClass : std::uint8_t {
    None = 1,
    Periodic,
    OnlineVerified,
};

enum class KeyCustodyClass : std::uint8_t {
    Self = 1,
    Hsm,
    Vendor,
};

enum class RecoveryModel : std::uint8_t {
    SelfService = 1,
    SignedReprovision,
    Quorum,
};

// Canonical on-wire text for RecoveryModel — symmetric with ProviderClass.
[[nodiscard]] constexpr std::string_view to_text(RecoveryModel m) noexcept;

enum class PrivacyModel : std::uint8_t {
    Pairwise = 1,
    Pseudonymous,
    // Raw hardware identity is forbidden (doc 10 §7) — no enum value.
};

enum class AttestationFormat : std::uint8_t {
    None = 1,
    LocalEmbeddedV1,
    TpmQuoteV1,
    TeeReportV1,
    CloudAttestationV1,
};

enum class ProviderStatus : std::uint8_t {
    Satisfied = 1,
    NotSatisfied,
    Degraded,  // doc 14 §7.1: never accepted for highsec
};

// Stable identifier for a rejection reason; public surface must not
// reveal which provider / tier / family triggered it (doc 14 §9.1).
enum class ProviderError : std::uint8_t {
    ArtifactPolicyNotSatisfied = 1,
    ProviderRequirementNotSatisfied,
    ProviderEvidenceInvalid,
    RecoveryRequired,
    RuntimeSpecializationUnavailable,
};

// doc 14 §4.
struct CapabilityStatement {
    ProviderClass provider_class;
    std::string provider_instance_pseudonym;  // NOT raw hardware id
    bool hardware_bound;
    bool non_exportable_key;
    bool online_required;
    bool migratable;
    CloneResistanceClass clone_resistance_class;
    FreshnessClass freshness_class;
    AttestationFormat attestation_format;
    KeyCustodyClass key_custody_class;
    RecoveryModel recovery_model;
    PrivacyModel privacy_model;
    std::vector<VMPilot::DomainLabels::PolicyId> supported_policy_floors;
    std::vector<VMPilot::DomainLabels::FamilyId> supported_family_set;
    std::array<std::uint8_t, 32> provider_measurement_hash;  // zeros → absent
};

// doc 14 §5. `attestation_payload` and `evidence_signature_or_mac` are
// provider-specific opaque bytes; the runtime only verifies the field
// bindings and hands the envelope to the appraiser.
struct ProviderEvidence {
    std::string evidence_version;
    ProviderClass provider_class;
    std::string provider_instance_pseudonym;
    std::array<std::uint8_t, 32> nonce;
    std::array<std::uint8_t, 32> package_binding_record_hash;
    std::array<std::uint8_t, 32> resolved_profile_table_hash;
    std::array<std::uint8_t, 32> policy_requirement_hash;
    std::array<std::uint8_t, 32> runtime_measurement_hash;  // zeros → absent
    std::vector<std::uint8_t> attestation_payload;
    std::array<std::uint8_t, 32> freshness_proof_hash;
    std::vector<std::uint8_t> evidence_signature_or_mac;
};

// doc 14 §6. Built per-call from ResolvedFamilyProfile +
// RuntimeSpecializationRegistry. The hash is a stable commitment to
// every field in this struct and is what entries in
// RuntimeSpecializationRegistry.entries[].provider_requirement_hash
// commit to.
struct PolicyRequirement {
    std::string requirement_version;
    VMPilot::DomainLabels::PolicyId required_policy_floor;
    std::vector<VMPilot::DomainLabels::FamilyId> required_family_set;
    bool require_hardware_bound;
    bool require_non_exportable_key;
    bool require_online_freshness;
    bool require_remote_attestation;
    RecoveryModel require_recovery_model;
    std::vector<ProviderClass> allowed_provider_classes;
    std::uint64_t minimum_provider_epoch;
};

// doc 14 §7.
struct ProviderResult {
    ProviderStatus status;
    std::vector<std::string> satisfied_requirements;
    std::vector<std::string> unsatisfied_requirements;
    ProviderClass provider_class;
    std::array<std::uint8_t, 32> evidence_hash;
    std::array<std::uint8_t, 32> nonce;
    std::vector<std::uint8_t> result_signature;  // verifier signature
};

// Context the gate hands down to TrustProvider::bind_artifact. All
// three hashes must already be verified by the caller (package /
// profile / requirement); the provider binds its evidence to them,
// never replaces them.
struct VerifiedArtifactContext {
    std::array<std::uint8_t, 32> package_binding_record_hash;
    std::array<std::uint8_t, 32> resolved_profile_table_hash;
    std::array<std::uint8_t, 32> policy_requirement_hash;
};

class TrustProvider {
public:
    virtual ~TrustProvider() = default;

    virtual CapabilityStatement get_capabilities() const noexcept = 0;

    virtual tl::expected<ProviderEvidence, ProviderError>
    attest_runtime(const std::array<std::uint8_t, 32>& nonce,
                   const std::array<std::uint8_t, 32>& runtime_measurement,
                   const std::array<std::uint8_t, 32>&
                       profile_requirement_hash) noexcept = 0;

    virtual tl::expected<ProviderEvidence, ProviderError>
    bind_artifact(const std::array<std::uint8_t, 32>& nonce,
                  const VerifiedArtifactContext& ctx) noexcept = 0;
};

// Parse producer-supplied canonical bytes into an in-memory
// PolicyRequirement. The producer emits strict-CBOR per the schema
// documented in runtime/src/provider/local_embedded.cpp; the runtime
// never serializes the struct itself, so there is no second
// implementation to keep in sync. Strict canonical encoding is
// enforced by the parser: unknown keys, duplicate keys,
// non-canonical integer encoding, trailing bytes, unknown enum
// values, and oversized arrays all reject.
enum class PolicyRequirementParseError : std::uint8_t {
    BadCbor = 1,
    NotAMap,
    MissingCoreField,
    WrongFieldType,
    UnknownCoreField,
    UnknownEnumValue,
    ArrayTooLong,
    UnsupportedRequirementVersion,
};

tl::expected<PolicyRequirement, PolicyRequirementParseError>
parse_policy_requirement(const std::uint8_t* data,
                         std::size_t size) noexcept;

inline tl::expected<PolicyRequirement, PolicyRequirementParseError>
parse_policy_requirement(
    const std::vector<std::uint8_t>& bytes) noexcept {
    return parse_policy_requirement(bytes.data(), bytes.size());
}

// Domain-separated hash of producer-supplied canonical bytes. The
// runtime compares this against the registry entry's committed
// provider_requirement_hash; mismatch = reject the entry outright.
std::array<std::uint8_t, 32>
policy_requirement_hash(const std::uint8_t* canonical_bytes,
                        std::size_t size) noexcept;

inline std::array<std::uint8_t, 32>
policy_requirement_hash(const std::vector<std::uint8_t>& bytes) noexcept {
    return policy_requirement_hash(bytes.data(), bytes.size());
}

// Core appraisal: checks evidence bindings and maps capability vs
// requirement into a ProviderResult. Does NOT replace vendor signature
// verification on the PackageBindingRecord (doc 14 §11).
ProviderResult appraise(const CapabilityStatement& caps,
                        const PolicyRequirement& req,
                        const ProviderEvidence& evidence,
                        const VerifiedArtifactContext& ctx) noexcept;

// Gate the runtime dispatch path uses. Returns ProviderError on any
// failure; maps doc 14 §7.1's degraded-vs-highsec rule internally.
tl::expected<ProviderResult, ProviderError>
evaluate_policy_requirement(TrustProvider& provider,
                            const PolicyRequirement& requirement,
                            const VerifiedArtifactContext& ctx,
                            const std::array<std::uint8_t, 32>&
                                requested_policy_floor_override) noexcept;

// Baseline provider bundled with 1.0 runtime. Claims no hardware root,
// no remote attestation, no freshness guarantees. Consequently it can
// only satisfy PolicyRequirements that explicitly allow
// ProviderClass::LocalEmbedded and require none of {hardware_bound,
// non_exportable_key, online_freshness, remote_attestation}.
class LocalEmbeddedProvider final : public TrustProvider {
public:
    LocalEmbeddedProvider() = default;
    explicit LocalEmbeddedProvider(std::string instance_pseudonym) noexcept;

    CapabilityStatement get_capabilities() const noexcept override;

    tl::expected<ProviderEvidence, ProviderError>
    attest_runtime(const std::array<std::uint8_t, 32>& nonce,
                   const std::array<std::uint8_t, 32>& runtime_measurement,
                   const std::array<std::uint8_t, 32>&
                       profile_requirement_hash) noexcept override;

    tl::expected<ProviderEvidence, ProviderError>
    bind_artifact(const std::array<std::uint8_t, 32>& nonce,
                  const VerifiedArtifactContext& ctx) noexcept override;

private:
    std::string instance_pseudonym_{"local-embedded-default"};
};

// TrustProvider singleton used by the runtime dispatch path. Tests may
// swap in an alternate provider via install_provider_for_testing().
TrustProvider& runtime_provider() noexcept;
void install_provider_for_testing(TrustProvider* provider) noexcept;

}  // namespace VMPilot::Runtime::Provider

// ─── Trait specializations + delegating to_text() definitions ──────────

namespace VMPilot {

template <>
struct EnumTextTraits<VMPilot::Runtime::Provider::ProviderClass> {
    using E = VMPilot::Runtime::Provider::ProviderClass;
    static constexpr std::array<std::pair<E, std::string_view>, 6>
        entries{{
            {E::LocalEmbedded,    "local_embedded"},
            {E::LocalTpm,         "local_tpm"},
            {E::LocalTee,         "local_tee"},
            {E::CloudAttestedVm,  "cloud_attested_vm"},
            {E::CloudHsm,         "cloud_hsm"},
            {E::ExternalKms,      "external_kms"},
        }};
};

template <>
struct EnumTextTraits<VMPilot::Runtime::Provider::RecoveryModel> {
    using E = VMPilot::Runtime::Provider::RecoveryModel;
    static constexpr std::array<std::pair<E, std::string_view>, 3>
        entries{{
            {E::SelfService,        "self_service"},
            {E::SignedReprovision,  "signed_reprovision"},
            {E::Quorum,             "quorum"},
        }};
};

}  // namespace VMPilot

namespace VMPilot::Runtime::Provider {

constexpr std::string_view to_text(ProviderClass c) noexcept {
    return VMPilot::enum_to_text(c);
}
constexpr std::string_view to_text(RecoveryModel m) noexcept {
    return VMPilot::enum_to_text(m);
}

}  // namespace VMPilot::Runtime::Provider

#endif  // VMPILOT_RUNTIME_PROVIDER_HPP
