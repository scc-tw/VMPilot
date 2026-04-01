#pragma once
#ifndef __COMMON_VM_SECURE_ZERO_HPP__
#define __COMMON_VM_SECURE_ZERO_HPP__

/// @file secure_zero.hpp
/// @brief Secure memory erasure — guaranteed not optimised away by the compiler.
///
/// WHY THIS EXISTS:
///
///   Standard `memset(ptr, 0, len)` can be removed by the compiler as a
///   "dead store" if the buffer is not read again before going out of scope.
///   This is a well-documented pitfall (CWE-14, MSC06-C) that leaves
///   sensitive data (keys, plaintexts) in memory for attackers to dump.
///
///   Doc 16 (Forward-Secrecy Extension, rev.8) mandates that ~650 bytes of
///   per-instruction temporaries (round keys, tweaks, plaintext operands,
///   fingerprints) are zeroed after every instruction.  Without a guaranteed
///   erasure primitive, the compiler could elide these zeroes and leave the
///   entire forward-secrecy chain's intermediate state recoverable from
///   a single memory dump.
///
/// PLATFORM DISPATCH:
///
///   Linux/macOS/FreeBSD: explicit_bzero() — POSIX, guaranteed not elided.
///   Windows (MSVC):      SecureZeroMemory() — compiler intrinsic.
///   Fallback:            volatile function pointer trick (see below).
///
/// SecureLocal<T> RAII WRAPPER:
///
///   Declares a local variable that is automatically zeroed on destruction.
///   Usage: SecureLocal<uint32_t[27]> round_keys;
///          // use round_keys.val as uint32_t[27]
///          // automatically zeroed when scope exits

#include <cstddef>
#include <cstring>

namespace VMPilot::Common::VM {

/// Securely zero `len` bytes at `ptr`.  Guaranteed not optimised away.
inline void secure_zero(void* ptr, size_t len) noexcept {
    if (!ptr || len == 0) return;

#if defined(_WIN32) && defined(_MSC_VER)
    // MSVC: SecureZeroMemory is a compiler intrinsic that cannot be elided.
    SecureZeroMemory(ptr, len);

#elif defined(__STDC_LIB_EXT1__) && __STDC_LIB_EXT1__ >= 201112L
    // C11 Annex K: memset_s is guaranteed not to be optimised away.
    memset_s(ptr, len, 0, len);

#elif (defined(__GNUC__) || defined(__clang__)) && !defined(__APPLE__)
    // GCC/Clang/MinGW on Linux/FreeBSD: explicit_bzero is available since
    // glibc 2.25, musl 1.1.20, FreeBSD 11 (all ≥2017).
    // Apple platforms excluded: <strings.h> needed for declaration, and
    // the volatile fallback below is equally safe on macOS/iOS.
    explicit_bzero(ptr, len);

#else
    // FALLBACK: volatile function pointer prevents dead-store elimination.
    //
    // WHY this works: the compiler cannot prove that `volatile_memset` points
    // to a function with no side effects, so it must emit the call.  The
    // `volatile` qualifier on the pointer itself (not the target) prevents
    // the compiler from caching the function address or deducing its identity.
    static void* (* volatile volatile_memset)(void*, int, size_t) = std::memset;
    volatile_memset(ptr, 0, len);
#endif
}

/// RAII wrapper that zeros its contents on destruction.
///
/// WHY RAII instead of manual secure_zero():
///   Manual calls are easy to forget, especially on early-return or exception
///   paths.  SecureLocal guarantees cleanup regardless of control flow.
///
/// Usage:
///   SecureLocal<uint32_t[27]> rk;
///   Speck64_KeySchedule(key, rk.val);  // use rk.val directly
///   // rk is automatically zeroed when scope exits
template<typename T>
struct SecureLocal {
    T val;

    SecureLocal() noexcept { std::memset(&val, 0, sizeof(T)); }
    ~SecureLocal() noexcept { secure_zero(&val, sizeof(T)); }

    // Non-copyable, non-movable — locals only.
    SecureLocal(const SecureLocal&) = delete;
    SecureLocal& operator=(const SecureLocal&) = delete;
    SecureLocal(SecureLocal&&) = delete;
    SecureLocal& operator=(SecureLocal&&) = delete;
};

}  // namespace VMPilot::Common::VM

#endif  // __COMMON_VM_SECURE_ZERO_HPP__
