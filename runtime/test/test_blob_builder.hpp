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
#include <vm/vm_crypto.hpp>
#include <vm/vm_encoding.hpp>

#include <cstdint>
#include <cstring>
#include <vector>

namespace VMPilot::Test {

using namespace Common::VM;
using namespace Common::VM::Crypto;

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

// ─────────────────────────────────────────────────────────────────────────────
// Encryption helpers (mirror the loader's decryption exactly)
// ─────────────────────────────────────────────────────────────────────────────

namespace detail {

/// Update enc_state: SipHash(enc_state_as_key_zeropadded_16, opcode(2)||aux(4))
inline uint64_t update_enc_state(uint64_t enc_state,
                                 uint16_t opcode_val,
                                 uint32_t aux) noexcept {
    uint8_t key[16] = {};
    std::memcpy(key, &enc_state, 8);
    uint8_t msg[6];
    std::memcpy(msg, &opcode_val, 2);
    std::memcpy(msg + 2, &aux, 4);
    return siphash_2_4(key, msg, 6);
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
    bool debug_mode = false) {

    // ── Compute total instruction count across all BBs ───────────────────
    uint32_t total_insns = 0;
    for (const auto& bb : bbs)
        total_insns += static_cast<uint32_t>(bb.instructions.size());

    const uint32_t bb_count   = static_cast<uint32_t>(bbs.size());
    const uint32_t pool_count = static_cast<uint32_t>(constant_pool_plaintext.size());

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

    // ── Derive keys (same derivation the loader uses) ────────────────────
    uint8_t fast_key[16];
    blake3_kdf(stored_seed, "fast", 4, fast_key, 16);

    uint8_t meta_key[16];
    blake3_kdf(stored_seed, "meta", 4, meta_key, 16);

    uint8_t pool_key[16];
    blake3_kdf(stored_seed, "pool", 4, pool_key, 16);

    uint8_t integrity_key[32];
    blake3_kdf(stored_seed, "integrity", 9, integrity_key, 32);

    // ── Encrypt instructions ─────────────────────────────────────────────
    //
    // For each BB:
    //   enc_state = bb_enc_seed (as uint64_t LE)
    //   for j in 0..len:
    //     keystream = SipHash(fast_key XOR enc_state, j)
    //     encrypted = plaintext XOR keystream
    //     enc_state = update_enc_state(enc_state, plaintext_opcode, plaintext_aux)

    const uint32_t insn_offset = blob_section_insn(header);
    uint32_t global_ip = 0;

    for (const auto& bb : bbs) {
        // Derive bb_enc_seed
        uint8_t enc_seed_bytes[8];
        detail::derive_bb_enc_seed(stored_seed, bb.bb_id, enc_seed_bytes);

        uint64_t enc_state = 0;
        std::memcpy(&enc_state, enc_seed_bytes, 8);

        for (uint32_t j = 0; j < static_cast<uint32_t>(bb.instructions.size()); ++j) {
            const auto& ti = bb.instructions[j];

            // Build plaintext 8-byte instruction
            VmInsn plain{};
            plain.opcode   = static_cast<uint16_t>(ti.opcode);
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

            // Advance enc_state with plaintext opcode + aux
            enc_state = detail::update_enc_state(
                enc_state, plain.opcode, plain.aux);

            ++global_ip;
        }
    }

    // ── Encrypt constant pool ────────────────────────────────────────────

    const uint32_t pool_offset = blob_section_pool(header);
    for (uint32_t i = 0; i < pool_count; ++i) {
        uint64_t idx = static_cast<uint64_t>(i);
        uint8_t idx_bytes[8];
        std::memcpy(idx_bytes, &idx, 8);
        uint64_t keystream = siphash_2_4(pool_key, idx_bytes, 8);
        uint64_t encrypted = constant_pool_plaintext[i] ^ keystream;
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

    const uint32_t mac_offset = blob_section_mac(header);

    for (uint32_t bb_idx = 0; bb_idx < bb_count; ++bb_idx) {
        const auto& tbb = bbs[bb_idx];
        const uint32_t n = static_cast<uint32_t>(tbb.instructions.size());

        // Build plaintext instruction bytes for this BB
        std::vector<uint8_t> plain_bytes(n * 8);
        for (uint32_t j = 0; j < n; ++j) {
            VmInsn insn{};
            insn.opcode   = static_cast<uint16_t>(tbb.instructions[j].opcode);
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
    // Identity mapping for test simplicity: alias[i] = i % VM_OPCODE_COUNT

    const uint32_t alias_off = blob_section_alias(header);
    for (int i = 0; i < 256; ++i) {
        blob[alias_off + i] = static_cast<uint8_t>(i % VM_OPCODE_COUNT);
    }

    // ── Config section ───────────────────────────────────────────────────
    // Write 8 zero bytes (default config); the loader ignores this in v1.

    // (already zero from vector initialization)

    return blob;
}

}  // namespace VMPilot::Test

#endif  // __RUNTIME_TEST_BLOB_BUILDER_HPP__
