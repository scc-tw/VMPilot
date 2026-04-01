/// @file platform_call_fallback.cpp
/// @brief Generic fallback for platform_call (integer args only).
///
/// Used on platforms without a dedicated ASM trampoline.
/// FP args are passed as integer bit-patterns — ABI-incorrect for
/// FP-typed parameters but works for integer-only native calls.

#include "platform_call.hpp"

// Only compile if no ASM trampoline is available
#if (!defined(__x86_64__) && !defined(_M_X64) && !defined(__aarch64__) && !defined(_M_ARM64) && !defined(__i386__) && !defined(_M_IX86))

extern "C"
uint64_t platform_call(const VMPilot::Runtime::PlatformCallDesc* desc) noexcept {
    using Fn = uint64_t(*)(uint64_t,uint64_t,uint64_t,uint64_t,
                            uint64_t,uint64_t,uint64_t,uint64_t);
    auto fn = reinterpret_cast<Fn>(desc->target);
    return fn(desc->int_regs[0], desc->int_regs[1], desc->int_regs[2], desc->int_regs[3],
              desc->int_regs[4], desc->int_regs[5], desc->int_regs[6], desc->int_regs[7]);
}

extern "C"
uint64_t platform_call_struct(
    const VMPilot::Runtime::PlatformCallDesc* desc,
    void* /*struct_return_ptr*/) noexcept
{
    // Fallback: struct ptr already in int_regs[0] via classify_args
    return platform_call(desc);
}

#endif
