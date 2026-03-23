#include <BinaryPatcher.hpp>
#include <BlobSerializer.hpp>
#include <LoaderTypes.hpp>
#include <MachOStructs.hpp>
#include <StubGenerator.hpp>
#include <diagnostic_collector.hpp>
#include <file_type_parser.hpp>
#include <instruction_t.hpp>

#include <gtest/gtest.h>

#include <cstring>
#include <fstream>

using namespace VMPilot;

// ============================================================================
// BlobSerializer tests
// ============================================================================

TEST(BlobSerializer, EmptyOutputsReturnsError) {
    Common::DiagnosticCollector diag;
    std::vector<SDK::BytecodeCompiler::CompilationOutput> empty;
    auto result = Loader::BlobSerializer::serialize(empty, diag);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Common::DiagnosticCode::PatchInputInvalid);
}

TEST(BlobSerializer, SingleRegionRoundTrip) {
    // Build a CompilationOutput with 3 dummy instructions
    SDK::BytecodeCompiler::CompilationOutput output;
    output.name = "test_func";
    output.addr = 0x401000;
    for (int i = 0; i < 3; ++i) {
        Common::Instruction_t insn{};
        insn.opcode = static_cast<uint16_t>(i + 1);
        insn.left_operand = 100 + i;
        insn.right_operand = 200 + i;
        insn.nounce = 0;
        insn.checksum = 0;
        output.bytecodes.push_back(insn);
    }

    auto result = Loader::BlobSerializer::serialize({output});
    ASSERT_TRUE(result.has_value());

    auto& blob = result.value();
    EXPECT_TRUE(Loader::BlobSerializer::validate(blob));

    // Check header
    Loader::BlobHeader hdr;
    std::memcpy(&hdr, blob.data(), sizeof(hdr));
    EXPECT_EQ(hdr.magic, 0x564D5031u);
    EXPECT_EQ(hdr.version, 1);
    EXPECT_EQ(hdr.entry_count, 1);

    // Check entry
    Loader::BlobEntry entry;
    std::memcpy(&entry, blob.data() + sizeof(hdr), sizeof(entry));
    EXPECT_EQ(entry.original_addr, 0x401000u);
    EXPECT_EQ(entry.bytecode_offset, 0u);
    EXPECT_EQ(entry.bytecode_size, 3 * sizeof(Common::Instruction_t));
}

TEST(BlobSerializer, MultipleRegions) {
    std::vector<SDK::BytecodeCompiler::CompilationOutput> outputs;
    for (int r = 0; r < 3; ++r) {
        SDK::BytecodeCompiler::CompilationOutput out;
        out.name = "func_" + std::to_string(r);
        out.addr = 0x401000 + r * 0x100;
        Common::Instruction_t insn{};
        insn.opcode = static_cast<uint16_t>(r);
        out.bytecodes.push_back(insn);
        outputs.push_back(std::move(out));
    }

    auto result = Loader::BlobSerializer::serialize(outputs);
    ASSERT_TRUE(result.has_value());

    Loader::BlobHeader hdr;
    std::memcpy(&hdr, result.value().data(), sizeof(hdr));
    EXPECT_EQ(hdr.entry_count, 3);

    // Verify entries are sequential
    uint32_t expected_offset = 0;
    for (int i = 0; i < 3; ++i) {
        Loader::BlobEntry entry;
        std::memcpy(&entry,
                    result.value().data() + sizeof(hdr) + i * sizeof(entry),
                    sizeof(entry));
        EXPECT_EQ(entry.bytecode_offset, expected_offset);
        EXPECT_EQ(entry.bytecode_size, sizeof(Common::Instruction_t));
        expected_offset += sizeof(Common::Instruction_t);
    }
}

TEST(BlobSerializer, ValidateRejectsTruncated) {
    std::vector<uint8_t> too_small = {0x01, 0x02};
    EXPECT_FALSE(Loader::BlobSerializer::validate(too_small));
}

TEST(BlobSerializer, ValidateRejectsBadMagic) {
    std::vector<uint8_t> bad(sizeof(Loader::BlobHeader), 0);
    EXPECT_FALSE(Loader::BlobSerializer::validate(bad));
}

// ============================================================================
// StubGenerator tests
// ============================================================================

TEST(StubGenerator, MinRegionSizeX86) {
    EXPECT_EQ(Loader::StubGenerator::min_region_size(
        Common::FileArch::X86, Common::FileMode::MODE_64), 5u);
    EXPECT_EQ(Loader::StubGenerator::min_region_size(
        Common::FileArch::X86, Common::FileMode::MODE_32), 5u);
}

TEST(StubGenerator, MinRegionSizeARM64) {
    EXPECT_EQ(Loader::StubGenerator::min_region_size(
        Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN), 4u);
}

TEST(StubGenerator, RegionPatchX64StartsWithJmp) {
    auto result = Loader::StubGenerator::generate_region_patch(
        Common::FileArch::X86, Common::FileMode::MODE_64,
        32,          // region size
        0x401000,    // from
        0x500000);   // to

    ASSERT_TRUE(result.has_value());
    auto& patch = result.value();

    // Must be exactly region_size bytes
    EXPECT_EQ(patch.size(), 32u);
    // First byte is E9 (JMP rel32)
    EXPECT_EQ(patch[0], 0xE9);
    // Remaining bytes after the 5-byte JMP should be NOP (0x90)
    for (size_t i = 5; i < patch.size(); ++i)
        EXPECT_EQ(patch[i], 0x90) << "byte " << i << " should be NOP";

    // Verify rel32 target
    int32_t rel32;
    std::memcpy(&rel32, patch.data() + 1, 4);
    EXPECT_EQ(static_cast<uint64_t>(0x401000 + 5 + rel32), 0x500000u);
}

TEST(StubGenerator, RegionPatchTooSmallFails) {
    auto result = Loader::StubGenerator::generate_region_patch(
        Common::FileArch::X86, Common::FileMode::MODE_64,
        4,           // too small for 5-byte JMP
        0x401000, 0x500000);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), Common::DiagnosticCode::PatchRegionTooSmall);
}

TEST(StubGenerator, EntryStubX64HasCorrectStructure) {
    auto result = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::X86, Common::FileMode::MODE_64,
        0, 0);

    ASSERT_TRUE(result.has_value());
    auto& stub = result.value();

    // Starts with push rbp (0x55)
    EXPECT_EQ(stub.code[0], 0x55);
    // Ends with ret (0xC3)
    EXPECT_EQ(stub.code.back(), 0xC3);
    // Has a call rel32 (0xE8) placeholder
    EXPECT_EQ(stub.code[stub.call_fixup_offset - 1], 0xE8);
    // blob_fixup_offset should be within bounds
    EXPECT_LT(stub.blob_fixup_offset, stub.code.size());
    EXPECT_EQ(stub.blob_insn_size, 4u);
}

TEST(StubGenerator, EntryStubARM64HasCorrectStructure) {
    auto result = Loader::StubGenerator::generate_entry_stub(
        Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN,
        0, 0);

    ASSERT_TRUE(result.has_value());
    auto& stub = result.value();

    // ARM64 instructions are all 4-byte aligned
    EXPECT_EQ(stub.code.size() % 4, 0u);
    // Ends with RET (0xD65F03C0)
    uint32_t last_insn;
    std::memcpy(&last_insn, stub.code.data() + stub.code.size() - 4, 4);
    EXPECT_EQ(last_insn, 0xD65F03C0u);
    // blob and call fixup offsets within bounds
    EXPECT_LT(stub.blob_fixup_offset, stub.code.size());
    EXPECT_LT(stub.call_fixup_offset, stub.code.size());
}

TEST(StubGenerator, RegionPatchARM64StartsWithBranch) {
    auto result = Loader::StubGenerator::generate_region_patch(
        Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN,
        16,          // region size (4 instructions)
        0x401000,
        0x401100);

    ASSERT_TRUE(result.has_value());
    auto& patch = result.value();
    EXPECT_EQ(patch.size(), 16u);

    // First instruction should be B (opcode bits 31:26 = 000101)
    uint32_t first_insn;
    std::memcpy(&first_insn, patch.data(), 4);
    EXPECT_EQ(first_insn >> 26, 0x05u);  // B encoding

    // Remaining should be NOP
    for (size_t i = 4; i < patch.size(); i += 4) {
        uint32_t insn;
        std::memcpy(&insn, patch.data() + i, 4);
        EXPECT_EQ(insn, 0xD503201Fu) << "insn at offset " << i << " should be NOP";
    }
}

// ============================================================================
// DiagnosticCode module helpers
// ============================================================================

TEST(DiagnosticCode, ModuleExtraction) {
    EXPECT_EQ(Common::module_of(Common::DiagnosticCode::None), 0x0000);
    EXPECT_EQ(Common::module_of(Common::DiagnosticCode::FileNotFound), 0x0001);
    EXPECT_EQ(Common::module_of(Common::DiagnosticCode::ContainedRegionDropped), 0x0002);
    EXPECT_EQ(Common::module_of(Common::DiagnosticCode::SerializationFailed), 0x0003);
    EXPECT_EQ(Common::module_of(Common::DiagnosticCode::InvalidInput), 0x0004);
    EXPECT_EQ(Common::module_of(Common::DiagnosticCode::PatchInputInvalid), 0x0007);
}

TEST(DiagnosticCode, CodeExtraction) {
    EXPECT_EQ(Common::code_of(Common::DiagnosticCode::FileNotFound), 0x0001);
    EXPECT_EQ(Common::code_of(Common::DiagnosticCode::PatchBinaryWriteFailed), 0x0008);
}

TEST(DiagnosticCode, ModuleName) {
    EXPECT_STREQ(Common::module_name(0x0001), "segmentator");
    EXPECT_STREQ(Common::module_name(0x0004), "compiler");
    EXPECT_STREQ(Common::module_name(0x0007), "loader");
    EXPECT_STREQ(Common::module_name(0x0006), "runtime");
    EXPECT_STREQ(Common::module_name(0xFFFF), "unknown_module");
}

// ============================================================================
// MachOPatcherTraits tests (using real test binaries)
// ============================================================================

static const char* MACHO_TEST_BINARY =
    "data/basic/bin/basic_binary.Darwin.arm64";

/// Helper: build a minimal PatchRequest from a Mach-O binary.
static Loader::PatchRequest make_macho_request(
    const std::string& input, const std::string& output,
    uint64_t region_addr, uint64_t region_size) {

    Loader::PatchRequest req;
    req.input_path = input;
    req.output_path = output;
    req.arch = Common::FileArch::ARM64;
    req.mode = Common::FileMode::MODE_LITTLE_ENDIAN;
    req.format = Common::FileFormat::MachO;

    req.regions.push_back({
        "test_func", region_addr, region_size
    });

    // Create a dummy compiled output
    SDK::BytecodeCompiler::CompilationOutput out;
    out.name = "test_func";
    out.addr = region_addr;
    Common::Instruction_t insn{};
    insn.opcode = 0x01;
    out.bytecodes.push_back(insn);
    req.compiled_outputs.push_back(std::move(out));

    return req;
}

TEST(MachOPatcher, OpenValidBinary) {
    // Verify the test binary exists
    std::ifstream check(MACHO_TEST_BINARY);
    if (!check.good()) {
        GTEST_SKIP() << "Test binary not found: " << MACHO_TEST_BINARY;
    }

    auto patcher = Loader::create_patcher(Common::FileFormat::MachO);
    ASSERT_NE(patcher, nullptr);
    EXPECT_EQ(patcher->name(), "MachO");
}

TEST(MachOPatcher, PatchProducesValidOutput) {
    std::ifstream check(MACHO_TEST_BINARY);
    if (!check.good()) {
        GTEST_SKIP() << "Test binary not found: " << MACHO_TEST_BINARY;
    }

    // Read the original to find __text VA for a valid region
    std::ifstream ifs(MACHO_TEST_BINARY, std::ios::binary | std::ios::ate);
    auto fsize = static_cast<size_t>(ifs.tellg());
    ifs.seekg(0);
    std::vector<uint8_t> orig(fsize);
    ifs.read(reinterpret_cast<char*>(orig.data()), fsize);

    // Parse header to find __text VA
    namespace MO = Loader::MachO;
    MO::mach_header_64 hdr{};
    std::memcpy(&hdr, orig.data(), sizeof(hdr));
    ASSERT_EQ(hdr.magic, MO::MH_MAGIC_64);

    // Walk load commands to find __text section
    uint64_t text_va = 0, text_size = 0;
    uint64_t offset = sizeof(MO::mach_header_64);
    for (uint32_t i = 0; i < hdr.ncmds; ++i) {
        MO::load_command lc{};
        std::memcpy(&lc, orig.data() + offset, sizeof(lc));
        if (lc.cmd == MO::LC_SEGMENT_64) {
            MO::segment_command_64 seg{};
            std::memcpy(&seg, orig.data() + offset, sizeof(seg));
            uint64_t sec_off = offset + sizeof(MO::segment_command_64);
            for (uint32_t s = 0; s < seg.nsects; ++s) {
                MO::section_64 sec{};
                std::memcpy(&sec, orig.data() + sec_off, sizeof(sec));
                if (std::strncmp(sec.sectname, "__text", 7) == 0 &&
                    std::strncmp(sec.segname, "__TEXT", 6) == 0) {
                    text_va = sec.addr;
                    text_size = sec.size;
                }
                sec_off += sizeof(MO::section_64);
            }
        }
        offset += lc.cmdsize;
    }
    ASSERT_NE(text_va, 0u);
    ASSERT_GE(text_size, 8u);  // need at least space for a B instruction

    // Patch
    const std::string output_path = "/tmp/test_macho_patched";
    auto req = make_macho_request(MACHO_TEST_BINARY, output_path, text_va, 8);

    Common::DiagnosticCollector diag;
    auto patcher = Loader::create_patcher(Common::FileFormat::MachO);
    auto result = patcher->patch(req, diag);

    ASSERT_TRUE(result.has_value()) << diag.summary();
    EXPECT_EQ(result->regions_patched, 1u);
    EXPECT_GT(result->blob_bytes_injected, 0u);

    // Verify the output file exists and is a valid Mach-O
    std::ifstream out_ifs(output_path, std::ios::binary | std::ios::ate);
    ASSERT_TRUE(out_ifs.good());
    auto out_size = static_cast<size_t>(out_ifs.tellg());
    EXPECT_GT(out_size, fsize);  // should be bigger (payload appended)

    out_ifs.seekg(0);
    std::vector<uint8_t> out_data(out_size);
    out_ifs.read(reinterpret_cast<char*>(out_data.data()), out_size);

    // Check header is still valid Mach-O with one more load command
    MO::mach_header_64 out_hdr{};
    std::memcpy(&out_hdr, out_data.data(), sizeof(out_hdr));
    EXPECT_EQ(out_hdr.magic, MO::MH_MAGIC_64);
    EXPECT_EQ(out_hdr.ncmds, hdr.ncmds + 1);

    // Check the patched region starts with B (ARM64 branch)
    // Find __text in output and check first 4 bytes at region offset
    uint64_t out_text_offset = 0;
    uint64_t off2 = sizeof(MO::mach_header_64);
    for (uint32_t i = 0; i < out_hdr.ncmds; ++i) {
        MO::load_command lc{};
        std::memcpy(&lc, out_data.data() + off2, sizeof(lc));
        if (lc.cmd == MO::LC_SEGMENT_64) {
            MO::segment_command_64 seg{};
            std::memcpy(&seg, out_data.data() + off2, sizeof(seg));
            uint64_t sec_off = off2 + sizeof(MO::segment_command_64);
            for (uint32_t s = 0; s < seg.nsects; ++s) {
                MO::section_64 sec{};
                std::memcpy(&sec, out_data.data() + sec_off, sizeof(sec));
                if (std::strncmp(sec.sectname, "__text", 7) == 0 &&
                    std::strncmp(sec.segname, "__TEXT", 6) == 0) {
                    out_text_offset = sec.offset;
                }
                sec_off += sizeof(MO::section_64);
            }
        }
        off2 += lc.cmdsize;
    }

    // First instruction should be B (bits 31:26 = 000101)
    uint32_t patched_insn;
    std::memcpy(&patched_insn, out_data.data() + out_text_offset, 4);
    EXPECT_EQ(patched_insn >> 26, 0x05u) << "expected ARM64 B instruction";

    // Clean up
    std::remove(output_path.c_str());
}
