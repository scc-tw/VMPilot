#pragma once
#ifndef __RUNTIME_NATIVE_BRIDGE_HPP__
#define __RUNTIME_NATIVE_BRIDGE_HPP__

/// @file native_bridge.hpp
/// @brief Phase 8: Native call bridge API for NATIVE_CALL opcode.
///
/// Plaintext operands exist in CPU registers during the native callee —
/// this is the acknowledged Class C security boundary (D15§11.8).
/// The exposure is register-transient: no addressable memory ever holds
/// plaintext args or return values.  The D3 uniform pipeline makes the
/// NATIVE_CALL handler indistinguishable from other opcodes at the
/// dispatcher level.
///
/// v1: Direct function pointer cast.  The decode→call→encode pattern
///   is visible to a DBI tool that hooks this function.
/// v2 (D15§6.1): Polymorphic stripper stubs with per-call-site (a,b)
///   transition coefficients, disguised as leaf functions.

#include <vm/vm_context.hpp>
#include <diagnostic.hpp>

#include <tl/expected.hpp>

#include <cstdint>

namespace VMPilot::Runtime {

/// Maximum number of arguments for a native call.
/// System V AMD64 ABI supports 6 integer register args;
/// AAPCS64 supports 8. We use 8 as the upper bound.
constexpr uint8_t NATIVE_CALL_MAX_ARGS = 8;

/// Execute a native function call through the legacy bridge (no transition
/// encoding).  Used when coeff_a == 0 (backward compat with old blobs).
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

/// Execute a native function call with ephemeral transition encoding.
///
/// Security property (D15§6.1, strengthened):
///   Each invocation generates a fresh random bijective LUT from
///   BLAKE3(stored_seed, call_site_ip, nonce).  The nonce is monotonic
///   and never repeats.  Therefore:
///
///   - Without stored_seed: each invocation's LUT is computationally
///     indistinguishable from a random permutation (PRF assumption on
///     BLAKE3).  Observing (plain, masked) at invocation i reveals one
///     entry of π_i, which is never reused.  I(π_i; π_{i+1}) = 0.
///
///   - With stored_seed: all LUTs are derivable → game over.  But this
///     is D15§11.8 (stored_seed compromise collapses ALL protections).
///
///   The transition encoding adds NO additional weakness beyond the
///   stored_seed dependency shared by D1/D2/D3/D4.
///
/// @param ctx          VMContext (provides stored_seed + nonce)
/// @param target       native function address
/// @param encoded_args encoded argument values (in register domain)
/// @param arg_regs     register indices for decoding each argument
/// @param arg_count    number of arguments (0-8)
/// @param call_site_ip instruction index of the NATIVE_CALL (for LUT derivation)
/// @return             plaintext return value, or DiagnosticCode on error
[[nodiscard]] tl::expected<uint64_t, Common::DiagnosticCode>
call_native_ephemeral(Common::VM::VMContext& ctx,
                      uintptr_t target,
                      const uint64_t* encoded_args,
                      const uint8_t* arg_regs,
                      uint8_t arg_count,
                      uint32_t call_site_ip) noexcept;

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_NATIVE_BRIDGE_HPP__
