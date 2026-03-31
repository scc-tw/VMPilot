/// @file native_call_bridge.cpp
/// @brief Phase 8: C++ native call bridge.
///
/// Security analysis (D15§6, D15§11.7, D15§11.8):
///
///   This is the lowest-security code path in the VM by design.
///   Plaintext operands exist in CPU registers for the duration of
///   the native callee.  This is unavoidable: external functions
///   (libc, OS APIs) require plaintext by definition.
///
///   Properties that DO hold at this boundary:
///     - D1 enc_state chain: NATIVE_CALL participates in the SipHash
///       chain.  Removing it desyncs all subsequent decryption.
///     - D3 uniform pipeline: the dispatcher executes the same 12
///       steps for NATIVE_CALL as for NOP or XOR — no distinguishing
///       side channel from the pipeline's perspective.
///     - D2 domain re-encoding: the return value is immediately
///       re-encoded into the register domain.  The plaintext return
///       value's lifetime is bounded by one encode_register() call.
///     - Reentrancy isolation: no global state.  100-level nesting
///       verified.
///
///   Properties that do NOT hold (acknowledged in D15§11.8):
///     - MCSP hardness: plaintext operands are fully exposed.
///     - SNR→0 within the handler: the decode→call→encode pattern
///       is distinguishable from Class A operations to a DBI tracer
///       that instruments call_native() itself.
///
///   v2 mitigation (D15§6.1): Polymorphic Stripper stubs will add
///   per-call-site (a,b) linear transition coefficients.  The encoding
///   domain transition is disguised as arithmetic in a leaf function,
///   making the decode point indistinguishable from unrelated code.
///   Until then, the security boundary here is Class C level.

#include <native_bridge.hpp>
#include <encoding.hpp>

namespace VMPilot::Runtime {

using Common::DiagnosticCode;
using Common::VM::VMContext;

tl::expected<uint64_t, DiagnosticCode>
call_native(VMContext& ctx,
            uintptr_t target,
            const uint64_t* encoded_args,
            const uint8_t* arg_regs,
            uint8_t arg_count) noexcept {

    // Validate inputs
    if (target == 0)
        return tl::make_unexpected(DiagnosticCode::NativeCallBridgeFailed);

    if (arg_count > NATIVE_CALL_MAX_ARGS)
        return tl::make_unexpected(DiagnosticCode::NativeCallBridgeFailed);

    // Decode arguments from register domain to plaintext.
    // Plaintext lives only in this stack frame and CPU registers.
    uint64_t plain_args[NATIVE_CALL_MAX_ARGS] = {};
    for (uint8_t i = 0; i < arg_count; ++i) {
        plain_args[i] = decode_register(ctx, arg_regs[i], encoded_args[i]);
    }

    // Cast target to function pointer and call.
    // v1: Always pass all slots (unused args are zero).
    // The compiler generates the correct calling convention (System V / AAPCS64).
    auto fn = reinterpret_cast<uint64_t(*)(
        uint64_t, uint64_t, uint64_t, uint64_t,
        uint64_t, uint64_t, uint64_t, uint64_t)>(target);

    uint64_t result = fn(plain_args[0], plain_args[1],
                         plain_args[2], plain_args[3],
                         plain_args[4], plain_args[5],
                         plain_args[6], plain_args[7]);

    return result;
}

}  // namespace VMPilot::Runtime
