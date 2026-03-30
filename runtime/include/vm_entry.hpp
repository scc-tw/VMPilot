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

/// All parameters for a VM execution, in a single struct.
///
/// Struct-based API is extensible without breaking callers — new
/// fields get default values.
struct VmExecRequest {
    const uint8_t*  blob_data       = nullptr;
    std::size_t     blob_size       = 0;
    const uint8_t*  stored_seed     = nullptr;
    int64_t         load_base_delta = 0;
    const uint64_t* initial_regs    = nullptr;
    uint8_t         num_regs        = 0;
    Common::VM::VmSecurityConfig config = {};
};

/// Execute a protected region (struct-based API).
[[nodiscard]] tl::expected<VmExecResult, Common::DiagnosticCode>
vm_execute(const VmExecRequest& req) noexcept;

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_VM_ENTRY_HPP__
