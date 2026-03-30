#pragma once
#ifndef __RUNTIME_VM_ENTRY_HPP__
#define __RUNTIME_VM_ENTRY_HPP__

/// @file vm_entry.hpp
/// @brief Phase 10: Public C++ API for VM entry/exit.
///
/// This is the top-level entry point that users (or the loader/patcher) call
/// to execute protected code. It orchestrates the full lifecycle:
///   1. Load and validate the blob (Phase 2)
///   2. Initialize VMContext with encoding tables (Phase 2)
///   3. Optionally set initial register values from native arguments
///   4. Run the 12-step uniform pipeline until HALT (Phase 7)
///   5. Decode the return value and return it as plaintext
///
/// v1: Entry is through these C++ functions directly.
/// v2: Assembly stubs (entry_exit/*.S) will be patched into the original
///     binary by the loader (Phase D1-D3 in doc 13).

#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <vm_loader.hpp>
#include <dispatcher.hpp>
#include <diagnostic.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <cstddef>

namespace VMPilot::Runtime {

/// Result of VM execution.
struct VmExecResult {
    VmResult status;           ///< how the VM terminated (Halted, Error, etc.)
    uint64_t return_value;     ///< plaintext return value (decoded from register 0)
};

/// Execute a protected region from a bytecode blob.
///
/// This is the primary entry point for the runtime VM. It:
///   1. Loads and validates the blob (Phase 2)
///   2. Initializes VMContext with encoding tables (Phase 2)
///   3. Runs the 12-step uniform pipeline until HALT (Phase 7)
///   4. Decodes register 0 and returns the plaintext result
///
/// The blob_data must remain valid for the lifetime of execution
/// (VMContext holds pointers into it for encrypted instructions and MACs).
///
/// stored_seed is the 32-byte root secret. It must be provided by the
/// caller (e.g., embedded by the loader, derived from a license key).
/// It is NEVER stored in the blob (spec S13.5).
///
/// @param blob_data        pointer to the raw blob bytes
/// @param blob_size        size of the blob in bytes
/// @param stored_seed      32-byte root secret (external input)
/// @param load_base_delta  PIE/ASLR delta: actual_base - static_base (D13§D3)
/// @param config           security configuration (or use defaults)
/// @return                 execution result, or DiagnosticCode on error
[[nodiscard]] tl::expected<VmExecResult, Common::DiagnosticCode>
vm_execute(const uint8_t* blob_data,
           size_t blob_size,
           const uint8_t stored_seed[32],
           int64_t load_base_delta = 0,
           const Common::VM::VmSecurityConfig& config = {}) noexcept;

/// Execute a protected region with pre-initialized register values.
///
/// Same as vm_execute but allows setting initial register values before
/// execution. This is used when the protected region receives arguments
/// from the native caller.
///
/// The initial_regs values are plaintext. They are encoded into the
/// register domain (D2) using the first BB's encoding tables before
/// execution begins.
///
/// @param blob_data      pointer to the raw blob bytes
/// @param blob_size      size of the blob in bytes
/// @param stored_seed    32-byte root secret
/// @param initial_regs   array of up to 16 plaintext register values to set
///                       before execution (will be encoded into register domain)
/// @param num_regs         number of initial registers to set (0-16)
/// @param load_base_delta  PIE/ASLR delta (D13§D3, RM§3.D3)
/// @param config           security configuration
/// @return                 execution result, or DiagnosticCode on error
[[nodiscard]] tl::expected<VmExecResult, Common::DiagnosticCode>
vm_execute_with_args(const uint8_t* blob_data,
                     size_t blob_size,
                     const uint8_t stored_seed[32],
                     const uint64_t* initial_regs,
                     uint8_t num_regs,
                     int64_t load_base_delta = 0,
                     const Common::VM::VmSecurityConfig& config = {}) noexcept;

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_VM_ENTRY_HPP__
