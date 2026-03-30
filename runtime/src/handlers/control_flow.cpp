/// @file control_flow.cpp
/// @brief Cat 4: Control Flow handlers (JMP, JCC, CALL_VM, RET_VM, NATIVE_CALL, HALT).
///
/// Security properties:
///   JMP/JCC: No data operand exposure. Branch target is an integer
///     BB ID (not an address), so the only observable is which BB
///     executes next -- inherent in any control flow.
///
///   CALL_VM/RET_VM: Shadow stack saves/restores epoch state.
///     Register snapshots are stored in their encoded-domain form
///     (no plaintext exposure).
///
///   NATIVE_CALL: Stub for Phase 8 (native bridge).
///   HALT: Sets halted flag for dispatcher.

#include <handlers.hpp>
#include <decoder.hpp>

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
// NATIVE_CALL: Phase 8 stub
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_native_call(VMContext& /*ctx*/, const DecodedInsn& /*insn*/) noexcept {
    return tl::make_unexpected(DiagnosticCode::NativeCallBridgeFailed);
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
