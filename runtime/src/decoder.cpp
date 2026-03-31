#include <decoder.hpp>
#include <handlers.hpp>
#include <vm/vm_crypto.hpp>
#include <vm/vm_encoding.hpp>
#include <vm/vm_blob.hpp>

#include <cstring>
#include <vector>

namespace VMPilot::Runtime {

using namespace Common;
using namespace Common::VM;
using namespace Common::VM::Crypto;
using namespace Common::VM::Encoding;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/// Find the BBMetadata index for a given bb_id (linear search for v1).
/// Returns -1 if not found.
static int find_bb_index(const VMContext& ctx, uint32_t bb_id) noexcept {
    for (uint32_t i = 0; i < ctx.bb_count; ++i) {
        if (ctx.bb_metadata[i].bb_id == bb_id)
            return static_cast<int>(i);
    }
    return -1;
}

/// Derive bb_enc_seed = BLAKE3_keyed(stored_seed, "enc" || bb_id_le32)[0:8]
static void derive_bb_enc_seed(const uint8_t stored_seed[32],
                               uint32_t bb_id,
                               uint8_t out[8]) noexcept {
    uint8_t msg[7];
    std::memcpy(msg, "enc", 3);
    std::memcpy(msg + 3, &bb_id, 4);
    blake3_keyed_hash(stored_seed, msg, 7, out, 8);
}

/// Update enc_state: SipHash(enc_state_as_key_zeropadded_16, opcode(2)||aux(4))
/// This matches test_blob_builder.hpp::detail::update_enc_state exactly.
static uint64_t update_enc_state_impl(uint64_t enc_state,
                                      uint16_t opcode_val,
                                      uint32_t aux) noexcept {
    uint8_t key[16] = {};
    std::memcpy(key, &enc_state, 8);
    uint8_t msg[6];
    std::memcpy(msg, &opcode_val, 2);
    std::memcpy(msg + 2, &aux, 4);
    return siphash_2_4(key, msg, 6);
}

/// Apply RE_TABLE to re-encode a single register value across epoch change.
static void apply_re_table(uint64_t& encoded_val,
                           const uint8_t re_table[8][256]) noexcept {
    uint8_t bytes[8];
    std::memcpy(bytes, &encoded_val, 8);
    for (int k = 0; k < 8; ++k)
        bytes[k] = re_table[k][bytes[k]];
    std::memcpy(&encoded_val, bytes, 8);
}

// ---------------------------------------------------------------------------
// fetch_decrypt_decode
// ---------------------------------------------------------------------------

tl::expected<DecodedInsn, DiagnosticCode>
fetch_decrypt_decode(VMContext& ctx) noexcept {

    // ── 1. FETCH: bounds check ──────────────────────────────────────────
    if (ctx.vm_ip >= ctx.insn_count)
        return tl::make_unexpected(DiagnosticCode::InstructionDecryptFailed);

    // Read 8 encrypted bytes
    uint64_t encrypted = 0;
    std::memcpy(&encrypted, ctx.bytecodes + ctx.vm_ip * 8, 8);

    // ── 2. DECRYPT: SipHash keystream ───────────────────────────────────
    uint64_t keystream = siphash_keystream(ctx.fast_key, ctx.enc_state,
                                           ctx.insn_index_in_bb);
    uint64_t plain_u64 = encrypted ^ keystream;

    // Interpret as VmInsn
    VmInsn insn{};
    std::memcpy(&insn, &plain_u64, 8);

    // ── 3. DECODE: opcode resolution ────────────────────────────────────

    // Two-layer PRP (spec §4.2): Layer 2 PRP was applied AFTER alias selection
    // at compile time, so runtime must undo PRP FIRST to recover the alias,
    // then resolve the alias to the semantic opcode via the GSS table.
    // Reversing this order would mix PRP-permuted indices into the alias LUT,
    // breaking the Shannon perfect secrecy property (§11.6): each alias
    // must appear with equal probability 1/256 to achieve I(Real_Op; Cipher_Op) = 0.
    uint8_t encrypted_alias = static_cast<uint8_t>(insn.opcode & 0xFF);
    uint8_t alias = ctx.opcode_perm_inv[encrypted_alias];  // PRP inverse FIRST
    uint8_t semantic_op = ctx.alias_lut[alias];             // alias_lut SECOND

    // Validate opcode range
    if (semantic_op >= VM_OPCODE_COUNT)
        return tl::make_unexpected(DiagnosticCode::InvalidOpcodeAlias);

    // ── 4. Build DecodedInsn ────────────────────────────────────────────
    DecodedInsn decoded{};
    decoded.opcode          = static_cast<VmOpcode>(semantic_op);
    decoded.operand_a_type  = insn.operand_a_type();
    decoded.operand_b_type  = insn.operand_b_type();
    decoded.condition       = insn.condition();
    decoded.reg_a           = insn.reg_a();
    decoded.reg_b           = insn.reg_b();
    decoded.plaintext_opcode = insn.opcode;
    decoded.aux             = insn.aux;

    return decoded;
}

// ---------------------------------------------------------------------------
// advance_enc_state
// ---------------------------------------------------------------------------

void advance_enc_state(VMContext& ctx,
                       uint16_t plaintext_opcode,
                       uint32_t plaintext_aux) noexcept {
    ctx.enc_state = update_enc_state_impl(ctx.enc_state,
                                          plaintext_opcode,
                                          plaintext_aux);
    ++ctx.insn_index_in_bb;
}

// ---------------------------------------------------------------------------
// verify_bb_mac
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
verify_bb_mac(const VMContext& ctx) noexcept {

    // Use cached current_bb_index instead of O(n) linear search.
    // The index is maintained by enter_basic_block() and load_blob().
    uint32_t bb_idx = ctx.current_bb_index;
    if (bb_idx >= ctx.bb_count)
        return tl::make_unexpected(DiagnosticCode::InvalidBBTransition);

    const BBMetadata& bb = ctx.bb_metadata[bb_idx];

    // Derive bb_enc_seed for this BB
    uint8_t enc_seed_bytes[8];
    derive_bb_enc_seed(ctx.stored_seed, bb.bb_id, enc_seed_bytes);

    uint64_t enc_state = 0;
    std::memcpy(&enc_state, enc_seed_bytes, 8);

    // Re-decrypt all instructions in this BB to get plaintext bytes
    std::vector<uint8_t> plaintext_bytes(bb.insn_count_in_bb * 8);

    for (uint32_t j = 0; j < bb.insn_count_in_bb; ++j) {
        // Read encrypted instruction
        uint64_t encrypted = 0;
        std::memcpy(&encrypted,
                     ctx.bytecodes + (bb.entry_ip + j) * 8, 8);

        // Decrypt
        uint64_t ks = siphash_keystream(ctx.fast_key, enc_state, j);
        uint64_t plain = encrypted ^ ks;

        // Store plaintext bytes
        std::memcpy(plaintext_bytes.data() + j * 8, &plain, 8);

        // Check if this instruction is REKEY — if so, replay the same
        // BLAKE3+SipHash mix that handle_rekey() applies at runtime.
        // Without this, the local enc_state diverges from runtime's
        // enc_state, causing all subsequent keystreams to mismatch.
        VmInsn insn{};
        std::memcpy(&insn, &plain, 8);
        {
            uint8_t enc_alias = static_cast<uint8_t>(insn.opcode & 0xFF);
            uint8_t alias = ctx.opcode_perm_inv[enc_alias];
            uint8_t sem_op = ctx.alias_lut[alias];
            if (sem_op == static_cast<uint8_t>(VmOpcode::REKEY)) {
                uint32_t rekey_counter = insn.aux;
                uint8_t rk_ctx[9];
                std::memcpy(rk_ctx, "rekey", 5);
                std::memcpy(rk_ctx + 5, &rekey_counter, 4);
                uint8_t rk_mat[16];
                blake3_kdf(ctx.stored_seed,
                           reinterpret_cast<const char*>(rk_ctx), 9,
                           rk_mat, 16);
                uint8_t es[8];
                std::memcpy(es, &enc_state, 8);
                enc_state = siphash_2_4(rk_mat, es, 8);
            }
        }
        enc_state = update_enc_state_impl(enc_state, insn.opcode, insn.aux);
    }

    // Compute expected MAC: BLAKE3_keyed(integrity_key, plaintext)[0:8]
    uint8_t computed_mac[8];
    blake3_keyed_hash(ctx.integrity_key,
                      plaintext_bytes.data(),
                      plaintext_bytes.size(),
                      computed_mac, 8);

    // Load stored MAC (indexed by bb position in bb_metadata array)
    uint64_t stored_mac = 0;
    std::memcpy(&stored_mac, ctx.bb_macs + bb_idx * 8, 8);

    uint64_t computed_mac_u64 = 0;
    std::memcpy(&computed_mac_u64, computed_mac, 8);

    // Constant-time compare
    if (computed_mac_u64 != stored_mac)
        return tl::make_unexpected(DiagnosticCode::BBMacVerificationFailed);

    return {};
}

// ---------------------------------------------------------------------------
// enter_basic_block
// ---------------------------------------------------------------------------

tl::expected<void, DiagnosticCode>
enter_basic_block(VMContext& ctx, uint32_t target_bb_id) noexcept {

    // ── 1. Find target BB metadata ──────────────────────────────────────
    int bb_idx = find_bb_index(ctx, target_bb_id);
    if (bb_idx < 0)
        return tl::make_unexpected(DiagnosticCode::InvalidBBTransition);

    const BBMetadata& target = ctx.bb_metadata[bb_idx];

    // ── 2. Derive bb_enc_seed and reset enc_state ───────────────────────
    uint8_t enc_seed_bytes[8];
    derive_bb_enc_seed(ctx.stored_seed, target.bb_id, enc_seed_bytes);

    uint64_t enc_seed_u64 = 0;
    std::memcpy(&enc_seed_u64, enc_seed_bytes, 8);
    ctx.enc_state = enc_seed_u64;

    // ── 3. Reset instruction tracking ───────────────────────────────────
    ctx.insn_index_in_bb = 0;
    ctx.vm_ip = target.entry_ip;

    // ── 4. Handle epoch change ──────────────────────────────────────────
    if (target.epoch != ctx.current_epoch) {
        // Save old decode tables before deriving new ones.
        // We need old decode + new encode to compute RE_TABLE.
        // Use a temporary buffer for old decode tables.
        uint8_t old_decode[VM_REG_COUNT][VM_BYTE_LANES][256];
        std::memcpy(old_decode, ctx.reg_decode, sizeof(old_decode));

        // Derive new per-BB register encoding tables
        derive_register_tables(target.epoch_seed, target.live_regs_bitmap,
                               ctx.reg_encode, ctx.reg_decode);

        // Derive RE_TABLE: maps old-encoded values to new-encoded values
        uint8_t re_tables[VM_REG_COUNT][VM_BYTE_LANES][256];
        derive_re_tables(old_decode, ctx.reg_encode,
                         target.live_regs_bitmap, re_tables);

        // Apply RE_TABLE to each live encoded register
        for (uint8_t r = 0; r < VM_REG_COUNT; ++r) {
            if (target.live_regs_bitmap & (1u << r))
                apply_re_table(ctx.encoded_regs[r], re_tables[r]);
        }

        // Re-derive STORE/LOAD domain conversion tables
        derive_store_load_tables(ctx.reg_encode, ctx.reg_decode,
                                 ctx.mem_encode, ctx.mem_decode,
                                 target.live_regs_bitmap,
                                 ctx.store_tables, ctx.load_tables);

        // Re-derive opcode permutation for new epoch
        derive_opcode_permutation(target.epoch_seed,
                                  ctx.opcode_perm, ctx.opcode_perm_inv);
    }

    // ── 5. Clear composition table cache ────────────────────────────────
    // Encoding tables change per-epoch; cached composition tables are invalid.
    clear_composition_cache();

    // ── 6. Update current BB tracking ───────────────────────────────────
    ctx.current_bb_id = target.bb_id;
    ctx.current_bb_index = static_cast<uint32_t>(bb_idx);
    ctx.current_epoch = target.epoch;

    return {};
}

}  // namespace VMPilot::Runtime
