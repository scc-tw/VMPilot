/// @file test_full_prp.cpp
/// @brief Full two-layer PRP with randomized alias tables (Phase 11+).
///
/// Tests the complete alias resolution pipeline:
///   Layer 1 (GSS): alias_lut[alias] -> semantic_opcode
///   Layer 2 (PRP): opcode_perm_inv[encrypted_alias] -> alias
///
/// The default test blob builder uses an identity alias table
/// (alias[i] = i % VM_OPCODE_COUNT). These tests verify that the
/// pipeline works correctly with non-identity, randomized alias tables
/// and with multiple epochs that derive different opcode permutations.
///
/// Why this matters for security: The two-layer PRP provides Shannon
/// perfect secrecy for opcodes (spec S4.2, S11.6). If the alias
/// resolution or PRP inversion is buggy, an attacker could distinguish
/// semantic opcodes from observed ciphertext values, defeating the
/// chosen-instruction attack defence.

#include "test_blob_builder.hpp"

#include <dispatcher.hpp>
#include <decoder.hpp>
#include <encoding.hpp>
#include <vm_loader.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_blob.hpp>
#include <vm/vm_encoding.hpp>
#include <vm/vm_crypto.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <cstring>
#include <cstdint>
#include <random>
#include <vector>

using namespace VMPilot::Common;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Common::VM::Encoding;
using namespace VMPilot::Common::VM::Crypto;
using namespace VMPilot::Runtime;
using namespace VMPilot::Test;

// Disambiguate detail namespace: use Test::detail for blob builder helpers
namespace test_detail = VMPilot::Test::detail;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static void fill_test_seed(uint8_t seed[32]) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(i + 1);
}

static void fill_epoch_seed(uint8_t seed[32], uint8_t base) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(base + i);
}

/// Make flags byte from operand types and condition.
static uint8_t make_flags(uint8_t op_a_type, uint8_t op_b_type,
                          uint8_t condition = 0) {
    return static_cast<uint8_t>((op_a_type << 6) | (op_b_type << 4) |
                                (condition & 0x0F));
}

static VmSecurityConfig default_config() {
    VmSecurityConfig cfg;
    cfg.debug_mode = true;
    return cfg;
}

/// Generate a random surjection [0..255] -> [0..VM_OPCODE_COUNT-1].
///
/// Every semantic opcode gets at least one alias (guaranteed by the
/// initial identity assignment). The remaining slots are filled randomly,
/// giving ~4.6 aliases per opcode on average (256 / 55).
/// The table is then shuffled to randomize alias positions.
///
/// This mimics the real compiler's alias table generation, which uses
/// random assignment to achieve a flat frequency distribution for
/// GSS (Galactic Side-channel Smoothing).
static void generate_random_alias_lut(uint8_t lut[256],
                                       std::mt19937& rng) {
    // Ensure every semantic opcode has at least one alias
    for (int i = 0; i < VM_OPCODE_COUNT; ++i)
        lut[i] = static_cast<uint8_t>(i);

    // Fill remaining slots randomly
    std::uniform_int_distribution<int> dist(0, VM_OPCODE_COUNT - 1);
    for (int i = VM_OPCODE_COUNT; i < 256; ++i)
        lut[i] = static_cast<uint8_t>(dist(rng));

    // Shuffle to randomize positions
    std::shuffle(lut, lut + 256, rng);
}

/// Build a blob with a custom (non-identity) alias LUT.
///
/// The standard build_test_blob uses identity alias (i % 55). This version
/// generates a randomized alias table and uses it consistently for both
/// instruction encryption (builder) and the alias section in the blob.
///
/// The key invariant: the builder encrypts instructions with the same
/// alias + PRP tables that the decoder will use for decryption.
static std::vector<uint8_t> build_blob_with_custom_alias(
    const uint8_t stored_seed[32],
    const std::vector<TestBB>& bbs,
    const std::vector<TestPoolEntry>& pool_entries,
    const uint8_t custom_alias_lut[256]) {

    // This is a modified version of build_test_blob_ex that uses a custom
    // alias LUT instead of the identity mapping.

    uint32_t total_insns = 0;
    for (const auto& bb : bbs)
        total_insns += static_cast<uint32_t>(bb.instructions.size());

    const uint32_t bb_count   = static_cast<uint32_t>(bbs.size());
    const uint32_t pool_count = static_cast<uint32_t>(pool_entries.size());

    BlobHeader header{};
    header.magic             = VM_BLOB_MAGIC;
    header.version           = VM_BLOB_VERSION;
    header.flags             = BLOB_FLAG_DEBUG;
    header.insn_count        = total_insns;
    header.bb_count          = bb_count;
    header.pool_entry_count  = pool_count;
    header.native_call_count = 0;
    header.reserved          = 0;
    header.total_size        = blob_expected_size(header);

    std::vector<uint8_t> blob(header.total_size, 0);
    std::memcpy(blob.data(), &header, sizeof(header));

    // Derive keys
    uint8_t fast_key[16];
    blake3_kdf(stored_seed, "fast", 4, fast_key, 16);

    uint8_t meta_key[16];
    blake3_kdf(stored_seed, "meta", 4, meta_key, 16);

    uint8_t pool_key[16];
    blake3_kdf(stored_seed, "pool", 4, pool_key, 16);

    uint8_t integrity_key[32];
    blake3_kdf(stored_seed, "integrity", 9, integrity_key, 32);

    // Use the custom alias LUT
    const uint8_t* alias_lut = custom_alias_lut;

    // Encrypt instructions
    const uint32_t insn_offset = blob_section_insn(header);
    uint32_t global_ip = 0;

    for (const auto& bb : bbs) {
        uint8_t enc_seed_bytes[8];
        {
            uint8_t msg[7];
            std::memcpy(msg, "enc", 3);
            std::memcpy(msg + 3, &bb.bb_id, 4);
            blake3_keyed_hash(stored_seed, msg, 7, enc_seed_bytes, 8);
        }

        uint64_t enc_state = 0;
        std::memcpy(&enc_state, enc_seed_bytes, 8);

        uint8_t opcode_perm[256], opcode_perm_inv[256];
        derive_opcode_permutation(bb.epoch_seed, opcode_perm, opcode_perm_inv);

        for (uint32_t j = 0; j < static_cast<uint32_t>(bb.instructions.size()); ++j) {
            const auto& ti = bb.instructions[j];

            uint8_t semantic_op = static_cast<uint8_t>(ti.opcode);
            uint8_t alias = test_detail::find_alias_for_opcode(alias_lut, semantic_op);
            uint8_t encrypted_alias = opcode_perm[alias];

            VmInsn plain{};
            plain.opcode   = static_cast<uint16_t>(encrypted_alias);
            plain.flags    = ti.flags;
            plain.reg_pack = static_cast<uint8_t>(
                (ti.reg_a << 4) | (ti.reg_b & 0x0F));
            plain.aux      = ti.aux;

            uint64_t plain_u64 = 0;
            std::memcpy(&plain_u64, &plain, 8);

            uint64_t keystream = siphash_keystream(fast_key, enc_state, j);
            uint64_t encrypted = plain_u64 ^ keystream;

            std::memcpy(blob.data() + insn_offset + global_ip * 8,
                        &encrypted, 8);

            enc_state = test_detail::update_enc_state(
                enc_state, plain.opcode, plain.aux);

            ++global_ip;
        }
    }

    // Encrypt constant pool (per-register encoding)
    const uint32_t pool_offset = blob_section_pool(header);

    struct BBEncodeTables {
        uint8_t tables[VM_REG_COUNT][VM_BYTE_LANES][256];
        bool derived = false;
    };
    std::vector<BBEncodeTables> bb_encode_cache(bb_count);

    for (uint32_t i = 0; i < pool_count; ++i) {
        const auto& pe = pool_entries[i];
        uint32_t bb_idx = pe.target_bb_index;
        uint8_t  reg    = pe.target_reg;

        if (bb_idx < bb_count && !bb_encode_cache[bb_idx].derived) {
            uint8_t decode_tables[VM_REG_COUNT][VM_BYTE_LANES][256];
            derive_register_tables(bbs[bb_idx].epoch_seed,
                                   bbs[bb_idx].live_regs_bitmap,
                                   bb_encode_cache[bb_idx].tables,
                                   decode_tables);
            bb_encode_cache[bb_idx].derived = true;
        }

        uint64_t encoded = 0;
        if (bb_idx < bb_count) {
            for (int k = 0; k < 8; ++k) {
                uint8_t lane = static_cast<uint8_t>(pe.plaintext >> (k * 8));
                encoded |= static_cast<uint64_t>(
                    bb_encode_cache[bb_idx].tables[reg & 0x0F][k][lane])
                    << (k * 8);
            }
        }

        uint64_t idx = static_cast<uint64_t>(i);
        uint8_t idx_bytes[8];
        std::memcpy(idx_bytes, &idx, 8);
        uint64_t keystream = siphash_2_4(pool_key, idx_bytes, 8);
        uint64_t enc_val = encoded ^ keystream;
        std::memcpy(blob.data() + pool_offset + i * 8, &enc_val, 8);
    }

    // Encrypt BB metadata
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
            uint64_t ks = siphash_2_4(meta_key, nonce_bytes, 8);
            words[w] ^= ks;
        }

        std::memcpy(blob.data() + meta_off + bb_idx * 64, words, 64);
        running_ip += static_cast<uint32_t>(tbb.instructions.size());
    }

    // BB MACs
    const uint32_t mac_offset = blob_section_mac(header);

    for (uint32_t bb_idx = 0; bb_idx < bb_count; ++bb_idx) {
        const auto& tbb = bbs[bb_idx];
        const uint32_t n = static_cast<uint32_t>(tbb.instructions.size());

        uint8_t mac_opcode_perm[256], mac_opcode_perm_inv[256];
        derive_opcode_permutation(tbb.epoch_seed,
                                  mac_opcode_perm, mac_opcode_perm_inv);

        std::vector<uint8_t> plain_bytes(n * 8);
        for (uint32_t j = 0; j < n; ++j) {
            uint8_t sem_op = static_cast<uint8_t>(tbb.instructions[j].opcode);
            uint8_t mac_alias = test_detail::find_alias_for_opcode(alias_lut, sem_op);
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
        blake3_keyed_hash(
            integrity_key, plain_bytes.data(), plain_bytes.size(), mac, 8);
        std::memcpy(blob.data() + mac_offset + bb_idx * 8, mac, 8);
    }

    // Alias LUT -- use the custom one
    const uint32_t alias_off = blob_section_alias(header);
    std::memcpy(blob.data() + alias_off, custom_alias_lut, 256);

    return blob;
}

// ---------------------------------------------------------------------------
// Test 1: RandomAliasTable
//
// Why this matters: The production compiler generates randomized alias
// tables where each of the 55 semantic opcodes maps to ~4.6 different
// alias values. If the runtime's two-layer PRP (opcode_perm_inv -> alias_lut)
// fails to correctly resolve a randomized alias, the wrong handler
// would be dispatched, causing silent corruption or crashes.
// ---------------------------------------------------------------------------

TEST(FullPRP, RandomAliasTable) {
    uint8_t seed[32];
    fill_test_seed(seed);

    // Generate random alias table
    std::mt19937 rng(42);
    uint8_t custom_alias[256];
    generate_random_alias_lut(custom_alias, rng);

    // Program: LOAD_CONST r0, pool[0]; LOAD_CONST r1, pool[1]; ADD r0, r1; HALT
    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xCC);

    TestInstruction lc0{};
    lc0.opcode = VmOpcode::LOAD_CONST;
    lc0.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc0.reg_a  = 0;
    lc0.aux    = 0;

    TestInstruction lc1{};
    lc1.opcode = VmOpcode::LOAD_CONST;
    lc1.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc1.reg_a  = 1;
    lc1.aux    = 1;

    TestInstruction add{};
    add.opcode = VmOpcode::ADD;
    add.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    add.reg_a  = 0;
    add.reg_b  = 1;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {lc0, lc1, add, halt};

    std::vector<TestPoolEntry> pool = {
        {42, 0, 0},
        {58, 0, 1},
    };

    auto blob = build_blob_with_custom_alias(seed, {bb}, pool, custom_alias);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value())
        << "load_blob failed: " << static_cast<uint32_t>(loaded.error());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value())
        << "execute failed: " << static_cast<uint32_t>(result.error());
    EXPECT_EQ(*result, VmResult::Halted);

    uint64_t decoded = decode_register(loaded->ctx, 0,
                                        loaded->ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 100u)
        << "Random alias table: ADD 42 + 58 should equal 100, got " << decoded;
}

// ---------------------------------------------------------------------------
// Test 2: MultipleEpochsNewPermutation
//
// Why this matters: Each epoch derives a different opcode permutation
// from its epoch_seed. A program that crosses multiple epoch boundaries
// exercises the PRP re-derivation at each enter_basic_block(). If the
// permutation tables are not correctly updated, opcode resolution would
// fail silently (wrong handler dispatched) or cause MAC verification
// errors (BB MAC was computed with the correct permutation).
// ---------------------------------------------------------------------------

TEST(FullPRP, MultipleEpochsNewPermutation) {
    uint8_t seed[32];
    fill_test_seed(seed);

    // Generate random alias table (shared across all BBs)
    std::mt19937 rng(42);
    uint8_t custom_alias[256];
    generate_random_alias_lut(custom_alias, rng);

    // BB0 (epoch 0): LOAD_CONST r0, pool[0]; JMP BB1
    TestBB bb0{};
    bb0.bb_id = 10;
    bb0.epoch = 0;
    bb0.live_regs_bitmap = 0xFFFF;
    bb0.flags = 0;
    fill_epoch_seed(bb0.epoch_seed, 0x10);

    TestInstruction lc0{};
    lc0.opcode = VmOpcode::LOAD_CONST;
    lc0.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc0.reg_a  = 0;
    lc0.aux    = 0;

    TestInstruction jmp1{};
    jmp1.opcode = VmOpcode::JMP;
    jmp1.aux    = 20;

    bb0.instructions = {lc0, jmp1};

    // BB1 (epoch 1): LOAD_CONST r1, pool[1]; ADD r0, r1; JMP BB2
    TestBB bb1{};
    bb1.bb_id = 20;
    bb1.epoch = 1;
    bb1.live_regs_bitmap = 0xFFFF;
    bb1.flags = BB_FLAG_EPOCH_CHANGED;
    fill_epoch_seed(bb1.epoch_seed, 0x20);

    TestInstruction lc1{};
    lc1.opcode = VmOpcode::LOAD_CONST;
    lc1.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_POOL);
    lc1.reg_a  = 1;
    lc1.aux    = 1;

    TestInstruction add{};
    add.opcode = VmOpcode::ADD;
    add.flags  = make_flags(VM_OPERAND_REG, VM_OPERAND_REG);
    add.reg_a  = 0;
    add.reg_b  = 1;

    TestInstruction jmp2{};
    jmp2.opcode = VmOpcode::JMP;
    jmp2.aux    = 30;

    bb1.instructions = {lc1, add, jmp2};

    // BB2 (epoch 2): HALT
    TestBB bb2{};
    bb2.bb_id = 30;
    bb2.epoch = 2;
    bb2.live_regs_bitmap = 0xFFFF;
    bb2.flags = BB_FLAG_EPOCH_CHANGED;
    fill_epoch_seed(bb2.epoch_seed, 0x30);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb2.instructions = {halt};

    // Pool: 42 -> BB0/r0, 58 -> BB1/r1
    std::vector<TestPoolEntry> pool = {
        {42, 0, 0},
        {58, 1, 1},
    };

    auto blob = build_blob_with_custom_alias(seed, {bb0, bb1, bb2},
                                              pool, custom_alias);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value())
        << "load_blob failed: " << static_cast<uint32_t>(loaded.error());

    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value())
        << "execute failed: " << static_cast<uint32_t>(result.error());
    EXPECT_EQ(*result, VmResult::Halted);

    uint64_t decoded = decode_register(loaded->ctx, 0,
                                        loaded->ctx.encoded_regs[0]);
    EXPECT_EQ(decoded, 100u)
        << "3-epoch program with random alias: 42 + 58 should equal 100, got "
        << decoded;
}

// ---------------------------------------------------------------------------
// Test 3: AllOpcodesResolveCorrectly
//
// Why this matters: With a randomized alias table, every semantic opcode
// must still resolve correctly through the two-layer PRP. This test
// verifies the PRP round-trip for all 55 opcodes by checking that
// opcode_perm_inv[opcode_perm[alias_for(op)]] == alias_for(op)
// and alias_lut[alias_for(op)] == op. This catches any off-by-one
// or table corruption bugs in the permutation derivation.
// ---------------------------------------------------------------------------

TEST(FullPRP, AllOpcodesResolveCorrectly) {
    // Generate random alias table
    std::mt19937 rng(42);
    uint8_t custom_alias[256];
    generate_random_alias_lut(custom_alias, rng);

    // Derive a permutation from a test epoch seed
    uint8_t epoch_seed[32];
    fill_epoch_seed(epoch_seed, 0xDD);

    uint8_t perm[256], perm_inv[256];
    derive_opcode_permutation(epoch_seed, perm, perm_inv);

    // For each semantic opcode, verify the full encode-decode round trip:
    //   semantic_op -> alias (via find_alias_for_opcode)
    //   alias -> encrypted (via perm[alias])
    //   encrypted -> recovered_alias (via perm_inv[encrypted])
    //   recovered_alias -> recovered_op (via custom_alias[recovered_alias])
    // This must equal the original semantic_op.
    for (uint8_t op = 0; op < VM_OPCODE_COUNT; ++op) {
        uint8_t alias = test_detail::find_alias_for_opcode(custom_alias, op);
        uint8_t encrypted = perm[alias];
        uint8_t recovered_alias = perm_inv[encrypted];
        uint8_t recovered_op = custom_alias[recovered_alias];

        EXPECT_EQ(recovered_op, op)
            << "PRP round-trip failed for opcode " << static_cast<int>(op)
            << " (" << to_string(static_cast<VmOpcode>(op)) << ")"
            << ": alias=" << static_cast<int>(alias)
            << " encrypted=" << static_cast<int>(encrypted)
            << " recovered_alias=" << static_cast<int>(recovered_alias)
            << " recovered_op=" << static_cast<int>(recovered_op);

        // Also verify the intermediate step
        EXPECT_EQ(recovered_alias, alias)
            << "PRP inverse not correct: perm_inv[perm[" << static_cast<int>(alias)
            << "]] = " << static_cast<int>(recovered_alias);
    }

    // Verify perm is actually a permutation (all 256 values appear exactly once)
    uint8_t seen[256] = {};
    for (int i = 0; i < 256; ++i) {
        ASSERT_LT(perm[i], 256);
        seen[perm[i]]++;
    }
    for (int i = 0; i < 256; ++i) {
        EXPECT_EQ(seen[i], 1)
            << "Permutation is not bijective: value " << i
            << " appears " << static_cast<int>(seen[i]) << " times";
    }
}
