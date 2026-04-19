#ifndef VMPILOT_COMMON_VM_FAMILY_POLICY_HPP
#define VMPILOT_COMMON_VM_FAMILY_POLICY_HPP

#include <array>
#include <cstdint>
#include <optional>
#include <string_view>
#include <utility>

#include "vm/enum_text.hpp"

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

}  // namespace VMPilot::DomainLabels

// Trait specializations — single source of truth for enum ↔ text.
namespace VMPilot {

template <>
struct EnumTextTraits<DomainLabels::FamilyId> {
    static constexpr std::array<
        std::pair<DomainLabels::FamilyId, std::string_view>, 3>
        entries{{
            {DomainLabels::FamilyId::F1, "f1"},
            {DomainLabels::FamilyId::F2, "f2"},
            {DomainLabels::FamilyId::F3, "f3"},
        }};
};

template <>
struct EnumTextTraits<DomainLabels::PolicyId> {
    static constexpr std::array<
        std::pair<DomainLabels::PolicyId, std::string_view>, 3>
        entries{{
            {DomainLabels::PolicyId::Debug,    "debug"},
            {DomainLabels::PolicyId::Standard, "standard"},
            {DomainLabels::PolicyId::HighSec,  "highsec"},
        }};
};

}  // namespace VMPilot

namespace VMPilot::DomainLabels {

// Backwards-compat overloads — kept so the 40+ existing call sites
// don't have to change. They delegate into the generic trait-driven
// helpers; adding a new enumerator only requires editing the trait.
[[nodiscard]] constexpr std::string_view to_text(FamilyId f) noexcept {
    return VMPilot::enum_to_text(f);
}
[[nodiscard]] constexpr std::string_view to_text(PolicyId p) noexcept {
    return VMPilot::enum_to_text(p);
}
[[nodiscard]] constexpr std::optional<FamilyId>
parse_family_id(std::string_view s) noexcept {
    return VMPilot::enum_from_text<FamilyId>(s);
}
[[nodiscard]] constexpr std::optional<PolicyId>
parse_policy_id(std::string_view s) noexcept {
    return VMPilot::enum_from_text<PolicyId>(s);
}

}  // namespace VMPilot::DomainLabels

#endif  // VMPILOT_COMMON_VM_FAMILY_POLICY_HPP
