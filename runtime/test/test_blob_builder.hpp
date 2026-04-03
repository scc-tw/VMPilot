#pragma once
#ifndef __RUNTIME_TEST_BLOB_BUILDER_HPP__
#define __RUNTIME_TEST_BLOB_BUILDER_HPP__

/// @file test_blob_builder.hpp
/// @brief Minimal "compiler" for building valid encrypted blobs in tests.
///
/// Constructs blobs that are byte-for-byte compatible with the loader's
/// decryption pipeline.  Used by all Phase 2+ runtime tests.

#include <vm/vm_blob.hpp>
#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_crypto.hpp>
#include <vm/vm_encoding.hpp>

#include <cstdint>
#include <cstring>
#include <vector>

namespace VMPilot::Test {

using namespace Common::VM;
using namespace Common::VM::Crypto;
using namespace Common::VM::Encoding;

// ─────────────────────────────────────────────────────────────────────────────
// Test data types
// ─────────────────────────────────────────────────────────────────────────────

struct TestInstruction {
    VmOpcode opcode;
    uint8_t  flags;
    uint8_t  reg_a;
    uint8_t  reg_b;
    uint32_t aux;
};

struct TestBB {
    uint32_t bb_id;
    uint32_t epoch;
    uint16_t live_regs_bitmap;
    uint16_t flags;  // BB_FLAG_EPOCH_CHANGED etc.
    uint8_t  epoch_seed[32];
    std::vector<TestInstruction> instructions;
};

/// Native call site descriptor for test blobs.
/// target_addr is stored as-is into TransitionEntry.target_offset.
/// In tests (load_base_delta=0), this is the absolute function pointer.
struct TestNativeCall {
    uint32_t call_site_ip;   ///< instruction index of the NATIVE_CALL
    uint32_t arg_count;      ///< number of arguments (r0..rN-1)
    uint64_t target_addr;    ///< absolute function pointer (or offset if PIE)
};

/// Pool entry with target register info for correct per-register encoding.
///
/// The compiler pre-encodes each constant using the target BB's register
/// encoding tables for the SPECIFIC register it will be loaded into.
/// Different registers have different encoding tables, so pool[0] encoded
/// for r0 differs from pool[0] encoded for r1.
struct TestPoolEntry {
    uint64_t plaintext;
    uint32_t target_bb_index;  ///< which BB's encoding tables to use
    uint8_t  target_reg;       ///< which register's encoding to use
};

// ─────────────────────────────────────────────────────────────────────────────
// Encryption helpers (mirror the loader's decryption exactly)
// ─────────────────────────────────────────────────────────────────────────────

namespace detail {

/// Update enc_state: SipHash(enc_state_as_key_zeropadded_16, full_plaintext_insn(8))
/// Doc 17: all 8 bytes of the plaintext instruction drive the ratchet.
inline uint64_t update_enc_state(uint64_t enc_state,
                                 uint64_t full_plaintext_insn) noexcept {
    uint8_t key[16] = {};
    std::memcpy(key, &enc_state, 8);
    uint8_t msg[8];
    std::memcpy(msg, &full_plaintext_insn, 8);
    return siphash_2_4(key, msg, 8);
}

/// Derive bb_enc_seed = BLAKE3_keyed(stored_seed, "enc"||bb_id_le32)[0:8]
inline void derive_bb_enc_seed(const uint8_t stored_seed[32],
                               uint32_t bb_id,
                               uint8_t out[8]) noexcept {
    uint8_t msg[7];
    std::memcpy(msg, "enc", 3);
    std::memcpy(msg + 3, &bb_id, 4);
    blake3_keyed_hash(stored_seed, msg, 7, out, 8);
}

/// Find an alias value that maps to the given semantic opcode.
/// In the real compiler, one is chosen randomly from ~4.6 options for GSS
/// smoothing (256/55 aliases per semantic opcode).
/// For testing, just return the first match.
inline uint8_t find_alias_for_opcode(const uint8_t alias_lut[256],
                                     uint8_t semantic_op) noexcept {
    for (int i = 0; i < 256; ++i) {
        if (alias_lut[i] == semantic_op)
            return static_cast<uint8_t>(i);
    }
    return 0;  // should never happen if alias_lut is properly constructed
}

}  // namespace detail

// ─────────────────────────────────────────────────────────────────────────────
// build_test_blob
// ─────────────────────────────────────────────────────────────────────────────

/// Build a complete valid encrypted blob from test data.
///
/// The builder reproduces the compiler's encryption pipeline:
///   1. Lays out sections per vm_blob.hpp format
///   2. Encrypts instructions with per-BB SipHash keystream chain
///   3. Encrypts BB metadata with SipHash(meta_key, word_index)
///   4. Encrypts constant pool with SipHash(pool_key, entry_index)
///   5. Computes 8-byte BB MACs (truncated BLAKE3_keyed for stub)
///   6. Writes identity alias LUT (alias[i] = i % VM_OPCODE_COUNT)
///
/// @param stored_seed             32-byte root secret
/// @param bbs                     basic blocks with plaintext instructions
/// @param constant_pool_plaintext plaintext constant pool entries
/// @param debug_mode              set BLOB_FLAG_DEBUG if true
/// @return                        complete encrypted blob bytes
inline std::vector<uint8_t> build_test_blob(
    const uint8_t stored_seed[32],
    const std::vector<TestBB>& bbs,
    const std::vector<uint64_t>& constant_pool_plaintext = {},
    bool debug_mode = false,
    const std::vector<TestNativeCall>& native_calls = {}) {

    // ── Compute total instruction count across all BBs ───────────────────
    uint32_t total_insns = 0;
    for (const auto& bb : bbs)
        total_insns += static_cast<uint32_t>(bb.instructions.size());

    const uint32_t bb_count   = static_cast<uint32_t>(bbs.size());
    const uint32_t pool_count = static_cast<uint32_t>(constant_pool_plaintext.size());
    const uint32_t nc_count   = static_cast<uint32_t>(native_calls.size());

    // ── Build header ─────────────────────────────────────────────────────
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

    // ── Allocate blob ────────────────────────────────────────────────────
    std::vector<uint8_t> blob(header.total_size, 0);
    std::memcpy(blob.data(), &header, sizeof(header));

    // ── Derive keys (same derivation the loader uses) ────────────────────
    uint8_t fast_key[16];
    blake3_keyed_hash(stored_seed, reinterpret_cast<const uint8_t*>("fast"), 4, fast_key, 16);

    uint8_t meta_key[16];
    blake3_keyed_hash(stored_seed, reinterpret_cast<const uint8_t*>("meta"), 4, meta_key, 16);

    uint8_t pool_key[16];
    blake3_keyed_hash(stored_seed, reinterpret_cast<const uint8_t*>("pool"), 4, pool_key, 16);

    uint8_t integrity_key[32];
    blake3_keyed_hash(stored_seed, reinterpret_cast<const uint8_t*>("integrity"), 9, integrity_key, 32);

    // ── Build alias LUT (needed before instruction encryption) ─────────
    // Identity mapping: alias[i] = i % VM_OPCODE_COUNT
    // This is the same table written into the blob's alias section.
    uint8_t alias_lut[256];
    for (int i = 0; i < 256; ++i)
        alias_lut[i] = static_cast<uint8_t>(i % VM_OPCODE_COUNT);

    // ── Encrypt instructions ─────────────────────────────────────────────
    //
    // The builder mirrors the real compiler's two-layer PRP pipeline:
    //   1. Map semantic opcode -> alias (via alias_lut inverse)
    //   2. Encrypt alias -> opcode_perm[alias] (Layer 2 PRP, per-epoch)
    //   3. Pack encrypted_alias into VmInsn.opcode low byte
    //   4. Encrypt VmInsn with SipHash keystream
    //
    // Runtime undoes this as: encrypted_alias -> opcode_perm_inv -> alias -> alias_lut -> semantic
    // The PRP must be applied per-BB because different epochs have different permutations.

    const uint32_t insn_offset = blob_section_insn(header);
    uint32_t global_ip = 0;

    for (const auto& bb : bbs) {
        // Derive bb_enc_seed
        uint8_t enc_seed_bytes[8];
        detail::derive_bb_enc_seed(stored_seed, bb.bb_id, enc_seed_bytes);

        uint64_t enc_state = 0;
        std::memcpy(&enc_state, enc_seed_bytes, 8);

        // Derive per-BB opcode permutation from epoch_seed (Layer 2 PRP).
        // Different BBs in different epochs will have different permutations.
        uint8_t opcode_perm[256], opcode_perm_inv[256];
        Encoding::derive_opcode_permutation(bb.epoch_seed, opcode_perm, opcode_perm_inv);

        for (uint32_t j = 0; j < static_cast<uint32_t>(bb.instructions.size()); ++j) {
            const auto& ti = bb.instructions[j];

            // Two-layer PRP encode (mirrors compiler):
            //   semantic_op -> alias (inverse of alias_lut)
            //   alias -> opcode_perm[alias] -> encrypted_alias
            uint8_t semantic_op = static_cast<uint8_t>(ti.opcode);
            uint8_t alias = detail::find_alias_for_opcode(alias_lut, semantic_op);
            uint8_t encrypted_alias = opcode_perm[alias];

            // Build plaintext 8-byte instruction with encrypted opcode field.
            // High byte of opcode is noise (0 for test determinism).
            VmInsn plain{};
            plain.opcode   = static_cast<uint16_t>(encrypted_alias);  // PRP-encoded
            plain.flags    = ti.flags;
            plain.reg_pack = static_cast<uint8_t>((ti.reg_a << 4) | (ti.reg_b & 0x0F));
            plain.aux      = ti.aux;

            uint64_t plain_u64 = 0;
            std::memcpy(&plain_u64, &plain, 8);

            // Keystream for this instruction
            uint64_t keystream = siphash_keystream(fast_key, enc_state, j);
            uint64_t encrypted = plain_u64 ^ keystream;

            // Write encrypted instruction into blob
            std::memcpy(blob.data() + insn_offset + global_ip * 8,
                        &encrypted, 8);

            // If this instruction is REKEY, replay the BLAKE3+SipHash mix
            // that handle_rekey() applies at runtime.  This keeps the
            // encryption enc_state in sync with what the decoder expects.
            {
                uint8_t sem = static_cast<uint8_t>(ti.opcode);
                if (sem == static_cast<uint8_t>(VmOpcode::REKEY)) {
                    uint32_t rk_counter = ti.aux;
                    uint8_t rk_ctx[9];
                    std::memcpy(rk_ctx, "rekey", 5);
                    std::memcpy(rk_ctx + 5, &rk_counter, 4);
                    uint8_t rk_mat[16];
                    // Use pre-derived rekey_key (matches runtime — 32 bytes for BLAKE3_KEYED)
                    uint8_t rekey_key[32];
                    blake3_keyed_hash(stored_seed,
                                      reinterpret_cast<const uint8_t*>("rekey"), 5,
                                      rekey_key, 32);
                    blake3_keyed_hash(rekey_key, rk_ctx, 9, rk_mat, 16);
                    uint8_t es[8];
                    std::memcpy(es, &enc_state, 8);
                    enc_state = siphash_2_4(rk_mat, es, 8);
                }
            }

            // Doc 17: advance enc_state with the full 8-byte plaintext instruction.
            // All bits participate in the ratchet for entanglement.
            enc_state = detail::update_enc_state(enc_state, plain_u64);

            ++global_ip;
        }
    }

    // ── Encrypt constant pool ────────────────────────────────────────────
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
        uint64_t plain = constant_pool_plaintext[i];

        // Pool stores plaintext, encrypted only with SipHash keystream (D1)
        uint64_t idx = static_cast<uint64_t>(i);
        uint8_t idx_bytes[8];
        std::memcpy(idx_bytes, &idx, 8);
        uint64_t keystream = siphash_2_4(pool_key, idx_bytes, 8);
        uint64_t encrypted = plain ^ keystream;
        std::memcpy(blob.data() + pool_offset + i * 8, &encrypted, 8);
    }

    // ── Encrypt BB metadata ──────────────────────────────────────────────

    const uint32_t meta_off = blob_section_meta(header);
    uint32_t running_ip = 0;

    for (uint32_t bb_idx = 0; bb_idx < bb_count; ++bb_idx) {
        const auto& tbb = bbs[bb_idx];

        // Build plaintext SerializedBBMeta
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

        // Convert to 8 words and encrypt
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

    // ── BB MACs ──────────────────────────────────────────────────────────
    //
    // mac[bb] = BLAKE3_keyed(integrity_key, plaintext_insns_of_bb)[0:8]
    //
    // The MAC covers the *decrypted* VmInsn bytes, which contain the
    // PRP-encoded opcode (not the semantic opcode). This must match what
    // the decoder's verify_bb_mac() re-derives after SipHash decryption.

    const uint32_t mac_offset = blob_section_mac(header);

    for (uint32_t bb_idx = 0; bb_idx < bb_count; ++bb_idx) {
        const auto& tbb = bbs[bb_idx];
        const uint32_t n = static_cast<uint32_t>(tbb.instructions.size());

        // Derive this BB's opcode permutation for PRP encoding
        uint8_t mac_opcode_perm[256], mac_opcode_perm_inv[256];
        Encoding::derive_opcode_permutation(tbb.epoch_seed,
                                            mac_opcode_perm, mac_opcode_perm_inv);

        // Build plaintext instruction bytes for this BB (with PRP-encoded opcodes)
        std::vector<uint8_t> plain_bytes(n * 8);
        for (uint32_t j = 0; j < n; ++j) {
            uint8_t sem_op = static_cast<uint8_t>(tbb.instructions[j].opcode);
            uint8_t mac_alias = detail::find_alias_for_opcode(alias_lut, sem_op);
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

    // ── Alias LUT ────────────────────────────────────────────────────────
    // ── Transition entries (native call targets) ─────────────────────────
    if (!native_calls.empty()) {
        const uint32_t trans_off = blob_section_trans(header);
        for (uint32_t i = 0; i < nc_count; ++i) {
            TransitionEntry te{};
            te.call_site_ip  = native_calls[i].call_site_ip;
            te.arg_count     = native_calls[i].arg_count;
            te.target_offset = native_calls[i].target_addr;
            te.coeff_a       = 0;  // v1: no polymorphic coefficients
            te.coeff_b       = 0;
            std::memcpy(blob.data() + trans_off + i * sizeof(TransitionEntry),
                        &te, sizeof(te));
        }
    }

    // Copy the alias_lut (built above) into the blob.
    // Same table: alias[i] = i % VM_OPCODE_COUNT.

    const uint32_t alias_off = blob_section_alias(header);
    std::memcpy(blob.data() + alias_off, alias_lut, 256);

    // ── Config section ───────────────────────────────────────────────────
    // Write 8 zero bytes (default config); the loader ignores this in v1.

    // (already zero from vector initialization)

    return blob;
}

/// Build a test blob with per-register pool encoding.
///
/// This overload accepts TestPoolEntry structs that specify which BB and
/// register each constant should be encoded for. This is necessary for
/// end-to-end tests where constants are loaded into different registers
/// (e.g., LOAD_CONST r0, pool[0]; LOAD_CONST r1, pool[1]).
///
/// @param stored_seed  32-byte root secret
/// @param bbs          basic blocks with plaintext instructions
/// @param pool_entries pool entries with per-register encoding info
/// @param debug_mode   set BLOB_FLAG_DEBUG if true
/// @return             complete encrypted blob bytes
inline std::vector<uint8_t> build_test_blob_ex(
    const uint8_t stored_seed[32],
    const std::vector<TestBB>& bbs,
    const std::vector<TestPoolEntry>& pool_entries,
    bool debug_mode = false) {

    // Extract plaintext values for header size computation
    std::vector<uint64_t> dummy_pool;
    dummy_pool.reserve(pool_entries.size());
    for (const auto& pe : pool_entries)
        dummy_pool.push_back(pe.plaintext);

    // ── Compute total instruction count across all BBs ───────────────────
    uint32_t total_insns = 0;
    for (const auto& bb : bbs)
        total_insns += static_cast<uint32_t>(bb.instructions.size());

    const uint32_t bb_count   = static_cast<uint32_t>(bbs.size());
    const uint32_t pool_count = static_cast<uint32_t>(pool_entries.size());

    // ── Build header ─────────────────────────────────────────────────────
    BlobHeader header{};
    header.magic             = VM_BLOB_MAGIC;
    header.version           = VM_BLOB_VERSION;
    header.flags             = debug_mode ? BLOB_FLAG_DEBUG : uint16_t(0);
    header.insn_count        = total_insns;
    header.bb_count          = bb_count;
    header.pool_entry_count  = pool_count;
    header.native_call_count = 0;
    header.reserved          = 0;
    header.total_size        = blob_expected_size(header);

    // ── Allocate blob ────────────────────────────────────────────────────
    std::vector<uint8_t> blob(header.total_size, 0);
    std::memcpy(blob.data(), &header, sizeof(header));

    // ── Derive keys ──────────────────────────────────────────────────────
    uint8_t fast_key[16];
    blake3_keyed_hash(stored_seed, reinterpret_cast<const uint8_t*>("fast"), 4, fast_key, 16);

    uint8_t meta_key[16];
    blake3_keyed_hash(stored_seed, reinterpret_cast<const uint8_t*>("meta"), 4, meta_key, 16);

    uint8_t pool_key[16];
    blake3_keyed_hash(stored_seed, reinterpret_cast<const uint8_t*>("pool"), 4, pool_key, 16);

    uint8_t integrity_key[32];
    blake3_keyed_hash(stored_seed, reinterpret_cast<const uint8_t*>("integrity"), 9, integrity_key, 32);

    // ── Build alias LUT ──────────────────────────────────────────────────
    uint8_t alias_lut[256];
    for (int i = 0; i < 256; ++i)
        alias_lut[i] = static_cast<uint8_t>(i % VM_OPCODE_COUNT);

    // ── Encrypt instructions ─────────────────────────────────────────────
    const uint32_t insn_offset = blob_section_insn(header);
    uint32_t global_ip = 0;

    for (const auto& bb : bbs) {
        uint8_t enc_seed_bytes[8];
        detail::derive_bb_enc_seed(stored_seed, bb.bb_id, enc_seed_bytes);

        uint64_t enc_state = 0;
        std::memcpy(&enc_state, enc_seed_bytes, 8);

        uint8_t opcode_perm[256], opcode_perm_inv[256];
        Encoding::derive_opcode_permutation(bb.epoch_seed, opcode_perm, opcode_perm_inv);

        for (uint32_t j = 0; j < static_cast<uint32_t>(bb.instructions.size()); ++j) {
            const auto& ti = bb.instructions[j];

            uint8_t semantic_op = static_cast<uint8_t>(ti.opcode);
            uint8_t alias = detail::find_alias_for_opcode(alias_lut, semantic_op);
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

            enc_state = detail::update_enc_state(enc_state, plain_u64);

            ++global_ip;
        }
    }

    // ── Encrypt constant pool ────────────────────────────────────────────
    // Doc 16: pool values are PLAINTEXT, encrypted only with SipHash (D1).
    // No per-register LUT pre-encoding — the runtime FPE-encodes at LOAD_CONST.
    const uint32_t pool_offset = blob_section_pool(header);

    for (uint32_t i = 0; i < pool_count; ++i) {
        const auto& pe = pool_entries[i];
        uint64_t plain = pe.plaintext;

        // Encrypt plaintext with SipHash keystream (D1 layer)
        uint64_t idx = static_cast<uint64_t>(i);
        uint8_t idx_bytes[8];
        std::memcpy(idx_bytes, &idx, 8);
        uint64_t keystream = siphash_2_4(pool_key, idx_bytes, 8);
        uint64_t enc_val = plain ^ keystream;
        std::memcpy(blob.data() + pool_offset + i * 8, &enc_val, 8);
    }

    // ── Encrypt BB metadata ──────────────────────────────────────────────
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

    // ── BB MACs ──────────────────────────────────────────────────────────
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
            uint8_t mac_alias = detail::find_alias_for_opcode(alias_lut, sem_op);
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

    // ── Alias LUT ────────────────────────────────────────────────────────
    const uint32_t alias_off = blob_section_alias(header);
    std::memcpy(blob.data() + alias_off, alias_lut, 256);

    return blob;
}

}  // namespace VMPilot::Test

#endif  // __RUNTIME_TEST_BLOB_BUILDER_HPP__
