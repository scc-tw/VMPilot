/// @file classify_args.cpp
/// @brief Platform-aware arg classification for NATIVE_CALL.

#include "platform_call.hpp"

#include <cstring>

namespace VMPilot::Runtime {

/// Push an 8-byte value to the stack buffer.
static void push_stack_8(PlatformCallDesc& desc, uint64_t val) noexcept {
    if (static_cast<size_t>(desc.stack_size) + 8 > PLATFORM_CALL_MAX_STACK) return;
    std::memcpy(desc.stack_buf + desc.stack_size, &val, 8);
    desc.stack_size += 8;
}

/// Push a 4-byte value to the stack buffer (x86-32 int/ptr).
static void push_stack_4(PlatformCallDesc& desc, uint32_t val) noexcept {
    if (static_cast<size_t>(desc.stack_size) + 4 > PLATFORM_CALL_MAX_STACK) return;
    std::memcpy(desc.stack_buf + desc.stack_size, &val, 4);
    desc.stack_size += 4;
}

// ─────────────────────────────────────────────────────────────────────────────

static void classify_sysv_x64(
    const uint64_t* raw, uint8_t argc, uint8_t fp_mask,
    bool ret_struct, void* sptr, PlatformCallDesc& d) noexcept
{
    // SysV: INDEPENDENT int/FP sequences.
    // 6 int regs (rdi,rsi,rdx,rcx,r8,r9), 8 FP regs (xmm0-7).
    // Overflow → stack (8 bytes each).
    // If returns_struct: struct pointer is the FIRST int arg (shifts others).

    constexpr uint8_t MAX_INT_REGS = 6;
    constexpr uint8_t MAX_FP_REGS  = 8;

    uint8_t ni = 0, nf = 0;

    // Insert struct return pointer as first int arg if needed
    if (ret_struct && sptr) {
        d.int_regs[ni++] = reinterpret_cast<uint64_t>(sptr);
    }

    for (uint8_t i = 0; i < argc; ++i) {
        if (fp_mask & (1u << i)) {
            if (nf < MAX_FP_REGS) {
                d.fp_regs[nf++] = raw[i];
            } else {
                push_stack_8(d, raw[i]);
            }
        } else {
            if (ni < MAX_INT_REGS) {
                d.int_regs[ni++] = raw[i];
            } else {
                push_stack_8(d, raw[i]);
            }
        }
    }

    d.reg_int_count = ni;
    d.fp_count = nf;
}

// ─────────────────────────────────────────────────────────────────────────────

static void classify_win_x64(
    const uint64_t* raw, uint8_t argc, uint8_t fp_mask,
    bool ret_struct, void* sptr, PlatformCallDesc& d) noexcept
{
    // Windows x64: SHARED slot model — 4 slots.
    // Slot i: if FP → xmm{i}, if int → rcx/rdx/r8/r9.
    // Only ONE of int/FP per slot (never both).
    // Slots 4+ → stack (8 bytes each).
    // If returns_struct: struct pointer in slot 0 (rcx), shifts others.

    constexpr uint8_t MAX_SLOTS = 4;

    uint8_t slot = 0;
    uint8_t nf = 0;

    // Insert struct return pointer as slot 0 if needed
    if (ret_struct && sptr) {
        d.int_regs[0] = reinterpret_cast<uint64_t>(sptr);
        slot = 1;
    }

    for (uint8_t i = 0; i < argc; ++i) {
        if (slot < MAX_SLOTS) {
            if (fp_mask & (1u << i)) {
                d.fp_regs[slot] = raw[i];
                d.int_regs[slot] = 0;  // slot occupied by FP
                nf++;
            } else {
                d.int_regs[slot] = raw[i];
                d.fp_regs[slot] = 0;   // slot occupied by int
            }
            slot++;
        } else {
            push_stack_8(d, raw[i]);
        }
    }

    d.reg_int_count = slot;  // slots used (some may be FP)
    d.fp_count = nf;
}

// ─────────────────────────────────────────────────────────────────────────────

static void classify_x86_32(
    const uint64_t* raw, uint8_t argc, uint8_t fp_mask,
    bool ret_struct, void* sptr, PlatformCallDesc& d) noexcept
{
    // x86-32: ALL args on stack.
    // int/pointer = 4 bytes, double = 8 bytes.
    // No register args (except thiscall/fastcall — deferred).
    // If returns_struct: struct pointer is first on stack (4 bytes).

    d.reg_int_count = 0;
    d.fp_count = 0;

    if (ret_struct && sptr) {
        auto ptr32 = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(sptr));
        push_stack_4(d, ptr32);
    }

    for (uint8_t i = 0; i < argc; ++i) {
        if (fp_mask & (1u << i)) {
            // Double: 8 bytes on stack (IEEE 754 bit pattern)
            push_stack_8(d, raw[i]);
        } else {
            // Integer/pointer: 4 bytes on stack (truncate to 32-bit)
            push_stack_4(d, static_cast<uint32_t>(raw[i]));
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────

static void classify_aapcs64(
    const uint64_t* raw, uint8_t argc, uint8_t fp_mask,
    PlatformCallDesc& d) noexcept
{
    // ARM64: INDEPENDENT int/FP sequences (like SysV but 8 int regs).
    // 8 int regs (x0-x7), 8 FP regs (d0-d7).
    // Overflow → stack (8 bytes each).
    // Struct return: NOT handled here — x8 loaded by platform_call_struct.

    constexpr uint8_t MAX_INT_REGS = 8;
    constexpr uint8_t MAX_FP_REGS  = 8;

    uint8_t ni = 0, nf = 0;

    for (uint8_t i = 0; i < argc; ++i) {
        if (fp_mask & (1u << i)) {
            if (nf < MAX_FP_REGS)
                d.fp_regs[nf++] = raw[i];
            else
                push_stack_8(d, raw[i]);
        } else {
            if (ni < MAX_INT_REGS)
                d.int_regs[ni++] = raw[i];
            else
                push_stack_8(d, raw[i]);
        }
    }

    d.reg_int_count = ni;
    d.fp_count = nf;
}

// ─────────────────────────────────────────────────────────────────────────────
// Public API
// ─────────────────────────────────────────────────────────────────────────────

void classify_args_for_abi(
    CallABI abi,
    const uint64_t* raw_args,
    uint8_t arg_count,
    uint8_t fp_mask,
    bool returns_struct,
    void* struct_ptr,
    PlatformCallDesc& desc) noexcept
{
    // Zero the output descriptor
    std::memset(&desc, 0, sizeof(desc));

    desc.total_args = arg_count;

    switch (abi) {
        case CallABI::SysV_x64:
            classify_sysv_x64(raw_args, arg_count, fp_mask,
                              returns_struct, struct_ptr, desc);
            break;

        case CallABI::Win_x64:
            classify_win_x64(raw_args, arg_count, fp_mask,
                             returns_struct, struct_ptr, desc);
            break;

        case CallABI::Cdecl_x86:
        case CallABI::Stdcall_x86:
            classify_x86_32(raw_args, arg_count, fp_mask,
                            returns_struct, struct_ptr, desc);
            break;

        case CallABI::AAPCS64:
            classify_aapcs64(raw_args, arg_count, fp_mask, desc);
            // ARM64 struct return handled by platform_call_struct (x8 register)
            break;
    }
}

}  // namespace VMPilot::Runtime
