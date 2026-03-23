#pragma once
#ifndef __RUNTIME_ORAM_HPP__
#define __RUNTIME_ORAM_HPP__

#include <vm/vm_context.hpp>
#include <diagnostic.hpp>

#include <tl/expected.hpp>

#include <cstdint>

namespace VMPilot::Runtime {

/// Read 8 bytes from the oblivious workspace at the given offset.
///
/// Performs rolling keystream ORAM: decrypts the cache line, reads
/// the 8-byte word, then re-encrypts with a fresh keystream.
///
/// @param ctx     VMContext with oblivious_workspace, oram_key, oram_nonce
/// @param offset  byte offset within the workspace (must be 8-byte aligned)
/// @return        plaintext 64-bit value, or DiagnosticCode on error
[[nodiscard]] tl::expected<uint64_t, Common::DiagnosticCode>
oram_read(Common::VM::VMContext& ctx, uint32_t offset) noexcept;

/// Write 8 bytes to the oblivious workspace at the given offset.
///
/// @param ctx     VMContext with oblivious_workspace, oram_key, oram_nonce
/// @param offset  byte offset within the workspace (must be 8-byte aligned)
/// @param value   plaintext 64-bit value to write
/// @return        success or DiagnosticCode
[[nodiscard]] tl::expected<void, Common::DiagnosticCode>
oram_write(Common::VM::VMContext& ctx, uint32_t offset, uint64_t value) noexcept;

/// Push a 64-bit value onto the VM stack (in oblivious workspace).
///
/// Decrements vm_sp by 8, then writes the value.
///
/// @param ctx    VMContext
/// @param value  value to push
/// @return       success or DiagnosticCode (StackOverflow if workspace exhausted)
[[nodiscard]] tl::expected<void, Common::DiagnosticCode>
oram_push(Common::VM::VMContext& ctx, uint64_t value) noexcept;

/// Pop a 64-bit value from the VM stack (in oblivious workspace).
///
/// Reads the value at vm_sp, then increments vm_sp by 8.
///
/// @param ctx  VMContext
/// @return     popped value, or DiagnosticCode (StackUnderflow if empty)
[[nodiscard]] tl::expected<uint64_t, Common::DiagnosticCode>
oram_pop(Common::VM::VMContext& ctx) noexcept;

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_ORAM_HPP__
