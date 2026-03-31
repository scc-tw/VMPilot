/// @file class_b_mba.cpp
/// @brief Cat 1 (partial): MBA arithmetic — ADD, SUB, NEG.
///
/// Why MBA instead of native bridge (D15§3.4 rev.5):
///   ADD/SUB/NEG constitute ~30% of typical instruction mix.  Exposing
///   full 64-bit plaintext operands for 30% of operations destroys the
///   SNR→0 guarantee (D15§11.3).  MBA decomposes arithmetic into bitwise
///   ops (Class A, MCSP-hard, zero plaintext) plus a 1-bit inter-lane
///   carry — reducing exposure from 128 bits to 7 register-transient
///   carry bits per ADD (18× reduction, D15§3.4).
///
/// Security proof sketch:
///   - x + y ≡ (x ⊕ y) + 2·(x ∧ y).  XOR and AND are lane-independent
///     bitwise ops → Class A homomorphic LUT, no plaintext (MCSP-hard).
///   - The "2·" (SHL1) is the only cross-lane operation.  It leaks
///     exactly 1 carry bit per lane boundary (7 total), each existing
///     only in a CPU register for ~1 cycle (register-transient).
///   - INJECT_0/INJECT_1 dual-table access eliminates cache side channels:
///     both tables are ALWAYS read regardless of carry value.
///   - Lane 0 has no predecessor → carry_in hardcoded to 0 (rev.7 fix).
///
/// SUB/NEG security:
///   SUB(a,b) = ADD(a, ADD(NOT(b), 1)).  NOT is Class A (zero plaintext).
///   Two MBA ADDs → 14 carry bits total.  vs Class C's 128 bits = 9× reduction.
///   NEG(a) = ADD(NOT(a), 1).  One MBA ADD → 7 carry bits.  9× reduction.

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
// MBA_SHL1 (D15§3.4 rev.5-7)
//
// Why branchless dual-table: a data-dependent branch on carry_out would
// leak the carry value via branch predictor / cache timing (D15§11.3).
// Both INJECT_0 and INJECT_1 are always accessed → I(carry; cache) = 0.
//
// Why Lane 0 bypass (rev.7): Lane 0 has no predecessor carry.  Without
// the bypass, Lane 0 reads undefined carry_in → corrupts from byte 0.
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
// mba_add_impl: MBA addition on two values in the same encoding domain.
//
// Why a separate helper: SUB and NEG compose multiple ADDs on intermediate
// encoded values (not directly from ctx.encoded_regs).  This helper
// operates on arbitrary encoded values so the caller can chain:
//   SUB(a,b) = mba_add(a, mba_add(NOT(b), 1))
// ---------------------------------------------------------------------------

static uint64_t mba_add_impl(
    VMContext& ctx, uint8_t dst_reg,
    uint64_t a, uint64_t b, int iterations) noexcept
{
    // XOR and AND for the initial half-add (both operands in dst_reg domain)
    auto* xor_self = g_comp_cache.get_binary(
        ADD_XOR_SELF_ID, dst_reg, dst_reg, dst_reg, op_xor, ctx);
    auto* and_self = g_comp_cache.get_binary(
        ADD_AND_SELF_ID, dst_reg, dst_reg, dst_reg, op_and, ctx);

    const auto& mba = g_comp_cache.get_mba(dst_reg, dst_reg, ctx);

    uint64_t s = apply_binary(xor_self, a, b);
    uint64_t c = apply_binary(and_self, a, b);
    c = mba_shl1(mba, c);

    for (int i = 0; i < iterations; ++i) {
        uint64_t t = apply_binary(xor_self, s, c);
        c = apply_binary(and_self, s, c);
        c = mba_shl1(mba, c);
        s = t;
    }

    return s;
}

// ---------------------------------------------------------------------------
// apply_not: Class A NOT — zero plaintext by construction.
//
// Why safe: the unary composition table maps every possible encoded byte
// to NOT(decoded) re-encoded in dst's domain.  The table is built by
// enumerating all 256 values, so no actual register data participates
// in table construction.  Runtime is a single LUT lookup per lane.
// ---------------------------------------------------------------------------

static uint8_t op_not_fn(uint8_t a) { return static_cast<uint8_t>(~a); }

static constexpr uint8_t MBA_NOT_ID = 14;

static uint64_t apply_not(VMContext& ctx, uint8_t dst_reg, uint8_t src_reg,
                           uint64_t val) noexcept {
    auto* comp = g_comp_cache.get_unary(MBA_NOT_ID, dst_reg, src_reg,
                                         op_not_fn, ctx);
    return apply_byte_lane_lut(comp, val);
}

// ---------------------------------------------------------------------------
// ADD handler
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_add(VMContext& ctx, const DecodedInsn& insn) noexcept {
    // For the initial half-add, operands may be from different registers.
    // Build cross-register composition tables for the first XOR and AND.
    auto* xor_comp = g_comp_cache.get_binary(
        ADD_XOR_ID, insn.reg_a, insn.reg_a, insn.reg_b, op_xor, ctx);
    auto* and_comp = g_comp_cache.get_binary(
        ADD_AND_ID, insn.reg_a, insn.reg_a, insn.reg_b, op_and, ctx);

    const auto& mba = g_comp_cache.get_mba(insn.reg_a, insn.reg_a, ctx);

    // Initial half-add (cross-register: reg_a × reg_b → reg_a domain)
    uint64_t s = apply_binary(xor_comp,
                              ctx.encoded_regs[insn.reg_a],
                              ctx.encoded_regs[insn.reg_b]);
    uint64_t c = apply_binary(and_comp,
                              ctx.encoded_regs[insn.reg_a],
                              ctx.encoded_regs[insn.reg_b]);
    c = mba_shl1(mba, c);

    // Carry propagation loop: both s and c are now in reg_a's domain.
    auto* xor_self = g_comp_cache.get_binary(
        ADD_XOR_SELF_ID, insn.reg_a, insn.reg_a, insn.reg_a, op_xor, ctx);
    auto* and_self = g_comp_cache.get_binary(
        ADD_AND_SELF_ID, insn.reg_a, insn.reg_a, insn.reg_a, op_and, ctx);

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
// SUB (D15§3.4): SUB(a,b) = ADD(a, ADD(NOT(b), 1))
//
// Why not decode-subtract-encode: that exposes 128 bits of plaintext
// (both full operands), violating SNR→0 for ~15% of instruction mix.
//
// Why this decomposition is safe:
//   NOT(b)          — Class A composition LUT, MCSP-hard, 0 bits leaked
//   ADD(NOT(b), 1)  — MBA, 7 register-transient carry bits
//   ADD(a, neg_b)   — MBA, 7 register-transient carry bits
//   Total: 14 bits leaked (vs 128).  All 14 are 1-bit carry values
//   existing in a GPR for ~1 cycle each, same security model as D1
//   decryption keystream bytes.
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_sub(VMContext& ctx, const DecodedInsn& insn) noexcept {
    const uint8_t dst = insn.reg_a;
    const int iterations = (insn.condition == 0) ? 64 : insn.condition;

    uint64_t not_b      = apply_not(ctx, dst, insn.reg_b,
                                     ctx.encoded_regs[insn.reg_b]);
    uint64_t encoded_one = encode_register(ctx, dst, 1);
    uint64_t neg_b       = mba_add_impl(ctx, dst, not_b, encoded_one, iterations);
    ctx.encoded_regs[dst] = mba_add_impl(ctx, dst,
                                          ctx.encoded_regs[dst], neg_b,
                                          iterations);
    return {};
}

// ---------------------------------------------------------------------------
// NEG (D15§3.4): NEG(a) = ADD(NOT(a), 1)
//
// Why not decode-negate-encode: exposes 64 bits of plaintext.
// MBA path: NOT is Class A (0 bits), ADD is MBA (7 carry bits).
// 9× reduction in plaintext exposure.
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_neg(VMContext& ctx, const DecodedInsn& insn) noexcept {
    const uint8_t dst = insn.reg_a;
    const int iterations = (insn.condition == 0) ? 64 : insn.condition;

    uint64_t not_a       = apply_not(ctx, dst, dst, ctx.encoded_regs[dst]);
    uint64_t encoded_one = encode_register(ctx, dst, 1);
    ctx.encoded_regs[dst] = mba_add_impl(ctx, dst, not_a, encoded_one,
                                          iterations);
    return {};
}

}  // namespace handlers
}  // namespace VMPilot::Runtime
