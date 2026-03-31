#pragma once
#ifndef __RUNTIME_TLS_HELPERS_HPP__
#define __RUNTIME_TLS_HELPERS_HPP__

/// @file tls_helpers.hpp
/// @brief Thread-Local Storage access helpers for VM NATIVE_CALL.
///
/// When protected code accesses TLS variables (fs: on x86-64, gs: on
/// x86-32, TPIDR_EL0 on ARM64), the compiler emits NATIVE_CALL
/// instructions targeting these helpers.  The VM calls them through the
/// standard native call bridge.
///
/// These are plain C functions with extern "C" linkage so the compiler
/// can reference them by symbol name in TransitionEntry.
///
/// Architecture-specific implementation uses inline asm to access the
/// TLS base register directly — this cannot be expressed in portable C.

#include <cstdint>

extern "C" {

/// Read a 64-bit value from thread-local storage at the given offset.
///   x86-64:  mov rax, fs:[offset]
///   x86-32:  mov eax, gs:[offset]
///   ARM64:   mrs x1, TPIDR_EL0; ldr x0, [x1, offset]
uint64_t vmpilot_tls_read64(uint64_t offset) noexcept;

/// Write a 64-bit value to thread-local storage at the given offset.
///   x86-64:  mov fs:[offset], value
///   x86-32:  mov gs:[offset], value
///   ARM64:   mrs x1, TPIDR_EL0; str value, [x1, offset]
void vmpilot_tls_write64(uint64_t offset, uint64_t value) noexcept;

/// Read a 32-bit value from thread-local storage (zero-extended to 64-bit).
uint64_t vmpilot_tls_read32(uint64_t offset) noexcept;

/// Write a 32-bit value to thread-local storage.
void vmpilot_tls_write32(uint64_t offset, uint64_t value) noexcept;

/// Layer 2 fallback: same semantics as above, but implemented via
/// OS API (arch_prctl/NtCurrentTeb/pthread) + pointer arithmetic.
/// Always available for testing both layers produce identical results.
uint64_t vmpilot_tls_read64_fallback(uint64_t offset) noexcept;
void     vmpilot_tls_write64_fallback(uint64_t offset, uint64_t value) noexcept;
uint64_t vmpilot_tls_read32_fallback(uint64_t offset) noexcept;
void     vmpilot_tls_write32_fallback(uint64_t offset, uint64_t value) noexcept;

}  // extern "C"

#endif  // __RUNTIME_TLS_HELPERS_HPP__
