/// @file native_call_bridge.cpp
/// @brief Phase 8: C++ native call bridge implementation.
///
/// Orchestrates: decode encoded args -> call native function -> return result.
///
/// Security properties:
///   Plaintext arguments exist only in CPU registers for the duration
///   of the native function call (register-transient exposure).
///   This is the same security model as Class C operations (MUL/DIV/MOD).
///
/// v1: Direct function pointer invocation. The C++ compiler generates
///   the correct System V AMD64 / AAPCS64 calling convention automatically.
///   No custom assembly needed.
///
/// v2 (future): Polymorphic stripper stubs per call-site with (a,b)
///   transition coefficients for encoding transitions and steganographic
///   embedding disguised as leaf functions (spec S6.1-6.4).

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
