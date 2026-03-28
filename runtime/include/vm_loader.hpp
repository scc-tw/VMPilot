#pragma once
#ifndef __RUNTIME_VM_LOADER_HPP__
#define __RUNTIME_VM_LOADER_HPP__

#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <vm/vm_blob.hpp>
#include <diagnostic.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <cstddef>
#include <vector>

namespace VMPilot::Runtime {

/// Owns heap-allocated data that VMContext points into.
///
/// VMContext is a POD-like struct whose pointers (constant_pool, bb_metadata)
/// must remain valid for the VM's lifetime. LoadedVM owns the backing
/// storage so callers get a self-contained object.
struct LoadedVM {
    Common::VM::VMContext ctx{};

    /// Decrypted constant pool (blob's pool section is encrypted).
    std::vector<uint8_t> decrypted_pool;

    /// Parsed BB metadata (deserialized from encrypted SerializedBBMeta).
    std::vector<Common::VM::BBMetadata> bb_meta_storage;
};

/// Load, validate, and decrypt a VM blob, producing a ready-to-execute VM.
///
/// The blob_data buffer must outlive the returned LoadedVM — ctx.bytecodes
/// and ctx.bb_macs point directly into it.
///
/// @param blob_data    pointer to the raw blob bytes (must remain valid)
/// @param blob_size    size of the blob in bytes
/// @param stored_seed  32-byte root secret shared with the compiler
/// @param config       security configuration
/// @return             initialized LoadedVM on success, DiagnosticCode on failure
[[nodiscard]] tl::expected<LoadedVM, Common::DiagnosticCode>
load_blob(const uint8_t* blob_data, size_t blob_size,
          const uint8_t stored_seed[32],
          const Common::VM::VmSecurityConfig& config) noexcept;

/// Initialize a VMContext with default values (zeros everything).
///
/// @param ctx  the VMContext to initialize
void init_context(Common::VM::VMContext& ctx) noexcept;

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_VM_LOADER_HPP__
