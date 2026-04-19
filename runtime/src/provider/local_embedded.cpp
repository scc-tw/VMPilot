#include "provider.hpp"

#include <algorithm>
#include <cstring>

#include "cbor/strict.hpp"
#include "vm/domain_labels.hpp"

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

tl::expected<ProviderClass, PolicyRequirementParseError>
parse_provider_class_text(std::string_view s) noexcept {
    if (s == "local_embedded")      return ProviderClass::LocalEmbedded;
    if (s == "local_tpm")           return ProviderClass::LocalTpm;
    if (s == "local_tee")           return ProviderClass::LocalTee;
    if (s == "cloud_attested_vm")   return ProviderClass::CloudAttestedVm;
    if (s == "cloud_hsm")           return ProviderClass::CloudHsm;
    if (s == "external_kms")        return ProviderClass::ExternalKms;
    return tl::make_unexpected(PolicyRequirementParseError::UnknownEnumValue);
}

tl::expected<VMPilot::DomainLabels::PolicyId, PolicyRequirementParseError>
parse_policy_floor_uint(std::uint64_t v) noexcept {
    using VMPilot::DomainLabels::PolicyId;
    switch (v) {
        case static_cast<std::uint64_t>(PolicyId::Debug):    return PolicyId::Debug;
        case static_cast<std::uint64_t>(PolicyId::Standard): return PolicyId::Standard;
        case static_cast<std::uint64_t>(PolicyId::HighSec):  return PolicyId::HighSec;
    }
    return tl::make_unexpected(PolicyRequirementParseError::UnknownEnumValue);
}

tl::expected<RecoveryModel, PolicyRequirementParseError>
parse_recovery_model_uint(std::uint64_t v) noexcept {
    switch (v) {
        case static_cast<std::uint64_t>(RecoveryModel::SelfService):
            return RecoveryModel::SelfService;
        case static_cast<std::uint64_t>(RecoveryModel::SignedReprovision):
            return RecoveryModel::SignedReprovision;
        case static_cast<std::uint64_t>(RecoveryModel::Quorum):
            return RecoveryModel::Quorum;
    }
    return tl::make_unexpected(PolicyRequirementParseError::UnknownEnumValue);
}

tl::expected<bool, PolicyRequirementParseError>
parse_bool_uint(std::uint64_t v) noexcept {
    if (v == 0) return false;
    if (v == 1) return true;
    return tl::make_unexpected(PolicyRequirementParseError::UnknownEnumValue);
}

bool is_known_requirement_key(std::uint64_t k) noexcept {
    switch (k) {
        case kReq_RequirementVersion:
        case kReq_RequiredPolicyFloor:
        case kReq_RequiredFamilySet:
        case kReq_RequireHardwareBound:
        case kReq_RequireNonExportableKey:
        case kReq_RequireOnlineFreshness:
        case kReq_RequireRemoteAttestation:
        case kReq_RequireRecoveryModel:
        case kReq_AllowedProviderClasses:
        case kReq_MinimumProviderEpoch:
            return true;
        default:
            return false;
    }
}

// Helpers used by the evidence serializer only — PolicyRequirement's
// serializer is gone (producer-carried bytes path).
void append_u8(std::vector<std::uint8_t>& out, std::uint8_t v) {
    out.push_back(v);
}
void append_u16(std::vector<std::uint8_t>& out, std::uint16_t v) {
    out.push_back(static_cast<std::uint8_t>(v & 0xFFu));
    out.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFFu));
}
void append_str(std::vector<std::uint8_t>& out, std::string_view s) {
    append_u16(out, static_cast<std::uint16_t>(s.size()));
    out.insert(out.end(), s.begin(), s.end());
}

std::vector<std::uint8_t> serialize_evidence_core(
    const ProviderEvidence& ev) {
    std::vector<std::uint8_t> out;
    out.reserve(256);
    append_u8(out, 0x01);
    append_str(out, ev.evidence_version);
    append_u8(out, static_cast<std::uint8_t>(ev.provider_class));
    append_str(out, ev.provider_instance_pseudonym);
    out.insert(out.end(), ev.nonce.begin(), ev.nonce.end());
    out.insert(out.end(), ev.package_binding_record_hash.begin(),
               ev.package_binding_record_hash.end());
    out.insert(out.end(), ev.resolved_profile_table_hash.begin(),
               ev.resolved_profile_table_hash.end());
    out.insert(out.end(), ev.policy_requirement_hash.begin(),
               ev.policy_requirement_hash.end());
    out.insert(out.end(), ev.runtime_measurement_hash.begin(),
               ev.runtime_measurement_hash.end());
    append_u16(out,
               static_cast<std::uint16_t>(ev.attestation_payload.size()));
    out.insert(out.end(), ev.attestation_payload.begin(),
               ev.attestation_payload.end());
    out.insert(out.end(), ev.freshness_proof_hash.begin(),
               ev.freshness_proof_hash.end());
    return out;
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

TrustProvider* g_installed_provider = nullptr;

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
    auto err = [](PolicyRequirementParseError e) {
        return tl::make_unexpected(e);
    };

    // parse_strict already rejects trailing bytes, duplicate map
    // keys, non-canonical integer encoding, indefinite length, and
    // floats; we only need to layer typed-field validation on top.
    auto root_or = parse_strict(data, size);
    if (!root_or) return err(PolicyRequirementParseError::BadCbor);
    const Value& root = *root_or;
    if (root.kind() != Value::Kind::Map) {
        return err(PolicyRequirementParseError::NotAMap);
    }

    // Unknown-key rejection (strict mode).
    for (std::size_t i = 0; i < root.map_size(); ++i) {
        const Value& key = root.map_key_at(i);
        if (key.kind() != Value::Kind::Uint) {
            return err(PolicyRequirementParseError::WrongFieldType);
        }
        if (!is_known_requirement_key(key.as_uint())) {
            return err(PolicyRequirementParseError::UnknownCoreField);
        }
    }

    auto require_text = [&](std::uint64_t k)
        -> tl::expected<std::string_view, PolicyRequirementParseError> {
        const Value* v = root.find_by_uint_key(k);
        if (v == nullptr) return err(PolicyRequirementParseError::MissingCoreField);
        if (v->kind() != Value::Kind::Text) {
            return err(PolicyRequirementParseError::WrongFieldType);
        }
        return std::string_view(v->as_text());
    };
    auto require_uint = [&](std::uint64_t k)
        -> tl::expected<std::uint64_t, PolicyRequirementParseError> {
        const Value* v = root.find_by_uint_key(k);
        if (v == nullptr) return err(PolicyRequirementParseError::MissingCoreField);
        if (v->kind() != Value::Kind::Uint) {
            return err(PolicyRequirementParseError::WrongFieldType);
        }
        return v->as_uint();
    };

    auto version_or = require_text(kReq_RequirementVersion);
    if (!version_or) return err(version_or.error());
    if (*version_or != kRequirementVersionV1) {
        return err(PolicyRequirementParseError::UnsupportedRequirementVersion);
    }

    auto floor_v_or = require_uint(kReq_RequiredPolicyFloor);
    if (!floor_v_or) return err(floor_v_or.error());
    auto floor_or = parse_policy_floor_uint(*floor_v_or);
    if (!floor_or) return err(floor_or.error());

    // required_family_set: array<text>, 1..kMaxFamilySetLength.
    std::vector<VMPilot::DomainLabels::FamilyId> families;
    {
        const Value* v = root.find_by_uint_key(kReq_RequiredFamilySet);
        if (v == nullptr) return err(PolicyRequirementParseError::MissingCoreField);
        if (v->kind() != Value::Kind::Array) {
            return err(PolicyRequirementParseError::WrongFieldType);
        }
        const auto& items = v->as_array();
        if (items.empty() || items.size() > kMaxFamilySetLength) {
            return err(PolicyRequirementParseError::ArrayTooLong);
        }
        families.reserve(items.size());
        for (const Value& item : items) {
            if (item.kind() != Value::Kind::Text) {
                return err(PolicyRequirementParseError::WrongFieldType);
            }
            auto fam = VMPilot::DomainLabels::parse_family_id(item.as_text());
            if (!fam.has_value()) {
                return err(PolicyRequirementParseError::UnknownEnumValue);
            }
            families.push_back(*fam);
        }
    }

    auto hw_or = require_uint(kReq_RequireHardwareBound);
    auto nek_or = require_uint(kReq_RequireNonExportableKey);
    auto fresh_or = require_uint(kReq_RequireOnlineFreshness);
    auto att_or = require_uint(kReq_RequireRemoteAttestation);
    auto recov_v_or = require_uint(kReq_RequireRecoveryModel);
    auto epoch_or = require_uint(kReq_MinimumProviderEpoch);

    if (!hw_or)     return err(hw_or.error());
    if (!nek_or)    return err(nek_or.error());
    if (!fresh_or)  return err(fresh_or.error());
    if (!att_or)    return err(att_or.error());
    if (!recov_v_or) return err(recov_v_or.error());
    if (!epoch_or)  return err(epoch_or.error());

    auto hw_b    = parse_bool_uint(*hw_or);
    auto nek_b   = parse_bool_uint(*nek_or);
    auto fresh_b = parse_bool_uint(*fresh_or);
    auto att_b   = parse_bool_uint(*att_or);
    auto recov_or = parse_recovery_model_uint(*recov_v_or);

    if (!hw_b)    return err(hw_b.error());
    if (!nek_b)   return err(nek_b.error());
    if (!fresh_b) return err(fresh_b.error());
    if (!att_b)   return err(att_b.error());
    if (!recov_or) return err(recov_or.error());

    // allowed_provider_classes: array<text>, 1..kMaxAllowedProviderClassesLength.
    std::vector<ProviderClass> classes;
    {
        const Value* v = root.find_by_uint_key(kReq_AllowedProviderClasses);
        if (v == nullptr) return err(PolicyRequirementParseError::MissingCoreField);
        if (v->kind() != Value::Kind::Array) {
            return err(PolicyRequirementParseError::WrongFieldType);
        }
        const auto& items = v->as_array();
        if (items.empty() || items.size() > kMaxAllowedProviderClassesLength) {
            return err(PolicyRequirementParseError::ArrayTooLong);
        }
        classes.reserve(items.size());
        for (const Value& item : items) {
            if (item.kind() != Value::Kind::Text) {
                return err(PolicyRequirementParseError::WrongFieldType);
            }
            auto cls = parse_provider_class_text(item.as_text());
            if (!cls) return err(cls.error());
            classes.push_back(*cls);
        }
    }

    PolicyRequirement out;
    out.requirement_version         = std::string(*version_or);
    out.required_policy_floor       = *floor_or;
    out.required_family_set         = std::move(families);
    out.require_hardware_bound      = *hw_b;
    out.require_non_exportable_key  = *nek_b;
    out.require_online_freshness    = *fresh_b;
    out.require_remote_attestation  = *att_b;
    out.require_recovery_model      = *recov_or;
    out.allowed_provider_classes    = std::move(classes);
    out.minimum_provider_epoch      = *epoch_or;
    return out;
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
    if (g_installed_provider != nullptr) return *g_installed_provider;
    return default_provider();
}

void install_provider_for_testing(TrustProvider* provider) noexcept {
    g_installed_provider = provider;
}

}  // namespace VMPilot::Runtime::Provider
