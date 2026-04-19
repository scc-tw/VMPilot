#include "eh_guard.hpp"

#include <cstring>

#include "cbor/schema.hpp"
#include "cbor/strict.hpp"

namespace VMPilot::Cbor {
template <>
struct CborConsumerTraits<VMPilot::Runtime::EH::ContractParseError> {
    using E = VMPilot::Runtime::EH::ContractParseError;
    static constexpr E missing_field    = E::MissingField;
    static constexpr E wrong_field_type = E::WrongFieldType;
    static constexpr E bad_cbor         = E::BadCbor;
    static constexpr E not_a_map        = E::NotAMap;
    static constexpr E unknown_enum_value = E::UnknownEnumValue;
};
}  // namespace VMPilot::Cbor

#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <csetjmp>
#include <csignal>
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

inline auto require_text(const Value& m, std::uint64_t k) noexcept {
    return VMPilot::Cbor::require_text<ContractParseError>(m, k);
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

#if defined(_WIN32)
// ── Vectored Exception Handler (Windows) ────────────────────────────────
//
// Why VEH rather than __try/__except:
//   Frame-based structured handlers run LATE in the Windows exception
//   pipeline, AFTER MSVC /fsanitize=address has had a chance to
//   observe the fault through its own SEH shim. ASAN's shadow-memory
//   bookkeeping corrupts the unwind context while walking through the
//   ASM platform_call trampoline, so by the time __except at an
//   outer frame could run, the stack walk is already broken.
//
//   AddVectoredExceptionHandler(/*FirstHandler=*/1, ...) registers
//   BEFORE any frame-based handler and before ASAN's shim.
//
// Why CONTEXT restore rather than longjmp:
//   On Windows x64, std::longjmp is not a plain register restore — it
//   invokes RtlUnwindEx to unwind frames, which is the same path ASAN
//   corrupts. Returning EXCEPTION_CONTINUE_EXECUTION with a rewritten
//   CONTEXT bypasses the unwinder entirely; the CPU just jumps back
//   to the saved Rip/Rsp and never touches unwind metadata.
//
// TODO: add SEH fallback support only when it is really needed
//       (e.g. if we ever need to run C++ destructors during unwind;
//       CONTEXT-restore skips every intermediate frame's destructors,
//       at which point a frame-based __try/__except layer can be
//       stacked on top of VEH).

thread_local volatile LONG g_guard_active = 0;
thread_local volatile LONG g_guard_trapped = 0;
thread_local CONTEXT g_guard_resume_ctx{};

LONG NTAPI native_boundary_veh(EXCEPTION_POINTERS* info) noexcept {
    if (g_guard_active == 0) return EXCEPTION_CONTINUE_SEARCH;

    // Debug exceptions keep flowing through normal dispatch so the
    // runtime stays debuggable; everything else — access violation,
    // illegal instruction, divide-by-zero, stack overflow, C++ throw
    // (0xE06D7363), raw RaiseException — is treated as a native
    // boundary trap and collapsed to fail-closed.
    const DWORD code = info->ExceptionRecord->ExceptionCode;
    switch (code) {
        case EXCEPTION_BREAKPOINT:
        case EXCEPTION_SINGLE_STEP:
        case DBG_CONTROL_C:
            return EXCEPTION_CONTINUE_SEARCH;
        default:
            break;
    }

    // Rewrite the fault frame's CPU state so it resumes at the saved
    // resume point (captured by RtlCaptureContext inside
    // guarded_platform_call) and flag the trap so the caller returns
    // the fail-closed diagnostic.
    g_guard_trapped = 1;
    g_guard_active = 0;
    *(info->ContextRecord) = g_guard_resume_ctx;
    return EXCEPTION_CONTINUE_EXECUTION;
}

struct VehRegistration {
    PVOID handle{nullptr};
    VehRegistration() noexcept
        : handle(AddVectoredExceptionHandler(/*FirstHandler=*/1,
                                             &native_boundary_veh)) {}
    ~VehRegistration() {
        if (handle != nullptr) RemoveVectoredExceptionHandler(handle);
    }
    VehRegistration(const VehRegistration&) = delete;
    VehRegistration& operator=(const VehRegistration&) = delete;
};

void ensure_veh_registered() noexcept {
    static VehRegistration reg;
    (void)reg;
}
#endif  // _WIN32

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

    using namespace VMPilot::Cbor::Schema;
    const auto schema = std::make_tuple(
        TextField<ExceptionUnwindContract>{
            kEh_EhContractVersion,
            &ExceptionUnwindContract::eh_contract_version},
        EnumTextField<ExceptionUnwindContract, ExecutableEhStatus,
                      ContractParseError>{
            kEh_ExecutableEhStatus,
            &ExceptionUnwindContract::executable_eh_status,
            ContractParseError::UnknownEnumValue},
        TextField<ExceptionUnwindContract>{
            kEh_PlannedExecutableEhEpoch,
            &ExceptionUnwindContract::planned_executable_eh_epoch},
        EnumTextField<ExceptionUnwindContract, CrossProtectedFrameUnwind,
                      ContractParseError>{
            kEh_CrossProtectedFrameUnwind,
            &ExceptionUnwindContract::cross_protected_frame_unwind,
            ContractParseError::UnknownEnumValue},
        EnumTextField<ExceptionUnwindContract, NativeBoundaryUnwindBehavior,
                      ContractParseError>{
            kEh_NativeBoundaryUnwindBehavior,
            &ExceptionUnwindContract::native_boundary_unwind_behavior,
            ContractParseError::UnknownEnumValue},
        EnumTextField<ExceptionUnwindContract, ReservedTableStatus,
                      ContractParseError>{
            kEh_HandlerTableStatus,
            &ExceptionUnwindContract::handler_table_status,
            ContractParseError::UnknownEnumValue},
        EnumTextField<ExceptionUnwindContract, ReservedTableStatus,
                      ContractParseError>{
            kEh_CleanupTableStatus,
            &ExceptionUnwindContract::cleanup_table_status,
            ContractParseError::UnknownEnumValue},
        TextField<ExceptionUnwindContract>{
            kEh_FrameContractRef,
            &ExceptionUnwindContract::frame_contract_ref},
        TextField<ExceptionUnwindContract>{
            kEh_StackmapContractRef,
            &ExceptionUnwindContract::stackmap_contract_ref},
        TextField<ExceptionUnwindContract>{
            kEh_ResumeContractRef,
            &ExceptionUnwindContract::resume_contract_ref},
        TextField<ExceptionUnwindContract>{
            kEh_VerifierRulesRef,
            &ExceptionUnwindContract::verifier_rules_ref},
        TextField<ExceptionUnwindContract>{
            kEh_FamilySpecificUnwindSurfaceRef,
            &ExceptionUnwindContract::family_specific_unwind_surface_ref}
    );
    auto parsed =
        parse_schema<ExceptionUnwindContract, ContractParseError>(contract, schema);
    if (!parsed) return parse_err(parsed.error());
    parsed->semantic_contract_version = std::move(*semantic_contract_version);
    return parsed;
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
    // Must run before the first call so the VEH is registered while the
    // handler is still holding a live pointer to native_boundary_veh.
    ensure_veh_registered();

    g_guard_trapped = 0;
    // RtlCaptureContext snapshots the current CPU state such that a
    // later assignment into an EXCEPTION_POINTERS context record +
    // EXCEPTION_CONTINUE_EXECUTION resumes execution at the return
    // site of this very call. The VEH uses that property to "return"
    // from a faulted frame without running the Windows unwinder.
    RtlCaptureContext(&g_guard_resume_ctx);

    // Both the first entry and the post-fault resume land here; the
    // trap flag distinguishes the two paths. Volatile thread-locals
    // guarantee the load is not hoisted above RtlCaptureContext.
    if (g_guard_trapped != 0) {
        g_guard_trapped = 0;
        g_guard_active = 0;
        return tl::make_unexpected(
            VMPilot::Common::DiagnosticCode::NativeBoundaryUnwindTrapped);
    }

    g_guard_active = 1;
    const std::uint64_t result =
        dispatch_platform_call(desc, returns_struct, struct_return_ptr);
    g_guard_active = 0;
    return result;
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