/// @file atomic.cpp
/// @brief Cat 6: Atomic operation handlers (5 opcodes).
///
/// Security properties:
///   v1: Class C security level (decode, non-atomic RMW, re-encode).
///   These are simplified non-atomic implementations for v1 testing.
///   Real atomic operations on arbitrary guest addresses are deferred
///   to v2 when hardware-atomic memory operations are properly
///   sandboxed via the guest memory model.
///
///   FENCE is a full memory fence (seq_cst).

#include <handlers.hpp>
#include <decoder.hpp>
#include <encoding.hpp>

#include <atomic>
#include <cstring>

namespace VMPilot::Runtime::handlers {

using Common::DiagnosticCode;
using Common::VM::VMContext;

// ---------------------------------------------------------------------------
// LOCK_ADD: Atomically add register to memory, return old value
//   reg_a  = addend (input) / old value (output)
//   aux    = guest memory address
//
// v1 simplification: non-atomic read-modify-write.
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_lock_add(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t addend = decode_register(ctx, insn.reg_a,
                                       ctx.encoded_regs[insn.reg_a]);
    auto* ptr = reinterpret_cast<uint64_t*>(static_cast<uintptr_t>(insn.aux));
    uint64_t old_val = 0;
    std::memcpy(&old_val, ptr, 8);
    uint64_t new_val = old_val + addend;
    std::memcpy(ptr, &new_val, 8);
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, old_val);
    return {};
}

// ---------------------------------------------------------------------------
// XCHG: Atomically exchange register with memory
//   reg_a  = new value (input) / old value (output)
//   aux    = guest memory address
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_xchg(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t new_val = decode_register(ctx, insn.reg_a,
                                        ctx.encoded_regs[insn.reg_a]);
    auto* ptr = reinterpret_cast<uint64_t*>(static_cast<uintptr_t>(insn.aux));
    uint64_t old_val = 0;
    std::memcpy(&old_val, ptr, 8);
    std::memcpy(ptr, &new_val, 8);
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, old_val);
    return {};
}

// ---------------------------------------------------------------------------
// CMPXCHG: Compare-and-exchange
//   reg_a  = expected value (input) / old value (output)
//   reg_b  = new value (input)
//   aux    = guest memory address
//   ZF set if exchange occurred
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_cmpxchg(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t expected = decode_register(ctx, insn.reg_a,
                                         ctx.encoded_regs[insn.reg_a]);
    uint64_t desired = decode_register(ctx, insn.reg_b,
                                        ctx.encoded_regs[insn.reg_b]);
    auto* ptr = reinterpret_cast<uint64_t*>(static_cast<uintptr_t>(insn.aux));
    uint64_t current = 0;
    std::memcpy(&current, ptr, 8);

    if (current == expected) {
        std::memcpy(ptr, &desired, 8);
        ctx.vm_flags |= 0x01;  // ZF = 1 (exchange succeeded)
    } else {
        ctx.vm_flags &= static_cast<uint8_t>(~0x01u);  // ZF = 0
    }
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, current);
    return {};
}

// ---------------------------------------------------------------------------
// FENCE: Full memory barrier
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_fence(VMContext& /*ctx*/, const DecodedInsn& /*insn*/) noexcept {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return {};
}

// ---------------------------------------------------------------------------
// ATOMIC_LOAD: Load from memory with acquire semantics
//   reg_a  = destination register
//   aux    = guest memory address
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_atomic_load(VMContext& ctx, const DecodedInsn& insn) noexcept {
    auto* ptr = reinterpret_cast<const uint64_t*>(
        static_cast<uintptr_t>(insn.aux));
    uint64_t val = 0;
    std::memcpy(&val, ptr, 8);
    std::atomic_thread_fence(std::memory_order_acquire);
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, val);
    return {};
}

}  // namespace VMPilot::Runtime::handlers
