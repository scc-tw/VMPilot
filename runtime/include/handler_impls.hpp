#pragma once
#ifndef __RUNTIME_HANDLER_IMPLS_HPP__
#define __RUNTIME_HANDLER_IMPLS_HPP__

/// @file handler_impls.hpp
/// @brief HandlerTraits specializations for all 55 opcodes.
///
/// This header provides the compile-time-complete set of handler
/// specializations.  Three categories:
///
///   1. Real implementations (MOVE, XOR, HALT) — fully functional,
///      demonstrate the traits pattern with phantom-typed values.
///
///   2. Stub implementations (remaining 52 opcodes) — return
///      DiagnosticCode::NotImplemented.  Will be ported from the
///      existing handlers/*.cpp during Phase 4.
///
///   3. ORAM-aware handlers (PUSH, POP) — provide template<Oram>
///      exec member.  Currently stubbed.
///
/// Include order: handler_traits.hpp → handler_impls.hpp

#include "handler_traits.hpp"

namespace VMPilot::Runtime {

// ─────────────────────────────────────────────────────────────────────────────
// Stub handler: returns NotImplemented for unported opcodes
// ─────────────────────────────────────────────────────────────────────────────

inline HandlerResult stub_not_implemented(
    VmExecution&, VmEpoch&, VmOramState&,
    const VmImmutable&, const DecodedInsn&) noexcept
{
    return tl::make_unexpected(DiagnosticCode::NotImplemented);
}

// ─────────────────────────────────────────────────────────────────────────────
// Macro for declaring a stub specialization (reduces boilerplate for 52 stubs)
// ─────────────────────────────────────────────────────────────────────────────

#define VMPILOT_HANDLER_STUB(OPCODE, CLASS)                              \
    template<typename Policy>                                             \
    struct HandlerTraits<VmOpcode::OPCODE, Policy> {                     \
        static constexpr auto security_class = SecurityClass::CLASS;     \
        using oram_tag = NoOramTag;                                       \
        static HandlerResult exec(                                        \
            VmExecution& e, VmEpoch& ep, VmOramState& o,                 \
            const VmImmutable& im, const DecodedInsn& i) noexcept        \
        { return stub_not_implemented(e, ep, o, im, i); }                \
    }

#define VMPILOT_HANDLER_ORAM_STUB(OPCODE, CLASS)                         \
    template<typename Policy>                                             \
    struct HandlerTraits<VmOpcode::OPCODE, Policy> {                     \
        static constexpr auto security_class = SecurityClass::CLASS;     \
        using oram_tag = UsesOramTag;                                     \
        template<typename Oram>                                           \
        static HandlerResult exec(                                        \
            VmExecution& e, VmEpoch& ep, VmOramState& o,                 \
            const VmImmutable& im, const DecodedInsn& i) noexcept        \
        { return stub_not_implemented(e, ep, o, im, i); }                \
    }

// ═════════════════════════════════════════════════════════════════════════════
// Cat 0: Data Movement — MOVE (real), rest stubbed
// ═════════════════════════════════════════════════════════════════════════════

/// MOVE: dst = src (same BB register domain, no encoding conversion).
///
/// Security: Class A (zero plaintext).  Both registers are in the same
/// BB's encoding domain, so copying the encoded value directly is correct.
template<typename Policy>
struct HandlerTraits<VmOpcode::MOVE, Policy> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;

    static HandlerResult exec(
        VmExecution& exec, VmEpoch&, VmOramState&,
        const VmImmutable&, const DecodedInsn& insn) noexcept
    {
        exec.regs[insn.reg_a] = exec.regs[insn.reg_b];
        return {};
    }
};

VMPILOT_HANDLER_STUB(LOAD, A);
VMPILOT_HANDLER_STUB(STORE, A);
VMPILOT_HANDLER_ORAM_STUB(PUSH, A);
VMPILOT_HANDLER_ORAM_STUB(POP, A);
VMPILOT_HANDLER_STUB(LOAD_CONST, A);
VMPILOT_HANDLER_STUB(LOAD_CTX, C);
VMPILOT_HANDLER_STUB(STORE_CTX, C);

// ═════════════════════════════════════════════════════════════════════════════
// Cat 1: Arithmetic — all stubbed (MBA porting is Phase 4)
// ═════════════════════════════════════════════════════════════════════════════

VMPILOT_HANDLER_STUB(ADD, B);
VMPILOT_HANDLER_STUB(SUB, B);
VMPILOT_HANDLER_STUB(MUL, C);
VMPILOT_HANDLER_STUB(IMUL, C);
VMPILOT_HANDLER_STUB(DIV, C);
VMPILOT_HANDLER_STUB(IDIV, C);
VMPILOT_HANDLER_STUB(NEG, B);
VMPILOT_HANDLER_STUB(MOD, C);

// ═════════════════════════════════════════════════════════════════════════════
// Cat 2: Logic — XOR (real), rest stubbed
// ═════════════════════════════════════════════════════════════════════════════

/// XOR: dst = src_a ^ src_b via composition table.
///
/// Security: Class A (zero plaintext).  The composition table
///   comp[k][a][b] = E_dst[k]( E_a^{-1}[k](a) ^ E_b^{-1}[k](b) )
/// maps encoded inputs directly to encoded output with no plaintext
/// intermediate.  MCSP-hard (doc 15 §3.4, §11.2).
template<typename Policy>
struct HandlerTraits<VmOpcode::XOR, Policy> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;

    static HandlerResult exec(
        VmExecution& exec, VmEpoch& epoch, VmOramState&,
        const VmImmutable&, const DecodedInsn& insn) noexcept
    {
        // For Phase 3 validation: direct byte-lane XOR through composition.
        // Full implementation uses CompositionCache (ported in Phase 4).
        //
        // Proof-of-concept: apply_binary_comp from encoded_value.hpp
        // with a lazily-built composition table.
        //
        // TODO(phase4): use epoch.cache->get_binary() for cached lookup.

        // Build a one-shot composition table for this register triple.
        uint8_t comp[8][256][256];
        auto op_xor = [](uint8_t a, uint8_t b) -> uint8_t { return a ^ b; };
        Common::VM::Encoding::build_composition_table(
            op_xor,
            epoch.reg.encode[insn.reg_a],
            epoch.reg.decode[insn.reg_a],  // dst is also src_a
            epoch.reg.decode[insn.reg_b],
            comp);

        exec.regs[insn.reg_a] = Common::VM::apply_binary_comp(
            comp, exec.regs[insn.reg_a], exec.regs[insn.reg_b]);

        return {};
    }
};

VMPILOT_HANDLER_STUB(AND, A);
VMPILOT_HANDLER_STUB(OR, A);
VMPILOT_HANDLER_STUB(NOT, A);
VMPILOT_HANDLER_STUB(SHL, C);
VMPILOT_HANDLER_STUB(SHR, C);
VMPILOT_HANDLER_STUB(SAR, C);
VMPILOT_HANDLER_STUB(ROL, C);
VMPILOT_HANDLER_STUB(ROR, C);

// ═════════════════════════════════════════════════════════════════════════════
// Cat 3: Comparison — all stubbed
// ═════════════════════════════════════════════════════════════════════════════

VMPILOT_HANDLER_STUB(CMP, C);
VMPILOT_HANDLER_STUB(TEST, C);
VMPILOT_HANDLER_STUB(SET_FLAG, C);
VMPILOT_HANDLER_STUB(GET_FLAG, C);

// ═════════════════════════════════════════════════════════════════════════════
// Cat 4: Control Flow — HALT (real), rest stubbed
// ═════════════════════════════════════════════════════════════════════════════

VMPILOT_HANDLER_STUB(JMP, A);
VMPILOT_HANDLER_STUB(JCC, A);
VMPILOT_HANDLER_STUB(CALL_VM, A);
VMPILOT_HANDLER_STUB(RET_VM, A);
VMPILOT_HANDLER_STUB(NATIVE_CALL, C);

/// HALT: stop execution.
///
/// Sets exec.halted = true.  The dispatcher loop checks this flag
/// and returns VmResult::Halted.
template<typename Policy>
struct HandlerTraits<VmOpcode::HALT, Policy> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;

    static HandlerResult exec(
        VmExecution& exec, VmEpoch&, VmOramState&,
        const VmImmutable&, const DecodedInsn&) noexcept
    {
        exec.halted = true;
        return {};
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// Cat 5: Width/Extension — all stubbed
// ═════════════════════════════════════════════════════════════════════════════

VMPILOT_HANDLER_STUB(SEXT8, C);
VMPILOT_HANDLER_STUB(SEXT16, C);
VMPILOT_HANDLER_STUB(SEXT32, C);
VMPILOT_HANDLER_STUB(ZEXT8, C);
VMPILOT_HANDLER_STUB(ZEXT16, C);
VMPILOT_HANDLER_STUB(ZEXT32, C);
VMPILOT_HANDLER_STUB(TRUNC8, C);
VMPILOT_HANDLER_STUB(TRUNC16, C);

// ═════════════════════════════════════════════════════════════════════════════
// Cat 6: Atomic — all stubbed
// ═════════════════════════════════════════════════════════════════════════════

VMPILOT_HANDLER_STUB(LOCK_ADD, C);
VMPILOT_HANDLER_STUB(XCHG, C);
VMPILOT_HANDLER_STUB(CMPXCHG, C);
VMPILOT_HANDLER_STUB(FENCE, A);
VMPILOT_HANDLER_STUB(ATOMIC_LOAD, C);

// ═════════════════════════════════════════════════════════════════════════════
// Cat 7: VM Internal — NOP (real), rest stubbed
// ═════════════════════════════════════════════════════════════════════════════

/// NOP: chaff instruction — writes to trash register file (GSS §4.3).
///
/// Chaff participates in the enc_state chain (cannot be NOPped by attacker).
/// Writing to trash_regs creates fake taint in dynamic analysis tools.
template<typename Policy>
struct HandlerTraits<VmOpcode::NOP, Policy> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;

    static HandlerResult exec(
        VmExecution& exec, VmEpoch&, VmOramState&,
        const VmImmutable&, const DecodedInsn& insn) noexcept
    {
        exec.trash_regs[insn.reg_a] = exec.regs[insn.reg_b].bits;
        return {};
    }
};

VMPILOT_HANDLER_STUB(CHECK_INTEGRITY, A);
VMPILOT_HANDLER_STUB(CHECK_DEBUG, A);
VMPILOT_HANDLER_STUB(MUTATE_ISA, A);
VMPILOT_HANDLER_STUB(REKEY, A);
VMPILOT_HANDLER_STUB(SAVE_EPOCH, A);
VMPILOT_HANDLER_STUB(RESYNC, A);

// ─────────────────────────────────────────────────────────────────────────────
// Cleanup macros
// ─────────────────────────────────────────────────────────────────────────────

#undef VMPILOT_HANDLER_STUB
#undef VMPILOT_HANDLER_ORAM_STUB

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_HANDLER_IMPLS_HPP__
