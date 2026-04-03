/// @file test_blob_view.cpp
/// @brief Tests for BlobView validation: malformed blobs are rejected with correct error codes.
///
/// Extracted from test_phase1_foundation.cpp into unit/ directory.

#include <vm/blob_view.hpp>
#include <vm/vm_blob.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <vector>

using namespace VMPilot::Common::VM;
using namespace VMPilot::Common;

// ============================================================================
// BlobView Validation Tests
// ============================================================================

/// Helper: build a minimal valid blob (1 BB, 2 instructions, 0 constants, 0 native calls).
static std::vector<uint8_t> build_minimal_blob() {
    BlobHeader hdr{};
    hdr.magic              = VM_BLOB_MAGIC;
    hdr.version            = VM_BLOB_VERSION;
    hdr.flags              = 0;
    hdr.insn_count         = 2;
    hdr.bb_count           = 1;
    hdr.pool_entry_count   = 0;
    hdr.native_call_count  = 0;
    hdr.reserved           = 0;
    hdr.total_size         = blob_expected_size(hdr);

    std::vector<uint8_t> blob(hdr.total_size, 0);
    std::memcpy(blob.data(), &hdr, sizeof(hdr));

    return blob;
}

TEST(BlobView, ValidMinimalBlob) {
    auto blob = build_minimal_blob();
    auto result = BlobView::create(blob.data(), blob.size());

    ASSERT_TRUE(result.has_value()) << "BlobView::create failed for valid blob";
    auto& view = *result;

    EXPECT_EQ(view.header().magic, VM_BLOB_MAGIC);
    EXPECT_EQ(view.header().version, VM_BLOB_VERSION);
    EXPECT_EQ(view.instructions().size(), 2u);
    EXPECT_EQ(view.constant_pool().size(), 0u);
    EXPECT_EQ(view.bb_metadata_raw().size(), 1u);
    EXPECT_EQ(view.native_calls().size(), 0u);
    EXPECT_EQ(view.raw_size(), blob.size());
    EXPECT_FALSE(view.is_debug());
}

TEST(BlobView, RejectBadMagic) {
    auto blob = build_minimal_blob();
    // Corrupt magic
    blob[0] = 0xFF;
    auto result = BlobView::create(blob.data(), blob.size());
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobMagicMismatch);
}

TEST(BlobView, RejectBadVersion) {
    auto blob = build_minimal_blob();
    // Corrupt version (bytes 4-5 in little-endian)
    blob[4] = 0xFF;
    blob[5] = 0xFF;
    auto result = BlobView::create(blob.data(), blob.size());
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobVersionMismatch);
}

TEST(BlobView, RejectTruncated) {
    auto blob = build_minimal_blob();
    // Pass correct header but truncated buffer
    auto result = BlobView::create(blob.data(), blob.size() - 1);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobTruncated);
}

TEST(BlobView, DebugFlagDetected) {
    auto blob = build_minimal_blob();
    auto* hdr = reinterpret_cast<BlobHeader*>(blob.data());
    hdr->flags = BLOB_FLAG_DEBUG;

    auto result = BlobView::create(blob.data(), blob.size());
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->is_debug());
}

// ============================================================================
// BlobView validation (from test_phase1_foundation.cpp)
// ============================================================================

TEST(BlobView, RejectNullPointer) {
    auto result = BlobView::create(nullptr, 0);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobHeaderInvalid);
}

TEST(BlobView, RejectTooSmallBuffer) {
    uint8_t buf[16] = {};
    auto result = BlobView::create(buf, sizeof(buf));
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobHeaderInvalid);
}

TEST(BlobView, RejectNonZeroReserved) {
    auto blob = build_minimal_blob();
    // reserved is at offset 28 (last 4 bytes of 32-byte header)
    blob[28] = 1;
    auto result = BlobView::create(blob.data(), blob.size());
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobHeaderInvalid);
}

TEST(BlobView, RejectZeroInsnCount) {
    auto blob = build_minimal_blob();
    auto* hdr = reinterpret_cast<BlobHeader*>(blob.data());
    hdr->insn_count = 0;
    hdr->total_size = blob_expected_size(*hdr);
    // Resize
    blob.resize(hdr->total_size, 0);

    auto result = BlobView::create(blob.data(), blob.size());
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobHeaderInvalid);
}

TEST(BlobView, RejectInconsistentTotalSize) {
    auto blob = build_minimal_blob();
    auto* hdr = reinterpret_cast<BlobHeader*>(blob.data());
    hdr->total_size += 8;  // claim larger than actual layout
    blob.resize(hdr->total_size, 0);  // buffer is big enough but layout disagrees

    auto result = BlobView::create(blob.data(), blob.size());
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobHeaderInvalid);
}

// ============================================================================
// EngineBlob tests (from test_engine_comprehensive.cpp)
// ============================================================================

#include "test_blob_builder.hpp"
#include "vm_engine.hpp"
#include "vm_state.hpp"
#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>
#include <vm/vm_crypto.hpp>

using namespace VMPilot::Runtime;
using namespace VMPilot::Test;

static uint8_t eb_none() { return 0; }

TEST(BlobView, EngineBlob_ValidMinimalBlob) {
    uint8_t seed[32]; fill_seed(seed);
    TestBB bb{}; bb.bb_id = 1; bb.epoch = 0;
    bb.live_regs_bitmap = 0xFFFF;
    fill_epoch(bb.epoch_seed, 0xAA);
    bb.instructions = {{VmOpcode::HALT, eb_none(), 0, 0, 0}};

    auto blob_data = build_test_blob(seed, {bb});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob_data.data(), blob_data.size(), seed);
    ASSERT_TRUE(engine.has_value());
}

TEST(BlobView, EngineBlob_RejectBadMagic) {
    uint8_t seed[32]; fill_seed(seed);
    TestBB bb{}; bb.bb_id = 1; bb.epoch = 0; bb.live_regs_bitmap = 0xFFFF;
    fill_epoch(bb.epoch_seed, 0xAA);
    bb.instructions = {{VmOpcode::HALT, eb_none(), 0, 0, 0}};
    auto blob_data = build_test_blob(seed, {bb});
    blob_data[0] = 0xFF; // corrupt magic

    auto engine = VmEngine<DebugPolicy>::create(blob_data.data(), blob_data.size(), seed);
    ASSERT_FALSE(engine.has_value());
    EXPECT_EQ(engine.error(), DiagnosticCode::BlobMagicMismatch);
}

TEST(BlobView, EngineBlob_RejectTruncated) {
    uint8_t seed[32]; fill_seed(seed);
    auto engine = VmEngine<DebugPolicy>::create(seed, 16, seed); // too small
    ASSERT_FALSE(engine.has_value());
}

TEST(BlobView, EngineBlob_KeyDerivationCorrect) {
    uint8_t seed[32]; fill_seed(seed);
    TestBB bb{}; bb.bb_id = 1; bb.epoch = 0; bb.live_regs_bitmap = 0xFFFF;
    fill_epoch(bb.epoch_seed, 0xBB);
    bb.instructions = {{VmOpcode::HALT, eb_none(), 0, 0, 0}};

    auto blob_data = build_test_blob(seed, {bb});
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob_data.data(), blob_data.size(), seed);
    ASSERT_TRUE(engine.has_value());

    // Verify keys were derived (fast_key, integrity_key are in VmImmutable)
    auto imm = engine->shared_immutable();
    uint8_t expected_fast[16];
    Crypto::blake3_keyed_hash(seed, reinterpret_cast<const uint8_t*>("fast"), 4, expected_fast, 16);
    EXPECT_EQ(std::memcmp(imm->fast_key, expected_fast, 16), 0);
}
