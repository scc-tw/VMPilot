#include "provider.hpp"

#include <algorithm>
#include <atomic>
#include <cstring>

#include "cbor/encode.hpp"
#include "cbor/schema.hpp"
#include "cbor/strict.hpp"
#include "vm/domain_labels.hpp"

namespace VMPilot::Cbor {
template <>
struct CborConsumerTraits<VMPilot::Runtime::Provider::PolicyRequirementParseError> {
    using E = VMPilot::Runtime::Provider::PolicyRequirementParseError;
    static constexpr E missing_field        = E::MissingCoreField;
    static constexpr E wrong_field_type     = E::WrongFieldType;
    static constexpr E bad_cbor             = E::BadCbor;
    static constexpr E not_a_map            = E::NotAMap;
    static constexpr E unknown_core_field   = E::UnknownCoreField;
    static constexpr E unknown_enum_value   = E::UnknownEnumValue;
    static constexpr E array_too_long       = E::ArrayTooLong;
    static constexpr E unsupported_version  = E::UnsupportedRequirementVersion;
};
}  // namespace VMPilot::Cbor

namespace VMPilot::Runtime::Provider {

namespace {

// PolicyRequirement canonical schema (strict CBOR map, keys are uint):
//
//   1:  text     requirement_version, must equal "policy-requirement-v1"
//   2:  uint     required_policy_floor       (PolicyId: 1=Debug, 2=Standard, 3=HighSec)
//   3:  array<text>  required_family_set     (length 1..3; items in {"f1","f2","f3"})
//   4:  uint     require_hardware_bound      (0 or 1)
//   5:  uint     require_non_exportable_key  (0 or 1)
//   6:  uint     require_online_freshness    (0 or 1)
//   7:  uint     require_remote_attestation  (0 or 1)
//   8:  uint     require_recovery_model      (RecoveryModel: 1..3)
//   9:  array<text>  allowed_provider_classes (length 1..8;
//                                              items in {"local_embedded",
//                                              "local_tpm","local_tee",
//                                              "cloud_attested_vm",
//                                              "cloud_hsm","external_kms"})
//   10: uint     minimum_provider_epoch       (u64)
//
// Unknown map keys / duplicate keys / non-canonical integer encoding /
// trailing bytes / unknown enum values / arrays beyond the length
// bound all reject. The runtime never constructs these bytes — the
// producer ships them inside the signed registry and the runtime only
// parses + re-hashes + compares.
//
// TODO: migrate requirement_version / enum text values to typed
// enum class constants (with canonical string mapping helpers) so
// stringly-typed comparisons against literal values disappear.

constexpr std::uint64_t kReq_RequirementVersion         = 1;
constexpr std::uint64_t kReq_RequiredPolicyFloor        = 2;
constexpr std::uint64_t kReq_RequiredFamilySet          = 3;
constexpr std::uint64_t kReq_RequireHardwareBound       = 4;
constexpr std::uint64_t kReq_RequireNonExportableKey    = 5;
constexpr std::uint64_t kReq_RequireOnlineFreshness     = 6;
constexpr std::uint64_t kReq_RequireRemoteAttestation   = 7;
constexpr std::uint64_t kReq_RequireRecoveryModel       = 8;
constexpr std::uint64_t kReq_AllowedProviderClasses     = 9;
constexpr std::uint64_t kReq_MinimumProviderEpoch       = 10;

constexpr std::size_t kMaxFamilySetLength             = 3;
constexpr std::size_t kMaxAllowedProviderClassesLength = 8;

constexpr std::string_view kRequirementVersionV1 = "policy-requirement-v1";

// Phase 9 replaced the hand-rolled helpers (parse_provider_class_text,
// parse_policy_floor_uint, parse_recovery_model_uint, parse_bool_uint,
// is_known_requirement_key) with the schema-driven parser. The schema
// descriptors (EnumUintField / EnumTextArrayField / BoolField /
// reject_unknown_keys) cover every validation the helpers used to do.

// ProviderEvidence canonical schema (strict CBOR map, uint keys):
//
//   1: evidence_version            text
//   2: provider_class              uint  (ProviderClass enum value)
//   3: provider_instance_pseudonym text
//   4: nonce                       bytes(32)
//   5: package_binding_record_hash bytes(32)
//   6: resolved_profile_table_hash bytes(32)
//   7: policy_requirement_hash     bytes(32)
//   8: runtime_measurement_hash    bytes(32)
//   9: attestation_payload         bytes
//  10: freshness_proof_hash        bytes(32)
//
// Previously hand-rolled little-endian layout; moved to strict CBOR
// so producers outside this process (future TPM/TEE providers) can
// emit bytes whose hash the runtime reproduces bit-for-bit, without
// a second serializer implementation to keep in sync. Uses the
// common/include/cbor/encode.hpp helpers.
std::vector<std::uint8_t> serialize_evidence_core(
    const ProviderEvidence& ev) {
    namespace E = VMPilot::Cbor::Encode;
    const auto to_vec = [](const std::array<std::uint8_t, 32>& a) {
        return std::vector<std::uint8_t>(a.begin(), a.end());
    };

    E::MapBuilder m;
    m.put_uint(1,  E::encode_text(ev.evidence_version));
    m.put_uint(2,  E::encode_uint(static_cast<std::uint64_t>(ev.provider_class)));
    m.put_uint(3,  E::encode_text(ev.provider_instance_pseudonym));
    m.put_uint(4,  E::encode_bytes(to_vec(ev.nonce)));
    m.put_uint(5,  E::encode_bytes(to_vec(ev.package_binding_record_hash)));
    m.put_uint(6,  E::encode_bytes(to_vec(ev.resolved_profile_table_hash)));
    m.put_uint(7,  E::encode_bytes(to_vec(ev.policy_requirement_hash)));
    m.put_uint(8,  E::encode_bytes(to_vec(ev.runtime_measurement_hash)));
    m.put_uint(9,  E::encode_bytes(ev.attestation_payload));
    m.put_uint(10, E::encode_bytes(to_vec(ev.freshness_proof_hash)));
    return m.build();
}

bool capabilities_cover_requirement(
    const CapabilityStatement& caps,
    const PolicyRequirement& req,
    std::vector<std::string>& unsatisfied) {
    bool ok = true;
    auto fail = [&](const char* why) {
        unsatisfied.emplace_back(why);
        ok = false;
    };

    if (req.require_hardware_bound && !caps.hardware_bound) {
        fail("hardware_bound");
    }
    if (req.require_non_exportable_key && !caps.non_exportable_key) {
        fail("non_exportable_key");
    }
    if (req.require_online_freshness &&
        caps.freshness_class != FreshnessClass::OnlineVerified) {
        fail("online_freshness");
    }
    if (req.require_remote_attestation &&
        caps.attestation_format == AttestationFormat::None) {
        fail("remote_attestation");
    }
    if (static_cast<std::uint8_t>(caps.recovery_model) <
        static_cast<std::uint8_t>(req.require_recovery_model)) {
        fail("recovery_model");
    }

    const bool class_allowed =
        std::find(req.allowed_provider_classes.begin(),
                  req.allowed_provider_classes.end(),
                  caps.provider_class) != req.allowed_provider_classes.end();
    if (!class_allowed) fail("provider_class");

    const bool floor_supported =
        std::find(caps.supported_policy_floors.begin(),
                  caps.supported_policy_floors.end(),
                  req.required_policy_floor) !=
        caps.supported_policy_floors.end();
    if (!floor_supported) fail("policy_floor");

    for (auto family : req.required_family_set) {
        if (std::find(caps.supported_family_set.begin(),
                      caps.supported_family_set.end(),
                      family) == caps.supported_family_set.end()) {
            fail("family_set");
            break;
        }
    }

    return ok;
}

bool evidence_binds_to_context(const ProviderEvidence& ev,
                               const VerifiedArtifactContext& ctx) noexcept {
    return ev.package_binding_record_hash == ctx.package_binding_record_hash &&
           ev.resolved_profile_table_hash == ctx.resolved_profile_table_hash &&
           ev.policy_requirement_hash == ctx.policy_requirement_hash;
}

// std::atomic so parallel test runners (and any future multi-worker
// runtime) observe a consistent provider pointer and cannot tear
// while another thread is mid-install.
std::atomic<TrustProvider*> g_installed_provider{nullptr};

LocalEmbeddedProvider& default_provider() noexcept {
    static LocalEmbeddedProvider instance;
    return instance;
}

}  // namespace

std::array<std::uint8_t, 32>
policy_requirement_hash(const std::uint8_t* canonical_bytes,
                        std::size_t size) noexcept {
    return VMPilot::Cbor::domain_hash_sha256(
        VMPilot::DomainLabels::Hash::PolicyRequirement,
        canonical_bytes, size);
}

tl::expected<PolicyRequirement, PolicyRequirementParseError>
parse_policy_requirement(const std::uint8_t* data,
                         std::size_t size) noexcept {
    using VMPilot::Cbor::Value;
    using VMPilot::Cbor::parse_strict;
    using namespace VMPilot::Cbor::Schema;

    // parse_strict already rejects trailing bytes, duplicate map
    // keys, non-canonical integer encoding, indefinite length, and
    // floats; we only need to layer typed-field validation on top.
    auto root_or = parse_strict(data, size);
    if (!root_or) {
        return tl::make_unexpected(PolicyRequirementParseError::BadCbor);
    }
    const Value& root = *root_or;

    auto unknown_or = reject_unknown_keys<PolicyRequirementParseError>(
        root,
        {kReq_RequirementVersion, kReq_RequiredPolicyFloor,
         kReq_RequiredFamilySet, kReq_RequireHardwareBound,
         kReq_RequireNonExportableKey, kReq_RequireOnlineFreshness,
         kReq_RequireRemoteAttestation, kReq_RequireRecoveryModel,
         kReq_AllowedProviderClasses, kReq_MinimumProviderEpoch});
    if (!unknown_or) return tl::make_unexpected(unknown_or.error());

    const auto schema = std::make_tuple(
        TextField<PolicyRequirement>{kReq_RequirementVersion,
                                     &PolicyRequirement::requirement_version,
                                     kRequirementVersionV1},
        EnumUintField<PolicyRequirement, VMPilot::DomainLabels::PolicyId>{
            kReq_RequiredPolicyFloor,
            &PolicyRequirement::required_policy_floor},
        EnumTextArrayField<PolicyRequirement, VMPilot::DomainLabels::FamilyId>{
            kReq_RequiredFamilySet,
            &PolicyRequirement::required_family_set,
            /*min=*/1, /*max=*/kMaxFamilySetLength},
        BoolField<PolicyRequirement>{kReq_RequireHardwareBound,
                                     &PolicyRequirement::require_hardware_bound},
        BoolField<PolicyRequirement>{kReq_RequireNonExportableKey,
                                     &PolicyRequirement::require_non_exportable_key},
        BoolField<PolicyRequirement>{kReq_RequireOnlineFreshness,
                                     &PolicyRequirement::require_online_freshness},
        BoolField<PolicyRequirement>{kReq_RequireRemoteAttestation,
                                     &PolicyRequirement::require_remote_attestation},
        EnumUintField<PolicyRequirement, RecoveryModel>{
            kReq_RequireRecoveryModel,
            &PolicyRequirement::require_recovery_model},
        EnumTextArrayField<PolicyRequirement, ProviderClass>{
            kReq_AllowedProviderClasses,
            &PolicyRequirement::allowed_provider_classes,
            /*min=*/1, /*max=*/kMaxAllowedProviderClassesLength},
        UintField<PolicyRequirement>{kReq_MinimumProviderEpoch,
                                     &PolicyRequirement::minimum_provider_epoch});

    return parse_schema<PolicyRequirement, PolicyRequirementParseError>(root, schema);
}

static std::array<std::uint8_t, 32>
evidence_hash(const ProviderEvidence& ev) noexcept {
    const auto bytes = serialize_evidence_core(ev);
    return VMPilot::Cbor::domain_hash_sha256(
        VMPilot::DomainLabels::Hash::ProviderEvidence, bytes);
}

ProviderResult appraise(const CapabilityStatement& caps,
                        const PolicyRequirement& req,
                        const ProviderEvidence& evidence,
                        const VerifiedArtifactContext& ctx) noexcept {
    ProviderResult result{};
    result.provider_class = caps.provider_class;
    result.nonce = evidence.nonce;
    result.evidence_hash = evidence_hash(evidence);

    // doc 14 §11: evidence must bind to the already-verified package +
    // profile + requirement. If it doesn't, the provider is either
    // buggy or being replayed — reject outright.
    if (!evidence_binds_to_context(evidence, ctx)) {
        result.status = ProviderStatus::NotSatisfied;
        result.unsatisfied_requirements.emplace_back("evidence_binding");
        return result;
    }

    // Provider class declared in evidence must match capability; a
    // local_embedded provider cannot masquerade as local_tpm (doc 10
    // §4).
    if (evidence.provider_class != caps.provider_class) {
        result.status = ProviderStatus::NotSatisfied;
        result.unsatisfied_requirements.emplace_back("provider_class_claim");
        return result;
    }

    if (!capabilities_cover_requirement(caps, req,
                                        result.unsatisfied_requirements)) {
        // Highsec never accepts degraded; but since capability gaps are
        // fatal for 1.0 regardless of tier, collapse to not_satisfied.
        result.status = ProviderStatus::NotSatisfied;
        return result;
    }

    result.status = ProviderStatus::Satisfied;
    result.satisfied_requirements.emplace_back("all");
    return result;
}

tl::expected<ProviderResult, ProviderError>
evaluate_policy_requirement(TrustProvider& provider,
                            const PolicyRequirement& requirement,
                            const VerifiedArtifactContext& ctx,
                            const std::array<std::uint8_t, 32>&
                                /*requested_policy_floor_override*/) noexcept {
    // 1. Capability gate — quickest reject path.
    const auto caps = provider.get_capabilities();
    std::vector<std::string> capability_misses;
    if (!capabilities_cover_requirement(caps, requirement,
                                        capability_misses)) {
        return tl::make_unexpected(
            ProviderError::ProviderRequirementNotSatisfied);
    }

    // 2. Evidence. Use the requirement hash from the caller-provided
    //    ctx as the nonce seed so the binding is deterministic for
    //    unit tests; production would sample a random nonce and
    //    persist it to defeat replay.
    std::array<std::uint8_t, 32> nonce = ctx.policy_requirement_hash;
    auto evidence_or = provider.bind_artifact(nonce, ctx);
    if (!evidence_or) return tl::make_unexpected(evidence_or.error());

    // 3. Appraisal.
    auto result = appraise(caps, requirement, *evidence_or, ctx);
    if (result.status != ProviderStatus::Satisfied) {
        // doc 14 §7.1: degraded never acceptable for highsec; in 1.0 we
        // accept only 'satisfied', making the degraded vs highsec
        // distinction implicit (never reachable).
        return tl::make_unexpected(
            ProviderError::ProviderRequirementNotSatisfied);
    }

    return result;
}

// ─── LocalEmbeddedProvider ──────────────────────────────────────────────

LocalEmbeddedProvider::LocalEmbeddedProvider(std::string instance_pseudonym) noexcept
    : instance_pseudonym_(std::move(instance_pseudonym)) {}

CapabilityStatement LocalEmbeddedProvider::get_capabilities() const noexcept {
    CapabilityStatement caps{};
    caps.provider_class = ProviderClass::LocalEmbedded;
    caps.provider_instance_pseudonym = instance_pseudonym_;
    caps.hardware_bound = false;
    caps.non_exportable_key = false;
    caps.online_required = false;
    caps.migratable = true;
    caps.clone_resistance_class = CloneResistanceClass::None;
    caps.freshness_class = FreshnessClass::None;
    caps.attestation_format = AttestationFormat::None;
    caps.key_custody_class = KeyCustodyClass::Self;
    caps.recovery_model = RecoveryModel::SelfService;
    caps.privacy_model = PrivacyModel::Pairwise;
    caps.supported_policy_floors = {VMPilot::DomainLabels::PolicyId::Debug,
                                    VMPilot::DomainLabels::PolicyId::Standard};
    caps.supported_family_set = {VMPilot::DomainLabels::FamilyId::F1};
    caps.provider_measurement_hash = {};  // zeros → absent
    return caps;
}

tl::expected<ProviderEvidence, ProviderError>
LocalEmbeddedProvider::attest_runtime(
    const std::array<std::uint8_t, 32>& nonce,
    const std::array<std::uint8_t, 32>& runtime_measurement,
    const std::array<std::uint8_t, 32>& profile_requirement_hash) noexcept {
    ProviderEvidence ev{};
    ev.evidence_version = "evidence-v1";
    ev.provider_class = ProviderClass::LocalEmbedded;
    ev.provider_instance_pseudonym = instance_pseudonym_;
    ev.nonce = nonce;
    ev.package_binding_record_hash = {};
    ev.resolved_profile_table_hash = {};
    ev.policy_requirement_hash = profile_requirement_hash;
    ev.runtime_measurement_hash = runtime_measurement;
    ev.freshness_proof_hash = {};
    // local_embedded ships no hardware MAC; the verifier layer will
    // notice attestation_format=None + empty signature and reject the
    // evidence for any requirement that asks for remote_attestation.
    return ev;
}

tl::expected<ProviderEvidence, ProviderError>
LocalEmbeddedProvider::bind_artifact(
    const std::array<std::uint8_t, 32>& nonce,
    const VerifiedArtifactContext& ctx) noexcept {
    ProviderEvidence ev{};
    ev.evidence_version = "evidence-v1";
    ev.provider_class = ProviderClass::LocalEmbedded;
    ev.provider_instance_pseudonym = instance_pseudonym_;
    ev.nonce = nonce;
    ev.package_binding_record_hash = ctx.package_binding_record_hash;
    ev.resolved_profile_table_hash = ctx.resolved_profile_table_hash;
    ev.policy_requirement_hash = ctx.policy_requirement_hash;
    ev.runtime_measurement_hash = {};
    ev.freshness_proof_hash = {};
    return ev;
}

// ─── Singleton access ───────────────────────────────────────────────────

TrustProvider& runtime_provider() noexcept {
    auto* p = g_installed_provider.load(std::memory_order_acquire);
    if (p != nullptr) return *p;
    return default_provider();
}

TrustProvider* install_provider_for_testing(TrustProvider* provider) noexcept {
    return g_installed_provider.exchange(provider, std::memory_order_acq_rel);
}

}  // namespace VMPilot::Runtime::Provider
