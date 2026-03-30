/// @file test_handover.cpp
/// Integration tests: full Loader pipeline (PayloadBuilder + StubEmitter).
/// Verifies the "handover" contract between SDK pipeline output and Loader
/// consumption — payload layout, machine-code fixups, cross-arch parity,
/// and error paths.

#include <ArchTraits.hpp>
#include <BinaryEditor.hpp>
#include <Loader.hpp>
#include <LoaderTypes.hpp>
#include <PayloadBuilder.hpp>
#include <PlatformTraits.hpp>
#include <StubEmitter.hpp>

#include <diagnostic_collector.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <numeric>
#include <vector>

using namespace VMPilot::Loader;
using namespace VMPilot::Common;
using DC = DiagnosticCode;

// ============================================================
// Helpers
// ============================================================

namespace {

std::vector<uint8_t> make_blob(size_t n) {
    std::vector<uint8_t> v(n);
    std::iota(v.begin(), v.end(), uint8_t{0x10});
    return v;
}

constexpr std::array<uint8_t, SEED_SIZE> TEST_SEED = {
    0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11, 0x22,
    0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x00,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
};

constexpr size_t CALL_SLOT_SIZE = 8;

// Typical ELF segment layout
constexpr uint64_t TEXT_BASE  = 0x401000;
constexpr uint64_t TEXT_SIZE  = 0x2000;
constexpr uint64_t SEGMENT_VA = 0x500000;

// ARM64 B instruction decoder: returns byte offset from instruction PC
int64_t decode_arm64_b_offset(uint32_t insn) {
    int32_t imm26 = static_cast<int32_t>((insn & 0x03FFFFFFu) << 6) >> 6;
    return static_cast<int64_t>(imm26) * 4;
}

uint32_t read32_le(const uint8_t* p) {
    uint32_t v;
    std::memcpy(&v, p, 4);
    return v;
}

int32_t read_i32_le(const uint8_t* p) {
    int32_t v;
    std::memcpy(&v, p, 4);
    return v;
}

uint64_t read64_le(const uint8_t* p) {
    uint64_t v;
    std::memcpy(&v, p, 8);
    return v;
}

}  // namespace

// ============================================================
// x86_64 Pipeline Integration
// ============================================================

class HandoverX64 : public ::testing::Test {
protected:
    void SetUp() override {
        emitter = create_emitter(FileArch::X86, FileMode::MODE_64);
        ASSERT_NE(emitter, nullptr);
    }
    std::unique_ptr<StubEmitter> emitter;
    DiagnosticCollector diag;
};

// --- Payload data integrity ---

TEST_F(HandoverX64, BlobPreservedAtOffset0) {
    auto blob = make_blob(256);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, 32}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());

    ASSERT_GE(r->data.size(), blob.size());
    EXPECT_EQ(std::memcmp(r->data.data(), blob.data(), blob.size()), 0);
    EXPECT_EQ(r->blob_size, 256u);
}

TEST_F(HandoverX64, SeedFollowsBlob) {
    auto blob = make_blob(128);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, 32}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());

    EXPECT_EQ(r->seed_offset, 128u);
    EXPECT_EQ(std::memcmp(r->data.data() + r->seed_offset,
                           TEST_SEED.data(), SEED_SIZE), 0);
}

TEST_F(HandoverX64, CallSlotZeroInitialized) {
    auto blob = make_blob(128);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, 32}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());

    EXPECT_EQ(r->call_slot_offset, 128u + SEED_SIZE);
    uint64_t slot = read64_le(r->data.data() + r->call_slot_offset);
    EXPECT_EQ(slot, 0u) << "call_slot must be zero (runtime fills it)";
}

TEST_F(HandoverX64, StubFollowsCallSlot) {
    auto blob = make_blob(64);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, 32}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r->layouts.size(), 1u);

    EXPECT_EQ(r->layouts[0].stub_offset, 64u + SEED_SIZE + CALL_SLOT_SIZE);
    EXPECT_GT(r->layouts[0].stub_size, 0u);
}

TEST_F(HandoverX64, PayloadTotalSizeMatchesLayout) {
    auto blob = make_blob(100);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, 32}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());

    const auto& last = r->layouts.back();
    EXPECT_EQ(r->data.size(), last.stub_offset + last.stub_size);
}

// --- Stub structure ---

TEST_F(HandoverX64, StubEndsWithJmpRel32) {
    auto blob = make_blob(64);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, 32}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());

    const auto& layout = r->layouts[0];
    const uint8_t* stub = r->data.data() + layout.stub_offset;
    ASSERT_GE(layout.stub_size, 5u);
    EXPECT_EQ(stub[layout.stub_size - 5], 0xE9) << "last instruction not JMP rel32";
}

TEST_F(HandoverX64, ResumeJumpTargetsRegionEnd) {
    constexpr uint64_t REGION_SIZE = 32;
    auto blob = make_blob(64);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, REGION_SIZE}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());

    const auto& layout = r->layouts[0];
    const uint8_t* stub = r->data.data() + layout.stub_offset;

    // Decode JMP rel32 at end of stub
    int32_t rel32 = read_i32_le(&stub[layout.stub_size - 4]);
    // x86_64: target = (addr_of_disp32 + 4) + rel32
    uint64_t disp32_addr = SEGMENT_VA + layout.stub_offset + layout.stub_size - 4;
    uint64_t target = static_cast<uint64_t>(
        static_cast<int64_t>(disp32_addr + 4) + rel32);

    EXPECT_EQ(target, TEXT_BASE + REGION_SIZE)
        << "resume JMP should target region end";
}

TEST_F(HandoverX64, RegionPatchTargetsStub) {
    auto blob = make_blob(64);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, 32}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());

    uint64_t stub_va = SEGMENT_VA + r->layouts[0].stub_offset;

    // Generate the JMP/NOP patch that replaces the protected region
    auto patch = emitter->emit_region_patch(32, TEXT_BASE, stub_va);
    ASSERT_TRUE(patch.has_value());
    ASSERT_EQ(patch->size(), 32u);

    EXPECT_EQ((*patch)[0], 0xE9);

    int32_t rel32 = read_i32_le(patch->data() + 1);
    uint64_t target = static_cast<uint64_t>(
        static_cast<int64_t>(TEXT_BASE + 5) + rel32);
    EXPECT_EQ(target, stub_va);

    // Rest is NOP fill
    for (size_t i = 5; i < 32; ++i)
        EXPECT_EQ((*patch)[i], 0x90) << "byte " << i;
}

TEST_F(HandoverX64, BlobSizeImmediate) {
    constexpr size_t BLOB_SIZE = 0x1234;
    auto blob = make_blob(BLOB_SIZE);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, 32}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());

    // Get a fresh stub to know where size_fixup_offset is
    auto s = emitter->emit_entry_stub();
    ASSERT_TRUE(s.has_value());

    const uint8_t* stub = r->data.data() + r->layouts[0].stub_offset;
    uint32_t patched_size;
    std::memcpy(&patched_size, &stub[s->size_fixup_offset], 4);
    EXPECT_EQ(patched_size, static_cast<uint32_t>(BLOB_SIZE));
}

// --- Multiple regions ---

TEST_F(HandoverX64, MultipleRegionsNonOverlapping) {
    auto blob = make_blob(128);
    std::vector<RegionPatchInfo> regions = {
        {"fn_a", TEXT_BASE,         64},
        {"fn_b", TEXT_BASE + 0x100, 32},
        {"fn_c", TEXT_BASE + 0x200, 48},
    };

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r->layouts.size(), 3u);

    for (size_t i = 1; i < r->layouts.size(); ++i) {
        size_t prev_end = r->layouts[i - 1].stub_offset +
                          r->layouts[i - 1].stub_size;
        EXPECT_LE(prev_end, r->layouts[i].stub_offset)
            << "stub " << i - 1 << " overlaps stub " << i;
    }
}

TEST_F(HandoverX64, MultipleRegionsEachResumesCorrectly) {
    auto blob = make_blob(128);
    std::vector<RegionPatchInfo> regions = {
        {"fn_a", TEXT_BASE,         64},
        {"fn_b", TEXT_BASE + 0x100, 32},
    };

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r->layouts.size(), 2u);

    for (size_t i = 0; i < 2; ++i) {
        const auto& layout = r->layouts[i];
        const uint8_t* stub = r->data.data() + layout.stub_offset;
        ASSERT_GE(layout.stub_size, 5u);
        ASSERT_EQ(stub[layout.stub_size - 5], 0xE9) << "region " << i;

        int32_t rel32 = read_i32_le(&stub[layout.stub_size - 4]);
        uint64_t disp_addr = SEGMENT_VA + layout.stub_offset + layout.stub_size - 4;
        uint64_t target = static_cast<uint64_t>(
            static_cast<int64_t>(disp_addr + 4) + rel32);
        EXPECT_EQ(target, regions[i].addr + regions[i].size)
            << "region " << i << " resume mismatch";
    }
}

// ============================================================
// ARM64 Pipeline Integration
// ============================================================

class HandoverARM64 : public ::testing::Test {
protected:
    void SetUp() override {
        emitter = create_emitter(FileArch::ARM64, FileMode::MODE_LITTLE_ENDIAN);
        ASSERT_NE(emitter, nullptr);
    }
    std::unique_ptr<StubEmitter> emitter;
    DiagnosticCollector diag;
};

TEST_F(HandoverARM64, PayloadLayoutCorrect) {
    auto blob = make_blob(256);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, 64}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());

    EXPECT_EQ(r->blob_size, 256u);
    EXPECT_EQ(r->seed_offset, 256u);
    EXPECT_EQ(r->call_slot_offset, 256u + SEED_SIZE);

    ASSERT_EQ(r->layouts.size(), 1u);
    EXPECT_EQ(r->layouts[0].stub_offset, 256u + SEED_SIZE + CALL_SLOT_SIZE);
    EXPECT_EQ(r->layouts[0].stub_size % 4, 0u)
        << "ARM64 stub must be instruction-aligned";
}

TEST_F(HandoverARM64, StubEndsWithBranch) {
    auto blob = make_blob(64);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, 64}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());

    const auto& layout = r->layouts[0];
    const uint8_t* stub = r->data.data() + layout.stub_offset;
    ASSERT_GE(layout.stub_size, 4u);

    uint32_t last = read32_le(&stub[layout.stub_size - 4]);
    EXPECT_EQ(last >> 26, 0b000101u) << "last instruction not B imm26";
}

TEST_F(HandoverARM64, ResumeJumpTargetsRegionEnd) {
    constexpr uint64_t REGION_SIZE = 64;
    auto blob = make_blob(64);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, REGION_SIZE}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());

    const auto& layout = r->layouts[0];
    const uint8_t* stub = r->data.data() + layout.stub_offset;

    uint32_t insn = read32_le(&stub[layout.stub_size - 4]);
    int64_t offset = decode_arm64_b_offset(insn);
    uint64_t b_pc = SEGMENT_VA + layout.stub_offset + layout.stub_size - 4;
    uint64_t target = static_cast<uint64_t>(static_cast<int64_t>(b_pc) + offset);

    EXPECT_EQ(target, TEXT_BASE + REGION_SIZE)
        << "resume B should target region end";
}

TEST_F(HandoverARM64, RegionPatchTargetsStub) {
    auto blob = make_blob(64);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, 64}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());

    uint64_t stub_va = SEGMENT_VA + r->layouts[0].stub_offset;

    auto patch = emitter->emit_region_patch(64, TEXT_BASE, stub_va);
    ASSERT_TRUE(patch.has_value());
    ASSERT_EQ(patch->size(), 64u);

    // First instruction: B
    uint32_t insn = read32_le(patch->data());
    EXPECT_EQ(insn >> 26, 0b000101u);

    int64_t offset = decode_arm64_b_offset(insn);
    uint64_t target = static_cast<uint64_t>(
        static_cast<int64_t>(TEXT_BASE) + offset);
    EXPECT_EQ(target, stub_va);

    // Remaining: NOP
    for (size_t i = 4; i < 64; i += 4) {
        uint32_t nop = read32_le(patch->data() + i);
        EXPECT_EQ(nop, 0xD503201Fu) << "non-NOP at offset " << i;
    }
}

TEST_F(HandoverARM64, DeltaFixupSizeIs16Bytes) {
    auto s = emitter->emit_entry_stub();
    ASSERT_TRUE(s.has_value());
    EXPECT_EQ(s->delta_fixup_size, 16u)
        << "ARM64 delta = MOVZ + 3*MOVK = 4 instructions = 16 bytes";
}

TEST_F(HandoverARM64, DeltaStaticVAEncodesCorrectly) {
    auto blob = make_blob(64);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, 64}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());

    // Get stub info for fixup offsets
    auto s = emitter->emit_entry_stub();
    ASSERT_TRUE(s.has_value());
    ASSERT_EQ(s->delta_fixup_size, 16u);

    const uint8_t* stub = r->data.data() + r->layouts[0].stub_offset;

    // Expected static VA = segment_va + stub_offset + delta_ref_offset
    uint64_t expected_va = SEGMENT_VA + r->layouts[0].stub_offset + s->delta_ref_offset;

    // Decode MOVZ + 3*MOVK at delta_fixup_offset
    uint32_t i0 = read32_le(&stub[s->delta_fixup_offset + 0]);
    uint32_t i1 = read32_le(&stub[s->delta_fixup_offset + 4]);
    uint32_t i2 = read32_le(&stub[s->delta_fixup_offset + 8]);
    uint32_t i3 = read32_le(&stub[s->delta_fixup_offset + 12]);

    uint64_t decoded = 0;
    decoded |= static_cast<uint64_t>((i0 >> 5) & 0xFFFFu) <<  0;
    decoded |= static_cast<uint64_t>((i1 >> 5) & 0xFFFFu) << 16;
    decoded |= static_cast<uint64_t>((i2 >> 5) & 0xFFFFu) << 32;
    decoded |= static_cast<uint64_t>((i3 >> 5) & 0xFFFFu) << 48;

    EXPECT_EQ(decoded, expected_va);
}

TEST_F(HandoverARM64, MultipleRegionsEachResumesCorrectly) {
    auto blob = make_blob(128);
    std::vector<RegionPatchInfo> regions = {
        {"fn_a", TEXT_BASE,         64},
        {"fn_b", TEXT_BASE + 0x100, 32},
    };

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());
    ASSERT_EQ(r->layouts.size(), 2u);

    for (size_t i = 0; i < 2; ++i) {
        const auto& layout = r->layouts[i];
        const uint8_t* stub = r->data.data() + layout.stub_offset;

        uint32_t insn = read32_le(&stub[layout.stub_size - 4]);
        ASSERT_EQ(insn >> 26, 0b000101u) << "region " << i;

        int64_t offset = decode_arm64_b_offset(insn);
        uint64_t b_pc = SEGMENT_VA + layout.stub_offset + layout.stub_size - 4;
        uint64_t target = static_cast<uint64_t>(
            static_cast<int64_t>(b_pc) + offset);
        EXPECT_EQ(target, regions[i].addr + regions[i].size)
            << "region " << i << " resume mismatch";
    }
}

// ============================================================
// x86_32 Pipeline Integration
// ============================================================

class HandoverX32 : public ::testing::Test {
protected:
    void SetUp() override {
        emitter = create_emitter(FileArch::X86, FileMode::MODE_32);
        ASSERT_NE(emitter, nullptr);
    }
    std::unique_ptr<StubEmitter> emitter;
    DiagnosticCollector diag;
};

TEST_F(HandoverX32, EmitterCreated) {
    EXPECT_EQ(emitter->min_region_size(), 5u);
    EXPECT_EQ(emitter->max_branch_distance(), INT32_MAX);
    EXPECT_EQ(emitter->pc_fixup_bias(), 4) << "x86_32 EIP advance = 4";
}

TEST_F(HandoverX32, StubEndsWithJmpRel32) {
    auto s = emitter->emit_entry_stub();
    ASSERT_TRUE(s.has_value());
    ASSERT_GE(s->code.size(), 5u);
    EXPECT_EQ(s->code[s->code.size() - 5], 0xE9);
}

TEST_F(HandoverX32, DeltaFixupSizeIs4Bytes) {
    auto s = emitter->emit_entry_stub();
    ASSERT_TRUE(s.has_value());
    EXPECT_EQ(s->delta_fixup_size, 4u)
        << "x86_32 delta = mov eax, imm32 = 4 bytes";
}

TEST_F(HandoverX32, PayloadLayoutCorrect) {
    auto blob = make_blob(128);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, 32}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());

    EXPECT_EQ(r->blob_size, 128u);
    EXPECT_EQ(r->seed_offset, 128u);
    EXPECT_EQ(r->call_slot_offset, 128u + SEED_SIZE + CALL_SLOT_SIZE - CALL_SLOT_SIZE);
    // call_slot = blob_size + SEED_SIZE
    EXPECT_EQ(r->call_slot_offset, 128u + SEED_SIZE);
    ASSERT_EQ(r->layouts.size(), 1u);
    EXPECT_EQ(r->layouts[0].stub_offset, 128u + SEED_SIZE + CALL_SLOT_SIZE);
}

TEST_F(HandoverX32, ResumeJumpTargetsRegionEnd) {
    constexpr uint64_t REGION_SIZE = 32;
    auto blob = make_blob(64);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, REGION_SIZE}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());

    const auto& layout = r->layouts[0];
    const uint8_t* stub = r->data.data() + layout.stub_offset;

    // Last 5 bytes: E9 rel32
    int32_t rel32 = read_i32_le(&stub[layout.stub_size - 4]);
    uint64_t disp32_addr = SEGMENT_VA + layout.stub_offset + layout.stub_size - 4;
    uint64_t target = static_cast<uint64_t>(
        static_cast<int64_t>(disp32_addr + 4) + rel32);

    EXPECT_EQ(target, TEXT_BASE + REGION_SIZE)
        << "resume JMP should target region end";
}

TEST_F(HandoverX32, RegionPatchTargetsStub) {
    auto blob = make_blob(64);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, 32}};

    auto r = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(r.has_value());

    uint64_t stub_va = SEGMENT_VA + r->layouts[0].stub_offset;

    auto patch = emitter->emit_region_patch(32, TEXT_BASE, stub_va);
    ASSERT_TRUE(patch.has_value());
    ASSERT_EQ(patch->size(), 32u);

    EXPECT_EQ((*patch)[0], 0xE9);

    int32_t rel32 = read_i32_le(patch->data() + 1);
    uint64_t target = static_cast<uint64_t>(
        static_cast<int64_t>(TEXT_BASE + 5) + rel32);
    EXPECT_EQ(target, stub_va);

    for (size_t i = 5; i < 32; ++i)
        EXPECT_EQ((*patch)[i], 0x90) << "byte " << i;
}

TEST(HandoverRoundtrip, X32PatchToStubAndBack) {
    auto emitter = create_emitter(FileArch::X86, FileMode::MODE_32);
    ASSERT_NE(emitter, nullptr);
    DiagnosticCollector diag;

    auto blob = make_blob(128);
    constexpr uint64_t REGION_ADDR = TEXT_BASE + 0x80;
    constexpr uint64_t REGION_SIZE = 16;
    std::vector<RegionPatchInfo> regions = {{"fn_x", REGION_ADDR, REGION_SIZE}};

    auto payload = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(payload.has_value());

    uint64_t stub_va = SEGMENT_VA + payload->layouts[0].stub_offset;

    // Forward: region -> stub
    auto patch = emitter->emit_region_patch(REGION_SIZE, REGION_ADDR, stub_va);
    ASSERT_TRUE(patch.has_value());
    int32_t fwd_rel32 = read_i32_le(patch->data() + 1);
    uint64_t fwd_target = static_cast<uint64_t>(
        static_cast<int64_t>(REGION_ADDR + 5) + fwd_rel32);
    EXPECT_EQ(fwd_target, stub_va) << "forward JMP misses stub";

    // Backward: stub resume -> region end
    const auto& layout = payload->layouts[0];
    const uint8_t* stub = payload->data.data() + layout.stub_offset;
    int32_t bwd_rel32 = read_i32_le(&stub[layout.stub_size - 4]);
    uint64_t disp_addr = SEGMENT_VA + layout.stub_offset + layout.stub_size - 4;
    uint64_t bwd_target = static_cast<uint64_t>(
        static_cast<int64_t>(disp_addr + 4) + bwd_rel32);
    EXPECT_EQ(bwd_target, REGION_ADDR + REGION_SIZE) << "resume JMP misses region end";
}

// ============================================================
// Cross-Architecture Parity
// ============================================================

TEST(HandoverParity, HeaderLayoutIsArchIndependent) {
    auto x64 = create_emitter(FileArch::X86, FileMode::MODE_64);
    auto arm = create_emitter(FileArch::ARM64, FileMode::MODE_LITTLE_ENDIAN);
    ASSERT_NE(x64, nullptr);
    ASSERT_NE(arm, nullptr);

    auto blob = make_blob(128);
    std::vector<RegionPatchInfo> regions = {
        {"fn_a", TEXT_BASE,         64},
        {"fn_b", TEXT_BASE + 0x100, 32},
    };

    DiagnosticCollector d1, d2;
    auto r1 = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *x64, d1);
    auto r2 = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *arm, d2);
    ASSERT_TRUE(r1.has_value());
    ASSERT_TRUE(r2.has_value());

    // Header region (blob + seed + call_slot) is identical
    EXPECT_EQ(r1->blob_size, r2->blob_size);
    EXPECT_EQ(r1->seed_offset, r2->seed_offset);
    EXPECT_EQ(r1->call_slot_offset, r2->call_slot_offset);
    EXPECT_EQ(r1->layouts.size(), r2->layouts.size());

    // First stub starts at the same offset (header is arch-independent)
    EXPECT_EQ(r1->layouts[0].stub_offset, r2->layouts[0].stub_offset);

    // Both produce non-empty stubs
    for (size_t i = 0; i < r1->layouts.size(); ++i) {
        EXPECT_GT(r1->layouts[i].stub_size, 0u) << "x64 stub " << i;
        EXPECT_GT(r2->layouts[i].stub_size, 0u) << "arm64 stub " << i;
    }
}

TEST(HandoverParity, BothArchesPreserveBlobAndSeed) {
    auto x64 = create_emitter(FileArch::X86, FileMode::MODE_64);
    auto arm = create_emitter(FileArch::ARM64, FileMode::MODE_LITTLE_ENDIAN);
    ASSERT_NE(x64, nullptr);
    ASSERT_NE(arm, nullptr);

    auto blob = make_blob(200);
    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, 32}};

    DiagnosticCollector d1, d2;
    auto r1 = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *x64, d1);
    auto r2 = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *arm, d2);
    ASSERT_TRUE(r1.has_value());
    ASSERT_TRUE(r2.has_value());

    // Header bytes (blob + seed + call_slot) must be identical
    size_t header_size = r1->call_slot_offset + CALL_SLOT_SIZE;
    ASSERT_EQ(r1->data.size() >= header_size, true);
    ASSERT_EQ(r2->data.size() >= header_size, true);
    EXPECT_EQ(std::memcmp(r1->data.data(), r2->data.data(), header_size), 0);
}

// ============================================================
// Architecture Support Matrix
// ============================================================

TEST(HandoverArchSupport, X86_64Supported) {
    auto e = create_emitter(FileArch::X86, FileMode::MODE_64);
    ASSERT_NE(e, nullptr);
    EXPECT_EQ(e->min_region_size(), 5u);       // JMP rel32
    EXPECT_EQ(e->max_branch_distance(), INT32_MAX);
}

TEST(HandoverArchSupport, ARM64Supported) {
    auto e = create_emitter(FileArch::ARM64, FileMode::MODE_LITTLE_ENDIAN);
    ASSERT_NE(e, nullptr);
    EXPECT_EQ(e->min_region_size(), 4u);       // B imm26
    EXPECT_EQ(e->max_branch_distance(), 128LL * 1024 * 1024);
}

TEST(HandoverArchSupport, X86_32Supported) {
    auto e = create_emitter(FileArch::X86, FileMode::MODE_32);
    ASSERT_NE(e, nullptr);
    EXPECT_EQ(e->min_region_size(), 5u);
    EXPECT_EQ(e->pc_fixup_bias(), 4);
}

TEST(HandoverArchSupport, UnsupportedArchReturnsNull) {
    EXPECT_EQ(create_emitter(FileArch::MIPS, FileMode::MODE_32), nullptr);
    EXPECT_EQ(create_emitter(FileArch::ARM, FileMode::MODE_32), nullptr);
    EXPECT_EQ(create_emitter(FileArch::RISCV, FileMode::MODE_64), nullptr);
}

// ============================================================
// FormatConfig Integration
// ============================================================

TEST(HandoverFormatConfig, ELFConfig) {
    auto cfg = FormatConfig::for_format(FileFormat::ELF);
    EXPECT_EQ(cfg.runtime_lib, "libvmpilot_runtime.so");
    EXPECT_EQ(cfg.section_name, ".vmpilot");
    EXPECT_EQ(cfg.page_align, 0x1000u);
}

TEST(HandoverFormatConfig, MachOConfig) {
    auto cfg = FormatConfig::for_format(FileFormat::MachO);
    EXPECT_EQ(cfg.runtime_lib, "@rpath/libvmpilot_runtime.dylib");
    EXPECT_EQ(cfg.section_name, ".vmpilot");
    EXPECT_EQ(cfg.page_align, 0x4000u);
}

TEST(HandoverFormatConfig, PEConfig) {
    auto cfg = FormatConfig::for_format(FileFormat::PE);
    EXPECT_EQ(cfg.runtime_lib, "vmpilot_runtime.dll");
    EXPECT_EQ(cfg.section_name, ".vmpltt");
    EXPECT_EQ(cfg.page_align, 0x1000u);
}

// ============================================================
// Error Paths
// ============================================================

TEST(HandoverErrors, EmptyBlobRejected) {
    auto emitter = create_emitter(FileArch::X86, FileMode::MODE_64);
    ASSERT_NE(emitter, nullptr);
    DiagnosticCollector diag;

    std::vector<RegionPatchInfo> regions = {{"fn_a", TEXT_BASE, 32}};
    auto r = build_payload(regions, {}, TEST_SEED, SEGMENT_VA, *emitter, diag);
    EXPECT_FALSE(r.has_value());
}

TEST(HandoverErrors, EmptyRegionsRejected) {
    auto emitter = create_emitter(FileArch::X86, FileMode::MODE_64);
    ASSERT_NE(emitter, nullptr);
    DiagnosticCollector diag;

    auto blob = make_blob(64);
    auto r = build_payload({}, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    EXPECT_FALSE(r.has_value());
}

TEST(HandoverErrors, ARM64BranchTooFar) {
    auto emitter = create_emitter(FileArch::ARM64, FileMode::MODE_LITTLE_ENDIAN);
    ASSERT_NE(emitter, nullptr);

    // ±128 MB limit
    constexpr uint64_t BEYOND_LIMIT = 256ULL * 1024 * 1024;
    auto r = emitter->emit_region_patch(64, 0x1000, 0x1000 + BEYOND_LIMIT);
    EXPECT_FALSE(r.has_value());
}

TEST(HandoverErrors, X64BranchTooFar) {
    auto emitter = create_emitter(FileArch::X86, FileMode::MODE_64);
    ASSERT_NE(emitter, nullptr);

    // ±2 GB limit for JMP rel32
    constexpr uint64_t BEYOND_LIMIT = 4ULL * 1024 * 1024 * 1024;
    auto r = emitter->emit_region_patch(32, 0x1000, 0x1000 + BEYOND_LIMIT);
    EXPECT_FALSE(r.has_value());
}

TEST(HandoverErrors, X64RegionTooSmall) {
    auto emitter = create_emitter(FileArch::X86, FileMode::MODE_64);
    ASSERT_NE(emitter, nullptr);

    // min_region_size = 5
    auto r = emitter->emit_region_patch(4, TEXT_BASE, SEGMENT_VA);
    EXPECT_FALSE(r.has_value());
}

TEST(HandoverErrors, ARM64RegionTooSmall) {
    auto emitter = create_emitter(FileArch::ARM64, FileMode::MODE_LITTLE_ENDIAN);
    ASSERT_NE(emitter, nullptr);

    // min_region_size = 4, so 3 is too small
    auto r = emitter->emit_region_patch(3, TEXT_BASE, SEGMENT_VA);
    EXPECT_FALSE(r.has_value());
}

// ============================================================
// Roundtrip: region patch → stub → resume
// ============================================================

TEST(HandoverRoundtrip, X64PatchToStubAndBack) {
    auto emitter = create_emitter(FileArch::X86, FileMode::MODE_64);
    ASSERT_NE(emitter, nullptr);
    DiagnosticCollector diag;

    auto blob = make_blob(128);
    constexpr uint64_t REGION_ADDR = TEXT_BASE + 0x80;
    constexpr uint64_t REGION_SIZE = 16;
    std::vector<RegionPatchInfo> regions = {{"fn_x", REGION_ADDR, REGION_SIZE}};

    auto payload = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(payload.has_value());

    uint64_t stub_va = SEGMENT_VA + payload->layouts[0].stub_offset;

    // Forward: region → stub
    auto patch = emitter->emit_region_patch(REGION_SIZE, REGION_ADDR, stub_va);
    ASSERT_TRUE(patch.has_value());
    int32_t fwd_rel32 = read_i32_le(patch->data() + 1);
    uint64_t fwd_target = static_cast<uint64_t>(
        static_cast<int64_t>(REGION_ADDR + 5) + fwd_rel32);
    EXPECT_EQ(fwd_target, stub_va) << "forward JMP misses stub";

    // Backward: stub resume → region end
    const auto& layout = payload->layouts[0];
    const uint8_t* stub = payload->data.data() + layout.stub_offset;
    int32_t bwd_rel32 = read_i32_le(&stub[layout.stub_size - 4]);
    uint64_t disp_addr = SEGMENT_VA + layout.stub_offset + layout.stub_size - 4;
    uint64_t bwd_target = static_cast<uint64_t>(
        static_cast<int64_t>(disp_addr + 4) + bwd_rel32);
    EXPECT_EQ(bwd_target, REGION_ADDR + REGION_SIZE) << "resume JMP misses region end";
}

TEST(HandoverRoundtrip, ARM64PatchToStubAndBack) {
    auto emitter = create_emitter(FileArch::ARM64, FileMode::MODE_LITTLE_ENDIAN);
    ASSERT_NE(emitter, nullptr);
    DiagnosticCollector diag;

    auto blob = make_blob(128);
    constexpr uint64_t REGION_ADDR = TEXT_BASE + 0x80;
    constexpr uint64_t REGION_SIZE = 32;
    std::vector<RegionPatchInfo> regions = {{"fn_x", REGION_ADDR, REGION_SIZE}};

    auto payload = build_payload(regions, blob, TEST_SEED, SEGMENT_VA, *emitter, diag);
    ASSERT_TRUE(payload.has_value());

    uint64_t stub_va = SEGMENT_VA + payload->layouts[0].stub_offset;

    // Forward: region → stub
    auto patch = emitter->emit_region_patch(REGION_SIZE, REGION_ADDR, stub_va);
    ASSERT_TRUE(patch.has_value());
    uint32_t fwd_insn = read32_le(patch->data());
    int64_t fwd_off = decode_arm64_b_offset(fwd_insn);
    uint64_t fwd_target = static_cast<uint64_t>(
        static_cast<int64_t>(REGION_ADDR) + fwd_off);
    EXPECT_EQ(fwd_target, stub_va) << "forward B misses stub";

    // Backward: stub resume → region end
    const auto& layout = payload->layouts[0];
    const uint8_t* stub = payload->data.data() + layout.stub_offset;
    uint32_t bwd_insn = read32_le(&stub[layout.stub_size - 4]);
    int64_t bwd_off = decode_arm64_b_offset(bwd_insn);
    uint64_t b_pc = SEGMENT_VA + layout.stub_offset + layout.stub_size - 4;
    uint64_t bwd_target = static_cast<uint64_t>(
        static_cast<int64_t>(b_pc) + bwd_off);
    EXPECT_EQ(bwd_target, REGION_ADDR + REGION_SIZE) << "resume B misses region end";
}
