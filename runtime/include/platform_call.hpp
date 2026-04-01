#pragma once
#ifndef __RUNTIME_PLATFORM_CALL_HPP__
#define __RUNTIME_PLATFORM_CALL_HPP__

/// @file platform_call.hpp
/// @brief ABI-correct native function call trampoline.
///
/// Handles integer AND floating-point arguments via platform-specific
/// assembly that places args in the correct registers:
///
///   x86-64 System V: int→rdi,rsi,rdx,rcx,r8,r9  FP→xmm0-7  AL=fp_count
///   ARM64 AAPCS64:   int→x0-x7                   FP→d0-d7
///   x86-64 Windows:  int→rcx,rdx,r8,r9           FP→xmm0-3
///
/// The fp_mask in TransitionEntry classifies each arg as int or FP.
/// The trampoline loads them into separate register sequences per ABI.

#include <cstdint>

namespace VMPilot::Runtime {

/// Pack fp_count and returns_fp into a single byte for the trampoline.
///   [5:0] = fp_count (0-8)
///   [6]   = returns_fp flag
constexpr uint8_t pack_call_flags(uint8_t fp_count, bool returns_fp) noexcept {
    return static_cast<uint8_t>((fp_count & 0x3F) | (returns_fp ? 0x40 : 0));
}

/// Platform-specific call trampoline (implemented in .S or fallback .cpp).
///
/// @param target    function pointer to call
/// @param int_regs  array of 8 integer arg values (excess slots zeroed)
/// @param fp_regs   array of 8 FP arg bit-patterns (uint64_t bit-cast of double)
/// @param flags     packed: [5:0]=fp_count for variadic AL, [6]=returns_fp
/// @return          integer result (rax/x0), or bit-cast of FP result if returns_fp
extern "C" uint64_t platform_call(
    void* target,
    const uint64_t* int_regs,
    const uint64_t* fp_regs,
    uint8_t flags) noexcept;

/// Classify raw args into separate integer and FP sequences per ABI.
///
/// The ABI uses INDEPENDENT counters for int and FP args:
///   func(int a, double b, int c)  →  a→rdi(int#0), b→xmm0(fp#0), c→rsi(int#1)
///
/// @param raw_args   decoded plaintext args (all as uint64_t)
/// @param arg_count  number of args (0-8)
/// @param fp_mask    bit i set → arg i is FP (double)
/// @param int_out    output: integer args in ABI order
/// @param fp_out     output: FP args in ABI order (as uint64_t bit-patterns)
/// @param nint_out   output: number of integer args
/// @param nfp_out    output: number of FP args
inline void classify_args(const uint64_t raw_args[8], uint8_t arg_count,
                           uint8_t fp_mask,
                           uint64_t int_out[8], uint64_t fp_out[8],
                           uint8_t& nint_out, uint8_t& nfp_out) noexcept {
    nint_out = 0;
    nfp_out = 0;
    for (uint8_t i = 0; i < arg_count && i < 8; ++i) {
        if (fp_mask & (1u << i)) {
            fp_out[nfp_out++] = raw_args[i];
        } else {
            int_out[nint_out++] = raw_args[i];
        }
    }
    // Zero remaining slots
    for (uint8_t i = nint_out; i < 8; ++i) int_out[i] = 0;
    for (uint8_t i = nfp_out; i < 8; ++i) fp_out[i] = 0;
}

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_PLATFORM_CALL_HPP__
