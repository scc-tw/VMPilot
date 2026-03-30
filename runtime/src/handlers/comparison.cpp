/// @file comparison.cpp
/// @brief Cat 3: Comparison handlers (CMP, TEST, SET_FLAG, GET_FLAG).
///
/// Security properties:
///   Plaintext flags (1-bit, zero MCSP hardness). D3 uniform pipeline
///   ensures constant-time via CMOV: the branch outcome is the only
///   observable effect, and any scheme must eventually act on the
///   comparison result. The 1-bit flag itself reveals nothing beyond
///   what the branch outcome already exposes.
///
///   CMP and TEST decode operands (register-transient, Class C level).
///   SET_FLAG and GET_FLAG manipulate plaintext flag bits directly.

#include <handlers.hpp>
#include <decoder.hpp>
#include <encoding.hpp>

namespace VMPilot::Runtime::handlers {

using Common::DiagnosticCode;
using Common::VM::VMContext;

// ---------------------------------------------------------------------------
// CMP: Compare two registers, set flags
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_cmp(VMContext& ctx, const DecodedInsn& insn) noexcept {
    auto sa = static_cast<int64_t>(
        decode_register(ctx, insn.reg_a, ctx.encoded_regs[insn.reg_a]));
    auto sb = static_cast<int64_t>(
        decode_register(ctx, insn.reg_b, ctx.encoded_regs[insn.reg_b]));
    auto ua = static_cast<uint64_t>(sa);
    auto ub = static_cast<uint64_t>(sb);
    int64_t diff = sa - sb;

    ctx.vm_flags = 0;
    if (diff == 0)  ctx.vm_flags |= 0x01;  // ZF (zero)
    if (diff < 0)   ctx.vm_flags |= 0x02;  // SF (sign)
    if (ua < ub)    ctx.vm_flags |= 0x04;  // CF (unsigned borrow)
    // OF: signed overflow when signs of operands differ and
    // result sign differs from first operand
    if (((sa ^ sb) & (sa ^ diff)) < 0)
        ctx.vm_flags |= 0x08;  // OF (overflow)

    return {};
}

// ---------------------------------------------------------------------------
// TEST: Bitwise AND of two registers, set flags (no result stored)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_test(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t a = decode_register(ctx, insn.reg_a,
                                  ctx.encoded_regs[insn.reg_a]);
    uint64_t b = decode_register(ctx, insn.reg_b,
                                  ctx.encoded_regs[insn.reg_b]);
    uint64_t result = a & b;

    ctx.vm_flags = 0;
    if (result == 0) ctx.vm_flags |= 0x01;                    // ZF
    if (static_cast<int64_t>(result) < 0) ctx.vm_flags |= 0x02;  // SF

    return {};
}

// ---------------------------------------------------------------------------
// SET_FLAG: Set a specific flag bit
//   insn.condition[1:0] = flag index (0=ZF, 1=SF, 2=CF, 3=OF)
//   insn.aux[0] = value (0 or 1)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_set_flag(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint8_t flag_bit = insn.condition & 0x03;
    uint8_t value = static_cast<uint8_t>(insn.aux & 1);
    ctx.vm_flags = static_cast<uint8_t>(
        (ctx.vm_flags & ~(1u << flag_bit)) | (value << flag_bit));
    return {};
}

// ---------------------------------------------------------------------------
// GET_FLAG: Read a specific flag bit into a register
//   insn.condition[1:0] = flag index
//   result -> reg_a (encoded)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_get_flag(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint8_t flag_bit = insn.condition & 0x03;
    uint64_t val = (ctx.vm_flags >> flag_bit) & 1u;
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, val);
    return {};
}

}  // namespace VMPilot::Runtime::handlers
