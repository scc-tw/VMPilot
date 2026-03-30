#pragma once
#ifndef __RUNTIME_DISPATCHER_HPP__
#define __RUNTIME_DISPATCHER_HPP__

#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <diagnostic.hpp>

#include <tl/expected.hpp>

#include <cstdint>

namespace VMPilot::Runtime {

/// Result returned by the VM execution loop.
enum class VmResult : uint8_t {
    Running,        ///< instruction executed, continue
    Halted,         ///< HALT instruction reached
    Error,          ///< unrecoverable error
    Breakpoint,     ///< debug breakpoint (debug_mode only)
    NativeReturn,   ///< returned from native call
};

/// Execute the VM until halt or error.
///
/// Main dispatch loop: fetch-decrypt-decode, then dispatch to handler,
/// then handle BB transitions.
///
/// @param ctx     mutable VMContext
/// @param config  security configuration
/// @return        VmResult on success, DiagnosticCode on failure
[[nodiscard]] tl::expected<VmResult, Common::DiagnosticCode>
execute(Common::VM::VMContext& ctx,
        const Common::VM::VmSecurityConfig& config) noexcept;

/// Execute a single instruction (for debugging / single-stepping).
///
/// @param ctx     mutable VMContext
/// @param config  security configuration
/// @return        VmResult on success, DiagnosticCode on failure
[[nodiscard]] tl::expected<VmResult, Common::DiagnosticCode>
step(Common::VM::VMContext& ctx,
     const Common::VM::VmSecurityConfig& config) noexcept;

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_DISPATCHER_HPP__
