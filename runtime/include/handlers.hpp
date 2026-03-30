#pragma once
#ifndef __RUNTIME_HANDLERS_HPP__
#define __RUNTIME_HANDLERS_HPP__

#include <vm/vm_context.hpp>
#include <vm/vm_opcode.hpp>
#include <diagnostic.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <array>

namespace VMPilot::Runtime {

struct DecodedInsn;  // forward declaration

/// Handler function signature.
///
/// Each handler receives the VMContext and the decoded instruction,
/// performs the operation, and returns success or a DiagnosticCode.
using HandlerFn = tl::expected<void, Common::DiagnosticCode>
    (*)(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

/// Handler dispatch table: one function pointer per semantic opcode.
using HandlerTable = std::array<HandlerFn, Common::VM::VM_OPCODE_COUNT>;

/// Initialize the handler dispatch table with all handler function pointers.
///
/// @return  the populated handler table
[[nodiscard]] HandlerTable init_handler_table() noexcept;

/// Clear the global composition table cache.
///
/// Must be called on every BB transition (enter_basic_block) because
/// encoding tables change per-epoch and cached composition tables
/// become invalid.
void clear_composition_cache() noexcept;

// ---------------------------------------------------------------------------
// Individual handler declarations (one per opcode)
// ---------------------------------------------------------------------------

namespace handlers {

// Fallback for unimplemented opcodes
tl::expected<void, Common::DiagnosticCode>
stub_handler(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

// -- Cat 0: Data Movement (8) -- data_movement.cpp
tl::expected<void, Common::DiagnosticCode>
handle_move(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_load(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_store(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_push(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_pop(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_load_const(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_load_ctx(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_store_ctx(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

// -- Cat 1: Arithmetic (8) -- class_b_mba.cpp + class_c_bridge.cpp
tl::expected<void, Common::DiagnosticCode>
handle_add(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_sub(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_mul(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_imul(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_div(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_idiv(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_neg(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_mod(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

// -- Cat 2: Logic (9) -- class_a_bitwise.cpp
tl::expected<void, Common::DiagnosticCode>
handle_and(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_or(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_xor(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_not(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_shl(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_shr(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_sar(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_rol(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_ror(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

// -- Cat 3: Comparison (4) -- comparison.cpp
tl::expected<void, Common::DiagnosticCode>
handle_cmp(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_test(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_set_flag(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_get_flag(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

// -- Cat 4: Control Flow (6) -- control_flow.cpp
tl::expected<void, Common::DiagnosticCode>
handle_jmp(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_jcc(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_call_vm(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_ret_vm(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_native_call(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_halt(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

// -- Cat 5: Width/Extension (8) -- width_extension.cpp
tl::expected<void, Common::DiagnosticCode>
handle_sext8(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_sext16(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_sext32(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_zext8(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_zext16(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_zext32(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_trunc8(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_trunc16(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

// -- Cat 6: Atomic (5) -- atomic.cpp
tl::expected<void, Common::DiagnosticCode>
handle_lock_add(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_xchg(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_cmpxchg(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_fence(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_atomic_load(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

// -- Cat 7: VM Internal (7) -- vm_internal.cpp
tl::expected<void, Common::DiagnosticCode>
handle_nop(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_check_integrity(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_check_debug(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_mutate_isa(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_rekey(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_save_epoch(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

tl::expected<void, Common::DiagnosticCode>
handle_resync(Common::VM::VMContext& ctx, const DecodedInsn& insn) noexcept;

}  // namespace handlers

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_HANDLERS_HPP__
