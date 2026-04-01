/// @file hardware_rng_linux.cpp
/// @brief Linux hardware RNG — covers x86, x86_64, and ARM64.
///
///   x86_64: RDRAND → /dev/urandom → zero
///   x86:    RDRAND (2×32-bit) → /dev/urandom → zero
///   ARM64:  RNDR → /dev/urandom → zero

#if defined(__linux__)

#include <vm/hardware_rng.hpp>
#include <cstdint>
#include <cstdio>

namespace VMPilot::Common::VM {

uint64_t hardware_random_u64() noexcept {

    // ── Tier 1: Hardware instruction ────────────────────────────────
#if defined(__x86_64__) || defined(_M_X64)
    {
        uint64_t val = 0;
        unsigned char ok = 0;
        for (int retries = 0; retries < 10 && !ok; ++retries)
            __asm__ volatile("rdrand %0; setc %1" : "=r"(val), "=q"(ok));
        if (ok) return val;
    }
#elif defined(__i386__) || defined(_M_IX86)
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
    // Linux kernel traps RNDR on cores without FEAT_RNG — safe to attempt.
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

    // ── Tier 2: /dev/urandom ────────────────────────────────────────
    {
        uint64_t val = 0;
        std::FILE* f = std::fopen("/dev/urandom", "rb");
        if (f) {
            auto n = std::fread(&val, sizeof(val), 1, f);
            std::fclose(f);
            if (n == 1) return val;
        }
    }

    // ── Tier 3: deterministic zero (test/emulator only) ─────────────
    return 0;
}

}  // namespace VMPilot::Common::VM

#endif  // __linux__
