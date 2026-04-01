/// @file platform_call_fallback.cpp
/// @brief Generic fallback for platform_call (integer args only).
///
/// Used on platforms without a dedicated ASM trampoline (Windows x86,
/// exotic architectures).  FP args are passed as integer bit-patterns
/// which is ABI-incorrect for FP-typed parameters but works for
/// integer-only native calls (the vast majority of protected code).

#include "platform_call.hpp"

// Only compile if no ASM trampoline is available for this platform.
#if !defined(__x86_64__) || defined(_WIN32)
#if !defined(__aarch64__)

extern "C"
uint64_t platform_call(
    void* target,
    const uint64_t* int_regs,
    const uint64_t* fp_regs,
    uint8_t flags) noexcept
{
    (void)fp_regs;
    (void)flags;

    // Integer-only fallback: pass all args as uint64_t.
    // FP args will be in the wrong registers (int instead of xmm/d).
    using Fn = uint64_t(*)(uint64_t,uint64_t,uint64_t,uint64_t,
                            uint64_t,uint64_t,uint64_t,uint64_t);
    auto fn = reinterpret_cast<Fn>(target);
    return fn(int_regs[0], int_regs[1], int_regs[2], int_regs[3],
              int_regs[4], int_regs[5], int_regs[6], int_regs[7]);
}

#endif  // !__aarch64__
#endif  // !__x86_64__ || _WIN32
