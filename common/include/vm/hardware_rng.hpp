#pragma once
#ifndef __COMMON_VM_HARDWARE_RNG_HPP__
#define __COMMON_VM_HARDWARE_RNG_HPP__

/// @file hardware_rng.hpp
/// @brief Hardware random number generation for per-execution nonces.
///
/// WHY HARDWARE RNG:
///
///   Doc 16's forward-secrecy chain requires a per-execution nonce in
///   bb_chain_state[0].  If this nonce is deterministic (e.g., zero), then
///   two executions of the same blob with the same seed produce identical
///   FPE keys — an attacker who captures one execution's register values
///   can decode all future executions.
///
///   RDRAND (x86) / RNDR (ARM64) provides a hardware-backed, non-deterministic
///   64-bit random value that differs across executions.  This makes bb_chain_state
///   (and therefore all FPE keys) unique per execution, even with the same blob+seed.
///
/// FALLBACK HIERARCHY:
///
///   1. RDRAND/RNDR instruction (hardware, ~100 cycles)
///   2. /dev/urandom (Linux/macOS, ~1μs)
///   3. BCryptGenRandom (Windows, ~1μs)
///   4. Constant zero (compile-time fallback, deterministic — test-only)

#include <cstdint>
#include <cstdio>   // FILE*, fopen, fread, fclose — portable POSIX fallback

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#endif

namespace VMPilot::Common::VM {

/// Generate a 64-bit hardware random value.
///
/// Thread-safe: RDRAND/RNDR are per-core, no shared state.
/// Not suitable for key material — use BLAKE3 KDF for that.
inline uint64_t hardware_random_u64() noexcept {

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
    // x86: RDRAND with retry loop (Intel/AMD, available since Ivy Bridge 2012).
    // Retry because the entropy pool can transiently deplete under heavy load.
    uint64_t val = 0;
    unsigned char ok = 0;
    for (int retries = 0; retries < 10 && !ok; ++retries) {
#if defined(__x86_64__) || defined(_M_X64)
        // "=q" (not "=qm"): force setc to a register to avoid ASan
        // conflicts with inline-asm memory operands.
        __asm__ volatile("rdrand %0; setc %1" : "=r"(val), "=q"(ok));
#else
        // x86-32: RDRAND yields 32-bit values, combine two
        uint32_t lo = 0, hi = 0;
        unsigned char ok_lo = 0, ok_hi = 0;
        __asm__ volatile("rdrand %0; setc %1" : "=r"(lo), "=q"(ok_lo));
        __asm__ volatile("rdrand %0; setc %1" : "=r"(hi), "=q"(ok_hi));
        ok = ok_lo & ok_hi;
        val = (static_cast<uint64_t>(hi) << 32) | lo;
#endif
    }
    if (ok) return val;

#elif defined(__aarch64__) || defined(_M_ARM64)
    // ARM64: MRS RNDR (requires FEAT_RNG, ARMv8.5+).
    // Apple Silicon does not implement FEAT_RNG — skip to OS fallback.
#if !defined(__APPLE__)
    {
        uint64_t val = 0;
        uint64_t ok = 0;
        __asm__ volatile(
            "mrs %0, s3_3_c2_c4_0\n"   // RNDR → val
            "cset %1, ne\n"             // ok = (NZCV.Z == 0)
            : "=r"(val), "=r"(ok));
        if (ok) return val;
    }
#endif
#endif

    // OS fallback when hardware instruction unavailable or failed
#if defined(_WIN32)
    {
        uint64_t val = 0;
        BCryptGenRandom(nullptr, reinterpret_cast<PUCHAR>(&val),
                        sizeof(val), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
        return val;
    }
#elif defined(__linux__) || defined(__APPLE__) || defined(__FreeBSD__)
    {
        uint64_t val = 0;
        // fopen/fread is portable across all POSIX and avoids raw syscall deps.
        std::FILE* f = std::fopen("/dev/urandom", "rb");
        if (f) {
            auto n = std::fread(&val, sizeof(val), 1, f);
            std::fclose(f);
            if (n == 1) return val;
            // fread failed (I/O error) — fall through to deterministic fallback
        }
    }
#endif

    // Final fallback: deterministic zero (test/emulator environments only).
    //
    // WARNING: This makes bb_chain_state deterministic across executions,
    // breaking per-execution independence (Theorem 7.4).  In production
    // this path should never be reached — all supported platforms have
    // RDRAND (x86), RNDR (ARM64), BCryptGenRandom (Windows), or /dev/urandom.
    return 0;
}

}  // namespace VMPilot::Common::VM

#endif  // __COMMON_VM_HARDWARE_RNG_HPP__
