/// @file control_flow.cpp
/// @brief Cat 4: Control Flow handlers (JMP, JCC, CALL_VM, RET_VM, NATIVE_CALL, HALT).
///
/// Security properties:
///   JMP/JCC: No data operand exposure. Branch target is an integer
///     BB ID (not an address), so the only observable is which BB
///     executes next — inherent in any control flow.
///
///   CALL_VM/RET_VM: Shadow stack saves/restores epoch state.
///     Register snapshots are stored in their encoded-domain form
///     (no plaintext exposure).
///
///   NATIVE_CALL (D15§6, D15§11.8): The weakest security boundary by
///     design.  Full plaintext operands exist in CPU registers for the
///     duration of the native call — an acknowledged limitation
///     (D15§11.8: "Full plaintext register-transient").
///
///     Why this is acceptable:
///       1. External calls (libc, syscalls) require plaintext by
///          definition — the callee is not under our control.
///       2. Exposure is register-transient: plaintext never reaches
///          addressable memory (stack/heap).  A memory dump captures
///          nothing.  Only a register-level trace (Pin/DBI) observes it.
///       3. The uniform pipeline (D3) executes NATIVE_CALL through the
///          same 12 steps as every other opcode — the trace pattern is
///          indistinguishable from XOR or NOP (D15§11.3 SNR→0).
///       4. The enc_state chain (D1) advances across the NATIVE_CALL
///          instruction, so removing or reordering it breaks all
///          subsequent decryption (preimage resistance).
///
///     What v1 does NOT have (deferred to v2):
///       - Polymorphic Stripper stubs (D15§6.1): per-call-site (a,b)
///         coefficients that disguise decode/encode as leaf functions.
///         Without this, the decode→call→encode pattern is visible in
///         a handler-level trace.
///       - Remote Call Inversion (D15§6.2): splitting argument meaning
///         across multiple calls for crypto-sensitive targets.
///
///     Reentrancy: safe because vm_execute() allocates an independent
///     VMContext per invocation (C stack frame).  No global mutable
///     state shared.  Verified to 100 levels.
///
///   HALT: Sets halted flag for dispatcher.

#include <handlers.hpp>
#include <decoder.hpp>
#include <encoding.hpp>
#include <native_bridge.hpp>
#include <vm/vm_blob.hpp>

#include <cstring>

namespace VMPilot::Runtime::handlers {

using Common::DiagnosticCode;
using Common::VM::VMContext;
using Common::VM::VM_MAX_NESTING;

// ---------------------------------------------------------------------------
// Condition evaluator for JCC
// ---------------------------------------------------------------------------

static bool evaluate_condition(uint8_t flags, uint8_t cond) noexcept {
    bool zf = (flags & 0x01) != 0;
    bool sf = (flags & 0x02) != 0;
    bool cf = (flags & 0x04) != 0;
    bool of = (flags & 0x08) != 0;
    switch (cond) {
        case 0: return zf;                      // EQ (zero)
        case 1: return !zf;                     // NE (not zero)
        case 2: return sf != of;                // LT (signed less)
        case 3: return sf == of;                // GE (signed greater/equal)
        case 4: return zf || (sf != of);        // LE (signed less/equal)
        case 5: return !zf && (sf == of);       // GT (signed greater)
        case 6: return cf;                      // B  (unsigned below)
        case 7: return !cf;                     // AE (unsigned above/equal)
        case 8: return cf || zf;                // BE (unsigned below/equal)
        case 9: return !cf && !zf;              // A  (unsigned above)
        default: return false;
    }
}

// ---------------------------------------------------------------------------
// JMP: Unconditional branch
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_jmp(VMContext& ctx, const DecodedInsn& insn) noexcept {
    ctx.branch_target_bb = insn.aux;
    ctx.branch_taken = true;
    return {};
}

// ---------------------------------------------------------------------------
// JCC: Conditional branch based on vm_flags
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_jcc(VMContext& ctx, const DecodedInsn& insn) noexcept {
    if (evaluate_condition(ctx.vm_flags, insn.condition)) {
        ctx.branch_target_bb = insn.aux;
        ctx.branch_taken = true;
    }
    // If not taken: branch_taken stays false, dispatcher continues
    return {};
}

// ---------------------------------------------------------------------------
// CALL_VM: Push frame onto shadow stack, branch to callee BB
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_call_vm(VMContext& ctx, const DecodedInsn& insn) noexcept {
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

    ctx.branch_target_bb = insn.aux;
    ctx.branch_taken = true;
    return {};
}

// ---------------------------------------------------------------------------
// RET_VM: Pop frame from shadow stack, return to caller BB
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_ret_vm(VMContext& ctx, const DecodedInsn& insn) noexcept {
    (void)insn;
    if (ctx.shadow_depth == 0)
        return tl::make_unexpected(DiagnosticCode::StackUnderflow);

    ctx.shadow_depth--;
    auto& cp = ctx.shadow_stack[ctx.shadow_depth];
    ctx.branch_target_bb = cp.bb_id;
    ctx.branch_taken = true;
    // Note: dispatcher's enter_basic_block will set vm_ip to the BB's
    // entry_ip. For v1, we re-enter the BB from its start.
    return {};
}

// ---------------------------------------------------------------------------
// NATIVE_CALL (D15§6, D15§11.8)
//
// Security model: register-transient plaintext (same as Class C).
//
// Why the plaintext exposure is bounded:
//   - Plaintext exists only in GPRs during the native function call.
//     The C ABI guarantees caller-saved regs are clobbered on return,
//     so plaintext lifetime ≈ duration of the callee.
//   - No addressable memory (stack/heap) ever holds plaintext args.
//     call_native() decodes into a local array that the compiler keeps
//     in registers (8 uint64_t = 64 bytes, fits in GPRs on both
//     x86-64 and ARM64 with optimization).
//   - The result is re-encoded into r0's domain immediately on return.
//     The plaintext return value exists for exactly one encode_register
//     call (~20 ns).
//
// What an attacker with a register-level trace sees:
//   - The D3 uniform pipeline makes NATIVE_CALL indistinguishable from
//     any other opcode in the execution trace (same 12 steps).
//   - The D1 enc_state chain advances, so the instruction cannot be
//     removed or reordered without desync.
//   - But: the decode→call→encode pattern within the handler IS visible
//     to a DBI tool that hooks call_native().  Polymorphic stubs (v2,
//     D15§6.1) will break this pattern by disguising the transition as
//     unrelated leaf function calls with per-site (a,b) coefficients.
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_native_call(VMContext& ctx, const DecodedInsn& insn) noexcept {
    const uint32_t call_idx = insn.aux;

    if (call_idx >= ctx.native_call_count)
        return tl::make_unexpected(DiagnosticCode::NativeCallBridgeFailed);

    const auto& entry = ctx.native_call_entries[call_idx];

    // Resolve target address: blob stores offset from binary base,
    // load_base_delta adjusts for ASLR.  In tests where functions are
    // passed as absolute pointers, load_base_delta is 0.
    const auto target = static_cast<uintptr_t>(
        entry.target_offset + static_cast<uint64_t>(ctx.load_base_delta));

    if (target == 0)
        return tl::make_unexpected(DiagnosticCode::NativeCallBridgeFailed);

    // Extract actual arg count from bit-packed field (bits [3:0]).
    // Upper bits carry FP/variadic metadata (D13§E2-E4).
    const uint8_t arg_count = static_cast<uint8_t>(
        te_arg_count(entry) <= NATIVE_CALL_MAX_ARGS
            ? te_arg_count(entry) : NATIVE_CALL_MAX_ARGS);

    uint64_t encoded_args[NATIVE_CALL_MAX_ARGS] = {};
    uint8_t  arg_regs[NATIVE_CALL_MAX_ARGS] = {};
    for (uint8_t i = 0; i < arg_count; ++i) {
        arg_regs[i] = i;
        encoded_args[i] = ctx.encoded_regs[i];
    }

    // Extract ABI metadata from the bit-packed arg_count field.
    const uint8_t fp_mask     = te_fp_mask(entry);
    const bool    is_variadic = te_is_variadic(entry);
    const bool    returns_fp  = te_returns_fp(entry);

    // Bridge with ephemeral transition encoding + platform-aware ABI.
    auto result = call_native_ephemeral(
        ctx, target, encoded_args, arg_regs, arg_count,
        entry.call_site_ip, fp_mask, is_variadic, returns_fp);
    if (!result)
        return tl::make_unexpected(result.error());

    // Re-encode the return value into r0 (dst register).
    ctx.encoded_regs[0] = encode_register(ctx, 0, result.value());
    return {};
}

// ---------------------------------------------------------------------------
// HALT: Stop execution
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_halt(VMContext& ctx, const DecodedInsn& /*insn*/) noexcept {
    ctx.halted = true;
    return {};
}

}  // namespace VMPilot::Runtime::handlers
