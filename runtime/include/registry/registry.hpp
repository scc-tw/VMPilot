#ifndef VMPILOT_RUNTIME_REGISTRY_REGISTRY_HPP
#define VMPILOT_RUNTIME_REGISTRY_REGISTRY_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <tl/expected.hpp>

#include "trust_root.hpp"
#include "vm/family_policy.hpp"

// RuntimeSpecializationRegistry parser + lookup.
//
// The registry is the signed runtime-path selection authority. Every
// executable unit entry follows:
//
//     (spec_id, family_id, policy_id, profile_revision)
//         -> registry.lookup(...) -> SpecializationEntry
//         -> dispatch to VmEngine<ResolvedPolicy, ResolvedOram>
//
// blob header flags are strictly a parse hint; they never decide which
// code path runs. That decision flows exclusively from the signed PBR
// (Stage 5) through the registry commitment to the entry returned here.

namespace VMPilot::Runtime::Registry {

// ─── On-disk entry — canonical CBOR map keyed by small unsigned ints ────
//
//     {  1: runtime_specialization_id           text
//        2: family_id                            text ("f1" / "f2" / "f3")
//        3: requested_policy_id                  text ("debug" / "standard" / "highsec")
//        4: profile_revision                     text
//        5: semantic_contract_version            text
//        6: execution_contract_ref               text
//        7: required_runtime_primitives_hash     bytes(32)
//        8: required_runtime_helpers_hash        bytes(32)
//        9: provider_requirement_hash            bytes(32)
//       10: accepted_profile_content_hash        bytes(32)
//       11: diagnostic_visibility_class          uint
//       12: enabled_in_this_runtime              uint (0 = false, 1 = true)
//       13: provider_requirement_canonical_bytes bytes (may be empty) }
//
// Field 13 carries the producer-supplied canonical CBOR bytes for
// the PolicyRequirement that field 9's hash commits to. The runtime
// parses field 13, recomputes the hash, compares to field 9, and
// rejects any mismatch. When a unit has no provider requirement the
// producer emits empty bytes + all-zero hash; any other combination
// is invalid.

struct SpecializationEntry {
    std::string runtime_specialization_id;
    VMPilot::DomainLabels::FamilyId family_id;
    VMPilot::DomainLabels::PolicyId requested_policy_id;
    std::string profile_revision;
    std::string semantic_contract_version;
    std::string execution_contract_ref;
    std::array<std::uint8_t, 32> required_runtime_primitives_hash;
    std::array<std::uint8_t, 32> required_runtime_helpers_hash;
    std::array<std::uint8_t, 32> provider_requirement_hash;
    std::array<std::uint8_t, 32> accepted_profile_content_hash;
    std::uint64_t diagnostic_visibility_class;
    bool enabled_in_this_runtime;
    std::vector<std::uint8_t> provider_requirement_canonical_bytes;
};

// ─── Registry header — the top-level map wrapping the entry list ────────
//
//     { 1: registry_version         text
//       2: runtime_build_id         text
//       3: package_schema_version   text
//       4: registry_epoch           uint
//       5: entries                  array<SpecializationEntry> }

struct Registry {
    std::string registry_version;
    std::string runtime_build_id;
    std::string package_schema_version;
    std::uint64_t registry_epoch;
    std::vector<SpecializationEntry> entries;
};

enum class ParseError : std::uint8_t {
    BadCbor = 1,
    NotAMap,
    MissingField,
    WrongFieldType,
    WrongHashSize,
    EnabledFlagOutOfRange,
    DuplicateEntry,           // two entries share the same lookup tuple
    UnknownFamilyId,
    UnknownPolicyId,

    // Partition wrapper (see parse_partition below).
    PartitionMalformed,
    AuthKindUnsupported,
    AuthKeyIdMismatch,
    AuthSignatureAlgMismatch,
    AuthCoveredDomainMismatch,
    SignatureWrongSize,
    SignatureInvalid,

    // provider_requirement field-13/field-9 consistency.
    InconsistentRequirementCommitment,   // bytes ↔ hash disagree
                                         // (empty bytes with non-zero
                                         // hash, non-empty bytes with
                                         // zero hash, or recomputed
                                         // hash over bytes != field 9)
};

enum class LookupError : std::uint8_t {
    NotFound = 1,
    Disabled,                 // matched but enabled_in_this_runtime == false
};

// Parse registry canonical bytes (the byte string that ends up inside
// the partition wrapper's element [0]). Callers that already unwrapped
// the partition or assembled the registry bytes themselves use this
// entry point; runtime dispatch should prefer `parse_partition` below.
tl::expected<Registry, ParseError>
parse(const std::uint8_t* data, std::size_t size) noexcept;

inline tl::expected<Registry, ParseError>
parse(const std::vector<std::uint8_t>& bytes) noexcept {
    return parse(bytes.data(), bytes.size());
}

// Parse a signed registry partition. The on-disk form is a strict-CBOR
// array[2] of [canonical_bytes, binding_auth_map], matching the pattern
// established by PackageBindingRecord partitions. The binding_auth
// signs the canonical bytes under covered_domain
// "runtime-specialization-registry-v1", verified against the supplied
// VendorTrustRoot.
//
// Contract source: internal research doc 08 §3.1.
tl::expected<Registry, ParseError>
parse_partition(const std::uint8_t* data, std::size_t size,
                const VMPilot::Runtime::VendorTrustRoot& root) noexcept;

inline tl::expected<Registry, ParseError>
parse_partition(const std::vector<std::uint8_t>& bytes,
                const VMPilot::Runtime::VendorTrustRoot& root) noexcept {
    return parse_partition(bytes.data(), bytes.size(), root);
}

// Look up an entry by its exact (spec_id, family, policy, profile_revision)
// tuple. Returns a pointer into `reg.entries` — the pointer remains valid
// as long as `reg` lives.
//
// LookupError::Disabled surfaces entries that exist but cannot be selected
// because their `enabled_in_this_runtime` flag is false. Surfacing the
// distinction lets callers (and tests) reject loudly rather than silently
// fall through to a different entry.
tl::expected<const SpecializationEntry*, LookupError>
lookup(const Registry& reg,
       std::string_view spec_id,
       VMPilot::DomainLabels::FamilyId family_id,
       VMPilot::DomainLabels::PolicyId policy_id,
       std::string_view profile_revision) noexcept;

}  // namespace VMPilot::Runtime::Registry

#endif  // VMPILOT_RUNTIME_REGISTRY_REGISTRY_HPP
