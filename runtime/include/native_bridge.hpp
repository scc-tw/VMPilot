#pragma once
#ifndef __RUNTIME_NATIVE_BRIDGE_HPP__
#define __RUNTIME_NATIVE_BRIDGE_HPP__

/// @file native_bridge.hpp
/// @brief Phase 8: Native call bridge API for NATIVE_CALL opcode.
///
/// The bridge orchestrates: decode args -> call native function -> return result.
/// Plaintext exists transiently in CPU registers during the call.
///
/// v1: Direct function pointer invocation. The compiler generates correct
/// calling conventions automatically.
///
/// v2 will add polymorphic stripper stubs per call-site with (a,b)
/// transition coefficients and steganographic embedding (spec S6.1-6.4).

#include <vm/vm_context.hpp>
#include <diagnostic.hpp>

#include <tl/expected.hpp>

#include <cstdint>

namespace VMPilot::Runtime {

/// Maximum number of arguments for a native call.
/// System V AMD64 ABI supports 6 integer register args;
/// AAPCS64 supports 8. We use 8 as the upper bound.
constexpr uint8_t NATIVE_CALL_MAX_ARGS = 8;

/// Execute a native function call through the bridge.
///
/// Decodes encoded arguments to plaintext (register-transient),
/// calls the native function via function pointer, and returns
/// the plaintext result. The caller is responsible for re-encoding
/// the result into the register domain.
///
/// Plaintext exists transiently in CPU registers during the call.
/// Future v2 will add polymorphic stripper stubs to disguise the
/// decode-call-encode pattern (spec S6.1-6.4).
///
/// @param ctx          VMContext for operand decoding
/// @param target       native function address
/// @param encoded_args encoded argument values (in register domain)
/// @param arg_regs     register indices for decoding each argument
/// @param arg_count    number of arguments (0-8)
/// @return             plaintext return value, or DiagnosticCode on error
[[nodiscard]] tl::expected<uint64_t, Common::DiagnosticCode>
call_native(Common::VM::VMContext& ctx,
            uintptr_t target,
            const uint64_t* encoded_args,
            const uint8_t* arg_regs,
            uint8_t arg_count) noexcept;

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_NATIVE_BRIDGE_HPP__
