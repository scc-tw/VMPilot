#ifndef VMPILOT_RUNTIME_BINDING_RESOLVED_PROFILE_HPP
#define VMPILOT_RUNTIME_BINDING_RESOLVED_PROFILE_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <tl/expected.hpp>

// Minimal ResolvedFamilyProfile header parser.
//
// Stage 8 only needs the five fields that feed registry dispatch:
//   profile_id / family_id / requested_policy_id / profile_revision /
//   runtime_specialization_id
//
// Full profile surface (Layer-1 correctness / Layer-2 security posture /
// Layer-3 review contracts per doc 05) is deferred until there is a
// concrete consumer in the runtime. The parser tolerates — in fact
// expects — additional fields beyond the ones it extracts, so future
// stages can add content without breaking this layer.

namespace VMPilot::Runtime::Binding {

struct ResolvedFamilyProfileHeader {
    std::string profile_id;
    std::string family_id;
    std::string requested_policy_id;
    std::string profile_revision;
    std::string runtime_specialization_id;
};

enum class ResolvedFamilyProfileParseError : std::uint8_t {
    BadCbor = 1,
    NotAMap,
    MissingField,
    WrongFieldType,
};

tl::expected<ResolvedFamilyProfileHeader, ResolvedFamilyProfileParseError>
parse_resolved_family_profile_header(const std::uint8_t* data, std::size_t size) noexcept;

inline tl::expected<ResolvedFamilyProfileHeader, ResolvedFamilyProfileParseError>
parse_resolved_family_profile_header(const std::vector<std::uint8_t>& bytes) noexcept {
    return parse_resolved_family_profile_header(bytes.data(), bytes.size());
}

}  // namespace VMPilot::Runtime::Binding

#endif  // VMPILOT_RUNTIME_BINDING_RESOLVED_PROFILE_HPP
