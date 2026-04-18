#include "eh_guard.hpp"

#include <cstring>

#include "cbor/strict.hpp"

#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <csignal>
#include <csetjmp>
#endif

namespace VMPilot::Runtime::EH {

namespace {

using VMPilot::Cbor::Value;
using VMPilot::Cbor::parse_strict;

constexpr std::uint64_t kProfile_CorrectnessLegalityContract = 6;
constexpr std::uint64_t kCorrectness_SemanticContractVersion = 1;
constexpr std::uint64_t kCorrectness_ExceptionUnwindContract = 2;

constexpr std::uint64_t kEh_EhContractVersion = 1;
constexpr std::uint64_t kEh_ExecutableEhStatus = 2;
constexpr std::uint64_t kEh_PlannedExecutableEhEpoch = 3;
constexpr std::uint64_t kEh_CrossProtectedFrameUnwind = 4;
constexpr std::uint64_t kEh_NativeBoundaryUnwindBehavior = 5;
constexpr std::uint64_t kEh_HandlerTableStatus = 6;
constexpr std::uint64_t kEh_CleanupTableStatus = 7;
constexpr std::uint64_t kEh_FrameContractRef = 8;
constexpr std::uint64_t kEh_StackmapContractRef = 9;
constexpr std::uint64_t kEh_ResumeContractRef = 10;
constexpr std::uint64_t kEh_VerifierRulesRef = 11;
constexpr std::uint64_t kEh_FamilySpecificUnwindSurfaceRef = 12;
constexpr std::uint64_t kEh_CriticalExtensions = 13;

inline tl::unexpected<ContractParseError>
parse_err(ContractParseError e) noexcept {
    return tl::make_unexpected(e);
}

inline tl::unexpected<ContractVerifyError>
verify_err(ContractVerifyError e) noexcept {
    return tl::make_unexpected(e);
}

tl::expected<std::string, ContractParseError>
require_text(const Value& map, std::uint64_t key) noexcept {
    const Value* value = map.find_by_uint_key(key);
    if (value == nullptr) return parse_err(ContractParseError::MissingField);
    if (value->kind() != Value::Kind::Text) {
        return parse_err(ContractParseError::WrongFieldType);
    }
    return value->as_text();
}

tl::expected<const Value*, ContractParseError>
require_map_field(const Value& map,
                  std::uint64_t key,
                  ContractParseError missing_error) noexcept {
    const Value* value = map.find_by_uint_key(key);
    if (value == nullptr) return parse_err(missing_error);
    if (value->kind() != Value::Kind::Map) {
        return parse_err(ContractParseError::WrongFieldType);
    }
    return value;
}

tl::expected<ExecutableEhStatus, ContractParseError>
parse_executable_eh_status(std::string_view text) noexcept {
    if (text == "reserved_disabled_v1") {
        return ExecutableEhStatus::ReservedDisabledV1;
    }
    if (text == "executable_v1_1") {
        return ExecutableEhStatus::ExecutableV1_1;
    }
    return parse_err(ContractParseError::UnknownEnumValue);
}

tl::expected<CrossProtectedFrameUnwind, ContractParseError>
parse_cross_protected_frame_unwind(std::string_view text) noexcept {
    if (text == "forbidden") {
        return CrossProtectedFrameUnwind::Forbidden;
    }
    if (text == "permitted_by_profile") {
        return CrossProtectedFrameUnwind::PermittedByProfile;
    }
    return parse_err(ContractParseError::UnknownEnumValue);
}

tl::expected<NativeBoundaryUnwindBehavior, ContractParseError>
parse_native_boundary_unwind_behavior(std::string_view text) noexcept {
    if (text == "translate_to_trap_or_fail_closed") {
        return NativeBoundaryUnwindBehavior::TranslateToTrapOrFailClosed;
    }
    if (text == "profile-upgraded") {
        return NativeBoundaryUnwindBehavior::ProfileUpgraded;
    }
    return parse_err(ContractParseError::UnknownEnumValue);
}

tl::expected<ReservedTableStatus, ContractParseError>
parse_reserved_table_status(std::string_view text) noexcept {
    if (text == "reserved_empty") {
        return ReservedTableStatus::ReservedEmpty;
    }
    if (text == "profile-specific") {
        return ReservedTableStatus::ProfileSpecific;
    }
    return parse_err(ContractParseError::UnknownEnumValue);
}

bool contract_has_unknown_extension(const Value& map) noexcept {
    for (std::size_t index = 0; index < map.map_size(); ++index) {
        const Value& key = map.map_key_at(index);
        if (key.kind() != Value::Kind::Uint) return true;
        switch (key.as_uint()) {
            case kEh_EhContractVersion:
            case kEh_ExecutableEhStatus:
            case kEh_PlannedExecutableEhEpoch:
            case kEh_CrossProtectedFrameUnwind:
            case kEh_NativeBoundaryUnwindBehavior:
            case kEh_HandlerTableStatus:
            case kEh_CleanupTableStatus:
            case kEh_FrameContractRef:
            case kEh_StackmapContractRef:
            case kEh_ResumeContractRef:
            case kEh_VerifierRulesRef:
            case kEh_FamilySpecificUnwindSurfaceRef:
            case kEh_CriticalExtensions:
                break;
            default:
                return true;
        }
    }
    return false;
}

tl::expected<bool, ContractParseError>
critical_extensions_present(const Value& map) noexcept {
    const Value* value = map.find_by_uint_key(kEh_CriticalExtensions);
    if (value == nullptr) return false;
    if (value->kind() != Value::Kind::Array) {
        return parse_err(ContractParseError::WrongFieldType);
    }
    for (const Value& entry : value->as_array()) {
        if (entry.kind() != Value::Kind::Text) {
            return parse_err(ContractParseError::WrongFieldType);
        }
    }
    return !value->as_array().empty();
}

std::uint64_t dispatch_platform_call(const PlatformCallDesc* desc,
                                     bool returns_struct,
                                     void* struct_return_ptr) noexcept {
    if (returns_struct) {
        return platform_call_struct(desc, struct_return_ptr);
    }
    return platform_call(desc);
}

#if defined(__unix__) || defined(__APPLE__)

thread_local sigjmp_buf* g_guard_jump_env = nullptr;
thread_local volatile sig_atomic_t g_guard_signal = 0;

extern "C" void guarded_platform_call_signal_handler(int signo,
                                                       siginfo_t*,
                                                       void*) {
    g_guard_signal = signo;
    if (g_guard_jump_env != nullptr) {
        siglongjmp(*g_guard_jump_env, 1);
    }
}

bool install_signal_handler(int signo, struct sigaction& old_action) noexcept {
    struct sigaction action;
    std::memset(&action, 0, sizeof(action));
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = guarded_platform_call_signal_handler;
    action.sa_flags = SA_SIGINFO;
    return sigaction(signo, &action, &old_action) == 0;
}

void restore_signal_handler(int signo, const struct sigaction& old_action) noexcept {
    sigaction(signo, &old_action, nullptr);
}

#endif

}  // namespace

std::string_view expected_family_specific_unwind_surface_ref(
    VMPilot::DomainLabels::FamilyId family_id) noexcept {
    switch (family_id) {
        case VMPilot::DomainLabels::FamilyId::F1:
            return "f1-unwind-surface-v1";
        case VMPilot::DomainLabels::FamilyId::F2:
            return "f2-unwind-surface-v1";
        case VMPilot::DomainLabels::FamilyId::F3:
            return "f3-unwind-surface-v1";
    }
    return "unknown-unwind-surface";
}

tl::expected<ExceptionUnwindContract, ContractParseError>
parse_exception_unwind_contract(const std::uint8_t* data,
                                std::size_t size) noexcept {
    auto root_or = parse_strict(data, size);
    if (!root_or) return parse_err(ContractParseError::BadCbor);
    const Value& root = *root_or;
    if (root.kind() != Value::Kind::Map) {
        return parse_err(ContractParseError::NotAMap);
    }

    auto correctness_or = require_map_field(
        root, kProfile_CorrectnessLegalityContract,
        ContractParseError::MissingCorrectnessLegalityContract);
    if (!correctness_or) return parse_err(correctness_or.error());
    const Value& correctness = **correctness_or;

    auto semantic_contract_version =
        require_text(correctness, kCorrectness_SemanticContractVersion);
    if (!semantic_contract_version) {
        return parse_err(semantic_contract_version.error());
    }

    auto contract_or = require_map_field(
        correctness, kCorrectness_ExceptionUnwindContract,
        ContractParseError::MissingExceptionUnwindContract);
    if (!contract_or) return parse_err(contract_or.error());
    const Value& contract = **contract_or;

    if (contract_has_unknown_extension(contract)) {
        return parse_err(ContractParseError::UnknownCriticalExtension);
    }

    auto critical_extensions = critical_extensions_present(contract);
    if (!critical_extensions) return parse_err(critical_extensions.error());
    if (*critical_extensions) {
        return parse_err(ContractParseError::UnknownCriticalExtension);
    }

    auto eh_contract_version = require_text(contract, kEh_EhContractVersion);
    auto executable_eh_status_text =
        require_text(contract, kEh_ExecutableEhStatus);
    auto planned_executable_eh_epoch =
        require_text(contract, kEh_PlannedExecutableEhEpoch);
    auto cross_protected_frame_unwind_text =
        require_text(contract, kEh_CrossProtectedFrameUnwind);
    auto native_boundary_unwind_behavior_text =
        require_text(contract, kEh_NativeBoundaryUnwindBehavior);
    auto handler_table_status_text =
        require_text(contract, kEh_HandlerTableStatus);
    auto cleanup_table_status_text =
        require_text(contract, kEh_CleanupTableStatus);
    auto frame_contract_ref = require_text(contract, kEh_FrameContractRef);
    auto stackmap_contract_ref = require_text(contract, kEh_StackmapContractRef);
    auto resume_contract_ref = require_text(contract, kEh_ResumeContractRef);
    auto verifier_rules_ref = require_text(contract, kEh_VerifierRulesRef);
    auto family_specific_unwind_surface_ref =
        require_text(contract, kEh_FamilySpecificUnwindSurfaceRef);

    if (!eh_contract_version) return parse_err(eh_contract_version.error());
    if (!executable_eh_status_text) {
        return parse_err(executable_eh_status_text.error());
    }
    if (!planned_executable_eh_epoch) {
        return parse_err(planned_executable_eh_epoch.error());
    }
    if (!cross_protected_frame_unwind_text) {
        return parse_err(cross_protected_frame_unwind_text.error());
    }
    if (!native_boundary_unwind_behavior_text) {
        return parse_err(native_boundary_unwind_behavior_text.error());
    }
    if (!handler_table_status_text) {
        return parse_err(handler_table_status_text.error());
    }
    if (!cleanup_table_status_text) {
        return parse_err(cleanup_table_status_text.error());
    }
    if (!frame_contract_ref) return parse_err(frame_contract_ref.error());
    if (!stackmap_contract_ref) return parse_err(stackmap_contract_ref.error());
    if (!resume_contract_ref) return parse_err(resume_contract_ref.error());
    if (!verifier_rules_ref) return parse_err(verifier_rules_ref.error());
    if (!family_specific_unwind_surface_ref) {
        return parse_err(family_specific_unwind_surface_ref.error());
    }

    auto executable_eh_status =
        parse_executable_eh_status(*executable_eh_status_text);
    auto cross_protected_frame_unwind =
        parse_cross_protected_frame_unwind(*cross_protected_frame_unwind_text);
    auto native_boundary_unwind_behavior =
        parse_native_boundary_unwind_behavior(
            *native_boundary_unwind_behavior_text);
    auto handler_table_status =
        parse_reserved_table_status(*handler_table_status_text);
    auto cleanup_table_status =
        parse_reserved_table_status(*cleanup_table_status_text);

    if (!executable_eh_status) return parse_err(executable_eh_status.error());
    if (!cross_protected_frame_unwind) {
        return parse_err(cross_protected_frame_unwind.error());
    }
    if (!native_boundary_unwind_behavior) {
        return parse_err(native_boundary_unwind_behavior.error());
    }
    if (!handler_table_status) return parse_err(handler_table_status.error());
    if (!cleanup_table_status) return parse_err(cleanup_table_status.error());

    ExceptionUnwindContract out;
    out.semantic_contract_version = std::move(*semantic_contract_version);
    out.eh_contract_version = std::move(*eh_contract_version);
    out.executable_eh_status = *executable_eh_status;
    out.planned_executable_eh_epoch = std::move(*planned_executable_eh_epoch);
    out.cross_protected_frame_unwind = *cross_protected_frame_unwind;
    out.native_boundary_unwind_behavior = *native_boundary_unwind_behavior;
    out.handler_table_status = *handler_table_status;
    out.cleanup_table_status = *cleanup_table_status;
    out.frame_contract_ref = std::move(*frame_contract_ref);
    out.stackmap_contract_ref = std::move(*stackmap_contract_ref);
    out.resume_contract_ref = std::move(*resume_contract_ref);
    out.verifier_rules_ref = std::move(*verifier_rules_ref);
    out.family_specific_unwind_surface_ref =
        std::move(*family_specific_unwind_surface_ref);
    return out;
}

tl::expected<ExceptionUnwindContract, ContractVerifyError>
verify_reserved_exception_unwind_contract(
    const std::uint8_t* data,
    std::size_t size,
    VMPilot::DomainLabels::FamilyId expected_family) noexcept {
    auto contract_or = parse_exception_unwind_contract(data, size);
    if (!contract_or) {
        if (contract_or.error() == ContractParseError::UnknownCriticalExtension) {
            return verify_err(ContractVerifyError::UnknownCriticalExtension);
        }
        return verify_err(ContractVerifyError::MalformedContract);
    }

    const ExceptionUnwindContract& contract = *contract_or;
    if (contract.executable_eh_status != ExecutableEhStatus::ReservedDisabledV1) {
        return verify_err(
            ContractVerifyError::ExecutableEhStatusNotReservedDisabled);
    }
    if (contract.cross_protected_frame_unwind !=
        CrossProtectedFrameUnwind::Forbidden) {
        return verify_err(ContractVerifyError::CrossProtectedFrameUnwindPermitted);
    }
    if (contract.native_boundary_unwind_behavior !=
        NativeBoundaryUnwindBehavior::TranslateToTrapOrFailClosed) {
        return verify_err(ContractVerifyError::NativeBoundaryBehaviorNotFailClosed);
    }
    if (contract.handler_table_status != ReservedTableStatus::ReservedEmpty) {
        return verify_err(ContractVerifyError::HandlerTableNotReservedEmpty);
    }
    if (contract.cleanup_table_status != ReservedTableStatus::ReservedEmpty) {
        return verify_err(ContractVerifyError::CleanupTableNotReservedEmpty);
    }
    if (contract.family_specific_unwind_surface_ref !=
        expected_family_specific_unwind_surface_ref(expected_family)) {
        return verify_err(ContractVerifyError::FamilySpecificUnwindSurfaceMismatch);
    }
    return std::move(*contract_or);
}

tl::expected<std::uint64_t, VMPilot::Common::DiagnosticCode>
guarded_platform_call(const PlatformCallDesc* desc,
                      bool returns_struct,
                      void* struct_return_ptr) noexcept {
#if defined(_WIN32)
    // This TU is compiled with /EHa (see runtime/CMakeLists.txt) so that
    // catch(...) intercepts both C++ throws and Windows SEH crossing the
    // platform_call trampoline. Either class becomes the fail-closed
    // NativeBoundaryUnwindTrapped diagnostic.
    try {
        return dispatch_platform_call(desc, returns_struct, struct_return_ptr);
    } catch (...) {
        return tl::make_unexpected(
            VMPilot::Common::DiagnosticCode::NativeBoundaryUnwindTrapped);
    }
#elif defined(__unix__) || defined(__APPLE__)
    struct sigaction old_segv;
    struct sigaction old_bus;
    struct sigaction old_ill;
    struct sigaction old_fpe;

    if (!install_signal_handler(SIGSEGV, old_segv) ||
        !install_signal_handler(SIGBUS, old_bus) ||
        !install_signal_handler(SIGILL, old_ill) ||
        !install_signal_handler(SIGFPE, old_fpe)) {
        return tl::make_unexpected(
            VMPilot::Common::DiagnosticCode::NativeBoundaryUnwindTrapped);
    }

    sigjmp_buf jump_env;
    g_guard_signal = 0;
    g_guard_jump_env = &jump_env;
    if (sigsetjmp(jump_env, 1) != 0) {
        g_guard_jump_env = nullptr;
        restore_signal_handler(SIGSEGV, old_segv);
        restore_signal_handler(SIGBUS, old_bus);
        restore_signal_handler(SIGILL, old_ill);
        restore_signal_handler(SIGFPE, old_fpe);
        return tl::make_unexpected(
            VMPilot::Common::DiagnosticCode::NativeBoundaryUnwindTrapped);
    }

    const std::uint64_t result =
        dispatch_platform_call(desc, returns_struct, struct_return_ptr);
    g_guard_jump_env = nullptr;

    restore_signal_handler(SIGSEGV, old_segv);
    restore_signal_handler(SIGBUS, old_bus);
    restore_signal_handler(SIGILL, old_ill);
    restore_signal_handler(SIGFPE, old_fpe);
    return result;
#else
    return dispatch_platform_call(desc, returns_struct, struct_return_ptr);
#endif
}

}  // namespace VMPilot::Runtime::EH