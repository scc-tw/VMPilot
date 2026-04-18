#ifndef VMPILOT_COMMON_VM_FAMILY_POLICY_HPP
#define VMPILOT_COMMON_VM_FAMILY_POLICY_HPP

#include <cstdint>
#include <optional>
#include <string_view>

// Spec-faithful enumerations for the two customer-facing vocabularies.
//
// Wire encoding stays text — the canonical CBOR for UnitDescriptor, UBR,
// ResolvedFamilyProfile, and RuntimeSpecializationRegistry carries these
// as strings so schema evolution ("f4" / "premium") doesn't require a
// coordinated numeric-id allocation across producer and consumer. These
// enums exist for in-process type safety and dispatch; parsers convert
// at the CBOR boundary and reject anything outside the closed set.

namespace VMPilot::DomainLabels {

enum class FamilyId : std::uint8_t {
    F1 = 1,
    F2 = 2,
    F3 = 3,
};

enum class PolicyId : std::uint8_t {
    Debug    = 1,
    Standard = 2,
    HighSec  = 3,
};

constexpr std::string_view to_text(FamilyId f) noexcept {
    switch (f) {
        case FamilyId::F1: return "f1";
        case FamilyId::F2: return "f2";
        case FamilyId::F3: return "f3";
    }
    return {};  // unreachable with valid enum
}

constexpr std::string_view to_text(PolicyId p) noexcept {
    switch (p) {
        case PolicyId::Debug:    return "debug";
        case PolicyId::Standard: return "standard";
        case PolicyId::HighSec:  return "highsec";
    }
    return {};
}

constexpr std::optional<FamilyId> parse_family_id(std::string_view s) noexcept {
    if (s == "f1") return FamilyId::F1;
    if (s == "f2") return FamilyId::F2;
    if (s == "f3") return FamilyId::F3;
    return std::nullopt;
}

constexpr std::optional<PolicyId> parse_policy_id(std::string_view s) noexcept {
    if (s == "debug")    return PolicyId::Debug;
    if (s == "standard") return PolicyId::Standard;
    if (s == "highsec")  return PolicyId::HighSec;
    return std::nullopt;
}

}  // namespace VMPilot::DomainLabels

#endif  // VMPILOT_COMMON_VM_FAMILY_POLICY_HPP
