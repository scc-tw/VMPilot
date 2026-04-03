/// @file blob_builder.cpp
/// @brief Blob builder implementation — constructs encrypted blobs from ProgramBuilder output.
///
/// Extracted from blob_builder.hpp.  None of these functions are templates;
/// they were inline in the header purely for convenience, not necessity.

#include "blob_builder.hpp"

#include <vm/vm_blob.hpp>
#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_crypto.hpp>
#include <vm/vm_encoding.hpp>

#include <cstdint>
#include <cstring>
#include <vector>

namespace VMPilot::Runtime {

using namespace Common::VM;
using namespace Common::VM::Crypto;
using namespace Common::VM::Encoding;

// ─────────────────────────────────────────────────────────────────────────────
// blob_detail helpers
// ─────────────────────────────────────────────────────────────────────────────

uint64_t blob_detail::update_enc_state(uint64_t enc_state,
                                       uint64_t full_plaintext_insn) noexcept {
    uint8_t key[16] = {};
    std::memcpy(key, &enc_state, 8);
    uint8_t msg[8];
    std::memcpy(msg, &full_plaintext_insn, 8);
    return siphash_2_4(key, msg, 8);
}

void blob_detail::derive_bb_enc_seed(const uint8_t stored_seed[32],
                                     uint32_t bb_id,
                                     uint8_t out[8]) noexcept {
    uint8_t msg[7];
    std::memcpy(msg, "enc", 3);
    std::memcpy(msg + 3, &bb_id, 4);
    blake3_keyed_hash(stored_seed, msg, 7, out, 8);
}

uint8_t blob_detail::find_alias_for_opcode(const uint8_t alias_lut[256],
                                           uint8_t semantic_op) noexcept {
    for (int i = 0; i < 256; ++i) {
        if (alias_lut[i] == semantic_op)
            return static_cast<uint8_t>(i);
    }
    return 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// build_blob — unified builder accepting pool_entries + native_calls
// ─────────────────────────────────────────────────────────────────────────────

std::vector<uint8_t> build_blob(
    const uint8_t stored_seed[32],
    const std::vector<BuilderBB>& bbs,
    const std::vector<BuilderPoolEntry>& pool_entries,
    const std::vector<BlobNativeCall>& native_calls,
    bool debug_mode)
{
    // ── Compute total instruction count ─────────────────────────────────
    uint32_t total_insns = 0;
    for (const auto& bb : bbs)
        total_insns += static_cast<uint32_t>(bb.instructions.size());

    const uint32_t bb_count   = static_cast<uint32_t>(bbs.size());
    const uint32_t pool_count = static_cast<uint32_t>(pool_entries.size());
    const uint32_t nc_count   = static_cast<uint32_t>(native_calls.size());

    // ── Build header ────────────────────────────────────────────────────
    BlobHeader header{};
    header.magic             = VM_BLOB_MAGIC;
    header.version           = VM_BLOB_VERSION;
    header.flags             = debug_mode ? BLOB_FLAG_DEBUG : uint16_t(0);
    header.insn_count        = total_insns;
    header.bb_count          = bb_count;
    header.pool_entry_count  = pool_count;
    header.native_call_count = nc_count;
    header.reserved          = 0;
    header.total_size        = blob_expected_size(header);

    // ── Allocate blob ───────────────────────────────────────────────────
    std::vector<uint8_t> blob(header.total_size, 0);
    std::memcpy(blob.data(), &header, sizeof(header));

    // ── Derive keys (doc 16 §6: all via BLAKE3_KEYED, matching engine) ──
    uint8_t fast_key[16];
    blake3_keyed_hash(stored_seed, reinterpret_cast<const uint8_t*>("fast"), 4, fast_key, 16);

    uint8_t meta_key[16];
    blake3_keyed_hash(stored_seed, reinterpret_cast<const uint8_t*>("meta"), 4, meta_key, 16);

    uint8_t pool_key[16];
    blake3_keyed_hash(stored_seed, reinterpret_cast<const uint8_t*>("pool"), 4, pool_key, 16);

    uint8_t integrity_key[32];
    blake3_keyed_hash(stored_seed, reinterpret_cast<const uint8_t*>("integrity"), 9, integrity_key, 32);

    uint8_t rekey_key[32];
    blake3_keyed_hash(stored_seed, reinterpret_cast<const uint8_t*>("rekey"), 5, rekey_key, 32);

    // ── Build alias LUT ─────────────────────────────────────────────────
    uint8_t alias_lut[256];
    for (int i = 0; i < 256; ++i)
        alias_lut[i] = static_cast<uint8_t>(i % VM_OPCODE_COUNT);

    // ── Encrypt instructions ────────────────────────────────────────────
    const uint32_t insn_offset = blob_section_insn(header);
    uint32_t global_ip = 0;

    for (const auto& bb : bbs) {
        uint8_t enc_seed_bytes[8];
        blob_detail::derive_bb_enc_seed(stored_seed, bb.bb_id, enc_seed_bytes);

        uint64_t enc_state = 0;
        std::memcpy(&enc_state, enc_seed_bytes, 8);

        uint8_t opcode_perm[256], opcode_perm_inv[256];
        Encoding::derive_opcode_permutation(bb.epoch_seed, opcode_perm, opcode_perm_inv);

        for (uint32_t j = 0; j < static_cast<uint32_t>(bb.instructions.size()); ++j) {
            const auto& ti = bb.instructions[j];

            uint8_t semantic_op = static_cast<uint8_t>(ti.opcode);
            uint8_t alias = blob_detail::find_alias_for_opcode(alias_lut, semantic_op);
            uint8_t encrypted_alias = opcode_perm[alias];

            VmInsn plain{};
            plain.opcode   = static_cast<uint16_t>(encrypted_alias);
            plain.flags    = ti.flags;
            plain.reg_pack = static_cast<uint8_t>((ti.reg_a << 4) | (ti.reg_b & 0x0F));
            plain.aux      = ti.aux;

            uint64_t plain_u64 = 0;
            std::memcpy(&plain_u64, &plain, 8);

            uint64_t keystream = siphash_keystream(fast_key, enc_state, j);
            uint64_t encrypted = plain_u64 ^ keystream;

            std::memcpy(blob.data() + insn_offset + global_ip * 8,
                        &encrypted, 8);

            // REKEY enc_state replay (must match runtime's handle_rekey)
            {
                uint8_t sem = static_cast<uint8_t>(ti.opcode);
                if (sem == static_cast<uint8_t>(VmOpcode::REKEY)) {
                    uint32_t rk_counter = ti.aux;
                    uint8_t rk_ctx[9];
                    std::memcpy(rk_ctx, "rekey", 5);
                    std::memcpy(rk_ctx + 5, &rk_counter, 4);
                    uint8_t rk_mat[16];
                    blake3_keyed_hash(rekey_key, rk_ctx, 9, rk_mat, 16);
                    uint8_t es[8];
                    std::memcpy(es, &enc_state, 8);
                    enc_state = siphash_2_4(rk_mat, es, 8);
                }
            }

            enc_state = blob_detail::update_enc_state(enc_state, plain_u64);

            ++global_ip;
        }
    }

    // ── Encrypt constant pool ──────────────────────────────────────────
    //
    // Doc 16 change: pool values are stored as PLAINTEXT (not pre-encoded
    // with per-BB LUT tables).  The runtime FPE-encodes them at LOAD_CONST
    // time using the per-instruction FPE key.
    //
    // WHY plaintext pool:  Per-instruction FPE keys depend on execution
    // history (RDRAND nonce + BLAKE3 chain), so the compiler cannot predict
    // the key at pool encode time.  Storing plaintext is the only option.
    // The SipHash encryption layer (D1) still protects pool values at rest.
    const uint32_t pool_offset = blob_section_pool(header);

    for (uint32_t i = 0; i < pool_count; ++i) {
        const auto& pe = pool_entries[i];
        uint64_t plain = pe.plaintext;

        // Pool stores plaintext, encrypted only with SipHash keystream (D1)
        uint64_t idx = static_cast<uint64_t>(i);
        uint8_t idx_bytes[8];
        std::memcpy(idx_bytes, &idx, 8);
        uint64_t keystream = siphash_2_4(pool_key, idx_bytes, 8);
        uint64_t enc_val = plain ^ keystream;
        std::memcpy(blob.data() + pool_offset + i * 8, &enc_val, 8);
    }

    // ── Encrypt BB metadata ─────────────────────────────────────────────
    const uint32_t meta_off = blob_section_meta(header);
    uint32_t running_ip = 0;

    for (uint32_t bb_idx = 0; bb_idx < bb_count; ++bb_idx) {
        const auto& tbb = bbs[bb_idx];

        SerializedBBMeta smeta{};
        smeta.bb_id            = tbb.bb_id;
        smeta.epoch            = tbb.epoch;
        smeta.entry_ip         = running_ip;
        smeta.insn_count_in_bb = static_cast<uint32_t>(tbb.instructions.size());
        smeta.live_regs_bitmap = tbb.live_regs_bitmap;
        smeta.flags            = tbb.flags;
        std::memcpy(smeta.epoch_seed, tbb.epoch_seed, 32);
        smeta.reserved_0 = 0;
        smeta.reserved_1 = 0;
        smeta.reserved_2 = 0;

        uint64_t words[8];
        std::memcpy(words, &smeta, 64);

        for (uint32_t w = 0; w < 8; ++w) {
            uint64_t nonce = static_cast<uint64_t>(bb_idx) * 8 + w;
            uint8_t nonce_bytes[8];
            std::memcpy(nonce_bytes, &nonce, 8);
            uint64_t keystream = siphash_2_4(meta_key, nonce_bytes, 8);
            words[w] ^= keystream;
        }

        std::memcpy(blob.data() + meta_off + bb_idx * 64, words, 64);
        running_ip += static_cast<uint32_t>(tbb.instructions.size());
    }

    // ── BB MACs ─────────────────────────────────────────────────────────
    const uint32_t mac_offset = blob_section_mac(header);

    for (uint32_t bb_idx = 0; bb_idx < bb_count; ++bb_idx) {
        const auto& tbb = bbs[bb_idx];
        const uint32_t n = static_cast<uint32_t>(tbb.instructions.size());

        uint8_t mac_opcode_perm[256], mac_opcode_perm_inv[256];
        Encoding::derive_opcode_permutation(tbb.epoch_seed,
                                            mac_opcode_perm, mac_opcode_perm_inv);

        std::vector<uint8_t> plain_bytes(n * 8);
        for (uint32_t j = 0; j < n; ++j) {
            uint8_t sem_op = static_cast<uint8_t>(tbb.instructions[j].opcode);
            uint8_t mac_alias = blob_detail::find_alias_for_opcode(alias_lut, sem_op);
            uint8_t mac_encrypted = mac_opcode_perm[mac_alias];

            VmInsn insn{};
            insn.opcode   = static_cast<uint16_t>(mac_encrypted);
            insn.flags    = tbb.instructions[j].flags;
            insn.reg_pack = static_cast<uint8_t>(
                (tbb.instructions[j].reg_a << 4) |
                (tbb.instructions[j].reg_b & 0x0F));
            insn.aux      = tbb.instructions[j].aux;
            std::memcpy(plain_bytes.data() + j * 8, &insn, 8);
        }

        uint8_t mac[8];
        blake3_keyed_hash(integrity_key, plain_bytes.data(),
                          plain_bytes.size(), mac, 8);
        std::memcpy(blob.data() + mac_offset + bb_idx * 8, mac, 8);
    }

    // ── Transition entries (native calls) ───────────────────────────────
    if (!native_calls.empty()) {
        const uint32_t trans_off = blob_section_trans(header);
        for (uint32_t i = 0; i < nc_count; ++i) {
            TransitionEntry te{};
            te.call_site_ip  = native_calls[i].call_site_ip;
            te.arg_count     = native_calls[i].arg_count;
            te.target_offset = native_calls[i].target_addr;
            te.coeff_a       = 0;
            te.coeff_b       = 0;
            std::memcpy(blob.data() + trans_off + i * sizeof(TransitionEntry),
                        &te, sizeof(te));
        }
    }

    // ── Alias LUT ───────────────────────────────────────────────────────
    const uint32_t alias_off = blob_section_alias(header);
    std::memcpy(blob.data() + alias_off, alias_lut, 256);

    // ── Config section (zeroed) ─────────────────────────────────────────
    // Already zero from vector initialization.

    return blob;
}

}  // namespace VMPilot::Runtime
