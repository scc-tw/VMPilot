#include <BlobSerializer.hpp>
#include <LoaderTypes.hpp>
#include <StubGenerator.hpp>
#include <diagnostic_collector.hpp>
#include <instruction_t.hpp>

#include <gtest/gtest.h>

#include <cstring>

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
