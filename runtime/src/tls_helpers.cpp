/// @file tls_helpers.cpp
/// @brief Architecture-specific TLS access via inline assembly.
///
/// These functions are called by the VM's NATIVE_CALL handler when
/// protected code accesses thread-local variables.  The compiler emits
/// NATIVE_CALL instructions referencing these symbols via TransitionEntry.
///
/// Security note: TLS access exposes the offset in a register (Class C
/// boundary, D15 S11.8).  The ephemeral transition encoding in
/// call_native_ephemeral() disguises the decode→call→encode pattern.

#include <tls_helpers.hpp>

// =========================================================================
// x86-64 (System V ABI): TLS via fs: segment register
// =========================================================================
#if defined(__x86_64__) || defined(_M_X64)

extern "C" uint64_t vmpilot_tls_read64(uint64_t offset) noexcept {
    uint64_t result;
    asm volatile("movq %%fs:(%1), %0"
                 : "=r"(result)
                 : "r"(offset)
                 : "memory");
    return result;
}

extern "C" void vmpilot_tls_write64(uint64_t offset, uint64_t value) noexcept {
    asm volatile("movq %0, %%fs:(%1)"
                 :
                 : "r"(value), "r"(offset)
                 : "memory");
}

extern "C" uint64_t vmpilot_tls_read32(uint64_t offset) noexcept {
    uint32_t result;
    asm volatile("movl %%fs:(%1), %0"
                 : "=r"(result)
                 : "r"(offset)
                 : "memory");
    return static_cast<uint64_t>(result);
}

extern "C" void vmpilot_tls_write32(uint64_t offset, uint64_t value) noexcept {
    auto val32 = static_cast<uint32_t>(value);
    asm volatile("movl %0, %%fs:(%1)"
                 :
                 : "r"(val32), "r"(offset)
                 : "memory");
}

// =========================================================================
// x86-32 (cdecl): TLS via gs: segment register
// =========================================================================
#elif defined(__i386__) || defined(_M_IX86)

extern "C" uint64_t vmpilot_tls_read64(uint64_t offset) noexcept {
    auto off32 = static_cast<uint32_t>(offset);
    uint32_t lo, hi;
    asm volatile("movl %%gs:(%2), %0\n\t"
                 "movl %%gs:4(%2), %1"
                 : "=r"(lo), "=r"(hi)
                 : "r"(off32)
                 : "memory");
    return static_cast<uint64_t>(hi) << 32 | lo;
}

extern "C" void vmpilot_tls_write64(uint64_t offset, uint64_t value) noexcept {
    auto off32 = static_cast<uint32_t>(offset);
    auto lo = static_cast<uint32_t>(value);
    auto hi = static_cast<uint32_t>(value >> 32);
    asm volatile("movl %0, %%gs:(%2)\n\t"
                 "movl %1, %%gs:4(%2)"
                 :
                 : "r"(lo), "r"(hi), "r"(off32)
                 : "memory");
}

extern "C" uint64_t vmpilot_tls_read32(uint64_t offset) noexcept {
    auto off32 = static_cast<uint32_t>(offset);
    uint32_t result;
    asm volatile("movl %%gs:(%1), %0"
                 : "=r"(result)
                 : "r"(off32)
                 : "memory");
    return static_cast<uint64_t>(result);
}

extern "C" void vmpilot_tls_write32(uint64_t offset, uint64_t value) noexcept {
    auto off32 = static_cast<uint32_t>(offset);
    auto val32 = static_cast<uint32_t>(value);
    asm volatile("movl %0, %%gs:(%1)"
                 :
                 : "r"(val32), "r"(off32)
                 : "memory");
}

// =========================================================================
// ARM64 (AAPCS64): TLS via TPIDR_EL0 system register
// =========================================================================
#elif defined(__aarch64__) || defined(_M_ARM64)

extern "C" uint64_t vmpilot_tls_read64(uint64_t offset) noexcept {
    uint64_t base;
    asm volatile("mrs %0, TPIDR_EL0" : "=r"(base));
    uint64_t result;
    asm volatile("ldr %0, [%1, %2]"
                 : "=r"(result)
                 : "r"(base), "r"(offset)
                 : "memory");
    return result;
}

extern "C" void vmpilot_tls_write64(uint64_t offset, uint64_t value) noexcept {
    uint64_t base;
    asm volatile("mrs %0, TPIDR_EL0" : "=r"(base));
    asm volatile("str %0, [%1, %2]"
                 :
                 : "r"(value), "r"(base), "r"(offset)
                 : "memory");
}

extern "C" uint64_t vmpilot_tls_read32(uint64_t offset) noexcept {
    uint64_t base;
    asm volatile("mrs %0, TPIDR_EL0" : "=r"(base));
    uint32_t result;
    asm volatile("ldr %w0, [%1, %2]"
                 : "=r"(result)
                 : "r"(base), "r"(offset)
                 : "memory");
    return static_cast<uint64_t>(result);
}

extern "C" void vmpilot_tls_write32(uint64_t offset, uint64_t value) noexcept {
    uint64_t base;
    asm volatile("mrs %0, TPIDR_EL0" : "=r"(base));
    auto val32 = static_cast<uint32_t>(value);
    asm volatile("str %w0, [%1, %2]"
                 :
                 : "r"(val32), "r"(base), "r"(offset)
                 : "memory");
}

// =========================================================================
// MSVC: Use intrinsics instead of inline asm
// =========================================================================
#elif defined(_MSC_VER) && defined(_M_X64)

#include <intrin.h>

extern "C" uint64_t vmpilot_tls_read64(uint64_t offset) noexcept {
    return __readgsqword(static_cast<unsigned long>(offset));
}

extern "C" void vmpilot_tls_write64(uint64_t offset, uint64_t value) noexcept {
    __writegsqword(static_cast<unsigned long>(offset), value);
}

extern "C" uint64_t vmpilot_tls_read32(uint64_t offset) noexcept {
    return __readgsdword(static_cast<unsigned long>(offset));
}

extern "C" void vmpilot_tls_write32(uint64_t offset, uint64_t value) noexcept {
    __writegsdword(static_cast<unsigned long>(offset),
                   static_cast<unsigned long>(value));
}

// =========================================================================
// Unsupported platform: compile error
// =========================================================================
#else
#error "TLS helpers not implemented for this platform"
#endif
