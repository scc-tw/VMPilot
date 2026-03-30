/// @file test_anti_tamper.cpp
/// @brief Phase 9.2: Anti-tamper (whole-blob integrity) tests.
///
/// Tests verify that verify_blob_integrity() detects any post-load
/// modification to any section of the blob. The blob is loaded via
/// load_blob() (which computes the integrity hash), then modified
/// after loading to simulate runtime tampering. Since the hash was
/// computed from the original blob, any modification is detected.

#include "test_blob_builder.hpp"

#include <anti_tamper.hpp>
#include <dispatcher.hpp>
#include <decoder.hpp>
#include <vm_loader.hpp>
#include <vm/vm_context.hpp>
#include <vm/vm_config.hpp>
#include <vm/vm_blob.hpp>
#include <vm/vm_insn.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <cstdint>
#include <vector>

using namespace VMPilot::Common;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Runtime;
using namespace VMPilot::Test;

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

static VmSecurityConfig default_config() {
    VmSecurityConfig cfg;
    cfg.debug_mode = true;  // disable GUARD step for focused tests
    return cfg;
}

/// Build a minimal blob with one BB containing a single HALT instruction.
/// Returns a mutable blob vector suitable for tamper tests.
static std::vector<uint8_t> build_minimal_blob(const uint8_t seed[32]) {
    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xAA);

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;
    bb.instructions = {halt};

    return build_test_blob(seed, {bb});
}

/// Build a multi-instruction blob with CHECK_INTEGRITY + HALT.
static std::vector<uint8_t> build_check_integrity_blob(const uint8_t seed[32]) {
    TestBB bb{};
    bb.bb_id = 1;
    bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    bb.flags = 0;
    fill_epoch_seed(bb.epoch_seed, 0xBB);

    TestInstruction check{};
    check.opcode = VmOpcode::CHECK_INTEGRITY;

    TestInstruction halt{};
    halt.opcode = VmOpcode::HALT;

    bb.instructions = {check, halt};

    return build_test_blob(seed, {bb});
}

// ---------------------------------------------------------------------------
// Test 1: ValidBlobPasses
// ---------------------------------------------------------------------------

TEST(AntiTamper, ValidBlobPasses) {
    uint8_t seed[32];
    fill_test_seed(seed);

    auto blob = build_minimal_blob(seed);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value())
        << "load_blob failed: " << static_cast<uint32_t>(loaded.error());

    // Verify blob integrity with unmodified blob -- should succeed
    auto result = verify_blob_integrity(
        loaded->ctx.blob_data_ptr, loaded->ctx.blob_data_size, loaded->ctx);
    EXPECT_TRUE(result.has_value())
        << "verify_blob_integrity failed on valid blob: "
        << static_cast<uint32_t>(result.error());
}

// ---------------------------------------------------------------------------
// Test 2: TamperedInstructionFails
// ---------------------------------------------------------------------------

TEST(AntiTamper, TamperedInstructionFails) {
    uint8_t seed[32];
    fill_test_seed(seed);

    auto blob = build_minimal_blob(seed);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    // Tamper: flip a bit in the instruction section (offset 32 = first insn byte)
    BlobHeader hdr;
    std::memcpy(&hdr, blob.data(), sizeof(hdr));
    uint32_t insn_off = blob_section_insn(hdr);
    blob[insn_off] ^= 0x01;

    auto result = verify_blob_integrity(blob.data(), blob.size(), loaded->ctx);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::IntegrityCheckFailed);
}

// ---------------------------------------------------------------------------
// Test 3: TamperedMetadataFails
// ---------------------------------------------------------------------------

TEST(AntiTamper, TamperedMetadataFails) {
    uint8_t seed[32];
    fill_test_seed(seed);

    auto blob = build_minimal_blob(seed);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    // Tamper: flip a bit in the metadata section
    BlobHeader hdr;
    std::memcpy(&hdr, blob.data(), sizeof(hdr));
    uint32_t meta_off = blob_section_meta(hdr);
    blob[meta_off + 4] ^= 0x80;

    auto result = verify_blob_integrity(blob.data(), blob.size(), loaded->ctx);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::IntegrityCheckFailed);
}

// ---------------------------------------------------------------------------
// Test 4: TamperedMacFails
// ---------------------------------------------------------------------------

TEST(AntiTamper, TamperedMacFails) {
    uint8_t seed[32];
    fill_test_seed(seed);

    auto blob = build_minimal_blob(seed);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    // Tamper: flip a bit in the MAC section
    BlobHeader hdr;
    std::memcpy(&hdr, blob.data(), sizeof(hdr));
    uint32_t mac_off = blob_section_mac(hdr);
    blob[mac_off] ^= 0xFF;

    auto result = verify_blob_integrity(blob.data(), blob.size(), loaded->ctx);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::IntegrityCheckFailed);
}

// ---------------------------------------------------------------------------
// Test 5: TamperedAliasFails
// ---------------------------------------------------------------------------

TEST(AntiTamper, TamperedAliasFails) {
    uint8_t seed[32];
    fill_test_seed(seed);

    auto blob = build_minimal_blob(seed);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value());

    // Tamper: flip a bit in the alias LUT section
    BlobHeader hdr;
    std::memcpy(&hdr, blob.data(), sizeof(hdr));
    uint32_t alias_off = blob_section_alias(hdr);
    blob[alias_off + 100] ^= 0x42;

    auto result = verify_blob_integrity(blob.data(), blob.size(), loaded->ctx);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::IntegrityCheckFailed);
}

// ---------------------------------------------------------------------------
// Test 6: CheckIntegrityHandler -- valid blob passes through execution
// ---------------------------------------------------------------------------

TEST(AntiTamper, CheckIntegrityHandler) {
    uint8_t seed[32];
    fill_test_seed(seed);

    auto blob = build_check_integrity_blob(seed);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value())
        << "load_blob failed: " << static_cast<uint32_t>(loaded.error());

    // Execute: CHECK_INTEGRITY then HALT. Should succeed on valid blob.
    auto result = execute(loaded->ctx, config);
    ASSERT_TRUE(result.has_value())
        << "execute failed: " << static_cast<uint32_t>(result.error());
    EXPECT_EQ(*result, VmResult::Halted);
}

// ---------------------------------------------------------------------------
// Test 7: CheckIntegrityHandlerTampered -- tampered blob detected by handler
// ---------------------------------------------------------------------------

TEST(AntiTamper, CheckIntegrityHandlerTampered) {
    uint8_t seed[32];
    fill_test_seed(seed);

    auto blob = build_check_integrity_blob(seed);
    auto config = default_config();
    auto loaded = load_blob(blob.data(), blob.size(), seed, config);
    ASSERT_TRUE(loaded.has_value())
        << "load_blob failed: " << static_cast<uint32_t>(loaded.error());

    // Tamper AFTER loading: flip a bit in the config section (end of blob).
    // This won't affect the BB MAC (which covers instruction plaintext only),
    // but WILL be caught by verify_blob_integrity's whole-blob hash.
    BlobHeader hdr;
    std::memcpy(&hdr, blob.data(), sizeof(hdr));
    uint32_t config_off = blob_section_config(hdr);
    blob[config_off] ^= 0x01;

    // Execute: CHECK_INTEGRITY should fail because the blob has been modified.
    // The CHECK_INTEGRITY handler calls verify_bb_mac (which may pass since
    // we didn't modify instruction bytes) then verify_blob_integrity (which
    // WILL fail because the config section changed).
    auto result = execute(loaded->ctx, config);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::IntegrityCheckFailed);
}
