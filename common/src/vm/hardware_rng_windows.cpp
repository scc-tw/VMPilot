/// @file hardware_rng_windows.cpp
/// @brief Windows hardware RNG — covers x86, x86_64, and ARM64.
///
///   x86_64: RDRAND → BCryptGenRandom → zero
///   x86:    RDRAND (2×32-bit) → BCryptGenRandom → zero
///   ARM64:  BCryptGenRandom → zero
///           (Windows ARM64 lacks inline-asm support in MSVC;
///            RNDR intrinsics may be added in future MSVC versions.)

#if defined(_WIN32)

#include <vm/hardware_rng.hpp>
#include <cstdint>
#include <immintrin.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

namespace VMPilot::Common::VM {

uint64_t hardware_random_u64() noexcept {

    // ── Tier 1: Hardware instruction ────────────────────────────────
#if defined(_M_X64) || defined(__x86_64__)
    {
        uint64_t val = 0;
        for (int retries = 0; retries < 10; ++retries) {
            if (_rdrand64_step(reinterpret_cast<unsigned __int64*>(&val))) {
                return val;
            }
        }
    }
#elif defined(_M_IX86) || defined(__i386__)
    {
        uint32_t lo = 0, hi = 0;
        for (int retries = 0; retries < 10; ++retries) {
            if (_rdrand32_step(reinterpret_cast<unsigned int*>(&lo)) && 
                _rdrand32_step(reinterpret_cast<unsigned int*>(&hi))) {
                return (static_cast<uint64_t>(hi) << 32) | lo;
            }
        }
    }
#elif defined(_M_ARM64)
    // MSVC does not support inline assembly on ARM64.
    // When MSVC adds RNDR intrinsics, they can be used here.
    // For now, fall through to BCryptGenRandom.
#endif

    // ── Tier 2: BCryptGenRandom (kernel CSPRNG) ─────────────────────
    {
        uint64_t val = 0;
        BCryptGenRandom(nullptr, reinterpret_cast<PUCHAR>(&val),
                        sizeof(val), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
        return val;
    }

    // Tier 3 unreachable: BCryptGenRandom is always available on Windows.
}

}  // namespace VMPilot::Common::VM

#endif  // _WIN32