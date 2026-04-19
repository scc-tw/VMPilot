#ifndef VMPILOT_RUNTIME_EH_GUARD_HPP
#define VMPILOT_RUNTIME_EH_GUARD_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <tl/expected.hpp>

#include "diagnostic.hpp"
#include "platform_call.hpp"
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

// Canonical on-wire text for each enum. Matches the strings the
// strict-CBOR parser in eh_guard.cpp accepts. Kept here alongside
// the enum definitions so fixtures and runtime share one source of
// truth — adding a new enumerator in one place without updating the
// other is now a compile-time visible gap.
constexpr std::string_view to_text(ExecutableEhStatus s) noexcept {
    switch (s) {
        case ExecutableEhStatus::ReservedDisabledV1: return "reserved_disabled_v1";
        case ExecutableEhStatus::ExecutableV1_1:     return "executable_v1_1";
    }
    return {};
}
constexpr std::string_view to_text(CrossProtectedFrameUnwind u) noexcept {
    switch (u) {
        case CrossProtectedFrameUnwind::Forbidden:           return "forbidden";
        case CrossProtectedFrameUnwind::PermittedByProfile:  return "permitted_by_profile";
    }
    return {};
}
constexpr std::string_view to_text(NativeBoundaryUnwindBehavior b) noexcept {
    switch (b) {
        case NativeBoundaryUnwindBehavior::TranslateToTrapOrFailClosed:
            return "translate_to_trap_or_fail_closed";
        case NativeBoundaryUnwindBehavior::ProfileUpgraded:
            return "profile-upgraded";
    }
    return {};
}
constexpr std::string_view to_text(ReservedTableStatus s) noexcept {
    switch (s) {
        case ReservedTableStatus::ReservedEmpty:    return "reserved_empty";
        case ReservedTableStatus::ProfileSpecific:  return "profile-specific";
    }
    return {};
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

tl::expected<ExceptionUnwindContract, ContractParseError>
parse_exception_unwind_contract(const std::uint8_t* data,
                                std::size_t size) noexcept;

inline tl::expected<ExceptionUnwindContract, ContractParseError>
parse_exception_unwind_contract(
    const std::vector<std::uint8_t>& bytes) noexcept {
    return parse_exception_unwind_contract(bytes.data(), bytes.size());
}

tl::expected<ExceptionUnwindContract, ContractVerifyError>
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

tl::expected<std::uint64_t, VMPilot::Common::DiagnosticCode>
guarded_platform_call(const VMPilot::Runtime::PlatformCallDesc* desc,
                      bool returns_struct,
                      void* struct_return_ptr) noexcept;

}  // namespace VMPilot::Runtime::EH

#endif  // VMPILOT_RUNTIME_EH_GUARD_HPP