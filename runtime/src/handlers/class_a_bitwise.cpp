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
// Shift / Rotate helpers
//
// v1: Class C security level. Cross-lane bit movement makes pure-LUT
// infeasible for shifts >= 8.  Full 64-bit plaintext exposure
// (register-transient).
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_shl(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t plain_val = decode_register(ctx, insn.reg_a,
                                          ctx.encoded_regs[insn.reg_a]);
    uint64_t shift_amt = decode_register(ctx, insn.reg_b,
                                          ctx.encoded_regs[insn.reg_b]);
    plain_val <<= (shift_amt & 63);
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, plain_val);
    return {};
}

tl::expected<void, DiagnosticCode>
handle_shr(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t plain_val = decode_register(ctx, insn.reg_a,
                                          ctx.encoded_regs[insn.reg_a]);
    uint64_t shift_amt = decode_register(ctx, insn.reg_b,
                                          ctx.encoded_regs[insn.reg_b]);
    plain_val >>= (shift_amt & 63);
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, plain_val);
    return {};
}

tl::expected<void, DiagnosticCode>
handle_sar(VMContext& ctx, const DecodedInsn& insn) noexcept {
    auto plain_val = static_cast<int64_t>(
        decode_register(ctx, insn.reg_a, ctx.encoded_regs[insn.reg_a]));
    uint64_t shift_amt = decode_register(ctx, insn.reg_b,
                                          ctx.encoded_regs[insn.reg_b]);
    plain_val >>= (shift_amt & 63);
    ctx.encoded_regs[insn.reg_a] = encode_register(
        ctx, insn.reg_a, static_cast<uint64_t>(plain_val));
    return {};
}

tl::expected<void, DiagnosticCode>
handle_rol(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t val = decode_register(ctx, insn.reg_a,
                                    ctx.encoded_regs[insn.reg_a]);
    uint64_t amt = decode_register(ctx, insn.reg_b,
                                    ctx.encoded_regs[insn.reg_b]) & 63;
    uint64_t result = (amt == 0) ? val : ((val << amt) | (val >> (64 - amt)));
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, result);
    return {};
}

tl::expected<void, DiagnosticCode>
handle_ror(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t val = decode_register(ctx, insn.reg_a,
                                    ctx.encoded_regs[insn.reg_a]);
    uint64_t amt = decode_register(ctx, insn.reg_b,
                                    ctx.encoded_regs[insn.reg_b]) & 63;
    uint64_t result = (amt == 0) ? val : ((val >> amt) | (val << (64 - amt)));
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, result);
    return {};
}

}  // namespace handlers
}  // namespace VMPilot::Runtime
