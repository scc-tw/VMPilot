/// @file vm_internal.cpp
/// @brief Cat 7: VM Internal handlers (7 opcodes).
///
/// Security properties:
///   NOP: Chaff fake taint; enc_state chain participation prevents
///     deletion (SipHash preimage resistance). Writes to trash_regs
///     to defeat taint-tracking-based dead code analysis.
///
///   CHECK_INTEGRITY: Verifies BB MAC (BLAKE3_keyed).
///   CHECK_DEBUG: Phase 9 stub (anti-debug traps).
///   MUTATE_ISA: Re-derives opcode permutation from epoch_seed.
///   REKEY: Phase 8 stub (re-keying).
///   SAVE_EPOCH / RESYNC: Shadow stack epoch checkpointing.

#include <handlers.hpp>
#include <decoder.hpp>
#include <vm/vm_encoding.hpp>

#include <cstring>

namespace VMPilot::Runtime::handlers {

using Common::DiagnosticCode;
using Common::VM::VMContext;
using Common::VM::VM_MAX_NESTING;

// ---------------------------------------------------------------------------
// NOP: Chaff instruction (fake taint for DPA/taint-tracking resistance)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_nop(VMContext& ctx, const DecodedInsn& insn) noexcept {
    // Write to trash register to create fake data dependency.
    // This prevents analysis tools from identifying NOP as dead code
    // and stripping it, which would break the enc_state chain
    // (SipHash preimage resistance guarantees enc_state depends on
    // every instruction in the BB).
    ctx.trash_regs[insn.reg_a] = ctx.encoded_regs[insn.reg_b];
    return {};
}

// ---------------------------------------------------------------------------
// CHECK_INTEGRITY: Verify BB MAC
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_check_integrity(VMContext& ctx, const DecodedInsn& /*insn*/) noexcept {
    // Delegate to the decoder's verify_bb_mac which re-decrypts the entire
    // BB and computes BLAKE3_keyed(integrity_key, all_plaintext_insns)[0:8].
    return verify_bb_mac(ctx);
}

// ---------------------------------------------------------------------------
// CHECK_DEBUG: Phase 9 stub (anti-debug)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_check_debug(VMContext& /*ctx*/, const DecodedInsn& /*insn*/) noexcept {
    // Phase 9: will check ptrace, debug registers, etc.
    return {};
}

// ---------------------------------------------------------------------------
// MUTATE_ISA: Re-derive opcode permutation from current BB's epoch_seed
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_mutate_isa(VMContext& ctx, const DecodedInsn& /*insn*/) noexcept {
    Common::VM::Encoding::derive_opcode_permutation(
        ctx.bb_metadata[ctx.current_bb_index].epoch_seed,
        ctx.opcode_perm, ctx.opcode_perm_inv);
    return {};
}

// ---------------------------------------------------------------------------
// REKEY: Phase 8 stub (re-keying)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_rekey(VMContext& /*ctx*/, const DecodedInsn& /*insn*/) noexcept {
    // Phase 8: will re-derive encryption keys mid-BB.
    return {};
}

// ---------------------------------------------------------------------------
// SAVE_EPOCH: Push epoch checkpoint onto shadow stack
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_save_epoch(VMContext& ctx, const DecodedInsn& /*insn*/) noexcept {
    if (ctx.shadow_depth >= VM_MAX_NESTING)
        return tl::make_unexpected(DiagnosticCode::ShadowStackOverflow);

    auto& cp = ctx.shadow_stack[ctx.shadow_depth];
    cp.vm_ip = ctx.vm_ip;
    cp.bb_id = ctx.current_bb_id;
    std::memcpy(cp.epoch_seed,
                ctx.bb_metadata[ctx.current_bb_index].epoch_seed, 32);
    cp.salt = 0;
    std::memcpy(cp.encoded_regs_snapshot,
                ctx.encoded_regs, sizeof(ctx.encoded_regs));
    ctx.shadow_depth++;
    return {};
}

// ---------------------------------------------------------------------------
// RESYNC: Pop epoch checkpoint, restore encoded registers
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_resync(VMContext& ctx, const DecodedInsn& /*insn*/) noexcept {
    if (ctx.shadow_depth == 0)
        return tl::make_unexpected(DiagnosticCode::StackUnderflow);

    ctx.shadow_depth--;
    std::memcpy(ctx.encoded_regs,
                ctx.shadow_stack[ctx.shadow_depth].encoded_regs_snapshot,
                sizeof(ctx.encoded_regs));
    return {};
}

// ---------------------------------------------------------------------------
// stub_handler: fallback for unimplemented opcodes
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
stub_handler(VMContext& /*ctx*/, const DecodedInsn& /*insn*/) noexcept {
    return tl::make_unexpected(DiagnosticCode::NotImplemented);
}

}  // namespace VMPilot::Runtime::handlers
