#include <BinaryPatcher.hpp>
#include <BlobSerializer.hpp>
#include <FormatPatcher.hpp>
#include <LoaderTypes.hpp>
#include <MachOEditor.hpp>
#include <MachOStructs.hpp>
#include <PayloadBuilder.hpp>
#include <StubGenerator.hpp>
#include <diagnostic_collector.hpp>
#include <file_type_parser.hpp>
#include <instruction_t.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <fstream>

using namespace VMPilot;
using DC = Common::DiagnosticCode;

// ============================================================================
// Test helpers
// ============================================================================

static Common::Instruction_t make_insn(uint16_t op = 1) {
    Common::Instruction_t insn{};
    insn.opcode = op;
    return insn;
}

static SDK::BytecodeCompiler::CompilationOutput
make_output(const std::string& name, uint64_t addr, int n_insns = 1) {
    SDK::BytecodeCompiler::CompilationOutput out;
    out.name = name;
    out.addr = addr;
    for (int i = 0; i < n_insns; ++i)
        out.bytecodes.push_back(make_insn(static_cast<uint16_t>(i + 1)));
    return out;
}

// ============================================================================
// MockEditor — dependency injection for FormatPatcher tests
// ============================================================================

struct MockEditor {
    // --- Configurable injected behavior ---
    struct Config {
        Loader::TextSectionInfo text = {0x1000, 0x2000};   // .text at 0x1000, 8KB
        Loader::NewSegmentInfo segment = {0x10000, 0};      // new segment at 0x10000
        bool fail_open = false;
        bool fail_add_segment = false;
        bool fail_overwrite = false;
        bool fail_save = false;
    };
    static Config cfg;  // set before each test

    // --- Call tracking ---
    struct Calls {
        bool opened = false;
        bool saved = false;
        int add_segment_count = 0;
        int overwrite_count = 0;
        std::vector<uint64_t> overwrite_vas;
        std::vector<size_t> overwrite_lens;
    };
    static Calls calls;

    static void reset() { cfg = {}; calls = {}; }

    // --- Editor interface ---
    [[nodiscard]] static tl::expected<MockEditor, DC>
    open(const std::string& /*path*/, Common::DiagnosticCollector& diag) noexcept {
        calls.opened = true;
        if (cfg.fail_open) {
            diag.error("mock", DC::PatchBinaryReadFailed, "mock open failed");
            return tl::unexpected(DC::PatchBinaryReadFailed);
        }
        return MockEditor{};
    }

    [[nodiscard]] Loader::TextSectionInfo text_section() const noexcept {
        return {cfg.text.base_addr, cfg.text.size};
    }

    [[nodiscard]] tl::expected<void, DC>
    overwrite_text(uint64_t va, const uint8_t* /*data*/, size_t len,
                   Common::DiagnosticCollector& diag) noexcept {
        calls.overwrite_count++;
        calls.overwrite_vas.push_back(va);
        calls.overwrite_lens.push_back(len);
        if (cfg.fail_overwrite) {
            diag.error("mock", DC::PatchSegmentCreationFailed, "mock overwrite failed");
            return tl::unexpected(DC::PatchSegmentCreationFailed);
        }
        return {};
    }

    [[nodiscard]] tl::expected<Loader::NewSegmentInfo, DC>
    add_segment(std::string_view /*name*/, const std::vector<uint8_t>& payload,
                uint64_t /*align*/, Common::DiagnosticCollector& diag) noexcept {
        calls.add_segment_count++;
        if (cfg.fail_add_segment) {
            diag.error("mock", DC::PatchSegmentCreationFailed, "mock add_segment failed");
            return tl::unexpected(DC::PatchSegmentCreationFailed);
        }
        return Loader::NewSegmentInfo{cfg.segment.va, payload.size()};
    }

    [[nodiscard]] tl::expected<void, DC>
    save(const std::string& /*path*/, Common::DiagnosticCollector& diag) noexcept {
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

/// Build a PatchRequest with matched regions and outputs.
static Loader::PatchRequest make_request(
    std::vector<std::pair<std::string, uint64_t>> regions,
    uint64_t region_size = 32) {

    Loader::PatchRequest req;
    req.input_path = "/fake/input";
    req.output_path = "/fake/output";
    req.arch = Common::FileArch::X86;
    req.mode = Common::FileMode::MODE_64;
    req.format = Common::FileFormat::ELF;

    for (auto& [name, addr] : regions) {
        req.regions.push_back({name, addr, region_size});
        req.compiled_outputs.push_back(make_output(name, addr));
    }
    return req;
}

// ============================================================================
// BlobSerializer
// ============================================================================

TEST(BlobSerializer, EmptyOutputs) {
    auto r = Loader::BlobSerializer::serialize({});
    EXPECT_FALSE(r);
    EXPECT_EQ(r.error(), DC::PatchInputInvalid);
}

TEST(BlobSerializer, SingleRegionHeaderAndEntry) {
    auto out = make_output("f", 0x1000, 3);
    auto r = Loader::BlobSerializer::serialize({out});
    ASSERT_TRUE(r);
    EXPECT_TRUE(Loader::BlobSerializer::validate(*r));

    Loader::BlobHeader hdr;
    std::memcpy(&hdr, r->data(), sizeof(hdr));
    EXPECT_EQ(hdr.magic, 0x564D5031u);
    EXPECT_EQ(hdr.version, 1);
    EXPECT_EQ(hdr.entry_count, 1);

    Loader::BlobEntry entry;
    std::memcpy(&entry, r->data() + sizeof(hdr), sizeof(entry));
    EXPECT_EQ(entry.original_addr, 0x1000u);
    EXPECT_EQ(entry.bytecode_offset, 0u);
    EXPECT_EQ(entry.bytecode_size, 3u * sizeof(Common::Instruction_t));
}

TEST(BlobSerializer, MultipleRegionsSequentialOffsets) {
    std::vector<SDK::BytecodeCompiler::CompilationOutput> outs;
    for (int i = 0; i < 4; ++i)
        outs.push_back(make_output("f" + std::to_string(i), 0x1000 + i * 0x100, 2));

    auto r = Loader::BlobSerializer::serialize(outs);
    ASSERT_TRUE(r);

    Loader::BlobHeader hdr;
    std::memcpy(&hdr, r->data(), sizeof(hdr));
    EXPECT_EQ(hdr.entry_count, 4);

    uint32_t expected = 0;
    for (int i = 0; i < 4; ++i) {
        Loader::BlobEntry e;
        std::memcpy(&e, r->data() + sizeof(hdr) + i * sizeof(e), sizeof(e));
        EXPECT_EQ(e.bytecode_offset, expected);
        expected += static_cast<uint32_t>(2 * sizeof(Common::Instruction_t));
    }
}

TEST(BlobSerializer, ValidateRejectsTruncated) {
    EXPECT_FALSE(Loader::BlobSerializer::validate({0x01, 0x02}));
}

TEST(BlobSerializer, ValidateRejectsBadMagic) {
    std::vector<uint8_t> bad(sizeof(Loader::BlobHeader), 0);
    EXPECT_FALSE(Loader::BlobSerializer::validate(bad));
}

TEST(BlobSerializer, ValidateRejectsMissingEntries) {
    // Valid header but claims entries that aren't there
    Loader::BlobHeader hdr;
    hdr.entry_count = 100;
    std::vector<uint8_t> buf(sizeof(hdr));
    std::memcpy(buf.data(), &hdr, sizeof(hdr));
    EXPECT_FALSE(Loader::BlobSerializer::validate(buf));
}

TEST(BlobSerializer, BytecodeDataIntegrity) {
    auto out = make_output("f", 0x1000, 1);
    auto r = Loader::BlobSerializer::serialize({out});
    ASSERT_TRUE(r);

    // Read back the bytecode and compare
    Loader::BlobEntry entry;
    std::memcpy(&entry, r->data() + sizeof(Loader::BlobHeader), sizeof(entry));
    size_t data_start = sizeof(Loader::BlobHeader) + sizeof(Loader::BlobEntry);

    Common::Instruction helper;
    auto expected = helper.flatten(out.bytecodes[0]);
    ASSERT_EQ(entry.bytecode_size, expected.size());
    EXPECT_EQ(std::memcmp(r->data() + data_start, expected.data(), expected.size()), 0);
}

// ============================================================================
// StubGenerator
// ============================================================================

TEST(StubGenerator, MinRegionSize) {
    EXPECT_EQ(Loader::StubGenerator::min_region_size(Common::FileArch::X86, Common::FileMode::MODE_64), 5u);
    EXPECT_EQ(Loader::StubGenerator::min_region_size(Common::FileArch::X86, Common::FileMode::MODE_32), 5u);
    EXPECT_EQ(Loader::StubGenerator::min_region_size(Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN), 4u);
    EXPECT_EQ(Loader::StubGenerator::min_region_size(Common::FileArch::MIPS, Common::FileMode::MODE_32), 0u);
}

TEST(StubGenerator, MaxBranchDistance) {
    EXPECT_EQ(Loader::StubGenerator::max_branch_distance(Common::FileArch::X86), INT32_MAX);
    EXPECT_EQ(Loader::StubGenerator::max_branch_distance(Common::FileArch::ARM64), 128LL * 1024 * 1024);
    EXPECT_EQ(Loader::StubGenerator::max_branch_distance(Common::FileArch::MIPS), 0);
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
    for (size_t i = 4; i < 16; i += 4) {
        std::memcpy(&insn, r->data() + i, 4);
        EXPECT_EQ(insn, 0xD503201Fu);  // NOP
    }
}

TEST(StubGenerator, RegionPatchARM64TooSmall) {
    auto r = Loader::StubGenerator::generate_region_patch(
        Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN,
        3, 0x1000, 0x2000);
    EXPECT_FALSE(r);
}

TEST(StubGenerator, RegionPatchUnsupportedArch) {
    auto r = Loader::StubGenerator::generate_region_patch(
        Common::FileArch::MIPS, Common::FileMode::MODE_32, 32, 0x1000, 0x2000);
    EXPECT_FALSE(r);
    EXPECT_EQ(r.error(), DC::PatchArchUnsupported);
}

TEST(StubGenerator, EntryStubX64Structure) {
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::X86, Common::FileMode::MODE_64, 42);
    ASSERT_TRUE(r);
    EXPECT_EQ(r->code[0], 0x55);     // push rbp
    EXPECT_EQ(r->code.back(), 0xC3); // ret
    EXPECT_LT(r->blob_fixup_offset, r->code.size());
    EXPECT_LT(r->call_fixup_offset, r->code.size());
    EXPECT_EQ(r->blob_insn_size, 4u);
    EXPECT_EQ(r->code[r->call_fixup_offset - 1], 0xE8); // call
}

TEST(StubGenerator, EntryStubX32Structure) {
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::X86, Common::FileMode::MODE_32, 7);
    ASSERT_TRUE(r);
    EXPECT_EQ(r->code[0], 0x55);     // push ebp
    EXPECT_EQ(r->code.back(), 0xC3); // ret
    EXPECT_LT(r->blob_fixup_offset, r->code.size());
    EXPECT_LT(r->call_fixup_offset, r->code.size());
}

TEST(StubGenerator, EntryStubARM64Structure) {
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN, 0);
    ASSERT_TRUE(r);
    EXPECT_EQ(r->code.size() % 4, 0u);
    uint32_t last;
    std::memcpy(&last, r->code.data() + r->code.size() - 4, 4);
    EXPECT_EQ(last, 0xD65F03C0u);  // ret
    EXPECT_LT(r->blob_fixup_offset, r->code.size());
    EXPECT_LT(r->call_fixup_offset, r->code.size());
}

TEST(StubGenerator, EntryStubUnsupportedArch) {
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::MIPS, Common::FileMode::MODE_32, 0);
    EXPECT_FALSE(r);
}

TEST(StubGenerator, FixupBlobDispX86InRange) {
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::X86, Common::FileMode::MODE_64, 0);
    ASSERT_TRUE(r);
    auto fx = Loader::StubGenerator::fixup_blob_displacement(*r, -1000, Common::FileArch::X86);
    EXPECT_TRUE(fx);
    int32_t patched;
    std::memcpy(&patched, r->code.data() + r->blob_fixup_offset, 4);
    EXPECT_EQ(patched, -1000);
}

TEST(StubGenerator, FixupBlobDispX86OutOfRange) {
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::X86, Common::FileMode::MODE_64, 0);
    ASSERT_TRUE(r);
    auto fx = Loader::StubGenerator::fixup_blob_displacement(
        *r, static_cast<int64_t>(INT32_MAX) + 1, Common::FileArch::X86);
    EXPECT_FALSE(fx);
}

TEST(StubGenerator, FixupBlobDispARM64InRange) {
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN, 0);
    ASSERT_TRUE(r);
    auto fx = Loader::StubGenerator::fixup_blob_displacement(*r, -256, Common::FileArch::ARM64);
    EXPECT_TRUE(fx);
}

TEST(StubGenerator, FixupBlobDispARM64OutOfRange) {
    auto r = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN, 0);
    ASSERT_TRUE(r);
    auto fx = Loader::StubGenerator::fixup_blob_displacement(
        *r, 2 * 1024 * 1024, Common::FileArch::ARM64); // 2MB > ±1MB
    EXPECT_FALSE(fx);
}

// ============================================================================
// PayloadBuilder
// ============================================================================

TEST(PayloadBuilder, EmptyRegionsFails) {
    auto r = Loader::build_payload({}, {make_output("f", 0x1000)},
        Common::FileArch::X86, Common::FileMode::MODE_64);
    EXPECT_FALSE(r);
    EXPECT_EQ(r.error(), DC::PatchInputInvalid);
}

TEST(PayloadBuilder, NameMismatchFails) {
    std::vector<Loader::RegionPatchInfo> regions = {{"func_A", 0x1000, 32}};
    auto r = Loader::build_payload(regions, {make_output("func_B", 0x1000)},
        Common::FileArch::X86, Common::FileMode::MODE_64);
    EXPECT_FALSE(r);
    EXPECT_EQ(r.error(), DC::PatchInputInvalid);
}

TEST(PayloadBuilder, SingleRegionProducesValidPayload) {
    std::vector<Loader::RegionPatchInfo> regions = {{"f", 0x1000, 32}};
    auto r = Loader::build_payload(regions, {make_output("f", 0x1000)},
        Common::FileArch::X86, Common::FileMode::MODE_64);
    ASSERT_TRUE(r);
    EXPECT_GT(r->data.size(), r->blob_size);  // payload = blob + stubs
    EXPECT_EQ(r->layouts.size(), 1u);
    EXPECT_EQ(r->layouts[0].name, "f");
    EXPECT_EQ(r->layouts[0].stub_offset, r->blob_size);  // stub right after blob
}

TEST(PayloadBuilder, MultipleRegionsLayout) {
    std::vector<Loader::RegionPatchInfo> regions = {
        {"a", 0x1000, 32}, {"b", 0x1100, 32}, {"c", 0x1200, 32}
    };
    std::vector<SDK::BytecodeCompiler::CompilationOutput> outs = {
        make_output("a", 0x1000), make_output("b", 0x1100), make_output("c", 0x1200)
    };
    auto r = Loader::build_payload(regions, outs,
        Common::FileArch::X86, Common::FileMode::MODE_64);
    ASSERT_TRUE(r);
    EXPECT_EQ(r->layouts.size(), 3u);

    // Stubs should be sequential, non-overlapping
    for (size_t i = 1; i < r->layouts.size(); ++i) {
        EXPECT_GE(r->layouts[i].stub_offset,
                   r->layouts[i-1].stub_offset + r->layouts[i-1].stub_size);
    }
}

TEST(PayloadBuilder, BlobDataIntact) {
    std::vector<Loader::RegionPatchInfo> regions = {{"f", 0x1000, 32}};
    auto outs = std::vector{make_output("f", 0x1000, 2)};
    auto payload = Loader::build_payload(regions, outs,
        Common::FileArch::X86, Common::FileMode::MODE_64);
    ASSERT_TRUE(payload);

    // First blob_size bytes should be valid VMP1 blob
    std::vector<uint8_t> blob_portion(
        payload->data.begin(), payload->data.begin() + payload->blob_size);
    EXPECT_TRUE(Loader::BlobSerializer::validate(blob_portion));
}

// ============================================================================
// FormatPatcher<MockEditor> — DI orchestration tests
// ============================================================================

class FormatPatcherTest : public ::testing::Test {
protected:
    void SetUp() override { MockEditor::reset(); }
    Common::DiagnosticCollector diag;
    MockPatcher patcher{"Mock"};
};

TEST_F(FormatPatcherTest, HappyPath) {
    auto req = make_request({{"f", 0x1500}});
    auto r = patcher.patch(req, diag);
    ASSERT_TRUE(r) << diag.summary();
    EXPECT_EQ(r->regions_patched, 1u);
    EXPECT_GT(r->blob_bytes_injected, 0u);
    EXPECT_TRUE(MockEditor::calls.opened);
    EXPECT_TRUE(MockEditor::calls.saved);
    EXPECT_EQ(MockEditor::calls.add_segment_count, 1);
    EXPECT_EQ(MockEditor::calls.overwrite_count, 1);
    EXPECT_EQ(MockEditor::calls.overwrite_vas[0], 0x1500u);
}

TEST_F(FormatPatcherTest, MultipleRegions) {
    auto req = make_request({{"a", 0x1000}, {"b", 0x1100}, {"c", 0x1200}});
    auto r = patcher.patch(req, diag);
    ASSERT_TRUE(r) << diag.summary();
    EXPECT_EQ(r->regions_patched, 3u);
    EXPECT_EQ(MockEditor::calls.overwrite_count, 3);
}

TEST_F(FormatPatcherTest, OpenFailure) {
    MockEditor::cfg.fail_open = true;
    auto req = make_request({{"f", 0x1500}});
    auto r = patcher.patch(req, diag);
    EXPECT_FALSE(r);
    EXPECT_EQ(r.error(), DC::PatchBinaryReadFailed);
    EXPECT_FALSE(MockEditor::calls.saved);
}

TEST_F(FormatPatcherTest, AddSegmentFailure) {
    MockEditor::cfg.fail_add_segment = true;
    auto req = make_request({{"f", 0x1500}});
    auto r = patcher.patch(req, diag);
    EXPECT_FALSE(r);
    EXPECT_EQ(r.error(), DC::PatchSegmentCreationFailed);
    EXPECT_FALSE(MockEditor::calls.saved);
}

TEST_F(FormatPatcherTest, OverwriteFailure) {
    MockEditor::cfg.fail_overwrite = true;
    auto req = make_request({{"f", 0x1500}});
    auto r = patcher.patch(req, diag);
    EXPECT_FALSE(r);
    EXPECT_EQ(r.error(), DC::PatchSegmentCreationFailed);
    EXPECT_FALSE(MockEditor::calls.saved);
}

TEST_F(FormatPatcherTest, SaveFailure) {
    MockEditor::cfg.fail_save = true;
    auto req = make_request({{"f", 0x1500}});
    auto r = patcher.patch(req, diag);
    EXPECT_FALSE(r);
    EXPECT_EQ(r.error(), DC::PatchBinaryWriteFailed);
}

TEST_F(FormatPatcherTest, RegionOutsideText) {
    auto req = make_request({{"f", 0xDEAD}});  // outside mock .text 0x1000-0x3000
    auto r = patcher.patch(req, diag);
    EXPECT_FALSE(r);
    EXPECT_EQ(r.error(), DC::PatchInputInvalid);
}

TEST_F(FormatPatcherTest, RegionTooSmallSkipped) {
    auto req = make_request({{"f", 0x1500}}, /*region_size=*/3);  // < 5 for x86
    auto r = patcher.patch(req, diag);
    ASSERT_TRUE(r) << diag.summary();
    EXPECT_EQ(r->regions_patched, 0u);  // skipped, not error
    EXPECT_EQ(MockEditor::calls.overwrite_count, 0);
    EXPECT_TRUE(MockEditor::calls.saved);
}

TEST_F(FormatPatcherTest, EmptyRegionsFails) {
    Loader::PatchRequest req;
    req.input_path = "/fake";
    req.output_path = "/fake";
    req.arch = Common::FileArch::X86;
    req.mode = Common::FileMode::MODE_64;
    req.compiled_outputs.push_back(make_output("f", 0x1000));
    // No regions
    auto r = patcher.patch(req, diag);
    EXPECT_FALSE(r);
}

TEST_F(FormatPatcherTest, EmptyOutputsFails) {
    Loader::PatchRequest req;
    req.input_path = "/fake";
    req.output_path = "/fake";
    req.arch = Common::FileArch::X86;
    req.mode = Common::FileMode::MODE_64;
    req.regions.push_back({"f", 0x1500, 32});
    // No compiled_outputs → name mismatch
    auto r = patcher.patch(req, diag);
    EXPECT_FALSE(r);
}

TEST_F(FormatPatcherTest, NameMismatchFails) {
    Loader::PatchRequest req;
    req.input_path = "/fake";
    req.output_path = "/fake";
    req.arch = Common::FileArch::X86;
    req.mode = Common::FileMode::MODE_64;
    req.regions.push_back({"func_A", 0x1500, 32});
    req.compiled_outputs.push_back(make_output("func_B", 0x1500));
    auto r = patcher.patch(req, diag);
    EXPECT_FALSE(r);
    EXPECT_EQ(r.error(), DC::PatchInputInvalid);
}

TEST_F(FormatPatcherTest, OverwriteCalledWithCorrectVA) {
    auto req = make_request({{"a", 0x1000}, {"b", 0x1800}});
    auto r = patcher.patch(req, diag);
    ASSERT_TRUE(r) << diag.summary();
    ASSERT_EQ(MockEditor::calls.overwrite_vas.size(), 2u);
    EXPECT_EQ(MockEditor::calls.overwrite_vas[0], 0x1000u);
    EXPECT_EQ(MockEditor::calls.overwrite_vas[1], 0x1800u);
    // Overwrite length should equal region size
    EXPECT_EQ(MockEditor::calls.overwrite_lens[0], 32u);
    EXPECT_EQ(MockEditor::calls.overwrite_lens[1], 32u);
}

// ============================================================================
// DiagnosticCode helpers
// ============================================================================

TEST(DiagnosticCode, ModuleOf) {
    EXPECT_EQ(Common::module_of(DC::None), 0x0000);
    EXPECT_EQ(Common::module_of(DC::FileNotFound), 0x0001);
    EXPECT_EQ(Common::module_of(DC::ContainedRegionDropped), 0x0002);
    EXPECT_EQ(Common::module_of(DC::SerializationFailed), 0x0003);
    EXPECT_EQ(Common::module_of(DC::InvalidInput), 0x0004);
    EXPECT_EQ(Common::module_of(DC::PatchInputInvalid), 0x0007);
}

TEST(DiagnosticCode, CodeOf) {
    EXPECT_EQ(Common::code_of(DC::FileNotFound), 0x0001);
    EXPECT_EQ(Common::code_of(DC::PatchBinaryWriteFailed), 0x0008);
}

TEST(DiagnosticCode, ModuleName) {
    EXPECT_STREQ(Common::module_name(0x0001), "segmentator");
    EXPECT_STREQ(Common::module_name(0x0004), "compiler");
    EXPECT_STREQ(Common::module_name(0x0006), "runtime");
    EXPECT_STREQ(Common::module_name(0x0007), "loader");
    EXPECT_STREQ(Common::module_name(0xFFFF), "unknown_module");
}

// ============================================================================
// MachOEditor integration tests (real binary)
// ============================================================================

static const char* MACHO_BINARY = "data/basic/bin/basic_binary.Darwin.arm64";

class MachOEditorTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::ifstream f(MACHO_BINARY);
        available_ = f.good();
    }
    bool available_ = false;
    Common::DiagnosticCollector diag;

    void skip_if_unavailable() {
        if (!available_) GTEST_SKIP() << "Mach-O test binary not found";
    }
};

TEST_F(MachOEditorTest, OpenValid) {
    skip_if_unavailable();
    auto ed = Loader::MachOEditor::open(MACHO_BINARY, diag);
    ASSERT_TRUE(ed) << diag.summary();
    auto text = ed->text_section();
    EXPECT_NE(text.base_addr, 0u);
    EXPECT_GT(text.size, 0u);
}

TEST_F(MachOEditorTest, OpenNonexistent) {
    auto ed = Loader::MachOEditor::open("/nonexistent/path", diag);
    EXPECT_FALSE(ed);
    EXPECT_EQ(ed.error(), DC::PatchBinaryReadFailed);
}

TEST_F(MachOEditorTest, OpenNotMachO) {
    // Create a temp file with garbage
    const char* tmp = "/tmp/test_not_macho";
    { std::ofstream f(tmp); f << "not a mach-o file at all"; }
    auto ed = Loader::MachOEditor::open(tmp, diag);
    EXPECT_FALSE(ed);
    std::remove(tmp);
}

TEST_F(MachOEditorTest, OverwriteOutsideTextFails) {
    skip_if_unavailable();
    auto ed = Loader::MachOEditor::open(MACHO_BINARY, diag);
    ASSERT_TRUE(ed);
    uint8_t byte = 0xCC;
    auto r = ed->overwrite_text(0xDEAD, &byte, 1, diag);
    EXPECT_FALSE(r);
}

TEST_F(MachOEditorTest, OverwriteWithinText) {
    skip_if_unavailable();
    auto ed = Loader::MachOEditor::open(MACHO_BINARY, diag);
    ASSERT_TRUE(ed);
    auto text = ed->text_section();
    uint8_t byte = 0xCC;
    auto r = ed->overwrite_text(text.base_addr, &byte, 1, diag);
    EXPECT_TRUE(r);
}

TEST_F(MachOEditorTest, AddSegmentAndSave) {
    skip_if_unavailable();
    auto ed = Loader::MachOEditor::open(MACHO_BINARY, diag);
    ASSERT_TRUE(ed);

    std::vector<uint8_t> payload(256, 0xAB);
    auto seg = ed->add_segment(".test", payload, 0x1000, diag);
    ASSERT_TRUE(seg) << diag.summary();
    EXPECT_NE(seg->va, 0u);
    EXPECT_EQ(seg->size, 256u);

    const char* out = "/tmp/test_macho_add_seg";
    auto sv = ed->save(out, diag);
    ASSERT_TRUE(sv);

    // Verify output is valid Mach-O with extra load command
    std::ifstream ifs(out, std::ios::binary);
    Loader::MachO::mach_header_64 hdr{};
    ifs.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
    EXPECT_EQ(hdr.magic, Loader::MachO::MH_MAGIC_64);

    // Re-open with MachOEditor to prove it's still parseable
    auto ed2 = Loader::MachOEditor::open(out, diag);
    EXPECT_TRUE(ed2) << diag.summary();

    std::remove(out);
}

TEST_F(MachOEditorTest, EndToEndPatch) {
    skip_if_unavailable();
    auto ed = Loader::MachOEditor::open(MACHO_BINARY, diag);
    ASSERT_TRUE(ed);
    auto text = ed->text_section();

    const char* out = "/tmp/test_macho_e2e";
    Loader::PatchRequest req;
    req.input_path = MACHO_BINARY;
    req.output_path = out;
    req.arch = Common::FileArch::ARM64;
    req.mode = Common::FileMode::MODE_LITTLE_ENDIAN;
    req.format = Common::FileFormat::MachO;
    req.regions.push_back({"test_func", text.base_addr, 8});
    req.compiled_outputs.push_back(make_output("test_func", text.base_addr));

    auto patcher = Loader::create_patcher(Common::FileFormat::MachO);
    auto r = patcher->patch(req, diag);
    ASSERT_TRUE(r) << diag.summary();
    EXPECT_EQ(r->regions_patched, 1u);

    // Verify patched binary is still a valid Mach-O
    auto ed2 = Loader::MachOEditor::open(out, diag);
    EXPECT_TRUE(ed2) << diag.summary();

    std::remove(out);
}
