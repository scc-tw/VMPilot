#include "test_blob_builder.hpp"

#include <vm_loader.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <vm/vm_blob.hpp>
#include <vm/vm_crypto.hpp>
#include <vm/vm_encoding.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <vector>
#include <numeric>

using namespace VMPilot::Common;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Common::VM::Crypto;
using namespace VMPilot::Common::VM::Encoding;
using namespace VMPilot::Runtime;
using namespace VMPilot::Test;

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────

/// Fixed test seed — deterministic across runs.
static void fill_test_seed(uint8_t seed[32]) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(i + 1);
}

/// Fixed epoch seed for a test BB.
static void fill_epoch_seed(uint8_t seed[32], uint8_t base) {
    for (int i = 0; i < 32; ++i)
        seed[i] = static_cast<uint8_t>(base + i);
}

/// Build a minimal 1-BB blob with a single HALT instruction.
static std::vector<uint8_t> make_minimal_blob(const uint8_t seed[32]) {
    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0x0001;  // only R0 live
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xAA);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    halt.flags  = 0;
    halt.reg_a  = 0;
    halt.reg_b  = 0;
    halt.aux    = 0;
    bb.instructions.push_back(halt);

    return build_test_blob(seed, {bb});
}

// ─────────────────────────────────────────────────────────────────────────────
// Tests
// ─────────────────────────────────────────────────────────────────────────────

TEST(BlobLoader, ValidMinimalBlob) {
    uint8_t seed[32];
    fill_test_seed(seed);

    auto blob = make_minimal_blob(seed);
    VmSecurityConfig config;

    auto result = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(result.has_value()) << "load_blob failed with code "
        << static_cast<uint32_t>(result.error());

    const LoadedVM& vm = result.value();
    const VMContext& ctx = vm.ctx;

    // Verify basic fields
    EXPECT_EQ(ctx.insn_count, 1u);
    EXPECT_EQ(ctx.bb_count, 1u);
    EXPECT_EQ(ctx.current_bb_id, 1u);
    EXPECT_EQ(ctx.current_epoch, 0u);
    EXPECT_EQ(ctx.vm_ip, 0u);
    EXPECT_EQ(ctx.insn_index_in_bb, 0u);
    EXPECT_EQ(ctx.vm_sp, VM_OBLIVIOUS_SIZE);
    EXPECT_FALSE(ctx.halted);
    EXPECT_FALSE(ctx.branch_taken);
    EXPECT_EQ(ctx.oram_nonce, 0u);
    EXPECT_EQ(ctx.shadow_depth, 0u);

    // Bytecodes pointer should be non-null and inside the blob
    EXPECT_NE(ctx.bytecodes, nullptr);
    EXPECT_GE(ctx.bytecodes, blob.data());
    EXPECT_LT(ctx.bytecodes, blob.data() + blob.size());

    // BB metadata should be populated
    EXPECT_NE(ctx.bb_metadata, nullptr);
    EXPECT_EQ(ctx.bb_metadata[0].bb_id, 1u);
}

TEST(BlobLoader, InvalidMagic) {
    uint8_t seed[32];
    fill_test_seed(seed);

    auto blob = make_minimal_blob(seed);

    // Corrupt the magic bytes
    blob[0] = 0xFF;
    blob[1] = 0xFF;

    VmSecurityConfig config;
    auto result = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobMagicMismatch);
}

TEST(BlobLoader, Truncated) {
    uint8_t seed[32];
    fill_test_seed(seed);

    // Blob too short for even a header
    uint8_t tiny[16] = {};
    VmSecurityConfig config;
    auto result = load_blob(tiny, sizeof(tiny), seed, config);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobTruncated);
}

TEST(BlobLoader, TruncatedBody) {
    uint8_t seed[32];
    fill_test_seed(seed);

    auto blob = make_minimal_blob(seed);

    // Pass valid header but truncated body
    VmSecurityConfig config;
    auto result = load_blob(blob.data(), sizeof(BlobHeader) + 1, seed, config);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobTruncated);
}

TEST(BlobLoader, InvalidVersion) {
    uint8_t seed[32];
    fill_test_seed(seed);

    auto blob = make_minimal_blob(seed);

    // Overwrite version field (bytes 4-5 in header)
    uint16_t bad_version = 99;
    std::memcpy(blob.data() + offsetof(BlobHeader, version),
                &bad_version, sizeof(bad_version));

    VmSecurityConfig config;
    auto result = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobVersionMismatch);
}

TEST(BlobLoader, KeyDerivation) {
    // Verify that the loader derives keys matching direct BLAKE3 KDF calls.
    uint8_t seed[32];
    fill_test_seed(seed);

    auto blob = make_minimal_blob(seed);
    VmSecurityConfig config;
    auto result = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(result.has_value());

    const VMContext& ctx = result.value().ctx;

    // Independently derive fast_key and compare
    uint8_t expected_fast[16];
    blake3_kdf(seed, "fast", 4, expected_fast, 16);
    EXPECT_EQ(std::memcmp(ctx.fast_key, expected_fast, 16), 0);

    // Independently derive oram_key and compare
    uint8_t expected_oram[16];
    blake3_kdf(seed, "oram", 4, expected_oram, 16);
    EXPECT_EQ(std::memcmp(ctx.oram_key, expected_oram, 16), 0);

    // Independently derive integrity_key and compare
    uint8_t expected_integrity[32];
    blake3_kdf(seed, "integrity", 9, expected_integrity, 32);
    EXPECT_EQ(std::memcmp(ctx.integrity_key, expected_integrity, 32), 0);
}

TEST(BlobLoader, MetadataDecryption) {
    // Build a blob with known epoch_seed, then verify the loader recovers it.
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 42;
    bb.epoch = 7;
    bb.live_regs_bitmap = 0x000F;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xBB);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions.push_back(halt);

    auto blob = build_test_blob(seed, {bb});
    VmSecurityConfig config;
    auto result = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(result.has_value());

    const LoadedVM& vm = result.value();
    ASSERT_EQ(vm.bb_meta_storage.size(), 1u);

    const BBMetadata& md = vm.bb_meta_storage[0];
    EXPECT_EQ(md.bb_id, 42u);
    EXPECT_EQ(md.epoch, 7u);
    EXPECT_EQ(md.entry_ip, 0u);
    EXPECT_EQ(md.insn_count_in_bb, 1u);
    EXPECT_EQ(md.live_regs_bitmap, 0x000F);

    // Verify epoch_seed matches what we put in
    uint8_t expected_epoch[32];
    fill_epoch_seed(expected_epoch, 0xBB);
    EXPECT_EQ(std::memcmp(md.epoch_seed, expected_epoch, 32), 0);
}

TEST(BlobLoader, EncodingTablesPopulated) {
    // After loading, reg_encode/reg_decode for live registers should be
    // non-trivial (not all zeros or identity).
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0x0003;  // R0, R1 live
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xCC);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions.push_back(halt);

    auto blob = build_test_blob(seed, {bb});
    VmSecurityConfig config;
    auto result = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(result.has_value());

    const VMContext& ctx = result.value().ctx;

    // Check that reg_encode[0][0] is not the identity permutation.
    // A random bijection on 256 elements equals identity with probability
    // 1/256! which is effectively zero.
    bool is_identity = true;
    for (int i = 0; i < 256; ++i) {
        if (ctx.reg_encode[0][0][i] != static_cast<uint8_t>(i)) {
            is_identity = false;
            break;
        }
    }
    EXPECT_FALSE(is_identity) << "reg_encode[0][0] is identity — tables not derived";

    // Verify encode/decode are inverses: decode[encode[x]] == x
    for (int x = 0; x < 256; ++x) {
        EXPECT_EQ(ctx.reg_decode[0][0][ctx.reg_encode[0][0][x]],
                  static_cast<uint8_t>(x));
    }

    // Verify opcode_perm is not identity
    bool perm_identity = true;
    for (int i = 0; i < 256; ++i) {
        if (ctx.opcode_perm[i] != static_cast<uint8_t>(i)) {
            perm_identity = false;
            break;
        }
    }
    EXPECT_FALSE(perm_identity) << "opcode_perm is identity — not derived";
}

TEST(BlobLoader, EncStateSeedMatch) {
    // Verify enc_state equals the expected bb_enc_seed for the first BB.
    uint8_t seed[32];
    fill_test_seed(seed);

    auto blob = make_minimal_blob(seed);
    VmSecurityConfig config;
    auto result = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(result.has_value());

    const VMContext& ctx = result.value().ctx;

    // Independently compute bb_enc_seed for bb_id=1
    uint8_t enc_msg[7];
    std::memcpy(enc_msg, "enc", 3);
    uint32_t bb_id = 1;
    std::memcpy(enc_msg + 3, &bb_id, 4);
    uint8_t expected_seed[8];
    blake3_keyed_hash(seed, enc_msg, 7, expected_seed, 8);

    uint64_t expected_enc_state = 0;
    std::memcpy(&expected_enc_state, expected_seed, 8);

    EXPECT_EQ(ctx.enc_state, expected_enc_state);
}

TEST(BlobLoader, ConstantPoolDecryption) {
    // Build a blob with known constant pool values, verify they survive
    // the encrypt-then-decrypt round-trip.
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0x0001;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xDD);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions.push_back(halt);

    std::vector<uint64_t> pool_plain = {
        0xDEADBEEFCAFEBABE,
        0x0123456789ABCDEF,
        0x0000000000000000,
        0xFFFFFFFFFFFFFFFF,
    };

    auto blob = build_test_blob(seed, {bb}, pool_plain);
    VmSecurityConfig config;
    auto result = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(result.has_value());

    const LoadedVM& vm = result.value();
    ASSERT_EQ(vm.decrypted_pool.size(), pool_plain.size() * 8);

    const auto* recovered = reinterpret_cast<const uint64_t*>(
        vm.decrypted_pool.data());
    for (size_t i = 0; i < pool_plain.size(); ++i) {
        EXPECT_EQ(recovered[i], pool_plain[i])
            << "pool entry " << i << " mismatch";
    }
}

TEST(BlobLoader, MultipleBBs) {
    // Verify a blob with multiple basic blocks loads correctly.
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb0{};
    bb0.bb_id = 10;
    bb0.epoch = 0;
    bb0.live_regs_bitmap = 0x0003;
    bb0.flags = 0;
    fill_epoch_seed(bb0.epoch_seed, 0x10);

    TestInstruction nop{};
    nop.opcode = VmOpcode::NOP;
    bb0.instructions.push_back(nop);
    bb0.instructions.push_back(nop);

    TestBB bb1{};
    bb1.bb_id = 20;
    bb1.epoch = 1;
    bb1.live_regs_bitmap = 0x000F;
    bb1.flags = BB_FLAG_EPOCH_CHANGED;
    fill_epoch_seed(bb1.epoch_seed, 0x20);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb1.instructions.push_back(halt);

    auto blob = build_test_blob(seed, {bb0, bb1});
    VmSecurityConfig config;
    auto result = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(result.has_value());

    const LoadedVM& vm = result.value();
    EXPECT_EQ(vm.ctx.insn_count, 3u);
    EXPECT_EQ(vm.ctx.bb_count, 2u);

    // First BB is entered at load time
    EXPECT_EQ(vm.ctx.current_bb_id, 10u);
    EXPECT_EQ(vm.ctx.vm_ip, 0u);

    // Metadata for both BBs
    ASSERT_EQ(vm.bb_meta_storage.size(), 2u);
    EXPECT_EQ(vm.bb_meta_storage[0].bb_id, 10u);
    EXPECT_EQ(vm.bb_meta_storage[0].insn_count_in_bb, 2u);
    EXPECT_EQ(vm.bb_meta_storage[1].bb_id, 20u);
    EXPECT_EQ(vm.bb_meta_storage[1].entry_ip, 2u);
    EXPECT_EQ(vm.bb_meta_storage[1].insn_count_in_bb, 1u);
}

TEST(BlobLoader, InvalidZeroInsnCount) {
    uint8_t seed[32];
    fill_test_seed(seed);

    auto blob = make_minimal_blob(seed);

    // Zero out insn_count in header
    uint32_t zero = 0;
    std::memcpy(blob.data() + offsetof(BlobHeader, insn_count),
                &zero, sizeof(zero));

    VmSecurityConfig config;
    auto result = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobHeaderInvalid);
}

TEST(BlobLoader, InvalidReservedNonZero) {
    uint8_t seed[32];
    fill_test_seed(seed);

    auto blob = make_minimal_blob(seed);

    // Set reserved field to non-zero
    uint32_t nonzero = 42;
    std::memcpy(blob.data() + offsetof(BlobHeader, reserved),
                &nonzero, sizeof(nonzero));

    VmSecurityConfig config;
    auto result = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobHeaderInvalid);
}
