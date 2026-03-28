#include "test_blob_builder.hpp"

#include <encoding.hpp>
#include <vm_loader.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <vm/vm_encoding.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <cstdint>
#include <numeric>
#include <random>
#include <vector>

using namespace VMPilot::Common::VM;
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

/// Build a minimal blob and load it, returning the LoadedVM.
/// Keeps blob alive in the output parameter.
static LoadedVM load_test_vm(std::vector<uint8_t>& blob_storage,
                             uint16_t live_regs = 0xFFFF) {
    uint8_t seed[32];
    fill_test_seed(seed);

    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = live_regs;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xCC);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions.push_back(halt);

    blob_storage = build_test_blob(seed, {bb});
    VmSecurityConfig config;
    auto result = load_blob(blob_storage.data(), blob_storage.size(),
                            seed, config);
    // Tests calling this helper expect success.
    if (!result.has_value()) {
        ADD_FAILURE() << "load_blob failed with code "
                      << static_cast<uint32_t>(result.error());
        // Return default — tests will fail meaningfully.
        return LoadedVM{};
    }
    return std::move(result.value());
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 1: apply_byte_lane_lut basics
// ─────────────────────────────────────────────────────────────────────────────

TEST(Encoding, ApplyByteLaneLutIdentity) {
    // Build an identity LUT: lut[k][x] = x for all lanes.
    uint8_t lut[8][256];
    for (int k = 0; k < 8; ++k)
        std::iota(lut[k], lut[k] + 256, 0);

    // Identity table should leave every value unchanged.
    EXPECT_EQ(apply_byte_lane_lut(lut, 0x0000000000000000ULL),
              0x0000000000000000ULL);
    EXPECT_EQ(apply_byte_lane_lut(lut, 0xFFFFFFFFFFFFFFFFULL),
              0xFFFFFFFFFFFFFFFFULL);
    EXPECT_EQ(apply_byte_lane_lut(lut, 0xDEADBEEFCAFEBABEULL),
              0xDEADBEEFCAFEBABEULL);

    // Random values via deterministic PRNG
    std::mt19937_64 rng(42);
    for (int i = 0; i < 100; ++i) {
        uint64_t val = rng();
        EXPECT_EQ(apply_byte_lane_lut(lut, val), val);
    }
}

TEST(Encoding, ApplyByteLaneLutRandomTable) {
    // Build a non-identity random permutation LUT.
    uint8_t lut[8][256];
    std::mt19937 rng(123);
    for (int k = 0; k < 8; ++k) {
        std::iota(lut[k], lut[k] + 256, 0);
        std::shuffle(lut[k], lut[k] + 256, rng);
    }

    // With random tables, almost all non-zero values should change.
    uint64_t val = 0x0123456789ABCDEFULL;
    uint64_t result = apply_byte_lane_lut(lut, val);
    EXPECT_NE(result, val) << "Random LUT should change the value";

    // Verify lane-by-lane: each byte is independently substituted.
    for (int k = 0; k < 8; ++k) {
        uint8_t input_byte = static_cast<uint8_t>(val >> (k * 8));
        uint8_t output_byte = static_cast<uint8_t>(result >> (k * 8));
        EXPECT_EQ(output_byte, lut[k][input_byte])
            << "Lane " << k << " mismatch";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 2: encode/decode register round-trip
// ─────────────────────────────────────────────────────────────────────────────

TEST(Encoding, EncodeDecodeRoundTrip) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    const VMContext& ctx = vm.ctx;

    std::mt19937_64 rng(999);

    // Test all 16 registers with multiple random values.
    for (uint8_t r = 0; r < VM_REG_COUNT; ++r) {
        for (int i = 0; i < 50; ++i) {
            uint64_t plain = rng();
            uint64_t encoded = encode_register(ctx, r, plain);
            uint64_t decoded = decode_register(ctx, r, encoded);
            EXPECT_EQ(decoded, plain)
                << "Round-trip failed for reg " << static_cast<int>(r)
                << " value 0x" << std::hex << plain;
        }
    }
}

TEST(Encoding, EncodeIsNonTrivial) {
    // Encoding should actually change the value (with overwhelming probability).
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    const VMContext& ctx = vm.ctx;

    uint64_t plain = 0x0123456789ABCDEFULL;
    uint64_t encoded = encode_register(ctx, 0, plain);
    EXPECT_NE(encoded, plain)
        << "Encoding should transform the value";
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 3: store/load round-trip
// ─────────────────────────────────────────────────────────────────────────────

TEST(Encoding, StoreLoadRoundTrip) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    const VMContext& ctx = vm.ctx;

    std::mt19937_64 rng(777);

    for (uint8_t r = 0; r < VM_REG_COUNT; ++r) {
        for (int i = 0; i < 50; ++i) {
            // Start with a register-domain encoded value
            uint64_t plain = rng();
            uint64_t encoded_reg = encode_register(ctx, r, plain);

            // Store converts register-domain -> memory-domain
            uint64_t mem_val = encode_for_store(ctx, r, encoded_reg);

            // Load converts memory-domain -> register-domain
            uint64_t recovered_reg = decode_for_load(ctx, r, mem_val);

            EXPECT_EQ(recovered_reg, encoded_reg)
                << "Store/load round-trip failed for reg "
                << static_cast<int>(r);

            // Full chain: plaintext -> encode -> store -> load -> decode == plaintext
            uint64_t final_plain = decode_register(ctx, r, recovered_reg);
            EXPECT_EQ(final_plain, plain)
                << "Full encode-store-load-decode chain failed for reg "
                << static_cast<int>(r);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 4: RE table round-trip (epoch transition)
// ─────────────────────────────────────────────────────────────────────────────

TEST(Encoding, ReTableRoundTrip) {
    // We need tables from two different epochs. Derive them manually.
    uint8_t epoch_seed_A[32];
    uint8_t epoch_seed_B[32];
    fill_epoch_seed(epoch_seed_A, 0xAA);
    fill_epoch_seed(epoch_seed_B, 0xBB);

    uint16_t live = 0x000F;  // R0-R3 live

    uint8_t encode_A[16][8][256], decode_A[16][8][256];
    uint8_t encode_B[16][8][256], decode_B[16][8][256];

    derive_register_tables(epoch_seed_A, live, encode_A, decode_A);
    derive_register_tables(epoch_seed_B, live, encode_B, decode_B);

    // Derive RE_TABLE = encode_B . decode_A
    uint8_t re_tables[16][8][256];
    derive_re_tables(decode_A, encode_B, live, re_tables);

    // Build a VMContext with registers encoded in epoch A
    VMContext ctx{};
    std::mt19937_64 rng(555);

    uint64_t plain_values[4];
    for (int r = 0; r < 4; ++r) {
        plain_values[r] = rng();
        ctx.encoded_regs[r] = apply_byte_lane_lut(encode_A[r], plain_values[r]);
    }

    // Apply RE tables to transition from epoch A to epoch B
    apply_re_tables(ctx, re_tables, live);

    // Now decode with epoch B tables — should recover original plaintext
    for (int r = 0; r < 4; ++r) {
        uint64_t decoded = apply_byte_lane_lut(decode_B[r], ctx.encoded_regs[r]);
        EXPECT_EQ(decoded, plain_values[r])
            << "RE table round-trip failed for reg " << r;
    }
}

TEST(Encoding, ReTableSkipsDeadRegisters) {
    uint8_t epoch_seed_A[32];
    uint8_t epoch_seed_B[32];
    fill_epoch_seed(epoch_seed_A, 0xAA);
    fill_epoch_seed(epoch_seed_B, 0xBB);

    uint16_t live = 0x0005;  // R0, R2 live (not R1, R3)

    uint8_t encode_A[16][8][256], decode_A[16][8][256];
    uint8_t encode_B[16][8][256], decode_B[16][8][256];

    derive_register_tables(epoch_seed_A, live, encode_A, decode_A);
    derive_register_tables(epoch_seed_B, live, encode_B, decode_B);

    uint8_t re_tables[16][8][256];
    derive_re_tables(decode_A, encode_B, live, re_tables);

    VMContext ctx{};
    ctx.encoded_regs[0] = 0x1111111111111111ULL;
    ctx.encoded_regs[1] = 0x2222222222222222ULL;  // dead — should not change
    ctx.encoded_regs[2] = 0x3333333333333333ULL;
    ctx.encoded_regs[3] = 0x4444444444444444ULL;  // dead — should not change

    uint64_t old_r1 = ctx.encoded_regs[1];
    uint64_t old_r3 = ctx.encoded_regs[3];

    apply_re_tables(ctx, re_tables, live);

    EXPECT_EQ(ctx.encoded_regs[1], old_r1) << "Dead register R1 was modified";
    EXPECT_EQ(ctx.encoded_regs[3], old_r3) << "Dead register R3 was modified";
}

// ─────────────────────────────────────────────────────────────────────────────
// Test 5: Stress round-trip
// ─────────────────────────────────────────────────────────────────────────────

TEST(Encoding, StressRoundTrip) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    const VMContext& ctx = vm.ctx;

    std::mt19937_64 rng(12345);

    for (int i = 0; i < 1000; ++i) {
        uint8_t reg = static_cast<uint8_t>(rng() % VM_REG_COUNT);
        uint64_t plain = rng();

        // Encode -> decode round-trip
        uint64_t encoded = encode_register(ctx, reg, plain);
        uint64_t decoded = decode_register(ctx, reg, encoded);
        ASSERT_EQ(decoded, plain)
            << "Stress encode/decode failed at iteration " << i
            << " reg=" << static_cast<int>(reg)
            << " val=0x" << std::hex << plain;

        // Full store/load chain
        uint64_t mem = encode_for_store(ctx, reg, encoded);
        uint64_t loaded = decode_for_load(ctx, reg, mem);
        uint64_t final_plain = decode_register(ctx, reg, loaded);
        ASSERT_EQ(final_plain, plain)
            << "Stress store/load failed at iteration " << i
            << " reg=" << static_cast<int>(reg)
            << " val=0x" << std::hex << plain;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Edge cases
// ─────────────────────────────────────────────────────────────────────────────

TEST(Encoding, ZeroValue) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    const VMContext& ctx = vm.ctx;

    for (uint8_t r = 0; r < VM_REG_COUNT; ++r) {
        uint64_t encoded = encode_register(ctx, r, 0);
        uint64_t decoded = decode_register(ctx, r, encoded);
        EXPECT_EQ(decoded, 0ULL)
            << "Zero round-trip failed for reg " << static_cast<int>(r);
    }
}

TEST(Encoding, MaxValue) {
    std::vector<uint8_t> blob;
    auto vm = load_test_vm(blob);
    const VMContext& ctx = vm.ctx;

    for (uint8_t r = 0; r < VM_REG_COUNT; ++r) {
        uint64_t max_val = 0xFFFFFFFFFFFFFFFFULL;
        uint64_t encoded = encode_register(ctx, r, max_val);
        uint64_t decoded = decode_register(ctx, r, encoded);
        EXPECT_EQ(decoded, max_val)
            << "Max value round-trip failed for reg " << static_cast<int>(r);
    }
}
