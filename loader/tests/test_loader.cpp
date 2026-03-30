#include <BinaryPatcher.hpp>
#include <FormatPatcher.hpp>
#include <LoaderTypes.hpp>
#include <MachOEditor.hpp>
#include <MachOStructs.hpp>
#include <PayloadBuilder.hpp>
#include <StubGenerator.hpp>
#include <diagnostic_collector.hpp>
#include <file_type_parser.hpp>

#include <gtest/gtest.h>

#include <array>
#include <cstring>
#include <fstream>

using namespace VMPilot;
using DC = Common::DiagnosticCode;

// ============================================================================
// Test helpers
// ============================================================================

/// Fake blob bytes (opaque to Loader — just needs to be non-empty).
static std::vector<uint8_t> make_fake_blob(size_t size = 256) {
    std::vector<uint8_t> blob(size);
    for (size_t i = 0; i < size; ++i)
        blob[i] = static_cast<uint8_t>(i & 0xFF);
    return blob;
}

static constexpr std::array<uint8_t, Loader::SEED_SIZE> FAKE_SEED = {
    0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
    0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,
    0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,
    0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x20,
};

// ============================================================================
// StubGenerator
// ============================================================================

TEST(StubGenerator, MinRegionSize) {
    EXPECT_EQ(Loader::StubGenerator::min_region_size(Common::FileArch::X86, Common::FileMode::MODE_64), 5u);
    EXPECT_EQ(Loader::StubGenerator::min_region_size(Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN), 4u);
    EXPECT_EQ(Loader::StubGenerator::min_region_size(Common::FileArch::MIPS, Common::FileMode::MODE_32), 0u);
}

TEST(StubGenerator, MaxBranchDistance) {
    EXPECT_EQ(Loader::StubGenerator::max_branch_distance(Common::FileArch::X86), INT32_MAX);
    EXPECT_EQ(Loader::StubGenerator::max_branch_distance(Common::FileArch::ARM64), 128LL * 1024 * 1024);
}

TEST(StubGenerator, RegionPatchX64) {
    auto r = Loader::StubGenerator::generate_region_patch(
        Common::FileArch::X86, Common::FileMode::MODE_64, 32, 0x401000, 0x500000);
    ASSERT_TRUE(r);
    EXPECT_EQ(r->size(), 32u);
    EXPECT_EQ((*r)[0], 0xE9);
    for (size_t i = 5; i < 32; ++i) EXPECT_EQ((*r)[i], 0x90);
    int32_t rel;
    std::memcpy(&rel, r->data() + 1, 4);
    EXPECT_EQ(0x401000u + 5 + rel, 0x500000u);
}

TEST(StubGenerator, RegionPatchX64TooSmall) {
    auto r = Loader::StubGenerator::generate_region_patch(
        Common::FileArch::X86, Common::FileMode::MODE_64, 4, 0x1000, 0x2000);
    EXPECT_FALSE(r);
    EXPECT_EQ(r.error(), DC::PatchRegionTooSmall);
}

TEST(StubGenerator, RegionPatchARM64) {
    auto r = Loader::StubGenerator::generate_region_patch(
        Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN,
        16, 0x1000, 0x1100);
    ASSERT_TRUE(r);
    EXPECT_EQ(r->size(), 16u);
    uint32_t insn;
    std::memcpy(&insn, r->data(), 4);
    EXPECT_EQ(insn >> 26, 0x05u);  // B
}

TEST(StubGenerator, RegionPatchUnsupportedArch) {
    auto r = Loader::StubGenerator::generate_region_patch(
        Common::FileArch::MIPS, Common::FileMode::MODE_32, 32, 0x1000, 0x2000);
    EXPECT_FALSE(r);
    EXPECT_EQ(r.error(), DC::PatchArchUnsupported);
}

TEST(StubGenerator, EntryStubX64Structure) {
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::X86, Common::FileMode::MODE_64);
    ASSERT_TRUE(r);
    EXPECT_EQ(r->code[0], 0x55);     // push rbp
    EXPECT_EQ(r->code.back(), 0xC3); // ret
    EXPECT_LT(r->blob_fixup_offset, r->code.size());
    EXPECT_LT(r->seed_fixup_offset, r->code.size());
    EXPECT_LT(r->size_fixup_offset, r->code.size());
    EXPECT_LT(r->call_fixup_offset, r->code.size());
}

TEST(StubGenerator, EntryStubARM64Structure) {
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN);
    ASSERT_TRUE(r);
    EXPECT_EQ(r->code.size() % 4, 0u);
    uint32_t last;
    std::memcpy(&last, r->code.data() + r->code.size() - 4, 4);
    EXPECT_EQ(last, 0xD65F03C0u);  // ret
    EXPECT_LT(r->blob_fixup_offset, r->code.size());
    EXPECT_LT(r->seed_fixup_offset, r->code.size());
    EXPECT_LT(r->size_fixup_offset, r->code.size());
    EXPECT_LT(r->call_fixup_offset, r->code.size());
}

TEST(StubGenerator, EntryStubX32Unsupported) {
    // 32-bit x86 not supported for vm_execute ABI
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::X86, Common::FileMode::MODE_32);
    EXPECT_FALSE(r);
}

TEST(StubGenerator, FixupBlobDispX86InRange) {
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::X86, Common::FileMode::MODE_64);
    ASSERT_TRUE(r);
    auto fx = Loader::StubGenerator::fixup_blob_displacement(*r, -1000, Common::FileArch::X86);
    EXPECT_TRUE(fx);
    int32_t patched;
    std::memcpy(&patched, r->code.data() + r->blob_fixup_offset, 4);
    EXPECT_EQ(patched, -1000);
}

TEST(StubGenerator, FixupBlobDispX86OutOfRange) {
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::X86, Common::FileMode::MODE_64);
    ASSERT_TRUE(r);
    auto fx = Loader::StubGenerator::fixup_blob_displacement(
        *r, static_cast<int64_t>(INT32_MAX) + 1, Common::FileArch::X86);
    EXPECT_FALSE(fx);
}

TEST(StubGenerator, FixupSeedDispX86) {
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::X86, Common::FileMode::MODE_64);
    ASSERT_TRUE(r);
    auto fx = Loader::StubGenerator::fixup_seed_displacement(*r, 500, Common::FileArch::X86);
    EXPECT_TRUE(fx);
    int32_t patched;
    std::memcpy(&patched, r->code.data() + r->seed_fixup_offset, 4);
    EXPECT_EQ(patched, 500);
}

TEST(StubGenerator, FixupBlobSize) {
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::X86, Common::FileMode::MODE_64);
    ASSERT_TRUE(r);
    Loader::StubGenerator::fixup_blob_size(*r, 0xDEAD);
    uint32_t patched;
    std::memcpy(&patched, r->code.data() + r->size_fixup_offset, 4);
    EXPECT_EQ(patched, 0xDEADu);
}

TEST(StubGenerator, FixupBlobDispARM64InRange) {
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN);
    ASSERT_TRUE(r);
    EXPECT_TRUE(Loader::StubGenerator::fixup_blob_displacement(*r, -256, Common::FileArch::ARM64));
}

TEST(StubGenerator, FixupBlobDispARM64OutOfRange) {
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN);
    ASSERT_TRUE(r);
    EXPECT_FALSE(Loader::StubGenerator::fixup_blob_displacement(
        *r, 2 * 1024 * 1024, Common::FileArch::ARM64));
}

// ============================================================================
// PayloadBuilder
// ============================================================================

TEST(PayloadBuilder, EmptyRegionsFails) {
    auto r = Loader::build_payload({}, make_fake_blob(), FAKE_SEED,
        Common::FileArch::X86, Common::FileMode::MODE_64);
    EXPECT_FALSE(r);
}

TEST(PayloadBuilder, EmptyBlobFails) {
    std::vector<Loader::RegionPatchInfo> regions = {{"f", 0x1000, 32}};
    auto r = Loader::build_payload(regions, {}, FAKE_SEED,
        Common::FileArch::X86, Common::FileMode::MODE_64);
    EXPECT_FALSE(r);
}

TEST(PayloadBuilder, SingleRegion) {
    std::vector<Loader::RegionPatchInfo> regions = {{"f", 0x1000, 32}};
    auto blob = make_fake_blob(128);
    auto r = Loader::build_payload(regions, blob, FAKE_SEED,
        Common::FileArch::X86, Common::FileMode::MODE_64);
    ASSERT_TRUE(r);
    EXPECT_EQ(r->blob_size, 128u);
    EXPECT_EQ(r->seed_offset, 128u);
    EXPECT_EQ(r->layouts.size(), 1u);
    EXPECT_EQ(r->layouts[0].stub_offset, 128u + Loader::SEED_SIZE);
    // Payload should contain blob + seed + stub
    EXPECT_GT(r->data.size(), 128u + Loader::SEED_SIZE);
    // Verify seed is embedded
    EXPECT_EQ(std::memcmp(r->data.data() + 128, FAKE_SEED.data(), Loader::SEED_SIZE), 0);
}

TEST(PayloadBuilder, MultipleRegionsLayout) {
    std::vector<Loader::RegionPatchInfo> regions = {
        {"a", 0x1000, 32}, {"b", 0x1100, 32}, {"c", 0x1200, 32}
    };
    auto r = Loader::build_payload(regions, make_fake_blob(), FAKE_SEED,
        Common::FileArch::X86, Common::FileMode::MODE_64);
    ASSERT_TRUE(r);
    EXPECT_EQ(r->layouts.size(), 3u);
    for (size_t i = 1; i < r->layouts.size(); ++i) {
        EXPECT_GE(r->layouts[i].stub_offset,
                   r->layouts[i-1].stub_offset + r->layouts[i-1].stub_size);
    }
}

TEST(PayloadBuilder, BlobDataPreserved) {
    auto blob = make_fake_blob(64);
    std::vector<Loader::RegionPatchInfo> regions = {{"f", 0x1000, 32}};
    auto r = Loader::build_payload(regions, blob, FAKE_SEED,
        Common::FileArch::X86, Common::FileMode::MODE_64);
    ASSERT_TRUE(r);
    EXPECT_EQ(std::memcmp(r->data.data(), blob.data(), blob.size()), 0);
}

// ============================================================================
// MockEditor — DI for FormatPatcher tests
// ============================================================================

struct MockEditor {
    struct Config {
        Loader::TextSectionInfo text = {0x1000, 0x2000};
        Loader::NewSegmentInfo segment = {0x10000, 0};
        bool fail_open = false;
        bool fail_add_segment = false;
        bool fail_overwrite = false;
        bool fail_save = false;
    };
    static Config cfg;

    struct Calls {
        bool opened = false;
        bool saved = false;
        int overwrite_count = 0;
        std::vector<uint64_t> overwrite_vas;
    };
    static Calls calls;

    static void reset() { cfg = {}; calls = {}; }

    static tl::expected<MockEditor, DC>
    open(const std::string&, Common::DiagnosticCollector& diag) noexcept {
        calls.opened = true;
        if (cfg.fail_open) {
            diag.error("mock", DC::PatchBinaryReadFailed, "mock open failed");
            return tl::unexpected(DC::PatchBinaryReadFailed);
        }
        return MockEditor{};
    }

    Loader::TextSectionInfo text_section() const noexcept {
        return {cfg.text.base_addr, cfg.text.size};
    }

    tl::expected<void, DC>
    overwrite_text(uint64_t va, const uint8_t*, size_t,
                   Common::DiagnosticCollector& diag) noexcept {
        calls.overwrite_count++;
        calls.overwrite_vas.push_back(va);
        if (cfg.fail_overwrite) {
            diag.error("mock", DC::PatchSegmentCreationFailed, "mock overwrite failed");
            return tl::unexpected(DC::PatchSegmentCreationFailed);
        }
        return {};
    }

    tl::expected<Loader::NewSegmentInfo, DC>
    add_segment(std::string_view, const std::vector<uint8_t>& payload,
                uint64_t, Common::DiagnosticCollector& diag) noexcept {
        if (cfg.fail_add_segment) {
            diag.error("mock", DC::PatchSegmentCreationFailed, "mock add_segment failed");
            return tl::unexpected(DC::PatchSegmentCreationFailed);
        }
        return Loader::NewSegmentInfo{cfg.segment.va, payload.size()};
    }

    tl::expected<void, DC>
    save(const std::string&, Common::DiagnosticCollector& diag) noexcept {
        calls.saved = true;
        if (cfg.fail_save) {
            diag.error("mock", DC::PatchBinaryWriteFailed, "mock save failed");
            return tl::unexpected(DC::PatchBinaryWriteFailed);
        }
        return {};
    }
};

MockEditor::Config MockEditor::cfg;
MockEditor::Calls MockEditor::calls;
using MockPatcher = Loader::FormatPatcher<MockEditor>;

static Loader::PatchRequest make_request(
    std::vector<std::pair<std::string, uint64_t>> regions,
    uint64_t region_size = 32) {
    Loader::PatchRequest req;
    req.input_path = "/fake/input";
    req.output_path = "/fake/output";
    req.arch = Common::FileArch::X86;
    req.mode = Common::FileMode::MODE_64;
    req.format = Common::FileFormat::ELF;
    req.blob_data = make_fake_blob();
    req.stored_seed = FAKE_SEED;
    for (auto& [name, addr] : regions) {
        req.regions.push_back({name, addr, region_size});
    }
    return req;
}

// ============================================================================
// FormatPatcher<MockEditor>
// ============================================================================

class FormatPatcherTest : public ::testing::Test {
protected:
    void SetUp() override { MockEditor::reset(); }
    Common::DiagnosticCollector diag;
    MockPatcher patcher{"Mock"};
};

TEST_F(FormatPatcherTest, HappyPath) {
    auto r = patcher.patch(make_request({{"f", 0x1500}}), diag);
    ASSERT_TRUE(r) << diag.summary();
    EXPECT_EQ(r->regions_patched, 1u);
    EXPECT_GT(r->blob_bytes_injected, 0u);
    EXPECT_TRUE(MockEditor::calls.opened);
    EXPECT_TRUE(MockEditor::calls.saved);
}

TEST_F(FormatPatcherTest, MultipleRegions) {
    auto r = patcher.patch(make_request({{"a", 0x1000}, {"b", 0x1100}, {"c", 0x1200}}), diag);
    ASSERT_TRUE(r) << diag.summary();
    EXPECT_EQ(r->regions_patched, 3u);
    EXPECT_EQ(MockEditor::calls.overwrite_count, 3);
}

TEST_F(FormatPatcherTest, OpenFailure) {
    MockEditor::cfg.fail_open = true;
    EXPECT_FALSE(patcher.patch(make_request({{"f", 0x1500}}), diag));
}

TEST_F(FormatPatcherTest, AddSegmentFailure) {
    MockEditor::cfg.fail_add_segment = true;
    EXPECT_FALSE(patcher.patch(make_request({{"f", 0x1500}}), diag));
}

TEST_F(FormatPatcherTest, OverwriteFailure) {
    MockEditor::cfg.fail_overwrite = true;
    EXPECT_FALSE(patcher.patch(make_request({{"f", 0x1500}}), diag));
}

TEST_F(FormatPatcherTest, SaveFailure) {
    MockEditor::cfg.fail_save = true;
    EXPECT_FALSE(patcher.patch(make_request({{"f", 0x1500}}), diag));
}

TEST_F(FormatPatcherTest, RegionOutsideText) {
    auto r = patcher.patch(make_request({{"f", 0xDEAD}}), diag);
    EXPECT_FALSE(r);
    EXPECT_EQ(r.error(), DC::PatchInputInvalid);
}

TEST_F(FormatPatcherTest, RegionTooSmallSkipped) {
    auto r = patcher.patch(make_request({{"f", 0x1500}}, 3), diag);
    ASSERT_TRUE(r) << diag.summary();
    EXPECT_EQ(r->regions_patched, 0u);
    EXPECT_TRUE(MockEditor::calls.saved);
}

TEST_F(FormatPatcherTest, OverwriteCalledWithCorrectVA) {
    auto r = patcher.patch(make_request({{"a", 0x1000}, {"b", 0x1800}}), diag);
    ASSERT_TRUE(r) << diag.summary();
    ASSERT_EQ(MockEditor::calls.overwrite_vas.size(), 2u);
    EXPECT_EQ(MockEditor::calls.overwrite_vas[0], 0x1000u);
    EXPECT_EQ(MockEditor::calls.overwrite_vas[1], 0x1800u);
}

// ============================================================================
// DiagnosticCode helpers
// ============================================================================

TEST(DiagnosticCode, ModuleOf) {
    EXPECT_EQ(Common::module_of(DC::None), 0x0000);
    EXPECT_EQ(Common::module_of(DC::FileNotFound), 0x0001);
    EXPECT_EQ(Common::module_of(DC::PatchInputInvalid), 0x0007);
}

TEST(DiagnosticCode, ModuleName) {
    EXPECT_STREQ(Common::module_name(0x0001), "segmentator");
    EXPECT_STREQ(Common::module_name(0x0006), "runtime");
    EXPECT_STREQ(Common::module_name(0x0007), "loader");
}

// ============================================================================
// MachOEditor integration (real binary)
// ============================================================================

static const char* MACHO_BINARY = "data/basic/bin/basic_binary.Darwin.arm64";

class MachOEditorTest : public ::testing::Test {
protected:
    void SetUp() override {
        available_ = std::ifstream(MACHO_BINARY).good();
    }
    bool available_ = false;
    Common::DiagnosticCollector diag;
    void skip_if_unavailable() { if (!available_) GTEST_SKIP(); }
};

TEST_F(MachOEditorTest, OpenValid) {
    skip_if_unavailable();
    auto ed = Loader::MachOEditor::open(MACHO_BINARY, diag);
    ASSERT_TRUE(ed) << diag.summary();
    EXPECT_NE(ed->text_section().base_addr, 0u);
}

TEST_F(MachOEditorTest, OpenNonexistent) {
    auto ed = Loader::MachOEditor::open("/nonexistent", diag);
    EXPECT_FALSE(ed);
}

TEST_F(MachOEditorTest, OpenNotMachO) {
    const char* tmp = "/tmp/test_not_macho";
    { std::ofstream f(tmp); f << "garbage"; }
    EXPECT_FALSE(Loader::MachOEditor::open(tmp, diag));
    std::remove(tmp);
}

TEST_F(MachOEditorTest, OverwriteOutsideText) {
    skip_if_unavailable();
    auto ed = Loader::MachOEditor::open(MACHO_BINARY, diag);
    ASSERT_TRUE(ed);
    uint8_t b = 0xCC;
    EXPECT_FALSE(ed->overwrite_text(0xDEAD, &b, 1, diag));
}

TEST_F(MachOEditorTest, OverwriteWithinText) {
    skip_if_unavailable();
    auto ed = Loader::MachOEditor::open(MACHO_BINARY, diag);
    ASSERT_TRUE(ed);
    uint8_t b = 0xCC;
    EXPECT_TRUE(ed->overwrite_text(ed->text_section().base_addr, &b, 1, diag));
}

TEST_F(MachOEditorTest, AddSegmentAndSave) {
    skip_if_unavailable();
    auto ed = Loader::MachOEditor::open(MACHO_BINARY, diag);
    ASSERT_TRUE(ed);
    std::vector<uint8_t> payload(256, 0xAB);
    auto seg = ed->add_segment(".test", payload, 0x1000, diag);
    ASSERT_TRUE(seg) << diag.summary();
    EXPECT_NE(seg->va, 0u);

    const char* out = "/tmp/test_macho_seg";
    ASSERT_TRUE(ed->save(out, diag));
    // Verify still parseable
    EXPECT_TRUE(Loader::MachOEditor::open(out, diag));
    std::remove(out);
}

TEST_F(MachOEditorTest, EndToEndPatch) {
    skip_if_unavailable();
    auto ed = Loader::MachOEditor::open(MACHO_BINARY, diag);
    ASSERT_TRUE(ed);

    const char* out = "/tmp/test_macho_e2e";
    Loader::PatchRequest req;
    req.input_path = MACHO_BINARY;
    req.output_path = out;
    req.arch = Common::FileArch::ARM64;
    req.mode = Common::FileMode::MODE_LITTLE_ENDIAN;
    req.format = Common::FileFormat::MachO;
    req.blob_data = make_fake_blob(512);
    req.stored_seed = FAKE_SEED;
    req.regions.push_back({"test_func", ed->text_section().base_addr, 8});

    auto patcher = Loader::create_patcher(Common::FileFormat::MachO);
    auto r = patcher->patch(req, diag);
    ASSERT_TRUE(r) << diag.summary();
    EXPECT_EQ(r->regions_patched, 1u);

    // Verify still parseable
    EXPECT_TRUE(Loader::MachOEditor::open(out, diag));
    std::remove(out);
}
