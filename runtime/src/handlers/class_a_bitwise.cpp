/// @file class_a_bitwise.cpp
/// @brief Cat 2: Bitwise Logic handlers (9 opcodes).
///
/// Security properties:
///   AND, OR, XOR, NOT: Zero plaintext exposure via homomorphic
///     composition LUT (MCSP-hard, spec S3.4). The composition table
///     maps (encoded_a, encoded_b) -> encoded_result with no
///     plaintext intermediate in any CPU register.
///
///   SHL, SHR, SAR, ROL, ROR: v1 Class C security level. Cross-lane
///     bit movement (shifts >= 8) prevents pure-LUT implementation.
///     Full 64-bit plaintext exposure (register-transient), same
///     security as Class C bridge operations.

#include <handlers.hpp>
#include <decoder.hpp>
#include <encoding.hpp>
#include <composition_cache.hpp>
#include <ephemeral_encoding.hpp>

#include <cstring>

namespace VMPilot::Runtime {

// Global composition cache (defined in handlers.cpp)
extern CompositionCache g_comp_cache;

namespace handlers {

using Common::DiagnosticCode;
using Common::VM::VMContext;

// ---------------------------------------------------------------------------
// Pointwise operation functions for composition table construction
// ---------------------------------------------------------------------------

static uint8_t op_and(uint8_t a, uint8_t b) { return a & b; }
static uint8_t op_or(uint8_t a, uint8_t b)  { return a | b; }
static uint8_t op_xor(uint8_t a, uint8_t b) { return a ^ b; }
static uint8_t op_not(uint8_t a)             { return static_cast<uint8_t>(~a); }

// Operation IDs for cache keying (arbitrary but unique)
static constexpr uint8_t AND_OP_ID = 0;
static constexpr uint8_t OR_OP_ID  = 1;
static constexpr uint8_t XOR_OP_ID = 2;
static constexpr uint8_t NOT_OP_ID = 3;

// ---------------------------------------------------------------------------
// Class A binary helper: apply comp[8][256][256] to two 64-bit encoded vals
// ---------------------------------------------------------------------------

static uint64_t apply_binary_comp(
    const uint8_t comp[][256][256],
    uint64_t a, uint64_t b) noexcept
{
    uint64_t result = 0;
    for (int k = 0; k < 8; ++k) {
        uint8_t la = static_cast<uint8_t>(a >> (k * 8));
        uint8_t lb = static_cast<uint8_t>(b >> (k * 8));
        result |= static_cast<uint64_t>(comp[k][la][lb]) << (k * 8);
    }
    return result;
}

// ---------------------------------------------------------------------------
// Class A binary operation template
// ---------------------------------------------------------------------------

static tl::expected<void, DiagnosticCode>
class_a_binary(VMContext& ctx, const DecodedInsn& insn,
               uint8_t op_id, uint8_t (*op_fn)(uint8_t, uint8_t)) noexcept {
    auto* comp = g_comp_cache.get_binary(
        op_id, insn.reg_a, insn.reg_a, insn.reg_b, op_fn, ctx);
    ctx.encoded_regs[insn.reg_a] = apply_binary_comp(
        comp, ctx.encoded_regs[insn.reg_a], ctx.encoded_regs[insn.reg_b]);
    return {};
}

// ---------------------------------------------------------------------------
// AND
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_and(VMContext& ctx, const DecodedInsn& insn) noexcept {
    return class_a_binary(ctx, insn, AND_OP_ID, op_and);
}

// ---------------------------------------------------------------------------
// OR
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_or(VMContext& ctx, const DecodedInsn& insn) noexcept {
    return class_a_binary(ctx, insn, OR_OP_ID, op_or);
}

// ---------------------------------------------------------------------------
// XOR
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_xor(VMContext& ctx, const DecodedInsn& insn) noexcept {
    return class_a_binary(ctx, insn, XOR_OP_ID, op_xor);
}

// ---------------------------------------------------------------------------
// NOT (unary)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_not(VMContext& ctx, const DecodedInsn& insn) noexcept {
    auto* comp = g_comp_cache.get_unary(
        NOT_OP_ID, insn.reg_a, insn.reg_a, op_not, ctx);
    ctx.encoded_regs[insn.reg_a] = apply_byte_lane_lut(
        comp, ctx.encoded_regs[insn.reg_a]);
    return {};
}

// ---------------------------------------------------------------------------
// Shift / Rotate — ephemeral transition encoding.
//
// Cross-lane bit movement prevents pure-LUT composition (shifts ≥ 8 bits
// move data between byte lanes).  Each decode→shift→encode now uses a
// per-invocation ephemeral LUT so an attacker observing the decode point
// sees a one-time random permutation (same security model as NATIVE_CALL).
// ---------------------------------------------------------------------------

/// Helper: decode two operands through ephemeral LUT for shift/rotate ops.
static void decode_shift_pair(VMContext& ctx, const DecodedInsn& insn,
                               uint64_t& val, uint64_t& amt,
                               EphemeralTables& et) noexcept {
    uint8_t extra[3] = {insn.reg_a, insn.reg_b,
                         static_cast<uint8_t>(insn.opcode)};
    generate_ephemeral_tables(ctx, "sh", 2, extra, sizeof(extra), et);
    uint64_t masked_v = ephemeral_decode(ctx, insn.reg_a,
                                          ctx.encoded_regs[insn.reg_a], et);
    uint64_t masked_a = ephemeral_decode(ctx, insn.reg_b,
                                          ctx.encoded_regs[insn.reg_b], et);
    val = apply_byte_lane_lut(et.dec, masked_v);
    amt = apply_byte_lane_lut(et.dec, masked_a) & 63;
}

tl::expected<void, DiagnosticCode>
handle_shl(VMContext& ctx, const DecodedInsn& insn) noexcept {
    EphemeralTables et;
    uint64_t val, amt;
    decode_shift_pair(ctx, insn, val, amt, et);
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, val << amt);
    ephemeral_zero(et);
    return {};
}

tl::expected<void, DiagnosticCode>
handle_shr(VMContext& ctx, const DecodedInsn& insn) noexcept {
    EphemeralTables et;
    uint64_t val, amt;
    decode_shift_pair(ctx, insn, val, amt, et);
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, val >> amt);
    ephemeral_zero(et);
    return {};
}

tl::expected<void, DiagnosticCode>
handle_sar(VMContext& ctx, const DecodedInsn& insn) noexcept {
    EphemeralTables et;
    uint64_t val, amt;
    decode_shift_pair(ctx, insn, val, amt, et);
    auto sval = static_cast<int64_t>(val);
    sval >>= amt;
    ctx.encoded_regs[insn.reg_a] = encode_register(
        ctx, insn.reg_a, static_cast<uint64_t>(sval));
    ephemeral_zero(et);
    return {};
}

tl::expected<void, DiagnosticCode>
handle_rol(VMContext& ctx, const DecodedInsn& insn) noexcept {
    EphemeralTables et;
    uint64_t val, amt;
    decode_shift_pair(ctx, insn, val, amt, et);
    uint64_t result = (amt == 0) ? val : ((val << amt) | (val >> (64 - amt)));
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, result);
    ephemeral_zero(et);
    return {};
}

tl::expected<void, DiagnosticCode>
handle_ror(VMContext& ctx, const DecodedInsn& insn) noexcept {
    EphemeralTables et;
    uint64_t val, amt;
    decode_shift_pair(ctx, insn, val, amt, et);
    uint64_t result = (amt == 0) ? val : ((val >> amt) | (val << (64 - amt)));
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, result);
    ephemeral_zero(et);
    return {};
}

}  // namespace handlers
}  // namespace VMPilot::Runtime
