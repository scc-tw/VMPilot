#include "provider.hpp"

#include <algorithm>
#include <cstring>

#include "cbor/strict.hpp"
#include "vm/domain_labels.hpp"

namespace VMPilot::Runtime::Provider {

namespace {

// Hand-rolled canonical serializer for PolicyRequirement — a closed
// struct, so a bespoke, deterministic byte layout is cheaper than
// threading the full CBOR encoder into the runtime. Layout:
//
//   u8  tag=0x01
//   u16 requirement_version.length | ... bytes
//   u8  required_policy_floor      (PolicyId enum value, 1..3)
//   u16 required_family_set.length | FamilyId byte per entry
//   u8  require_hardware_bound     (0/1)
//   u8  require_non_exportable_key (0/1)
//   u8  require_online_freshness   (0/1)
//   u8  require_remote_attestation (0/1)
//   u8  require_recovery_model     (RecoveryModel enum value, 1..3)
//   u16 allowed_provider_classes.length | ProviderClass byte per entry
//   u64 minimum_provider_epoch     (little-endian)
//
// All integers are little-endian. Enum values are fixed at the
// header. The label prefix is added by domain_hash_sha256.

void append_u8(std::vector<std::uint8_t>& out, std::uint8_t v) {
    out.push_back(v);
}

void append_u16(std::vector<std::uint8_t>& out, std::uint16_t v) {
    out.push_back(static_cast<std::uint8_t>(v & 0xFFu));
    out.push_back(static_cast<std::uint8_t>((v >> 8) & 0xFFu));
}

void append_u64(std::vector<std::uint8_t>& out, std::uint64_t v) {
    for (int i = 0; i < 8; ++i) {
        out.push_back(static_cast<std::uint8_t>((v >> (i * 8)) & 0xFFu));
    }
}

void append_str(std::vector<std::uint8_t>& out, std::string_view s) {
    append_u16(out, static_cast<std::uint16_t>(s.size()));
    out.insert(out.end(), s.begin(), s.end());
}

std::vector<std::uint8_t> serialize_policy_requirement(
    const PolicyRequirement& req) {
    std::vector<std::uint8_t> out;
    out.reserve(128);
    append_u8(out, 0x01);  // tag
    append_str(out, req.requirement_version);
    append_u8(out, static_cast<std::uint8_t>(req.required_policy_floor));
    append_u16(out, static_cast<std::uint16_t>(req.required_family_set.size()));
    for (auto f : req.required_family_set) {
        append_u8(out, static_cast<std::uint8_t>(f));
    }
    append_u8(out, req.require_hardware_bound ? 1 : 0);
    append_u8(out, req.require_non_exportable_key ? 1 : 0);
    append_u8(out, req.require_online_freshness ? 1 : 0);
    append_u8(out, req.require_remote_attestation ? 1 : 0);
    append_u8(out, static_cast<std::uint8_t>(req.require_recovery_model));
    append_u16(out, static_cast<std::uint16_t>(
                        req.allowed_provider_classes.size()));
    for (auto c : req.allowed_provider_classes) {
        append_u8(out, static_cast<std::uint8_t>(c));
    }
    append_u64(out, req.minimum_provider_epoch);
    return out;
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
policy_requirement_hash(const PolicyRequirement& req) noexcept {
    const auto bytes = serialize_policy_requirement(req);
    return VMPilot::Cbor::domain_hash_sha256(
        VMPilot::DomainLabels::Hash::PolicyRequirement, bytes);
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

    // 2. Evidence. Use the requirement hash as the nonce seed so the
    //    binding is deterministic for unit tests; production would
    //    sample a random nonce and persist it to defeat replay.
    const auto req_hash = policy_requirement_hash(requirement);
    std::array<std::uint8_t, 32> nonce = req_hash;
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
