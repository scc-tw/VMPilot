/// @file atomic.cpp
/// @brief Cat 6: Atomic operation handlers (5 opcodes).
///
/// Security properties:
///   LOCK_ADD, XCHG, CMPXCHG: std::atomic operations (seq_cst) on guest
///     memory with MCSP register encoding on input/output values.
///   ATOMIC_LOAD: std::atomic load (seq_cst) with register encoding.
///   FENCE: std::atomic_thread_fence (seq_cst).
///
/// All guest memory addresses are adjusted by ctx.load_base_delta for
/// PIE/ASLR support.

#include <handlers.hpp>
#include <decoder.hpp>
#include <encoding.hpp>

#include <atomic>
#include <cstring>

namespace VMPilot::Runtime::handlers {

using Common::DiagnosticCode;
using Common::VM::VMContext;

/// Compute the relocated guest address from insn.aux + load_base_delta.
static inline uintptr_t guest_addr(const VMContext& ctx,
                                   const DecodedInsn& insn) noexcept {
    return static_cast<uintptr_t>(
        static_cast<int64_t>(insn.aux) + ctx.load_base_delta);
}

// ---------------------------------------------------------------------------
// LOCK_ADD: Atomic fetch-add on guest memory, return old value
//   reg_a  = addend (input) / old value (output)
//   aux    = guest memory address
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_lock_add(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t addend = decode_register(ctx, insn.reg_a,
                                       ctx.encoded_regs[insn.reg_a]);
    auto* atomic_ptr = reinterpret_cast<std::atomic<uint64_t>*>(
        guest_addr(ctx, insn));
    uint64_t old_val = atomic_ptr->fetch_add(addend,
                                              std::memory_order_seq_cst);
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, old_val);
    return {};
}

// ---------------------------------------------------------------------------
// XCHG: Atomic exchange register with memory
//   reg_a  = new value (input) / old value (output)
//   aux    = guest memory address
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_xchg(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t new_val = decode_register(ctx, insn.reg_a,
                                        ctx.encoded_regs[insn.reg_a]);
    auto* atomic_ptr = reinterpret_cast<std::atomic<uint64_t>*>(
        guest_addr(ctx, insn));
    uint64_t old_val = atomic_ptr->exchange(new_val,
                                             std::memory_order_seq_cst);
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, old_val);
    return {};
}

// ---------------------------------------------------------------------------
// CMPXCHG: Atomic compare-and-exchange
//   reg_a  = expected value (input) / actual old value (output)
//   reg_b  = desired value (input)
//   aux    = guest memory address
//   ZF set if exchange occurred
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_cmpxchg(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t expected = decode_register(ctx, insn.reg_a,
                                         ctx.encoded_regs[insn.reg_a]);
    uint64_t desired = decode_register(ctx, insn.reg_b,
                                        ctx.encoded_regs[insn.reg_b]);
    auto* atomic_ptr = reinterpret_cast<std::atomic<uint64_t>*>(
        guest_addr(ctx, insn));
    bool success = atomic_ptr->compare_exchange_strong(
        expected, desired, std::memory_order_seq_cst);
    // On failure, 'expected' is updated to the current value by
    // compare_exchange_strong, which is exactly what we want to
    // return in reg_a (the actual old value).
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, expected);
    ctx.vm_flags = success
        ? static_cast<uint8_t>(ctx.vm_flags | 0x01u)    // ZF = 1
        : static_cast<uint8_t>(ctx.vm_flags & ~0x01u);  // ZF = 0
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
// ATOMIC_LOAD: Atomic load from guest memory (seq_cst)
//   reg_a  = destination register
//   aux    = guest memory address
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_atomic_load(VMContext& ctx, const DecodedInsn& insn) noexcept {
    auto* atomic_ptr = reinterpret_cast<const std::atomic<uint64_t>*>(
        guest_addr(ctx, insn));
    uint64_t val = atomic_ptr->load(std::memory_order_seq_cst);
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, val);
    return {};
}

}  // namespace VMPilot::Runtime::handlers
