#pragma once
#ifndef __RUNTIME_PLATFORM_CALL_HPP__
#define __RUNTIME_PLATFORM_CALL_HPP__

/// @file platform_call.hpp
/// @brief ABI-correct native function call trampoline — all platforms.
///
/// Supports:
///   - Integer AND floating-point arguments in correct registers
///   - >8 args via stack passing (byte-level stack buffer)
///   - Variadic (AL = fp_count on x86-64 SysV)
///   - Struct return (hidden pointer arg; ARM64 uses x8)
///   - Windows x64 shared-slot model (rcx/xmm0 share slot 0)
///   - Windows x86-32 cdecl/stdcall with x87 FP return
///
/// The PlatformCallDesc struct separates register args (uint64_t[])
/// from stack args (byte-level buffer), because:
///   - x86-32: int = 4 bytes, double = 8 bytes — mixed on stack
///   - x86-64: all stack args are 8 bytes
///   - The byte buffer IS the exact stack frame the callee expects

#include <cstdint>
#include <cstddef>
#include <cstring>

namespace VMPilot::Runtime {

// ─────────────────────────────────────────────────────────────────────────────
// ABI enumeration
// ─────────────────────────────────────────────────────────────────────────────

enum class CallABI : uint8_t {
    SysV_x64,       ///< Linux/macOS x86-64: 6 int regs, 8 FP regs, independent sequences
    Win_x64,        ///< Windows x86-64: 4 slots shared between int/FP
    Cdecl_x86,      ///< x86-32 cdecl: all on stack, caller cleanup
    Stdcall_x86,    ///< x86-32 stdcall: all on stack, callee cleanup
    AAPCS64,        ///< ARM64: 8 int regs, 8 FP regs, independent sequences
};

/// Detect the native ABI from compile-time platform macros.
constexpr CallABI native_abi() noexcept {
#if defined(__x86_64__) && !defined(_WIN32)
    return CallABI::SysV_x64;
#elif (defined(__x86_64__) || defined(_M_X64)) && defined(_WIN32)
    return CallABI::Win_x64;
#elif defined(__i386__) || defined(_M_IX86)
    return CallABI::Cdecl_x86;   // convention override via desc.convention
#elif defined(__aarch64__) || defined(_M_ARM64)
    return CallABI::AAPCS64;
#else
    return CallABI::SysV_x64;    // fallback
#endif
}

// ─────────────────────────────────────────────────────────────────────────────
// PlatformCallDesc — unified call descriptor for all ABIs
// ─────────────────────────────────────────────────────────────────────────────

/// Maximum stack arg bytes: 16 args × 8 bytes = 128.
constexpr size_t PLATFORM_CALL_MAX_STACK = 128;

/// Unified descriptor that the platform-specific trampoline reads.
///
/// Register args and stack args are separated:
///   - int_regs[]: loaded into platform-specific integer registers
///   - fp_regs[]:  loaded into platform-specific FP registers
///   - stack_buf[]: exact byte image pushed to stack (no reinterpretation)
struct PlatformCallDesc {
    void*    target;                                    // offset 0

    // ── Register args ────────────────────────────────────────────────
    //
    // SysV x64:  [0..5] → rdi,rsi,rdx,rcx,r8,r9
    // Win x64:   [0..3] → rcx,rdx,r8,r9 (shared slot with fp_regs)
    // ARM64:     [0..7] → x0-x7
    // x86-32:    unused (all goes to stack_buf)
    uint64_t int_regs[8];                               // offset 8

    // FP register args (independent on SysV/ARM64, shared slot on Win x64)
    // SysV x64:  [0..7] → xmm0-7
    // Win x64:   [0..3] → xmm0-3 (same slot index as int_regs)
    // ARM64:     [0..7] → d0-d7
    // x86-32:    unused (FP goes to stack_buf as 8-byte values)
    uint64_t fp_regs[8];                                // offset 72

    // ── Stack args (byte-level exact image) ──────────────────────────
    //
    // classify_args_for_abi writes the exact bytes the callee expects:
    //   x86-32: int32 = 4B, double = 8B, pointer = 4B
    //   x86-64: all 8B per slot
    //   ARM64:  all 8B per slot
    //
    // Trampoline: sub rsp, stack_size; memcpy [rsp], stack_buf, stack_size
    alignas(16) uint8_t stack_buf[PLATFORM_CALL_MAX_STACK]; // offset 136

    uint16_t stack_size;        ///< actual bytes in stack_buf (0 if none)  // offset 264
    uint8_t  total_args;        ///< total arg count (0-31)
    uint8_t  reg_int_count;     ///< how many int_regs are populated
    uint8_t  fp_count;          ///< how many fp_regs are populated
    uint8_t  flags;             ///< [5:0]=fp_count for AL, [6]=returns_fp
    uint8_t  convention;        ///< 0=default, 1=stdcall (see TE_CONV_*)
    uint8_t  _pad;
};

/// Pack fp_count and returns_fp into the flags byte.
constexpr uint8_t pack_call_flags(uint8_t fp_count, bool returns_fp) noexcept {
    return static_cast<uint8_t>((fp_count & 0x3F) | (returns_fp ? 0x40 : 0));
}

// ─────────────────────────────────────────────────────────────────────────────
// Trampoline entry points (implemented in platform-specific .S/.asm files)
// ─────────────────────────────────────────────────────────────────────────────

/// Standard call (no struct return).
///
/// Trampoline reads PlatformCallDesc fields by offset:
///   - Loads int_regs → platform integer registers
///   - Loads fp_regs → platform FP registers
///   - Copies stack_buf[0..stack_size] to [rsp/esp] for stack args
///   - Sets AL = fp_count (SysV variadic)
///   - Calls target
///   - Returns integer result in rax/eax, or bit-cast FP if returns_fp
extern "C" uint64_t platform_call(const PlatformCallDesc* desc) noexcept;

/// Struct-return variant.
///
/// Like platform_call but also passes the struct return pointer:
///   - x86-64 SysV: struct_ptr inserted as rdi (shifts other int args)
///                   (handled by classify_args, regular platform_call works)
///   - Windows x64:  struct_ptr inserted as rcx (shifts other args)
///                   (handled by classify_args, regular platform_call works)
///   - ARM64 AAPCS:  struct_ptr → x8 (SEPARATE, does NOT shift x0-x7)
///                   (requires this separate variant)
///   - x86-32:       struct_ptr is first stack arg (shifted in stack_buf)
///                   (handled by classify_args, regular platform_call works)
///
/// Only ARM64 NEEDS this variant.  Other platforms use regular platform_call
/// with the struct pointer pre-inserted into int_regs by classify_args.
extern "C" uint64_t platform_call_struct(
    const PlatformCallDesc* desc,
    void* struct_return_ptr          // ARM64: → x8
) noexcept;

// ─────────────────────────────────────────────────────────────────────────────
// classify_args_for_abi — platform-aware arg classification
// ─────────────────────────────────────────────────────────────────────────────

/// Classify decoded plaintext args into PlatformCallDesc for a specific ABI.
///
/// This function fills int_regs, fp_regs, and stack_buf according to the
/// ABI's register/stack split rules.  After this call, the trampoline just
/// reads the descriptor — no ABI knowledge needed in the trampoline.
///
/// @param abi             target calling convention
/// @param raw_args        decoded plaintext args (all as uint64_t)
/// @param arg_count       total number of args (0-31)
/// @param fp_mask         bit i set → arg i is FP (double)
/// @param returns_struct  if true and ABI != ARM64, insert struct_ptr as first int arg
/// @param struct_ptr      struct return pointer (only used if returns_struct)
/// @param desc            output: classified args
void classify_args_for_abi(
    CallABI abi,
    const uint64_t* raw_args,
    uint8_t arg_count,
    uint8_t fp_mask,
    bool returns_struct,
    void* struct_ptr,
    PlatformCallDesc& desc) noexcept;

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_PLATFORM_CALL_HPP__
