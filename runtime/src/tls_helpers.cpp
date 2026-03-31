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
///            Fastest — zero function call overhead.
///   Layer 2: OS API to get segment base, then pointer arithmetic.
///            Slower (syscall/function call) but works if asm is
///            unavailable or the compiler doesn't support inline asm.
///   Layer 3: Compile error for unknown platforms.

#include <tls_helpers.hpp>
#include <cstring>

// =========================================================================
// OS detection
// =========================================================================
#if defined(__linux__) || defined(__FreeBSD__)
#define VMPILOT_OS_LINUX 1
#elif defined(__APPLE__)
#define VMPILOT_OS_DARWIN 1
#elif defined(_WIN32)
#define VMPILOT_OS_WINDOWS 1
#endif

// =========================================================================
// Fallback: get segment base via OS API, then pointer arithmetic.
//
// Each platform has a way to retrieve the segment base address as a
// regular pointer.  Once we have the base, tls_read(offset) is just
// *(base + offset) — no inline asm needed.
//
// This is Layer 2: slower than direct segment access (Layer 1) because
// it involves a syscall or function call, but it's always available.
// =========================================================================

namespace {

#if defined(VMPILOT_OS_LINUX) && defined(__x86_64__)
// Linux x86_64: arch_prctl(ARCH_GET_FS) returns the fs: base.
#include <asm/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>

inline uint8_t* get_segment_base_fallback() noexcept {
    uint64_t base = 0;
    syscall(SYS_arch_prctl, ARCH_GET_FS, &base);
    return reinterpret_cast<uint8_t*>(base);
}

#elif defined(VMPILOT_OS_LINUX) && defined(__i386__)
// Linux x86_32: get_thread_area or arch_prctl isn't clean on 32-bit.
// Fallback: read gs base via gs:[0] which on Linux points to the TCB
// that contains a self-pointer at offset 0 for most glibc versions.
// This is a pragmatic fallback — the TCB self-pointer is a de facto ABI.
inline uint8_t* get_segment_base_fallback() noexcept {
    uint32_t base;
    asm volatile("movl %%gs:0, %0" : "=r"(base));
    return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(base));
}

#elif (defined(VMPILOT_OS_LINUX) || defined(VMPILOT_OS_DARWIN)) && defined(__aarch64__)
// ARM64: TPIDR_EL0 is the thread pointer — readable via mrs instruction.
// The "fallback" is the same instruction since there's no alternative.
inline uint8_t* get_segment_base_fallback() noexcept {
    uint64_t base;
    asm volatile("mrs %0, TPIDR_EL0" : "=r"(base));
    return reinterpret_cast<uint8_t*>(base);
}

#elif defined(VMPILOT_OS_DARWIN) && defined(__x86_64__)
// macOS x86_64 (Intel, discontinued but may still run in CI).
// macOS doesn't expose fs:/gs: for user TLS.  Use pthread_self()
// which returns the thread struct base — TLS lives at negative offsets.
#include <pthread.h>
inline uint8_t* get_segment_base_fallback() noexcept {
    return reinterpret_cast<uint8_t*>(pthread_self());
}

#elif defined(VMPILOT_OS_WINDOWS)
// Windows: NtCurrentTeb() returns the TEB pointer (same as gs:0 on x64,
// fs:0x18 on x86).  Available in all Windows versions via <winnt.h>.
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winnt.h>

inline uint8_t* get_segment_base_fallback() noexcept {
    return reinterpret_cast<uint8_t*>(NtCurrentTeb());
}

#endif

// Layer 2 implementation: pointer arithmetic on segment base.
inline uint64_t fallback_read64(uint64_t offset) noexcept {
    uint64_t result;
    std::memcpy(&result, get_segment_base_fallback() + offset, 8);
    return result;
}

inline void fallback_write64(uint64_t offset, uint64_t value) noexcept {
    std::memcpy(get_segment_base_fallback() + offset, &value, 8);
}

inline uint64_t fallback_read32(uint64_t offset) noexcept {
    uint32_t result;
    std::memcpy(&result, get_segment_base_fallback() + offset, 4);
    return static_cast<uint64_t>(result);
}

inline void fallback_write32(uint64_t offset, uint64_t value) noexcept {
    auto v = static_cast<uint32_t>(value);
    std::memcpy(get_segment_base_fallback() + offset, &v, 4);
}

}  // namespace

// =========================================================================
// Layer 1: Linux x86_64 — TLS via fs: segment register
// =========================================================================
#if defined(VMPILOT_OS_LINUX) && defined(__x86_64__) && !defined(VMPILOT_TLS_FORCE_FALLBACK)

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
#elif defined(VMPILOT_OS_LINUX) && defined(__i386__) && !defined(VMPILOT_TLS_FORCE_FALLBACK)

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
// Layer 1: Linux/macOS ARM64 — TLS via TPIDR_EL0
// =========================================================================
#elif (defined(VMPILOT_OS_LINUX) || defined(VMPILOT_OS_DARWIN)) && defined(__aarch64__) && !defined(VMPILOT_TLS_FORCE_FALLBACK)

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
// Layer 1: Windows x64 (MSVC) — TEB via gs: intrinsics
// =========================================================================
#elif defined(VMPILOT_OS_WINDOWS) && defined(_MSC_VER) && defined(_M_X64) && !defined(VMPILOT_TLS_FORCE_FALLBACK)

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
// Layer 1: Windows x86 (MSVC) — TEB via fs: intrinsics
// =========================================================================
#elif defined(VMPILOT_OS_WINDOWS) && defined(_MSC_VER) && defined(_M_IX86) && !defined(VMPILOT_TLS_FORCE_FALLBACK)

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
// Layer 1: Windows x64 (MinGW/Clang) — TEB via gs: inline asm
// =========================================================================
#elif defined(VMPILOT_OS_WINDOWS) && defined(__x86_64__) && !defined(VMPILOT_TLS_FORCE_FALLBACK)

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
// Layer 1: Windows x86 (MinGW/Clang) — TEB via fs: inline asm
// =========================================================================
#elif defined(VMPILOT_OS_WINDOWS) && defined(__i386__) && !defined(VMPILOT_TLS_FORCE_FALLBACK)

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
// Layer 2: All platforms — fallback via get_segment_base + pointer math
//
// Reached when:
//   - VMPILOT_TLS_FORCE_FALLBACK is defined (for testing)
//   - macOS x86_64 (no segment register user TLS)
//   - Any future platform without inline asm support
// =========================================================================
#elif defined(VMPILOT_OS_LINUX) || defined(VMPILOT_OS_DARWIN) || defined(VMPILOT_OS_WINDOWS)

extern "C" uint64_t vmpilot_tls_read64(uint64_t offset) noexcept {
    return fallback_read64(offset);
}

extern "C" void vmpilot_tls_write64(uint64_t offset, uint64_t value) noexcept {
    fallback_write64(offset, value);
}

extern "C" uint64_t vmpilot_tls_read32(uint64_t offset) noexcept {
    return fallback_read32(offset);
}

extern "C" void vmpilot_tls_write32(uint64_t offset, uint64_t value) noexcept {
    fallback_write32(offset, value);
}

// =========================================================================
// Layer 3: Unsupported platform
// =========================================================================
#else
#error "TLS/TEB helpers: unsupported OS+arch combination. " \
       "Supported: Linux (x86_64/x86_32/ARM64), macOS (ARM64/x86_64), " \
       "Windows (x64/x86 via MSVC or MinGW)"
#endif

// =========================================================================
// Layer 2 direct access for testing: vmpilot_tls_read64_fallback etc.
// Always compiled (on supported platforms) so tests can verify both
// layers produce the same result.
// =========================================================================
#if defined(VMPILOT_OS_LINUX) || defined(VMPILOT_OS_DARWIN) || defined(VMPILOT_OS_WINDOWS)

extern "C" uint64_t vmpilot_tls_read64_fallback(uint64_t offset) noexcept {
    return fallback_read64(offset);
}

extern "C" void vmpilot_tls_write64_fallback(uint64_t offset, uint64_t value) noexcept {
    fallback_write64(offset, value);
}

extern "C" uint64_t vmpilot_tls_read32_fallback(uint64_t offset) noexcept {
    return fallback_read32(offset);
}

extern "C" void vmpilot_tls_write32_fallback(uint64_t offset, uint64_t value) noexcept {
    fallback_write32(offset, value);
}

#endif
