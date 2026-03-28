#include <vm_loader.hpp>
#include <vm/vm_blob.hpp>
#include <vm/vm_crypto.hpp>
#include <vm/vm_encoding.hpp>

#include <cstring>
#include <algorithm>

namespace VMPilot::Runtime {

using namespace Common;
using namespace Common::VM;
using namespace Common::VM::Crypto;
using namespace Common::VM::Encoding;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/// Derive the per-BB encryption seed from stored_seed and bb_id.
/// bb_enc_seed = BLAKE3_keyed(stored_seed, "enc" || bb_id_le32)[0:8]
static void derive_bb_enc_seed(const uint8_t stored_seed[32],
                               uint32_t bb_id,
                               uint8_t out[8]) noexcept {
    uint8_t msg[4 + 3];  // "enc" + bb_id
    std::memcpy(msg, "enc", 3);
    std::memcpy(msg + 3, &bb_id, 4);
    uint8_t full[8];
    blake3_keyed_hash(stored_seed, msg, 7, full, 8);
    std::memcpy(out, full, 8);
}

// ---------------------------------------------------------------------------
// load_blob
// ---------------------------------------------------------------------------

tl::expected<LoadedVM, DiagnosticCode>
load_blob(const uint8_t* blob_data, size_t blob_size,
          const uint8_t stored_seed[32],
          const VmSecurityConfig& config) noexcept {

    // ── 1. Header validation ─────────────────────────────────────────────

    if (blob_size < sizeof(BlobHeader))
        return tl::make_unexpected(DiagnosticCode::BlobTruncated);

    BlobHeader header;
    std::memcpy(&header, blob_data, sizeof(BlobHeader));

    if (header.magic != VM_BLOB_MAGIC)
        return tl::make_unexpected(DiagnosticCode::BlobMagicMismatch);

    if (header.version != VM_BLOB_VERSION)
        return tl::make_unexpected(DiagnosticCode::BlobVersionMismatch);

    if (header.insn_count == 0 || header.bb_count == 0)
        return tl::make_unexpected(DiagnosticCode::BlobHeaderInvalid);

    if (header.reserved != 0)
        return tl::make_unexpected(DiagnosticCode::BlobHeaderInvalid);

    const uint32_t expected = blob_expected_size(header);
    if (expected != header.total_size)
        return tl::make_unexpected(DiagnosticCode::BlobHeaderInvalid);

    if (header.total_size > blob_size)
        return tl::make_unexpected(DiagnosticCode::BlobTruncated);

    // ── 2. Key derivation ────────────────────────────────────────────────

    LoadedVM vm;
    VMContext& ctx = vm.ctx;

    std::memcpy(ctx.stored_seed, stored_seed, 32);

    blake3_kdf(stored_seed, "fast", 4, ctx.fast_key, 16);

    blake3_kdf(stored_seed, "oram", 4, ctx.oram_key, 16);

    uint8_t meta_key[16];
    blake3_kdf(stored_seed, "meta", 4, meta_key, 16);

    uint8_t pool_key[16];
    blake3_kdf(stored_seed, "pool", 4, pool_key, 16);

    blake3_kdf(stored_seed, "integrity", 9, ctx.integrity_key, 32);

    // ── 3. Decrypt BB metadata ───────────────────────────────────────────
    //
    // Each SerializedBBMeta is 64 bytes = 8 x uint64_t words.
    // Decrypt: ciphertext[word] ^= SipHash(meta_key, bb_index * 8 + word)

    const uint32_t meta_offset = blob_section_meta(header);
    vm.bb_meta_storage.resize(header.bb_count);

    for (uint32_t bb = 0; bb < header.bb_count; ++bb) {
        // Copy encrypted metadata to a mutable buffer
        uint64_t words[8];
        std::memcpy(words, blob_data + meta_offset + bb * 64, 64);

        for (uint32_t w = 0; w < 8; ++w) {
            uint64_t nonce = static_cast<uint64_t>(bb) * 8 + w;
            uint8_t nonce_bytes[8];
            std::memcpy(nonce_bytes, &nonce, 8);
            uint64_t keystream = siphash_2_4(meta_key, nonce_bytes, 8);
            words[w] ^= keystream;
        }

        // Deserialize the plaintext metadata
        SerializedBBMeta smeta;
        std::memcpy(&smeta, words, 64);

        BBMetadata& md = vm.bb_meta_storage[bb];
        md.bb_id            = smeta.bb_id;
        md.epoch            = smeta.epoch;
        md.entry_ip         = smeta.entry_ip;
        md.insn_count_in_bb = smeta.insn_count_in_bb;
        md.live_regs_bitmap = smeta.live_regs_bitmap;
        std::memcpy(md.epoch_seed, smeta.epoch_seed, 32);

        // Derive bb_enc_seed for this BB
        derive_bb_enc_seed(stored_seed, md.bb_id, md.bb_enc_seed);
    }

    // ── 4. Decrypt constant pool ─────────────────────────────────────────
    //
    // Each 8-byte pool entry: pool[i] ^= SipHash(pool_key, i)

    const uint32_t pool_offset = blob_section_pool(header);
    const uint32_t pool_bytes = header.pool_entry_count * 8;
    vm.decrypted_pool.resize(pool_bytes);

    if (pool_bytes > 0) {
        std::memcpy(vm.decrypted_pool.data(),
                    blob_data + pool_offset, pool_bytes);

        auto* pool_words = reinterpret_cast<uint64_t*>(vm.decrypted_pool.data());
        for (uint32_t i = 0; i < header.pool_entry_count; ++i) {
            uint64_t idx = static_cast<uint64_t>(i);
            uint8_t idx_bytes[8];
            std::memcpy(idx_bytes, &idx, 8);
            uint64_t keystream = siphash_2_4(pool_key, idx_bytes, 8);
            pool_words[i] ^= keystream;
        }
    }

    // ── 5. Derive global memory encoding ─────────────────────────────────

    derive_memory_tables(stored_seed, ctx.mem_encode, ctx.mem_decode);

    // ── 6. Set VMContext fields ──────────────────────────────────────────

    // Bytecodes: point into the blob (encrypted; decoder decrypts per-insn)
    ctx.bytecodes  = blob_data + blob_section_insn(header);
    ctx.insn_count = header.insn_count;

    // Constant pool: point into our decrypted copy
    ctx.constant_pool = vm.decrypted_pool.data();
    ctx.pool_size     = header.pool_entry_count;

    // BB MACs: point into the blob
    ctx.bb_macs = blob_data + blob_section_mac(header);

    // Alias LUT: copy 256 bytes from blob
    const uint32_t alias_offset = blob_section_alias(header);
    std::memcpy(ctx.alias_lut, blob_data + alias_offset, 256);

    // BB metadata
    ctx.bb_metadata = vm.bb_meta_storage.data();
    ctx.bb_count    = header.bb_count;

    // Copy config (serialised as 8 raw bytes in the blob).
    // For v1 we trust the caller-provided config rather than the
    // blob-embedded one, so just store what was passed in.
    (void)config;  // blob section_config is authoritative for future use

    // ── 7. Initialize runtime state ──────────────────────────────────────

    std::memset(ctx.encoded_regs, 0, sizeof(ctx.encoded_regs));
    std::memset(ctx.oblivious_workspace, 0, sizeof(ctx.oblivious_workspace));
    ctx.oram_nonce    = 0;
    ctx.vm_sp         = VM_OBLIVIOUS_SIZE;  // stack grows downward
    ctx.shadow_depth  = 0;
    ctx.halted        = false;
    ctx.branch_taken  = false;
    ctx.branch_target_bb = 0;
    ctx.native_ctx    = nullptr;
    ctx.transition_table = nullptr;
    std::memset(ctx.trash_regs, 0, sizeof(ctx.trash_regs));
    std::memset(ctx.shadow_stack, 0, sizeof(ctx.shadow_stack));

    // ── 8. Enter first BB ────────────────────────────────────────────────

    const BBMetadata& first = vm.bb_meta_storage[0];
    ctx.current_bb_id    = first.bb_id;
    ctx.current_epoch    = first.epoch;
    ctx.vm_ip            = first.entry_ip;
    ctx.insn_index_in_bb = 0;

    // enc_state = bb_enc_seed interpreted as uint64_t LE
    uint64_t enc_seed_u64 = 0;
    std::memcpy(&enc_seed_u64, first.bb_enc_seed, 8);
    ctx.enc_state = enc_seed_u64;

    // Derive per-BB encoding tables from epoch_seed
    derive_register_tables(first.epoch_seed, first.live_regs_bitmap,
                           ctx.reg_encode, ctx.reg_decode);

    // Derive STORE/LOAD domain conversion tables
    derive_store_load_tables(ctx.reg_encode, ctx.reg_decode,
                             ctx.mem_encode, ctx.mem_decode,
                             first.live_regs_bitmap,
                             ctx.store_tables, ctx.load_tables);

    // Derive opcode permutation (D4)
    derive_opcode_permutation(first.epoch_seed,
                              ctx.opcode_perm, ctx.opcode_perm_inv);

    return vm;
}

// ---------------------------------------------------------------------------
// init_context
// ---------------------------------------------------------------------------

void init_context(VMContext& ctx) noexcept {
    std::memset(&ctx, 0, sizeof(ctx));
}

}  // namespace VMPilot::Runtime
