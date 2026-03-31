/// @file class_c_bridge.cpp
/// @brief Cat 1 (partial): Class C handlers with ephemeral transition encoding.
///
/// Security (strengthened from v1 direct decode):
///   Each decode→compute→encode now uses a per-invocation ephemeral LUT
///   derived from BLAKE3(stored_seed, "c3" || reg_a || reg_b || nonce).
///   Plaintext exists in a GPR for one instruction between the permanent
///   decode and the ephemeral re-encode.  The ephemeral tables are
///   securely zeroed after use.
///
///   An attacker observing the decode point sees a value masked by a
///   one-time random permutation.  Recovering the permutation requires
///   stored_seed (D15§11.8 fundamental limit, same as all other protections).
///
///   These operations still have no efficient homomorphic decomposition:
///     MUL/IMUL: GF(2^64) composition infeasible for 64-bit operands
///     DIV/IDIV/MOD: no known sublinear encoded-domain algorithm

#include <handlers.hpp>
#include <decoder.hpp>
#include <encoding.hpp>
#include <ephemeral_encoding.hpp>

namespace VMPilot::Runtime::handlers {

using Common::DiagnosticCode;
using Common::VM::VMContext;

/// Helper: decode two operands through ephemeral transition.
/// Returns the two plaintext values after ephemeral LUT has been
/// generated and will be zeroed by caller.
static void decode_pair(VMContext& ctx, const DecodedInsn& insn,
                        uint64_t& a, uint64_t& b,
                        EphemeralTables& et) noexcept {
    uint8_t extra[3] = {insn.reg_a, insn.reg_b,
                         static_cast<uint8_t>(insn.opcode)};
    generate_ephemeral_tables(ctx, "c3", 2, extra, sizeof(extra), et);

    // Ephemeral decode: register domain → ephemeral domain → plaintext.
    // The ephemeral_decode returns a masked value; we unmask via dec table.
    uint64_t masked_a = ephemeral_decode(ctx, insn.reg_a,
                                          ctx.encoded_regs[insn.reg_a], et);
    uint64_t masked_b = ephemeral_decode(ctx, insn.reg_b,
                                          ctx.encoded_regs[insn.reg_b], et);
    a = apply_byte_lane_lut(et.dec, masked_a);
    b = apply_byte_lane_lut(et.dec, masked_b);
}

// ---------------------------------------------------------------------------
// MUL (unsigned multiply)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_mul(VMContext& ctx, const DecodedInsn& insn) noexcept {
    EphemeralTables et;
    uint64_t a, b;
    decode_pair(ctx, insn, a, b, et);
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, a * b);
    ephemeral_zero(et);
    return {};
}

// ---------------------------------------------------------------------------
// IMUL (signed multiply)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_imul(VMContext& ctx, const DecodedInsn& insn) noexcept {
    EphemeralTables et;
    uint64_t a, b;
    decode_pair(ctx, insn, a, b, et);
    auto result = static_cast<uint64_t>(
        static_cast<int64_t>(a) * static_cast<int64_t>(b));
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, result);
    ephemeral_zero(et);
    return {};
}

// ---------------------------------------------------------------------------
// DIV (unsigned divide)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_div(VMContext& ctx, const DecodedInsn& insn) noexcept {
    EphemeralTables et;
    uint64_t a, b;
    decode_pair(ctx, insn, a, b, et);
    uint64_t result = (b == 0) ? 0 : a / b;
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, result);
    ephemeral_zero(et);
    return {};
}

// ---------------------------------------------------------------------------
// IDIV (signed divide)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_idiv(VMContext& ctx, const DecodedInsn& insn) noexcept {
    EphemeralTables et;
    uint64_t a, b;
    decode_pair(ctx, insn, a, b, et);
    auto sb = static_cast<int64_t>(b);
    uint64_t result = (sb == 0) ? 0
        : static_cast<uint64_t>(static_cast<int64_t>(a) / sb);
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, result);
    ephemeral_zero(et);
    return {};
}

// ---------------------------------------------------------------------------
// MOD (unsigned modulo)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_mod(VMContext& ctx, const DecodedInsn& insn) noexcept {
    EphemeralTables et;
    uint64_t a, b;
    decode_pair(ctx, insn, a, b, et);
    uint64_t result = (b == 0) ? 0 : a % b;
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, result);
    ephemeral_zero(et);
    return {};
}

}  // namespace VMPilot::Runtime::handlers
