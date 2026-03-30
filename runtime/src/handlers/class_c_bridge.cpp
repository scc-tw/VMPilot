/// @file class_c_bridge.cpp
/// @brief Cat 1 (partial): Class C native bridge handlers (MUL, IMUL, DIV, IDIV, MOD).
///
/// Security properties:
///   Full plaintext register-transient; acceptable for rare operations
///   (~5% of dynamic instruction mix, spec S11.8). These operations
///   have no known efficient homomorphic or MBA decomposition:
///     - MUL/IMUL: GF(2^64) multiplication requires O(n^2) composition
///       tables, which is infeasible for 64-bit operands.
///     - DIV/IDIV/MOD: No known sublinear encoded-domain algorithm.
///
///   The native bridge pattern: decode -> native op -> re-encode.
///   Plaintext exists only in CPU registers for the duration of the
///   native operation (register-transient exposure).

#include <handlers.hpp>
#include <decoder.hpp>
#include <encoding.hpp>

namespace VMPilot::Runtime::handlers {

using Common::DiagnosticCode;
using Common::VM::VMContext;

// ---------------------------------------------------------------------------
// MUL (unsigned multiply)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_mul(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t a = decode_register(ctx, insn.reg_a,
                                  ctx.encoded_regs[insn.reg_a]);
    uint64_t b = decode_register(ctx, insn.reg_b,
                                  ctx.encoded_regs[insn.reg_b]);
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, a * b);
    return {};
}

// ---------------------------------------------------------------------------
// IMUL (signed multiply)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_imul(VMContext& ctx, const DecodedInsn& insn) noexcept {
    auto sa = static_cast<int64_t>(
        decode_register(ctx, insn.reg_a, ctx.encoded_regs[insn.reg_a]));
    auto sb = static_cast<int64_t>(
        decode_register(ctx, insn.reg_b, ctx.encoded_regs[insn.reg_b]));
    ctx.encoded_regs[insn.reg_a] = encode_register(
        ctx, insn.reg_a, static_cast<uint64_t>(sa * sb));
    return {};
}

// ---------------------------------------------------------------------------
// DIV (unsigned divide)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_div(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t a = decode_register(ctx, insn.reg_a,
                                  ctx.encoded_regs[insn.reg_a]);
    uint64_t b = decode_register(ctx, insn.reg_b,
                                  ctx.encoded_regs[insn.reg_b]);
    if (b == 0) {
        // v1: silent zero on div-by-zero (no exception model yet)
        ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, 0);
        return {};
    }
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, a / b);
    return {};
}

// ---------------------------------------------------------------------------
// IDIV (signed divide)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_idiv(VMContext& ctx, const DecodedInsn& insn) noexcept {
    auto sa = static_cast<int64_t>(
        decode_register(ctx, insn.reg_a, ctx.encoded_regs[insn.reg_a]));
    auto sb = static_cast<int64_t>(
        decode_register(ctx, insn.reg_b, ctx.encoded_regs[insn.reg_b]));
    if (sb == 0) {
        ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, 0);
        return {};
    }
    ctx.encoded_regs[insn.reg_a] = encode_register(
        ctx, insn.reg_a, static_cast<uint64_t>(sa / sb));
    return {};
}

// ---------------------------------------------------------------------------
// MOD (unsigned modulo)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_mod(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t a = decode_register(ctx, insn.reg_a,
                                  ctx.encoded_regs[insn.reg_a]);
    uint64_t b = decode_register(ctx, insn.reg_b,
                                  ctx.encoded_regs[insn.reg_b]);
    if (b == 0) {
        ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, 0);
        return {};
    }
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, a % b);
    return {};
}

}  // namespace VMPilot::Runtime::handlers
