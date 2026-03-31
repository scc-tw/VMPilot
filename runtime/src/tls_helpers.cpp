/// @file tls_helpers.cpp
/// @brief Segment-register-relative memory access for TLS/TEB.
///
/// The compiler converts guest `fs:[offset]` / `gs:[offset]` instructions
/// into `NATIVE_CALL tls_read(offset)`.  These helpers replicate the
/// original segment-relative access so the VM produces the same result.
///
/// Linux and Windows use OPPOSITE segment registers for the same arch:
///   Linux  x86_64: fs: → TCB     |  Windows x64:  gs: → TEB
///   Linux  x86_32: gs: → TCB     |  Windows x86:  fs: → TEB
///
/// Multi-layer fallback per platform:
///   Layer 1: Direct segment register access (inline asm / intrinsics)
///   Layer 2: OS API (pthread / Win32 TLS) — slower but always works
///   Layer 3: Compile error for unknown platforms

#include <tls_helpers.hpp>

// =========================================================================
// Detect OS — needed because Linux and Windows use opposite segment regs
// =========================================================================
#if defined(__linux__) || defined(__FreeBSD__)
#define VMPILOT_OS_LINUX 1
#elif defined(__APPLE__)
#define VMPILOT_OS_DARWIN 1
#elif defined(_WIN32)
#define VMPILOT_OS_WINDOWS 1
#endif

// =========================================================================
// Layer 1: Linux x86_64 — TLS via fs: segment register
// =========================================================================
#if defined(VMPILOT_OS_LINUX) && defined(__x86_64__)

extern "C" uint64_t vmpilot_tls_read64(uint64_t offset) noexcept {
    uint64_t result;
    asm volatile("movq %%fs:(%1), %0"
                 : "=r"(result) : "r"(offset) : "memory");
    return result;
}

extern "C" void vmpilot_tls_write64(uint64_t offset, uint64_t value) noexcept {
    asm volatile("movq %0, %%fs:(%1)"
                 : : "r"(value), "r"(offset) : "memory");
}

extern "C" uint64_t vmpilot_tls_read32(uint64_t offset) noexcept {
    uint32_t result;
    asm volatile("movl %%fs:(%1), %0"
                 : "=r"(result) : "r"(offset) : "memory");
    return static_cast<uint64_t>(result);
}

extern "C" void vmpilot_tls_write32(uint64_t offset, uint64_t value) noexcept {
    auto v = static_cast<uint32_t>(value);
    asm volatile("movl %0, %%fs:(%1)"
                 : : "r"(v), "r"(offset) : "memory");
}

// =========================================================================
// Layer 1: Linux x86_32 — TLS via gs: segment register
// =========================================================================
#elif defined(VMPILOT_OS_LINUX) && defined(__i386__)

extern "C" uint64_t vmpilot_tls_read64(uint64_t offset) noexcept {
    auto off32 = static_cast<uint32_t>(offset);
    uint32_t lo, hi;
    asm volatile("movl %%gs:(%2), %0\n\t"
                 "movl %%gs:4(%2), %1"
                 : "=r"(lo), "=r"(hi) : "r"(off32) : "memory");
    return static_cast<uint64_t>(hi) << 32 | lo;
}

extern "C" void vmpilot_tls_write64(uint64_t offset, uint64_t value) noexcept {
    auto off32 = static_cast<uint32_t>(offset);
    auto lo = static_cast<uint32_t>(value);
    auto hi = static_cast<uint32_t>(value >> 32);
    asm volatile("movl %0, %%gs:(%2)\n\t"
                 "movl %1, %%gs:4(%2)"
                 : : "r"(lo), "r"(hi), "r"(off32) : "memory");
}

extern "C" uint64_t vmpilot_tls_read32(uint64_t offset) noexcept {
    auto off32 = static_cast<uint32_t>(offset);
    uint32_t result;
    asm volatile("movl %%gs:(%1), %0"
                 : "=r"(result) : "r"(off32) : "memory");
    return static_cast<uint64_t>(result);
}

extern "C" void vmpilot_tls_write32(uint64_t offset, uint64_t value) noexcept {
    auto off32 = static_cast<uint32_t>(offset);
    auto v = static_cast<uint32_t>(value);
    asm volatile("movl %0, %%gs:(%1)"
                 : : "r"(v), "r"(off32) : "memory");
}

// =========================================================================
// Layer 1: Linux/macOS ARM64 — TLS via TPIDR_EL0 system register
// =========================================================================
#elif (defined(VMPILOT_OS_LINUX) || defined(VMPILOT_OS_DARWIN)) && defined(__aarch64__)

extern "C" uint64_t vmpilot_tls_read64(uint64_t offset) noexcept {
    uint64_t base;
    asm volatile("mrs %0, TPIDR_EL0" : "=r"(base));
    uint64_t result;
    asm volatile("ldr %0, [%1, %2]"
                 : "=r"(result) : "r"(base), "r"(offset) : "memory");
    return result;
}

extern "C" void vmpilot_tls_write64(uint64_t offset, uint64_t value) noexcept {
    uint64_t base;
    asm volatile("mrs %0, TPIDR_EL0" : "=r"(base));
    asm volatile("str %0, [%1, %2]"
                 : : "r"(value), "r"(base), "r"(offset) : "memory");
}

extern "C" uint64_t vmpilot_tls_read32(uint64_t offset) noexcept {
    uint64_t base;
    asm volatile("mrs %0, TPIDR_EL0" : "=r"(base));
    uint32_t result;
    asm volatile("ldr %w0, [%1, %2]"
                 : "=r"(result) : "r"(base), "r"(offset) : "memory");
    return static_cast<uint64_t>(result);
}

extern "C" void vmpilot_tls_write32(uint64_t offset, uint64_t value) noexcept {
    uint64_t base;
    asm volatile("mrs %0, TPIDR_EL0" : "=r"(base));
    auto v = static_cast<uint32_t>(value);
    asm volatile("str %w0, [%1, %2]"
                 : : "r"(v), "r"(base), "r"(offset) : "memory");
}

// =========================================================================
// Layer 1: Windows x64 (MSVC) — TEB via gs: segment register
//
// Windows x64 uses gs: for the Thread Environment Block.  This is the
// OPPOSITE of Linux x64 which uses fs:.  Guest code on Windows accesses
// gs:[offset] for TEB fields; our helper replicates that access.
// =========================================================================
#elif defined(VMPILOT_OS_WINDOWS) && defined(_MSC_VER) && defined(_M_X64)

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
// Layer 1: Windows x86 (MSVC) — TEB via fs: segment register
//
// Windows x86 (including WoW64) uses fs: for the 32-bit TEB.
// This is the OPPOSITE of Linux x86 which uses gs:.
// =========================================================================
#elif defined(VMPILOT_OS_WINDOWS) && defined(_MSC_VER) && defined(_M_IX86)

#include <intrin.h>

extern "C" uint64_t vmpilot_tls_read64(uint64_t offset) noexcept {
    auto off32 = static_cast<unsigned long>(offset);
    uint32_t lo = __readfsdword(off32);
    uint32_t hi = __readfsdword(off32 + 4);
    return static_cast<uint64_t>(hi) << 32 | lo;
}

extern "C" void vmpilot_tls_write64(uint64_t offset, uint64_t value) noexcept {
    auto off32 = static_cast<unsigned long>(offset);
    __writefsdword(off32,     static_cast<unsigned long>(value));
    __writefsdword(off32 + 4, static_cast<unsigned long>(value >> 32));
}

extern "C" uint64_t vmpilot_tls_read32(uint64_t offset) noexcept {
    return __readfsdword(static_cast<unsigned long>(offset));
}

extern "C" void vmpilot_tls_write32(uint64_t offset, uint64_t value) noexcept {
    __writefsdword(static_cast<unsigned long>(offset),
                   static_cast<unsigned long>(value));
}

// =========================================================================
// Layer 1: Windows x64 (MinGW/Clang) — TEB via gs: using inline asm
//
// MinGW defines __x86_64__ but uses Windows ABI (gs: for TEB, not fs:).
// Must come AFTER the Linux checks to avoid matching Linux+GCC.
// =========================================================================
#elif defined(VMPILOT_OS_WINDOWS) && defined(__x86_64__)

extern "C" uint64_t vmpilot_tls_read64(uint64_t offset) noexcept {
    uint64_t result;
    asm volatile("movq %%gs:(%1), %0"
                 : "=r"(result) : "r"(offset) : "memory");
    return result;
}

extern "C" void vmpilot_tls_write64(uint64_t offset, uint64_t value) noexcept {
    asm volatile("movq %0, %%gs:(%1)"
                 : : "r"(value), "r"(offset) : "memory");
}

extern "C" uint64_t vmpilot_tls_read32(uint64_t offset) noexcept {
    uint32_t result;
    asm volatile("movl %%gs:(%1), %0"
                 : "=r"(result) : "r"(offset) : "memory");
    return static_cast<uint64_t>(result);
}

extern "C" void vmpilot_tls_write32(uint64_t offset, uint64_t value) noexcept {
    auto v = static_cast<uint32_t>(value);
    asm volatile("movl %0, %%gs:(%1)"
                 : : "r"(v), "r"(offset) : "memory");
}

// =========================================================================
// Layer 1: Windows x86 (MinGW/Clang) — TEB via fs: using inline asm
// =========================================================================
#elif defined(VMPILOT_OS_WINDOWS) && defined(__i386__)

extern "C" uint64_t vmpilot_tls_read64(uint64_t offset) noexcept {
    auto off32 = static_cast<uint32_t>(offset);
    uint32_t lo, hi;
    asm volatile("movl %%fs:(%2), %0\n\t"
                 "movl %%fs:4(%2), %1"
                 : "=r"(lo), "=r"(hi) : "r"(off32) : "memory");
    return static_cast<uint64_t>(hi) << 32 | lo;
}

extern "C" void vmpilot_tls_write64(uint64_t offset, uint64_t value) noexcept {
    auto off32 = static_cast<uint32_t>(offset);
    auto lo = static_cast<uint32_t>(value);
    auto hi = static_cast<uint32_t>(value >> 32);
    asm volatile("movl %0, %%fs:(%2)\n\t"
                 "movl %1, %%fs:4(%2)"
                 : : "r"(lo), "r"(hi), "r"(off32) : "memory");
}

extern "C" uint64_t vmpilot_tls_read32(uint64_t offset) noexcept {
    auto off32 = static_cast<uint32_t>(offset);
    uint32_t result;
    asm volatile("movl %%fs:(%1), %0"
                 : "=r"(result) : "r"(off32) : "memory");
    return static_cast<uint64_t>(result);
}

extern "C" void vmpilot_tls_write32(uint64_t offset, uint64_t value) noexcept {
    auto off32 = static_cast<uint32_t>(offset);
    auto v = static_cast<uint32_t>(value);
    asm volatile("movl %0, %%fs:(%1)"
                 : : "r"(v), "r"(off32) : "memory");
}

// =========================================================================
// Unsupported platform: compile error
// =========================================================================
#else
#error "TLS/TEB helpers: unsupported OS+arch combination. " \
       "Supported: Linux (x86_64/x86_32/ARM64), macOS (ARM64), " \
       "Windows (x64/x86 via MSVC or MinGW)"
#endif
