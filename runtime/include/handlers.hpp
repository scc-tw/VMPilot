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

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_HANDLERS_HPP__
