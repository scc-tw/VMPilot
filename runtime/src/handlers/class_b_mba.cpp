/// @file class_b_mba.cpp
/// @brief Cat 1 (partial): MBA arithmetic handlers (ADD, SUB, NEG).
///
/// Security properties:
///   ADD: Mixed Boolean-Arithmetic (MBA) decomposition. ADD(a,b) =
///     XOR(a,b) with carry propagation via MBA_SHL1 (spec S3.4).
///     7 register-transient carry bits per MBA_SHL1 (18x reduction
///     vs native bridge, spec S3.4). The XOR and AND operations
///     are performed entirely in encoded domain via Class A composition
///     tables. Only the carry bit (1-bit, zero MCSP hardness) leaks
///     per lane per iteration.
///
///   SUB, NEG: v1 Class C security level (decode, native op, re-encode).
///     Full MBA SUB = ADD(a, ADD(NOT(b), encoded_1)) requires
///     pre-encoded constant 1 which adds complexity without changing
///     the security level. Defer MBA SUB/NEG to v2.

#include <handlers.hpp>
#include <decoder.hpp>
#include <encoding.hpp>
#include <composition_cache.hpp>

#include <cstring>

namespace VMPilot::Runtime {

extern CompositionCache g_comp_cache;

namespace handlers {

using Common::DiagnosticCode;
using Common::VM::VMContext;

// Pointwise ops for composition table building
static uint8_t op_xor(uint8_t a, uint8_t b) { return a ^ b; }
static uint8_t op_and(uint8_t a, uint8_t b) { return a & b; }

// Cache key IDs (must not collide with class_a_bitwise IDs within same BB)
static constexpr uint8_t ADD_XOR_ID      = 10;
static constexpr uint8_t ADD_AND_ID      = 11;
static constexpr uint8_t ADD_XOR_SELF_ID = 12;
static constexpr uint8_t ADD_AND_SELF_ID = 13;

// ---------------------------------------------------------------------------
// Helper: apply binary comp table to two 64-bit encoded values
// ---------------------------------------------------------------------------

static uint64_t apply_binary(
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
// MBA_SHL1: encoded shift-left by 1 with carry propagation (spec S3.4)
//
// Rev.7 Lane 0 fix: Lane 0 has no carry-in (always 0).
// Branchless dual-table carry injection prevents cache side channels.
// ---------------------------------------------------------------------------

static uint64_t mba_shl1(
    const CompositionCache::MbaEntry& mba,
    uint64_t encoded) noexcept
{
    uint8_t bytes[8], result_bytes[8];
    std::memcpy(bytes, &encoded, 8);

    // Lane 0: no carry-in (rev.7 fix)
    result_bytes[0] = mba.shl1_intra[0][bytes[0]];
    uint8_t carry_out = mba.carry_extract[0][bytes[0]];

    // Lanes 1-7: branchless dual-table carry injection
    for (int k = 1; k < 8; ++k) {
        uint8_t shifted = mba.shl1_intra[k][bytes[k]];
        uint8_t no_carry  = mba.inject_0[k][shifted];   // ALWAYS access
        uint8_t yes_carry = mba.inject_1[k][shifted];    // ALWAYS access (no cache side channel)
        uint8_t mask = static_cast<uint8_t>(-static_cast<int8_t>(carry_out));  // 0x00 or 0xFF
        result_bytes[k] = static_cast<uint8_t>((no_carry & ~mask) | (yes_carry & mask));
        carry_out = mba.carry_extract[k][bytes[k]];
    }

    uint64_t result;
    std::memcpy(&result, result_bytes, 8);
    return result;
}

// ---------------------------------------------------------------------------
// ADD: MBA decomposition
//   s = XOR(a, b);  c = AND(a, b);  c = MBA_SHL1(c);
//   repeat: t = XOR(s, c); c = AND(s, c); c = MBA_SHL1(c); s = t;
//   result = s;
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_add(VMContext& ctx, const DecodedInsn& insn) noexcept {
    // Build composition tables for XOR(reg_a, reg_b) and AND(reg_a, reg_b)
    auto* xor_comp = g_comp_cache.get_binary(
        ADD_XOR_ID, insn.reg_a, insn.reg_a, insn.reg_b, op_xor, ctx);
    auto* and_comp = g_comp_cache.get_binary(
        ADD_AND_ID, insn.reg_a, insn.reg_a, insn.reg_b, op_and, ctx);

    // MBA tables for shift-left-1
    const auto& mba = g_comp_cache.get_mba(insn.reg_a, insn.reg_a, ctx);

    // Initial half-add
    uint64_t s = apply_binary(xor_comp,
                              ctx.encoded_regs[insn.reg_a],
                              ctx.encoded_regs[insn.reg_b]);
    uint64_t c = apply_binary(and_comp,
                              ctx.encoded_regs[insn.reg_a],
                              ctx.encoded_regs[insn.reg_b]);
    c = mba_shl1(mba, c);

    // For XOR(s, c) and AND(s, c) in the carry propagation loop,
    // both operands are in reg_a's domain (s and c are encoded with
    // reg_a's encoding), so we need self-composition tables.
    auto* xor_self = g_comp_cache.get_binary(
        ADD_XOR_SELF_ID, insn.reg_a, insn.reg_a, insn.reg_a, op_xor, ctx);
    auto* and_self = g_comp_cache.get_binary(
        ADD_AND_SELF_ID, insn.reg_a, insn.reg_a, insn.reg_a, op_and, ctx);

    // Carry propagation: 64 iterations for full 64-bit carry chain.
    // The compiler may hint a smaller count via insn.condition if static
    // analysis proves a shorter chain suffices.
    int iterations = (insn.condition == 0) ? 64 : insn.condition;
    for (int i = 0; i < iterations; ++i) {
        uint64_t t = apply_binary(xor_self, s, c);
        c = apply_binary(and_self, s, c);
        c = mba_shl1(mba, c);
        s = t;
    }

    ctx.encoded_regs[insn.reg_a] = s;
    return {};
}

// ---------------------------------------------------------------------------
// SUB: v1 Class C (decode, native subtract, re-encode)
//
// Full MBA SUB = ADD(a, ADD(NOT(b), encoded_1)) requires pre-encoded
// constant 1 which adds complexity without changing the security level
// (ADD itself already exposes 7 carry bits per MBA_SHL1). Defer to v2.
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_sub(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t a = decode_register(ctx, insn.reg_a,
                                  ctx.encoded_regs[insn.reg_a]);
    uint64_t b = decode_register(ctx, insn.reg_b,
                                  ctx.encoded_regs[insn.reg_b]);
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, a - b);
    return {};
}

// ---------------------------------------------------------------------------
// NEG: v1 Class C (decode, negate, re-encode)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_neg(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t a = decode_register(ctx, insn.reg_a,
                                  ctx.encoded_regs[insn.reg_a]);
    ctx.encoded_regs[insn.reg_a] = encode_register(
        ctx, insn.reg_a, static_cast<uint64_t>(-static_cast<int64_t>(a)));
    return {};
}

}  // namespace handlers
}  // namespace VMPilot::Runtime
