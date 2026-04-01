/// @file hardware_rng_darwin.cpp
/// @brief Darwin (macOS) hardware RNG — covers both x86_64 and ARM64.
///
///   x86_64: RDRAND → arc4random_buf → /dev/urandom → zero
///   ARM64:  RNDR (runtime-detected) → arc4random_buf → /dev/urandom → zero

#if defined(__APPLE__)

#include <vm/hardware_rng.hpp>
#include <cstdint>
#include <cstdio>
#include <stdlib.h>       // arc4random_buf — kernel CSPRNG, never fails

#if defined(__aarch64__) || defined(_M_ARM64)
#include <sys/sysctl.h>   // sysctlbyname — runtime FEAT_RNG detection
#endif

namespace VMPilot::Common::VM {

// ── ARM64-specific: runtime FEAT_RNG detection ──────────────────────
#if defined(__aarch64__) || defined(_M_ARM64)
namespace {
bool has_feat_rng() noexcept {
    static const bool supported = [] {
        int32_t val = 0;
        size_t len = sizeof(val);
        if (sysctlbyname("hw.optional.arm.FEAT_RNG", &val, &len,
                          nullptr, 0) == 0)
            return val != 0;
        return false;
    }();
    return supported;
}
}  // namespace
#endif

uint64_t hardware_random_u64() noexcept {

    // ── Tier 1: Hardware instruction ────────────────────────────────
#if defined(__x86_64__) || defined(_M_X64)
    // Intel Mac: RDRAND (available since Ivy Bridge 2012).
    {
        uint64_t val = 0;
        unsigned char ok = 0;
        for (int retries = 0; retries < 10 && !ok; ++retries)
            __asm__ volatile("rdrand %0; setc %1" : "=r"(val), "=q"(ok));
        if (ok) return val;
    }
#elif defined(__i386__)
    // x86-32 (unlikely on modern macOS, but complete for correctness).
    {
        uint32_t lo = 0, hi = 0;
        unsigned char ok_lo = 0, ok_hi = 0;
        for (int retries = 0; retries < 10 && !(ok_lo & ok_hi); ++retries) {
            __asm__ volatile("rdrand %0; setc %1" : "=r"(lo), "=q"(ok_lo));
            __asm__ volatile("rdrand %0; setc %1" : "=r"(hi), "=q"(ok_hi));
        }
        if (ok_lo & ok_hi)
            return (static_cast<uint64_t>(hi) << 32) | lo;
    }
#elif defined(__aarch64__) || defined(_M_ARM64)
    // Apple Silicon: current chips (M1-M4) lack FEAT_RNG; future chips may add it.
    if (has_feat_rng()) {
        uint64_t val = 0;
        uint64_t ok = 0;
        __asm__ volatile(
            "mrs %0, s3_3_c2_c4_0\n"   // RNDR → val
            "cset %1, ne\n"             // ok = (NZCV.Z == 0)
            : "=r"(val), "=r"(ok));
        if (ok) return val;
    }
#endif

    // ── Tier 2: arc4random_buf (kernel CSPRNG, never fails, no fd) ──
    {
        uint64_t val = 0;
        arc4random_buf(&val, sizeof(val));
        return val;
    }

    // Tiers 3-4 unreachable: arc4random_buf never fails on Darwin.
    // Kept as documentation of the full hierarchy:
    //   Tier 3: /dev/urandom
    //   Tier 4: deterministic zero
}

}  // namespace VMPilot::Common::VM

#endif  // __APPLE__
