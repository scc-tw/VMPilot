#pragma once
#ifndef __RUNTIME_HANDLER_TRAITS_HPP__
#define __RUNTIME_HANDLER_TRAITS_HPP__

/// @file handler_traits.hpp
/// @brief Traits-based handler registry with compile-time opcode coverage.
///
/// Design:
///
///   1. HandlerTraits<Op, Policy> is the primary template (UNDEFINED).
///      If a specialization is missing for any (opcode, policy) pair,
///      build_handler_table() will fail to compile — providing a static
///      guarantee of complete opcode coverage.
///
///   2. Handler signature is UNIFORM (D3 uniform pipeline):
///        HandlerResult(VmExecution&, VmEpoch&, VmOramState&,
///                      const VmImmutable&, const DecodedInsn&) noexcept
///      All 4 state pieces passed to every handler.  Most handlers ignore
///      what they don't need — this is intentional for D3 uniformity.
///
///   3. ORAM-dependent handlers (PUSH, POP) provide a template member
///      `template<typename Oram> static exec(...)`.  The table builder
///      resolves this by taking the address with the concrete Oram type:
///        &HandlerTraits<PUSH, Policy>::template exec<Oram>
///
///   4. Policy-dependent handlers (ADD/SUB/NEG) have full specializations
///      per policy.  Class A handlers (XOR, AND, etc.) have a partial
///      specialization that works for all policies.
///
///   5. SecurityClass annotation per handler (A/B/C) enables static
///      analysis.  Class A handlers must NEVER construct PlainVal —
///      the phantom type system enforces this.

#include "decoded_insn.hpp"
#include "vm_state.hpp"
#include "vm_policy.hpp"
#include "oram_strategy.hpp"

#include <vm/vm_opcode.hpp>
#include <diagnostic.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <array>

namespace VMPilot::Runtime {

using Common::DiagnosticCode;
using Common::VM::VmOpcode;
using Common::VM::VM_OPCODE_COUNT;

// ─────────────────────────────────────────────────────────────────────────────
// Handler function type
// ─────────────────────────────────────────────────────────────────────────────

/// Result type for all handlers.
using HandlerResult = tl::expected<void, DiagnosticCode>;

/// Uniform handler function signature (D3 pipeline — all handlers look identical).
///
/// All 4 state pieces passed to every handler.  This is intentional:
/// the uniform pipeline (doc 15 §5.2) requires every instruction to
/// execute through the same 12-step template with no data-dependent
/// branches or shortcuts.
using HandlerFn = HandlerResult(*)(
    VmExecution& exec,
    VmEpoch& epoch,
    VmOramState& oram,
    const VmImmutable& imm,
    const DecodedInsn& insn) noexcept;

/// Handler dispatch table: one function pointer per semantic opcode.
using HandlerTable = std::array<HandlerFn, VM_OPCODE_COUNT>;

// ─────────────────────────────────────────────────────────────────────────────
// HandlerTraits<Op, Policy> — primary template (INTENTIONALLY UNDEFINED)
//
// Every (VmOpcode, Policy) pair MUST have a specialization.
// If one is missing, build_handler_table will reference this undefined
// primary template and fail to compile.
// ─────────────────────────────────────────────────────────────────────────────

/// Primary template — intentionally undefined.
///
/// Compile error here means: "Handler not implemented for this (opcode, policy) pair."
template<VmOpcode Op, typename Policy>
struct HandlerTraits;

// ─────────────────────────────────────────────────────────────────────────────
// ORAM-aware handler marker
//
// Handlers that access the ORAM workspace (PUSH, POP) must provide
// a template member `template<typename Oram> static exec(...)` instead
// of a plain `static exec(...)`.  The table builder resolves the Oram
// type when constructing the table.
// ─────────────────────────────────────────────────────────────────────────────

/// Tag type: handler does NOT use ORAM (default).
struct NoOramTag {};

/// Tag type: handler DOES use ORAM (PUSH, POP).
/// HandlerTraits with this tag must provide `template<typename Oram> static exec(...)`.
struct UsesOramTag {};

// ─────────────────────────────────────────────────────────────────────────────
// build_handler_table<Policy, Oram>()
//
// Constructs the dispatch table for a specific (Policy, Oram) combination.
// For each opcode:
//   - If HandlerTraits<Op, Policy>::oram_tag is UsesOramTag:
//       table[Op] = &HandlerTraits<Op, Policy>::template exec<Oram>
//   - Otherwise:
//       table[Op] = &HandlerTraits<Op, Policy>::exec
// ─────────────────────────────────────────────────────────────────────────────

namespace detail {

/// Resolve the handler function pointer for one opcode.
/// Dispatches based on oram_tag to handle ORAM-aware vs regular handlers.
template<VmOpcode Op, typename Policy, typename Oram>
constexpr HandlerFn resolve_handler() noexcept {
    using Traits = HandlerTraits<Op, Policy>;
    if constexpr (std::is_same_v<typename Traits::oram_tag, UsesOramTag>) {
        return &Traits::template exec<Oram>;
    } else {
        return &Traits::exec;
    }
}

}  // namespace detail

/// Build a complete handler dispatch table for the given (Policy, Oram).
///
/// Compile-time guarantee: if ANY HandlerTraits<Op, Policy> specialization
/// is missing, this function fails to compile at the resolve_handler<Op>()
/// call for that opcode.
template<typename Policy, typename Oram>
HandlerTable build_handler_table() noexcept {
    HandlerTable table{};

    // Cat 0: Data Movement (8)
    table[uint8_t(VmOpcode::MOVE)]       = detail::resolve_handler<VmOpcode::MOVE, Policy, Oram>();
    table[uint8_t(VmOpcode::LOAD)]       = detail::resolve_handler<VmOpcode::LOAD, Policy, Oram>();
    table[uint8_t(VmOpcode::STORE)]      = detail::resolve_handler<VmOpcode::STORE, Policy, Oram>();
    table[uint8_t(VmOpcode::PUSH)]       = detail::resolve_handler<VmOpcode::PUSH, Policy, Oram>();
    table[uint8_t(VmOpcode::POP)]        = detail::resolve_handler<VmOpcode::POP, Policy, Oram>();
    table[uint8_t(VmOpcode::LOAD_CONST)] = detail::resolve_handler<VmOpcode::LOAD_CONST, Policy, Oram>();
    table[uint8_t(VmOpcode::LOAD_CTX)]   = detail::resolve_handler<VmOpcode::LOAD_CTX, Policy, Oram>();
    table[uint8_t(VmOpcode::STORE_CTX)]  = detail::resolve_handler<VmOpcode::STORE_CTX, Policy, Oram>();

    // Cat 1: Arithmetic (8)
    table[uint8_t(VmOpcode::ADD)]  = detail::resolve_handler<VmOpcode::ADD, Policy, Oram>();
    table[uint8_t(VmOpcode::SUB)]  = detail::resolve_handler<VmOpcode::SUB, Policy, Oram>();
    table[uint8_t(VmOpcode::MUL)]  = detail::resolve_handler<VmOpcode::MUL, Policy, Oram>();
    table[uint8_t(VmOpcode::IMUL)] = detail::resolve_handler<VmOpcode::IMUL, Policy, Oram>();
    table[uint8_t(VmOpcode::DIV)]  = detail::resolve_handler<VmOpcode::DIV, Policy, Oram>();
    table[uint8_t(VmOpcode::IDIV)] = detail::resolve_handler<VmOpcode::IDIV, Policy, Oram>();
    table[uint8_t(VmOpcode::NEG)]  = detail::resolve_handler<VmOpcode::NEG, Policy, Oram>();
    table[uint8_t(VmOpcode::MOD)]  = detail::resolve_handler<VmOpcode::MOD, Policy, Oram>();

    // Cat 2: Logic (9)
    table[uint8_t(VmOpcode::AND)] = detail::resolve_handler<VmOpcode::AND, Policy, Oram>();
    table[uint8_t(VmOpcode::OR)]  = detail::resolve_handler<VmOpcode::OR, Policy, Oram>();
    table[uint8_t(VmOpcode::XOR)] = detail::resolve_handler<VmOpcode::XOR, Policy, Oram>();
    table[uint8_t(VmOpcode::NOT)] = detail::resolve_handler<VmOpcode::NOT, Policy, Oram>();
    table[uint8_t(VmOpcode::SHL)] = detail::resolve_handler<VmOpcode::SHL, Policy, Oram>();
    table[uint8_t(VmOpcode::SHR)] = detail::resolve_handler<VmOpcode::SHR, Policy, Oram>();
    table[uint8_t(VmOpcode::SAR)] = detail::resolve_handler<VmOpcode::SAR, Policy, Oram>();
    table[uint8_t(VmOpcode::ROL)] = detail::resolve_handler<VmOpcode::ROL, Policy, Oram>();
    table[uint8_t(VmOpcode::ROR)] = detail::resolve_handler<VmOpcode::ROR, Policy, Oram>();

    // Cat 3: Comparison (4)
    table[uint8_t(VmOpcode::CMP)]      = detail::resolve_handler<VmOpcode::CMP, Policy, Oram>();
    table[uint8_t(VmOpcode::TEST)]     = detail::resolve_handler<VmOpcode::TEST, Policy, Oram>();
    table[uint8_t(VmOpcode::SET_FLAG)] = detail::resolve_handler<VmOpcode::SET_FLAG, Policy, Oram>();
    table[uint8_t(VmOpcode::GET_FLAG)] = detail::resolve_handler<VmOpcode::GET_FLAG, Policy, Oram>();

    // Cat 4: Control Flow (6)
    table[uint8_t(VmOpcode::JMP)]         = detail::resolve_handler<VmOpcode::JMP, Policy, Oram>();
    table[uint8_t(VmOpcode::JCC)]         = detail::resolve_handler<VmOpcode::JCC, Policy, Oram>();
    table[uint8_t(VmOpcode::CALL_VM)]     = detail::resolve_handler<VmOpcode::CALL_VM, Policy, Oram>();
    table[uint8_t(VmOpcode::RET_VM)]      = detail::resolve_handler<VmOpcode::RET_VM, Policy, Oram>();
    table[uint8_t(VmOpcode::NATIVE_CALL)] = detail::resolve_handler<VmOpcode::NATIVE_CALL, Policy, Oram>();
    table[uint8_t(VmOpcode::HALT)]        = detail::resolve_handler<VmOpcode::HALT, Policy, Oram>();

    // Cat 5: Width/Extension (8)
    table[uint8_t(VmOpcode::SEXT8)]   = detail::resolve_handler<VmOpcode::SEXT8, Policy, Oram>();
    table[uint8_t(VmOpcode::SEXT16)]  = detail::resolve_handler<VmOpcode::SEXT16, Policy, Oram>();
    table[uint8_t(VmOpcode::SEXT32)]  = detail::resolve_handler<VmOpcode::SEXT32, Policy, Oram>();
    table[uint8_t(VmOpcode::ZEXT8)]   = detail::resolve_handler<VmOpcode::ZEXT8, Policy, Oram>();
    table[uint8_t(VmOpcode::ZEXT16)]  = detail::resolve_handler<VmOpcode::ZEXT16, Policy, Oram>();
    table[uint8_t(VmOpcode::ZEXT32)]  = detail::resolve_handler<VmOpcode::ZEXT32, Policy, Oram>();
    table[uint8_t(VmOpcode::TRUNC8)]  = detail::resolve_handler<VmOpcode::TRUNC8, Policy, Oram>();
    table[uint8_t(VmOpcode::TRUNC16)] = detail::resolve_handler<VmOpcode::TRUNC16, Policy, Oram>();

    // Cat 6: Atomic (5)
    table[uint8_t(VmOpcode::LOCK_ADD)]    = detail::resolve_handler<VmOpcode::LOCK_ADD, Policy, Oram>();
    table[uint8_t(VmOpcode::XCHG)]       = detail::resolve_handler<VmOpcode::XCHG, Policy, Oram>();
    table[uint8_t(VmOpcode::CMPXCHG)]    = detail::resolve_handler<VmOpcode::CMPXCHG, Policy, Oram>();
    table[uint8_t(VmOpcode::FENCE)]      = detail::resolve_handler<VmOpcode::FENCE, Policy, Oram>();
    table[uint8_t(VmOpcode::ATOMIC_LOAD)] = detail::resolve_handler<VmOpcode::ATOMIC_LOAD, Policy, Oram>();

    // Cat 7: VM Internal (7)
    table[uint8_t(VmOpcode::NOP)]             = detail::resolve_handler<VmOpcode::NOP, Policy, Oram>();
    table[uint8_t(VmOpcode::CHECK_INTEGRITY)] = detail::resolve_handler<VmOpcode::CHECK_INTEGRITY, Policy, Oram>();
    table[uint8_t(VmOpcode::CHECK_DEBUG)]     = detail::resolve_handler<VmOpcode::CHECK_DEBUG, Policy, Oram>();
    table[uint8_t(VmOpcode::MUTATE_ISA)]      = detail::resolve_handler<VmOpcode::MUTATE_ISA, Policy, Oram>();
    table[uint8_t(VmOpcode::REKEY)]           = detail::resolve_handler<VmOpcode::REKEY, Policy, Oram>();
    table[uint8_t(VmOpcode::SAVE_EPOCH)]      = detail::resolve_handler<VmOpcode::SAVE_EPOCH, Policy, Oram>();
    table[uint8_t(VmOpcode::RESYNC)]          = detail::resolve_handler<VmOpcode::RESYNC, Policy, Oram>();

    return table;
}

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_HANDLER_TRAITS_HPP__
