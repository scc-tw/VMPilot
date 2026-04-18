#ifndef VMPILOT_RUNTIME_REGISTRY_REGISTRY_HPP
#define VMPILOT_RUNTIME_REGISTRY_REGISTRY_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <tl/expected.hpp>

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
//     { 1: runtime_specialization_id        text
//       2: family_id                         text ("f1" / "f2" / "f3")
//       3: requested_policy_id               text ("debug" / "standard" / "highsec")
//       4: profile_revision                  text
//       5: semantic_contract_version         text
//       6: execution_contract_ref            text
//       7: required_runtime_primitives_hash  bytes(32)
//       8: required_runtime_helpers_hash     bytes(32)
//       9: provider_requirement_hash         bytes(32)
//      10: accepted_profile_content_hash     bytes(32)
//      11: diagnostic_visibility_class       uint
//      12: enabled_in_this_runtime           uint (0 = false, 1 = true) }

struct SpecializationEntry {
    std::string runtime_specialization_id;
    std::string family_id;
    std::string requested_policy_id;
    std::string profile_revision;
    std::string semantic_contract_version;
    std::string execution_contract_ref;
    std::array<std::uint8_t, 32> required_runtime_primitives_hash;
    std::array<std::uint8_t, 32> required_runtime_helpers_hash;
    std::array<std::uint8_t, 32> provider_requirement_hash;
    std::array<std::uint8_t, 32> accepted_profile_content_hash;
    std::uint64_t diagnostic_visibility_class;
    bool enabled_in_this_runtime;
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
};

enum class LookupError : std::uint8_t {
    NotFound = 1,
    Disabled,                 // matched but enabled_in_this_runtime == false
};

// Parse registry bytes produced by the fixture / compiler. Caller is
// expected to have already verified that `data` matches the PBR's
// runtime_specialization_registry_hash commitment.
tl::expected<Registry, ParseError>
parse(const std::uint8_t* data, std::size_t size) noexcept;

inline tl::expected<Registry, ParseError>
parse(const std::vector<std::uint8_t>& bytes) noexcept {
    return parse(bytes.data(), bytes.size());
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
       std::string_view family_id,
       std::string_view policy_id,
       std::string_view profile_revision) noexcept;

}  // namespace VMPilot::Runtime::Registry

#endif  // VMPILOT_RUNTIME_REGISTRY_REGISTRY_HPP
