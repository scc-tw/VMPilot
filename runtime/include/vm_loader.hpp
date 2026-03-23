#pragma once
#ifndef __RUNTIME_VM_LOADER_HPP__
#define __RUNTIME_VM_LOADER_HPP__

#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <diagnostic.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <cstddef>
#include <vector>

namespace VMPilot::Runtime {

/// Load and validate a VM blob from raw bytes.
///
/// Parses the blob header, verifies magic/version, and extracts
/// pointers to bytecodes, constant pool, BB metadata, encoding tables, etc.
///
/// @param blob_data  pointer to the raw blob bytes
/// @param blob_size  size of the blob in bytes
/// @param config     security configuration
/// @return           initialized VMContext on success, DiagnosticCode on failure
[[nodiscard]] tl::expected<Common::VM::VMContext, Common::DiagnosticCode>
load_blob(const uint8_t* blob_data, size_t blob_size,
          const Common::VM::VmSecurityConfig& config) noexcept;

/// Initialize a VMContext with default values before blob loading.
///
/// Zeros all registers, sets up default state.
///
/// @param ctx  the VMContext to initialize
void init_context(Common::VM::VMContext& ctx) noexcept;

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_VM_LOADER_HPP__
