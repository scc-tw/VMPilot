#ifndef VMPILOT_RUNTIME_EH_GUARD_HPP
#define VMPILOT_RUNTIME_EH_GUARD_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <tl/expected.hpp>

#include "diagnostic.hpp"
#include "platform_call.hpp"
#include "vm/enum_text.hpp"
#include "vm/family_policy.hpp"

namespace VMPilot::Runtime::EH {

enum class ExecutableEhStatus : std::uint8_t {
    ReservedDisabledV1 = 1,
    ExecutableV1_1,
};

enum class CrossProtectedFrameUnwind : std::uint8_t {
    Forbidden = 1,
    PermittedByProfile,
};

enum class NativeBoundaryUnwindBehavior : std::uint8_t {
    TranslateToTrapOrFailClosed = 1,
    ProfileUpgraded,
};

enum class ReservedTableStatus : std::uint8_t {
    ReservedEmpty = 1,
    ProfileSpecific,
};

}  // namespace VMPilot::Runtime::EH

// Trait specializations — canonical on-wire text for each EH enum.
// Single source of truth; the to_text overloads below just delegate.
namespace VMPilot {

template <>
struct EnumTextTraits<VMPilot::Runtime::EH::ExecutableEhStatus> {
    using E = VMPilot::Runtime::EH::ExecutableEhStatus;
    static constexpr std::array<std::pair<E, std::string_view>, 2>
        entries{{
            {E::ReservedDisabledV1, "reserved_disabled_v1"},
            {E::ExecutableV1_1,     "executable_v1_1"},
        }};
};

template <>
struct EnumTextTraits<VMPilot::Runtime::EH::CrossProtectedFrameUnwind> {
    using E = VMPilot::Runtime::EH::CrossProtectedFrameUnwind;
    static constexpr std::array<std::pair<E, std::string_view>, 2>
        entries{{
            {E::Forbidden,          "forbidden"},
            {E::PermittedByProfile, "permitted_by_profile"},
        }};
};

template <>
struct EnumTextTraits<VMPilot::Runtime::EH::NativeBoundaryUnwindBehavior> {
    using E = VMPilot::Runtime::EH::NativeBoundaryUnwindBehavior;
    static constexpr std::array<std::pair<E, std::string_view>, 2>
        entries{{
            {E::TranslateToTrapOrFailClosed, "translate_to_trap_or_fail_closed"},
            {E::ProfileUpgraded,             "profile-upgraded"},
        }};
};

template <>
struct EnumTextTraits<VMPilot::Runtime::EH::ReservedTableStatus> {
    using E = VMPilot::Runtime::EH::ReservedTableStatus;
    static constexpr std::array<std::pair<E, std::string_view>, 2>
        entries{{
            {E::ReservedEmpty,    "reserved_empty"},
            {E::ProfileSpecific,  "profile-specific"},
        }};
};

}  // namespace VMPilot

namespace VMPilot::Runtime::EH {

// Canonical on-wire text — delegates into VMPilot::EnumTextTraits
// specializations above; adding a new enumerator only requires
// editing the trait's entries array.
[[nodiscard]] constexpr std::string_view to_text(ExecutableEhStatus s) noexcept {
    return VMPilot::enum_to_text(s);
}
[[nodiscard]] constexpr std::string_view to_text(CrossProtectedFrameUnwind u) noexcept {
    return VMPilot::enum_to_text(u);
}
[[nodiscard]] constexpr std::string_view to_text(NativeBoundaryUnwindBehavior b) noexcept {
    return VMPilot::enum_to_text(b);
}
[[nodiscard]] constexpr std::string_view to_text(ReservedTableStatus s) noexcept {
    return VMPilot::enum_to_text(s);
}

struct ExceptionUnwindContract {
    std::string semantic_contract_version;
    std::string eh_contract_version;
    ExecutableEhStatus executable_eh_status;
    std::string planned_executable_eh_epoch;
    CrossProtectedFrameUnwind cross_protected_frame_unwind;
    NativeBoundaryUnwindBehavior native_boundary_unwind_behavior;
    ReservedTableStatus handler_table_status;
    ReservedTableStatus cleanup_table_status;
    std::string frame_contract_ref;
    std::string stackmap_contract_ref;
    std::string resume_contract_ref;
    std::string verifier_rules_ref;
    std::string family_specific_unwind_surface_ref;
};

enum class ContractParseError : std::uint8_t {
    BadCbor = 1,
    NotAMap,
    MissingCorrectnessLegalityContract,
    MissingExceptionUnwindContract,
    MissingField,
    WrongFieldType,
    UnknownEnumValue,
    UnknownCriticalExtension,
};

enum class ContractVerifyError : std::uint8_t {
    MalformedContract = 1,
    ExecutableEhStatusNotReservedDisabled,
    CrossProtectedFrameUnwindPermitted,
    NativeBoundaryBehaviorNotFailClosed,
    HandlerTableNotReservedEmpty,
    CleanupTableNotReservedEmpty,
    UnknownCriticalExtension,
    FamilySpecificUnwindSurfaceMismatch,
};

[[nodiscard]] tl::expected<ExceptionUnwindContract, ContractParseError>
parse_exception_unwind_contract(const std::uint8_t* data,
                                std::size_t size) noexcept;

inline tl::expected<ExceptionUnwindContract, ContractParseError>
parse_exception_unwind_contract(
    const std::vector<std::uint8_t>& bytes) noexcept {
    return parse_exception_unwind_contract(bytes.data(), bytes.size());
}

[[nodiscard]] tl::expected<ExceptionUnwindContract, ContractVerifyError>
verify_reserved_exception_unwind_contract(
    const std::uint8_t* data,
    std::size_t size,
    VMPilot::DomainLabels::FamilyId expected_family) noexcept;

inline tl::expected<ExceptionUnwindContract, ContractVerifyError>
verify_reserved_exception_unwind_contract(
    const std::vector<std::uint8_t>& bytes,
    VMPilot::DomainLabels::FamilyId expected_family) noexcept {
    return verify_reserved_exception_unwind_contract(
        bytes.data(), bytes.size(), expected_family);
}

std::string_view expected_family_specific_unwind_surface_ref(
    VMPilot::DomainLabels::FamilyId family_id) noexcept;

[[nodiscard]] tl::expected<std::uint64_t, VMPilot::Common::DiagnosticCode>
guarded_platform_call(const VMPilot::Runtime::PlatformCallDesc* desc,
                      bool returns_struct,
                      void* struct_return_ptr) noexcept;

}  // namespace VMPilot::Runtime::EH

#endif  // VMPILOT_RUNTIME_EH_GUARD_HPP