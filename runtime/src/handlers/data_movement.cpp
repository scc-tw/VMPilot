/// @file data_movement.cpp
/// @brief Cat 0: Data Movement handlers (8 opcodes).
///
/// Security properties:
///   MOVE:       Zero plaintext -- same-domain register copy.
///   LOAD/STORE: Domain conversion via composed bijection LUT (MCSP-hard).
///   PUSH/POP:   Domain conversion + ORAM (rolling keystream).
///   LOAD_CONST: Zero plaintext -- compiler pre-encoded in target BB domain.
///   LOAD_CTX:   Encode from plaintext (context fields are internal).
///   STORE_CTX:  Decode to plaintext  (context fields are internal).

#include <handlers.hpp>
#include <decoder.hpp>
#include <encoding.hpp>
#include <oram.hpp>

#include <cstring>

namespace VMPilot::Runtime::handlers {

using Common::DiagnosticCode;
using Common::VM::VMContext;

// ---------------------------------------------------------------------------
// MOVE: dst = src (same BB domain, no conversion)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_move(VMContext& ctx, const DecodedInsn& insn) noexcept {
    ctx.encoded_regs[insn.reg_a] = ctx.encoded_regs[insn.reg_b];
    return {};
}

// ---------------------------------------------------------------------------
// LOAD: Read guest memory at aux address, convert memory -> register domain
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_load(VMContext& ctx, const DecodedInsn& insn) noexcept {
    // Apply load_base_delta for PIE/ASLR support.
    // Guest addresses in bytecode are relative to the static binary base.
    // At runtime under ASLR, the actual address = static_addr + load_base_delta.
    uintptr_t guest_addr = static_cast<uintptr_t>(
        static_cast<int64_t>(insn.aux) + ctx.load_base_delta);

    uint64_t mem_val = 0;
    std::memcpy(&mem_val,
                reinterpret_cast<const uint8_t*>(guest_addr),
                8);
    ctx.encoded_regs[insn.reg_a] = decode_for_load(ctx, insn.reg_a, mem_val);
    return {};
}

// ---------------------------------------------------------------------------
// STORE: Convert register -> memory domain, write to guest memory at aux
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_store(VMContext& ctx, const DecodedInsn& insn) noexcept {
    // Apply load_base_delta for PIE/ASLR support (see handle_load).
    uintptr_t guest_addr = static_cast<uintptr_t>(
        static_cast<int64_t>(insn.aux) + ctx.load_base_delta);

    uint64_t mem_val = encode_for_store(ctx, insn.reg_a,
                                        ctx.encoded_regs[insn.reg_a]);
    std::memcpy(reinterpret_cast<uint8_t*>(guest_addr),
                &mem_val, 8);
    return {};
}

// ---------------------------------------------------------------------------
// PUSH: Convert to memory domain, push to ORAM stack
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_push(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t mem_val = encode_for_store(ctx, insn.reg_a,
                                        ctx.encoded_regs[insn.reg_a]);
    auto r = oram_push(ctx, mem_val);
    if (!r) return tl::make_unexpected(r.error());
    return {};
}

// ---------------------------------------------------------------------------
// POP: Pop from ORAM stack, convert memory -> register domain
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_pop(VMContext& ctx, const DecodedInsn& insn) noexcept {
    auto val = oram_pop(ctx);
    if (!val) return tl::make_unexpected(val.error());
    ctx.encoded_regs[insn.reg_a] = decode_for_load(ctx, insn.reg_a, *val);
    return {};
}

// ---------------------------------------------------------------------------
// LOAD_CONST: Load pre-encoded constant from pool
//
// No encoding conversion at runtime -- the compiler pre-encoded the
// constant in the target BB's register domain (spec S1.1). This closes
// the SyntIA/QSynth I/O sampling attack vector on constant loads.
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_load_const(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t pool_val = 0;
    std::memcpy(&pool_val, ctx.constant_pool + insn.aux * 8, 8);
    ctx.encoded_regs[insn.reg_a] = pool_val;
    return {};
}

// ---------------------------------------------------------------------------
// LOAD_CTX: Read VM context field, encode into register domain
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_load_ctx(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t plain = 0;
    switch (insn.aux) {
        case 0: plain = ctx.vm_ip;          break;
        case 1: plain = ctx.vm_sp;          break;
        case 2: plain = ctx.current_bb_id;  break;
        case 3: plain = ctx.current_epoch;  break;
        default: break;
    }
    ctx.encoded_regs[insn.reg_a] = encode_register(ctx, insn.reg_a, plain);
    return {};
}

// ---------------------------------------------------------------------------
// STORE_CTX: Decode register, write to VM context field
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_store_ctx(VMContext& ctx, const DecodedInsn& insn) noexcept {
    uint64_t plain = decode_register(ctx, insn.reg_a,
                                     ctx.encoded_regs[insn.reg_a]);
    switch (insn.aux) {
        case 1: ctx.vm_sp = plain; break;
        // vm_ip, bb_id, epoch are read-only for guest code
        default: break;
    }
    return {};
}

}  // namespace VMPilot::Runtime::handlers
