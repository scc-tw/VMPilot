/// @file width_extension.cpp
/// @brief Cat 5: Width/Extension handlers (8 opcodes).
///
/// Security properties:
///   v1: Class C security level (decode, native width op, re-encode).
///   Cross-lane data rearrangement (sign extension propagates bits
///   across byte lanes) prevents a pure per-lane LUT approach.
///   Full 64-bit plaintext exposure (register-transient).

#include <handlers.hpp>
#include <decoder.hpp>
#include <encoding.hpp>

namespace VMPilot::Runtime::handlers {

using Common::DiagnosticCode;
using Common::VM::VMContext;

// ---------------------------------------------------------------------------
// Width operation template: decode -> transform -> re-encode
// ---------------------------------------------------------------------------

static tl::expected<void, DiagnosticCode>
width_op(VMContext& ctx, const DecodedInsn& insn,
         uint64_t (*transform)(uint64_t)) noexcept {
    uint64_t plain = decode_register(ctx, insn.reg_a,
                                      ctx.encoded_regs[insn.reg_a]);
    ctx.encoded_regs[insn.reg_a] = encode_register(
        ctx, insn.reg_a, transform(plain));
    return {};
}

// ---------------------------------------------------------------------------
// Transform functions
// ---------------------------------------------------------------------------

static uint64_t sext8(uint64_t v)  {
    return static_cast<uint64_t>(
        static_cast<int64_t>(static_cast<int8_t>(v)));
}
static uint64_t sext16(uint64_t v) {
    return static_cast<uint64_t>(
        static_cast<int64_t>(static_cast<int16_t>(v)));
}
static uint64_t sext32(uint64_t v) {
    return static_cast<uint64_t>(
        static_cast<int64_t>(static_cast<int32_t>(v)));
}
static uint64_t zext8(uint64_t v)  { return v & 0xFFu; }
static uint64_t zext16(uint64_t v) { return v & 0xFFFFu; }
static uint64_t zext32(uint64_t v) { return v & 0xFFFFFFFFu; }
static uint64_t trunc8(uint64_t v) { return v & 0xFFu; }
static uint64_t trunc16(uint64_t v){ return v & 0xFFFFu; }

// ---------------------------------------------------------------------------
// Individual handlers
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_sext8(VMContext& ctx, const DecodedInsn& insn) noexcept {
    return width_op(ctx, insn, sext8);
}

tl::expected<void, DiagnosticCode>
handle_sext16(VMContext& ctx, const DecodedInsn& insn) noexcept {
    return width_op(ctx, insn, sext16);
}

tl::expected<void, DiagnosticCode>
handle_sext32(VMContext& ctx, const DecodedInsn& insn) noexcept {
    return width_op(ctx, insn, sext32);
}

tl::expected<void, DiagnosticCode>
handle_zext8(VMContext& ctx, const DecodedInsn& insn) noexcept {
    return width_op(ctx, insn, zext8);
}

tl::expected<void, DiagnosticCode>
handle_zext16(VMContext& ctx, const DecodedInsn& insn) noexcept {
    return width_op(ctx, insn, zext16);
}

tl::expected<void, DiagnosticCode>
handle_zext32(VMContext& ctx, const DecodedInsn& insn) noexcept {
    return width_op(ctx, insn, zext32);
}

tl::expected<void, DiagnosticCode>
handle_trunc8(VMContext& ctx, const DecodedInsn& insn) noexcept {
    return width_op(ctx, insn, trunc8);
}

tl::expected<void, DiagnosticCode>
handle_trunc16(VMContext& ctx, const DecodedInsn& insn) noexcept {
    return width_op(ctx, insn, trunc16);
}

}  // namespace VMPilot::Runtime::handlers
