/// @file vm_internal.cpp
/// @brief Cat 7: VM Internal handlers (7 opcodes).
///
/// Security properties:
///   NOP: Chaff fake taint; enc_state chain participation prevents
///     deletion (SipHash preimage resistance). Writes to trash_regs
///     to defeat taint-tracking-based dead code analysis.
///
///   CHECK_INTEGRITY: Verifies BB MAC (BLAKE3_keyed).
///   CHECK_DEBUG: Phase 9 stub (anti-debug traps).
///   MUTATE_ISA: Re-derives opcode permutation from epoch_seed.
///   REKEY (D14§5.4): Mixes BLAKE3-derived entropy into enc_state mid-BB.
///     Why: increases offline simulation cost — each REKEY requires a
///     BLAKE3 evaluation (~100ns) that the attacker must reproduce.
///     Does NOT add forward secrecy (D15§11.8 applies).
///   SAVE_EPOCH / RESYNC: Shadow stack epoch checkpointing.

#include <handlers.hpp>
#include <decoder.hpp>
#include <anti_tamper.hpp>
#include <vm/vm_encoding.hpp>
#include <vm/vm_crypto.hpp>

#include <cstring>

namespace VMPilot::Runtime::handlers {

using Common::DiagnosticCode;
namespace Crypto = Common::VM::Crypto;
using Common::VM::VMContext;
using Common::VM::VM_MAX_NESTING;

// ---------------------------------------------------------------------------
// NOP: Chaff instruction (fake taint for DPA/taint-tracking resistance)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_nop(VMContext& ctx, const DecodedInsn& insn) noexcept {
    // Write to trash register to create fake data dependency.
    // This prevents analysis tools from identifying NOP as dead code
    // and stripping it, which would break the enc_state chain
    // (SipHash preimage resistance guarantees enc_state depends on
    // every instruction in the BB).
    ctx.trash_regs[insn.reg_a] = ctx.encoded_regs[insn.reg_b];
    return {};
}

// ---------------------------------------------------------------------------
// CHECK_INTEGRITY: Verify BB MAC
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_check_integrity(VMContext& ctx, const DecodedInsn& /*insn*/) noexcept {
    // 1. Per-BB MAC check: re-decrypts the entire BB and computes
    //    BLAKE3_keyed(integrity_key, all_plaintext_insns)[0:8].
    auto mac_result = verify_bb_mac(ctx);
    if (!mac_result)
        return mac_result;

    // 2. Whole-blob integrity check (Phase 9.2): recomputes
    //    BLAKE3_keyed(integrity_key, entire_blob) and compares against
    //    the hash stored at load time. Catches any post-load modification
    //    to any section of the blob.
    return verify_blob_integrity(ctx.blob_data_ptr, ctx.blob_data_size, ctx);
}

// ---------------------------------------------------------------------------
// CHECK_DEBUG: Phase 9 stub (anti-debug)
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_check_debug(VMContext& /*ctx*/, const DecodedInsn& /*insn*/) noexcept {
    // Phase 9: will check ptrace, debug registers, etc.
    return {};
}

// ---------------------------------------------------------------------------
// MUTATE_ISA: Re-derive opcode permutation from current BB's epoch_seed
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_mutate_isa(VMContext& ctx, const DecodedInsn& /*insn*/) noexcept {
    Common::VM::Encoding::derive_opcode_permutation(
        ctx.bb_metadata[ctx.current_bb_index].epoch_seed,
        ctx.opcode_perm, ctx.opcode_perm_inv);
    return {};
}

// ---------------------------------------------------------------------------
// REKEY (D14§5.4): Mix BLAKE3-derived entropy into enc_state mid-BB.
//
// Why this strengthens D1: an attacker simulating the enc_state chain
// offline must execute BLAKE3 at each REKEY point (~100ns each).  With
// N REKEY instructions per BB, simulation cost += N × BLAKE3_eval.
//
// Why this does NOT add forward secrecy: stored_seed compromise still
// allows full chain simulation (D15§11.8 fundamental limitation).
// The value of REKEY is cost amplification, not new security invariants.
//
// The dispatcher's advance_enc_state() still runs AFTER this handler,
// so enc_state is double-mixed: REKEY transform + normal SipHash advance.
// verify_bb_mac() must replay this transform to keep the chain in sync.
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_rekey(VMContext& ctx, const DecodedInsn& insn) noexcept {
    const uint32_t rekey_counter = insn.aux;

    // Derive 16 bytes of key material from stored_seed + "rekey" + counter.
    // BLAKE3 in KDF mode ensures independence from all other key derivations
    // (enc_state, fast_key, oram_key, etc.) via domain separation.
    uint8_t context[9];
    std::memcpy(context, "rekey", 5);
    std::memcpy(context + 5, &rekey_counter, 4);

    uint8_t rekey_material[16];
    Crypto::blake3_kdf(ctx.stored_seed,
                       reinterpret_cast<const char*>(context), 9,
                       rekey_material, 16);

    // Mix into enc_state: SipHash(rekey_material_as_key, enc_state_as_msg)
    // This is a one-way transform — knowing enc_state after REKEY does not
    // help recover enc_state before REKEY (preimage resistance of SipHash).
    uint8_t enc_state_bytes[8];
    std::memcpy(enc_state_bytes, &ctx.enc_state, 8);
    ctx.enc_state = Crypto::siphash_2_4(rekey_material,
                                         enc_state_bytes, 8);
    return {};
}

// ---------------------------------------------------------------------------
// SAVE_EPOCH: Push epoch checkpoint onto shadow stack
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_save_epoch(VMContext& ctx, const DecodedInsn& /*insn*/) noexcept {
    if (ctx.shadow_depth >= VM_MAX_NESTING)
        return tl::make_unexpected(DiagnosticCode::ShadowStackOverflow);

    auto& cp = ctx.shadow_stack[ctx.shadow_depth];
    cp.vm_ip = ctx.vm_ip;
    cp.bb_id = ctx.current_bb_id;
    std::memcpy(cp.epoch_seed,
                ctx.bb_metadata[ctx.current_bb_index].epoch_seed, 32);
    cp.salt = 0;
    std::memcpy(cp.encoded_regs_snapshot,
                ctx.encoded_regs, sizeof(ctx.encoded_regs));
    ctx.shadow_depth++;
    return {};
}

// ---------------------------------------------------------------------------
// RESYNC: Pop epoch checkpoint, restore encoded registers
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
handle_resync(VMContext& ctx, const DecodedInsn& /*insn*/) noexcept {
    if (ctx.shadow_depth == 0)
        return tl::make_unexpected(DiagnosticCode::StackUnderflow);

    ctx.shadow_depth--;
    std::memcpy(ctx.encoded_regs,
                ctx.shadow_stack[ctx.shadow_depth].encoded_regs_snapshot,
                sizeof(ctx.encoded_regs));
    return {};
}

// ---------------------------------------------------------------------------
// stub_handler: fallback for unimplemented opcodes
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
stub_handler(VMContext& /*ctx*/, const DecodedInsn& /*insn*/) noexcept {
    return tl::make_unexpected(DiagnosticCode::NotImplemented);
}

}  // namespace VMPilot::Runtime::handlers
