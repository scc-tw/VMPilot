/// @file vm_stub_entry.cpp
/// @brief C-linkage entry point wired to VmEngine<Policy, Oram>.
///
/// This is the production execution path.  The loader-generated assembly
/// stubs call vm_stub_entry() which:
///   1. Validates the VmStubArgs
///   2. Selects the security policy based on the blob's debug flag
///   3. Creates a VmEngine and executes until HALT
///   4. Returns the decoded result in the platform return register
///
/// Policy selection layer:
///   Blob debug flag → DebugPolicy (native bridge arithmetic, no constant-time)
///   Otherwise       → StandardPolicy (MBA arithmetic, constant-time pipeline)
///
/// The policy is selected ONCE at entry — not per-instruction.
/// VmEngine is a template, so each policy variant is a separate
/// compilation unit with full dead-code elimination.

#include <vm_stub_entry.hpp>
#include <vm_engine.hpp>
#include <vm/blob_view.hpp>

#include <binding/inner_partition.hpp>
#include <binding/package.hpp>
#include <binding/resolved_profile.hpp>
#include <binding/unit.hpp>
#include <envelope/outer.hpp>
#include <registry/registry.hpp>
#include <trust_root.hpp>

#include <climits>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

using namespace VMPilot::Common::VM;
using namespace VMPilot::Runtime;

// ─────────────────────────────────────────────────────────────────────────────
// Policy dispatch layer: template function per policy, called once at entry
// ─────────────────────────────────────────────────────────────────────────────

template<typename Policy>
static int64_t execute_with_policy(const VmStubArgs* args) noexcept {
    auto engine = VmEngine<Policy>::create(
        args->blob_data, args->blob_size,
        args->stored_seed,
        args->load_base_delta,
        args->initial_regs, args->num_regs);

    if (!engine.has_value()) {
#ifdef NDEBUG
        std::abort();
#else
        return INT64_MIN;
#endif
    }

    auto result = engine->execute();
    if (!result.has_value()) {
#ifdef NDEBUG
        std::abort();
#else
        return INT64_MIN;
#endif
    }

    return static_cast<int64_t>(result->return_value);
}

// ─────────────────────────────────────────────────────────────────────────────
// C-linkage entry point
// ─────────────────────────────────────────────────────────────────────────────

extern "C"
int64_t vm_stub_entry(const VmStubArgs* args) noexcept {

    // ── 1. Validation layer ─────────────────────────────────────────────

    if (!args || args->version != VM_STUB_ABI_VERSION) {
#ifdef NDEBUG
        std::abort();
#else
        return INT64_MIN;
#endif
    }

    if (!args->blob_data || args->blob_size < sizeof(BlobHeader)) {
#ifdef NDEBUG
        std::abort();
#else
        return INT64_MIN;
#endif
    }

    if (!args->stored_seed) {
#ifdef NDEBUG
        std::abort();
#else
        return INT64_MIN;
#endif
    }

    // ── 2. Policy selection layer ───────────────────────────────────────
    //
    // Peek at blob header to determine debug mode.  BlobView::create()
    // validates the full header; here we just need the debug flag to
    // select the policy.  This is a single branch executed once at entry,
    // not per-instruction.

    auto blob_or = BlobView::create(args->blob_data, args->blob_size);
    if (!blob_or.has_value()) {
#ifdef NDEBUG
        std::abort();
#else
        return INT64_MIN;
#endif
    }

    // ── 3. Dispatch to policy-specific execution ────────────────────────

    if (blob_or->is_debug()) {
        return execute_with_policy<DebugPolicy>(args);
    } else {
        return execute_with_policy<StandardPolicy>(args);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Stage 8 — redesigned entry point driven by the signed artifact chain.
// ─────────────────────────────────────────────────────────────────────────────
//
// Flow:
//   1. Outer envelope parse (Stage 4)
//   2. accept_package  — PBR signature + epoch + table hashes (Stage 5)
//   3. accept_unit_entry — descriptor / UBR / profile / payload hashes (Stage 7)
//   4. parse profile header — surface runtime_specialization_id + revision
//   5. Registry lookup (Stage 6)
//   6. entry.accepted_profile_content_hash ==
//        ubr.resolved_family_profile_content_hash    (doc 08 §4)
//   7. dispatch via (family, policy) → VmEngine<Policy, Oram> template
//
// blob header flags are not consulted at any point; dispatch is entirely
// driven by signed metadata.

namespace {

// Raw payload + seed slice extracted from the accepted artifact. Passed
// to the policy-dispatch helper as a plain-data handoff so the template
// caller doesn't need to know about VmStubArtifactArgs.
struct PayloadDispatch {
    const std::uint8_t*  payload_data;
    std::uint64_t        payload_size;
    const std::uint8_t*  stored_seed;
    std::int64_t         load_base_delta;
    const std::uint64_t* initial_regs;
    std::uint8_t         num_regs;
};

template <typename Policy>
int64_t execute_with_policy_on_payload(const PayloadDispatch& d) noexcept {
    auto engine = VmEngine<Policy>::create(
        d.payload_data,
        static_cast<std::size_t>(d.payload_size),
        d.stored_seed,
        d.load_base_delta,
        d.initial_regs,
        d.num_regs);
    if (!engine.has_value()) {
#ifdef NDEBUG
        std::abort();
#else
        return INT64_MIN;
#endif
    }
    auto result = engine->execute();
    if (!result.has_value()) {
#ifdef NDEBUG
        std::abort();
#else
        return INT64_MIN;
#endif
    }
    return static_cast<int64_t>(result->return_value);
}

// Dispatch to VmEngine<Policy> based on the signed (family, policy) tuple.
// Only F1 is supported today; F2 / F3 are reserved families from doc 01
// that fail closed here until their runtime paths land.
int64_t dispatch_to_engine(VMPilot::DomainLabels::FamilyId family_id,
                           VMPilot::DomainLabels::PolicyId policy_id,
                           const PayloadDispatch& d) noexcept {
    if (family_id != VMPilot::DomainLabels::FamilyId::F1) {
#ifdef NDEBUG
        std::abort();
#else
        return INT64_MIN;
#endif
    }
    switch (policy_id) {
        case VMPilot::DomainLabels::PolicyId::Debug:
            return execute_with_policy_on_payload<DebugPolicy>(d);
        case VMPilot::DomainLabels::PolicyId::Standard:
            return execute_with_policy_on_payload<StandardPolicy>(d);
        case VMPilot::DomainLabels::PolicyId::HighSec:
            return execute_with_policy_on_payload<HighSecPolicy>(d);
    }
#ifdef NDEBUG
    std::abort();
#else
    return INT64_MIN;
#endif
}

// Fail-closed return. Compiled-in debug diagnostics surface INT64_MIN so
// tests can assert on it; release builds abort so no attacker-controlled
// control flow leaks past a rejected artifact.
[[nodiscard]] int64_t fail_closed() noexcept {
#ifdef NDEBUG
    std::abort();
#else
    return INT64_MIN;
#endif
}

}  // namespace

extern "C"
int64_t vm_stub_entry_artifact(const VmStubArtifactArgs* args) noexcept {
    if (!args || args->version != VM_STUB_ARTIFACT_ABI_VERSION) {
        return fail_closed();
    }
    if (!args->artifact_data || args->artifact_size == 0 ||
        !args->unit_id || !args->stored_seed) {
        return fail_closed();
    }

    // Stage 4: outer envelope.
    auto env_or = VMPilot::Runtime::Envelope::parse_outer_envelope(
        args->artifact_data, static_cast<std::size_t>(args->artifact_size));
    if (!env_or) return fail_closed();

    // Stage 5: package acceptance. Runtime epoch state is a compile-time
    // default for now — Stage 12 will wire in persistent state.
    //
    // The supported schema and canonical-encoding lists are central to
    // every acceptance check, so they live in a single named constant
    // rather than an inline initializer list duplicated at every call
    // site. Future Stage-12 config loading will replace this function.
    const auto cfg = [] {
        VMPilot::Runtime::Binding::AcceptConfig c;
        c.supported_schema_versions     = {"package-schema-v1"};
        c.supported_canonical_encodings = {"canonical-metadata-bytes-v1"};
        c.epoch = VMPilot::Runtime::Binding::RuntimeEpochState{
            /*runtime_epoch          */ 2,
            /*minimum_accepted_epoch */ 1,
        };
        return c;
    }();

    // Root epoch cross-check. VendorTrustRoot.root_epoch tracks trust-
    // anchor rotation; rotation handling is Stage-12 territory but a
    // zero value in a shipping runtime is always a provisioning bug.
    if (VMPilot::Runtime::trust_root().root_epoch == 0) {
        return fail_closed();
    }

    auto pkg_or = VMPilot::Runtime::Binding::accept_package(
        args->artifact_data, static_cast<std::size_t>(args->artifact_size),
        VMPilot::Runtime::trust_root(), *env_or, cfg);
    if (!pkg_or) return fail_closed();

    // Stage 7: unit acceptance keyed by the stub-provided unit id.
    auto unit_or = VMPilot::Runtime::Binding::accept_unit_entry(
        args->artifact_data, static_cast<std::size_t>(args->artifact_size),
        *env_or, *pkg_or, std::string_view{args->unit_id}, cfg);
    if (!unit_or) return fail_closed();

    // Stage 8 core: parse the profile header to learn the specialization
    // id + profile revision, then consult the signed registry carried in
    // the same inner partition.
    auto profile_or =
        VMPilot::Runtime::Binding::parse_resolved_family_profile_header(
            unit_or->resolved_profile_bytes);
    if (!profile_or) return fail_closed();

    // Inner partition must re-open to expose the registry bytes. The
    // hash was already verified by accept_package, so this parse is safe.
    auto inner_or = VMPilot::Runtime::Binding::parse_inner_partition(
        args->artifact_data + env_or->inner_metadata_partition.offset,
        env_or->inner_metadata_partition.length);
    if (!inner_or) return fail_closed();

    // Registry partition (doc 08 §3.1): strict-CBOR array of
    // [canonical_bytes, RegistryBindingAuth]. parse_partition verifies
    // the signature against VendorTrustRoot with covered_domain
    // "runtime-specialization-registry-v1" before handing back the
    // parsed Registry. The PBR-level commitment over the whole
    // partition bytes was already verified by accept_package.
    auto reg_or = VMPilot::Runtime::Registry::parse_partition(
        inner_or->runtime_specialization_registry,
        VMPilot::Runtime::trust_root());
    if (!reg_or) return fail_closed();

    auto entry_or = VMPilot::Runtime::Registry::lookup(
        *reg_or,
        std::string_view{profile_or->runtime_specialization_id},
        unit_or->ubr.family_id,
        unit_or->ubr.requested_policy_id,
        std::string_view{profile_or->profile_revision});
    if (!entry_or) return fail_closed();
    const VMPilot::Runtime::Registry::SpecializationEntry* entry = *entry_or;

    // Registry commits to the specific profile content it was built for.
    // Compare against the UBR's commitment; a mismatch means the producer
    // shipped a registry that doesn't correspond to the profile this
    // unit resolved to.
    if (std::memcmp(entry->accepted_profile_content_hash.data(),
                    unit_or->ubr.resolved_family_profile_content_hash.data(),
                    32) != 0) {
        return fail_closed();
    }

    // provider_satisfies gate (doc 08 §4 rule 4). An all-zero hash
    // encodes the "no provider requirement" convention; anything else
    // names a signed ProviderRequirement that only Stage 10's
    // TrustProvider infrastructure can evaluate. Until that lands,
    // any entry that demands a provider is fail-closed — which is
    // exactly the stance doc 08 §7 forbids us from working around.
    constexpr std::array<std::uint8_t, 32> kNoProviderRequirement{};
    if (entry->provider_requirement_hash != kNoProviderRequirement) {
        return fail_closed();
    }

    // Dispatch.
    PayloadDispatch d;
    d.payload_data = args->artifact_data + env_or->payload_partition.offset;
    d.payload_size = env_or->payload_partition.length;
    d.stored_seed = args->stored_seed;
    d.load_base_delta = args->load_base_delta;
    d.initial_regs = args->initial_regs;
    d.num_regs = static_cast<std::uint8_t>(args->num_regs);

    return dispatch_to_engine(entry->family_id, entry->requested_policy_id, d);
}
