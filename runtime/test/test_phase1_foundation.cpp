/// @file test_phase1_foundation.cpp
/// @brief Tests for Phase 1 foundation types: Encoded<Domain> phantom types and BlobView.
///
/// These tests validate:
///   1. Phantom type safety: domain mixing is a compile-time error (verified by
///      the build — if these tests compile, the type system works).
///   2. BlobView validation: malformed blobs are rejected with correct error codes.
///   3. Domain conversion roundtrips: encode→decode = identity for all domains.
///   4. Byte-lane LUT application correctness.

#include <vm/encoded_value.hpp>
#include <vm/blob_view.hpp>
#include <vm/vm_blob.hpp>
#include <vm/vm_encoding.hpp>
#include <vm/vm_crypto.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <vector>

using namespace VMPilot::Common::VM;
using namespace VMPilot::Common;

// ============================================================================
// 1. Phantom Type Safety Tests
//    If these compile, the type system correctly prevents domain mixing.
// ============================================================================

TEST(PhantomTypes, SameDomainEquality) {
    RegVal a(0xDEADBEEFCAFEBABEull);
    RegVal b(0xDEADBEEFCAFEBABEull);
    RegVal c(0x1234567890ABCDEFull);

    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
}

TEST(PhantomTypes, DefaultInitZero) {
    RegVal r;
    MemVal m;
    PlainVal p;

    EXPECT_EQ(r.bits, 0u);
    EXPECT_EQ(m.bits, 0u);
    EXPECT_EQ(p.bits, 0u);
}

TEST(PhantomTypes, ByteLaneAccess) {
    RegVal v(0x0807060504030201ull);

    EXPECT_EQ(v.lane(0), 0x01u);
    EXPECT_EQ(v.lane(1), 0x02u);
    EXPECT_EQ(v.lane(2), 0x03u);
    EXPECT_EQ(v.lane(3), 0x04u);
    EXPECT_EQ(v.lane(4), 0x05u);
    EXPECT_EQ(v.lane(5), 0x06u);
    EXPECT_EQ(v.lane(6), 0x07u);
    EXPECT_EQ(v.lane(7), 0x08u);
}

TEST(PhantomTypes, ByteLaneSetRoundtrip) {
    RegVal v;
    for (unsigned k = 0; k < 8; ++k) {
        v.set_lane(k, static_cast<uint8_t>(k + 0xA0));
    }

    for (unsigned k = 0; k < 8; ++k) {
        EXPECT_EQ(v.lane(k), static_cast<uint8_t>(k + 0xA0));
    }
}

TEST(PhantomTypes, TriviallyCopyable) {
    // Zero runtime overhead: trivially copyable, same size as uint64_t
    static_assert(sizeof(RegVal) == sizeof(uint64_t));
    static_assert(sizeof(MemVal) == sizeof(uint64_t));
    static_assert(sizeof(PlainVal) == sizeof(uint64_t));
    static_assert(std::is_trivially_copyable_v<RegVal>);
    static_assert(std::is_trivially_copyable_v<MemVal>);
    static_assert(std::is_trivially_copyable_v<PlainVal>);

    SUCCEED();  // static_asserts are the real test
}

// ============================================================================
// 2. ByteLaneLUT Application Tests
// ============================================================================

TEST(ByteLaneLUT, IdentityTable) {
    // Identity permutation: LUT[k][x] = x for all k, x
    uint8_t identity[8][256];
    for (int k = 0; k < 8; ++k)
        for (int x = 0; x < 256; ++x)
            identity[k][x] = static_cast<uint8_t>(x);

    ByteLaneLUT lut{identity};

    // apply(v) == v for identity
    uint64_t val = 0xDEADBEEFCAFEBABEull;
    EXPECT_EQ(lut.apply(val), val);
}

TEST(ByteLaneLUT, ReverseTable) {
    // Each lane reverses the byte: LUT[k][x] = 255 - x
    uint8_t rev[8][256];
    for (int k = 0; k < 8; ++k)
        for (int x = 0; x < 256; ++x)
            rev[k][x] = static_cast<uint8_t>(255 - x);

    ByteLaneLUT lut{rev};

    uint64_t val = 0x0000000000000001ull;
    uint64_t result = lut.apply(val);

    // lane 0: 255 - 1 = 254 = 0xFE
    // lane 1..7: 255 - 0 = 255 = 0xFF
    EXPECT_EQ(result, 0xFFFFFFFFFFFFFFFEull);
}

// ============================================================================
// 3. Domain Conversion Roundtrip Tests
// ============================================================================

TEST(DomainConversion, EncodeDecodeRoundtrip) {
    // Generate a real bijection from a seed, then verify encode→decode = identity
    uint8_t seed[32];
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);

    uint8_t encode_tbl[8][256];
    uint8_t decode_tbl[8][256];

    // Derive tables for a single register (lane by lane)
    for (int k = 0; k < 8; ++k) {
        uint8_t lane_seed[32];
        // Derive per-lane seed via BLAKE3 KDF with lane index as context
        char ctx_str[8];
        int ctx_len = std::snprintf(ctx_str, sizeof(ctx_str), "lane%d", k);
        Crypto::blake3_kdf(seed, ctx_str, static_cast<size_t>(ctx_len), lane_seed, 32);

        Encoding::generate_bijection(lane_seed, encode_tbl[k]);
        Encoding::invert_bijection(encode_tbl[k], decode_tbl[k]);
    }

    ByteLaneLUT enc{encode_tbl};
    ByteLaneLUT dec{decode_tbl};

    // Test 1000 random-ish values
    for (uint64_t i = 0; i < 1000; ++i) {
        uint64_t plain = i * 0x0101010101010101ull + i;
        PlainVal pv(plain);

        RegVal encoded = encode_register(enc, pv);
        PlainVal decoded = decode_register(dec, encoded);

        EXPECT_EQ(decoded.bits, pv.bits)
            << "Roundtrip failed for plain=" << plain;
    }
}

TEST(DomainConversion, StoreLoadRoundtrip) {
    // Verify store_convert → load_convert = identity
    // (composed bijections: STORE = mem_enc ∘ reg_dec, LOAD = reg_enc ∘ mem_dec)

    uint8_t seed[32];
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 0x42);

    // Derive register encoding tables
    uint8_t reg_enc[16][8][256], reg_dec[16][8][256];
    Encoding::derive_register_tables(seed, 0xFFFF, reg_enc, reg_dec);

    // Derive memory encoding tables
    uint8_t mem_enc[8][256], mem_dec[8][256];
    Encoding::derive_memory_tables(seed, mem_enc, mem_dec);

    // Derive store/load tables
    uint8_t store_tbl[16][8][256], load_tbl[16][8][256];
    Encoding::derive_store_load_tables(reg_enc, reg_dec, mem_enc, mem_dec,
                                       0xFFFF, store_tbl, load_tbl);

    // For register 0: store_convert(load_convert(v)) should give back the same RegVal
    ByteLaneLUT store_lut{store_tbl[0]};
    ByteLaneLUT load_lut{load_tbl[0]};

    for (uint64_t i = 0; i < 100; ++i) {
        RegVal original(i * 0xABCDABCDABCDABCDull + i);

        MemVal stored = store_convert(store_lut, original);
        RegVal loaded = load_convert(load_lut, stored);

        EXPECT_EQ(loaded, original)
            << "Store→Load roundtrip failed for reg 0, value=" << original.bits;
    }
}

// ============================================================================
// 4. BlobView Validation Tests
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

TEST(BlobView, RejectNonZeroReserved) {
    auto blob = build_minimal_blob();
    // reserved is at offset 28 (last 4 bytes of 32-byte header)
    blob[28] = 1;
    auto result = BlobView::create(blob.data(), blob.size());
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobHeaderInvalid);
}

TEST(BlobView, RejectTruncatedBlob) {
    auto blob = build_minimal_blob();
    // Pass correct header but truncated buffer
    auto result = BlobView::create(blob.data(), blob.size() - 1);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BlobTruncated);
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

TEST(BlobView, DebugFlagDetection) {
    auto blob = build_minimal_blob();
    auto* hdr = reinterpret_cast<BlobHeader*>(blob.data());
    hdr->flags = BLOB_FLAG_DEBUG;

    auto result = BlobView::create(blob.data(), blob.size());
    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(result->is_debug());
}
