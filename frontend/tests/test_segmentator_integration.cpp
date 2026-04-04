#include <ARM64Handler.hpp>
#include <CompilationContext.hpp>
#include <ELFHandler.hpp>
#include <MachOHandler.hpp>
#include <PEHandler.hpp>
#include <RegionRefiner.hpp>
#include <X86Handler.hpp>
#include <file_type_parser.hpp>
#include <utilities.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::Segmentator;

class SegmentatorIntegrationX64 : public ::testing::Test {
   protected:
    const std::string binary_path =
        std::string(TEST_DATA_DIR) + "/basic_binary.Linux.x86_64";
};

class SegmentatorIntegrationX86 : public ::testing::Test {
   protected:
    const std::string binary_path =
        std::string(TEST_DATA_DIR) + "/basic_binary.Linux.x86";
};

TEST_F(SegmentatorIntegrationX64, FullPipeline) {
    // 1. Metadata
    auto metadata = VMPilot::Common::get_file_metadata(binary_path);
    EXPECT_EQ(metadata.format, VMPilot::Common::FileFormat::ELF);
    EXPECT_EQ(metadata.mode, VMPilot::Common::FileMode::MODE_64);

    // 2. ELFHandler
    ELFFileHandlerStrategy elf(binary_path);
    auto text = elf.getTextSection();
    auto base = elf.getTextBaseAddr();
    auto symbols = elf.getNativeSymbolTable();
    ASSERT_FALSE(text.empty());
    ASSERT_FALSE(symbols.empty());

    // 3. X86Handler
    X86Handler x86(metadata.mode, symbols);
    ASSERT_TRUE(x86.Load(text, base));
    auto regions = x86.getNativeFunctions();
    ASSERT_EQ(regions.size(), 2u);

    // 4. RegionRefiner (no overlaps expected in this simple binary)
    auto refined = VMPilot::SDK::RegionRefiner::refine(std::move(regions));
    EXPECT_EQ(refined.size(), 2u);

    // 5. Verify each region has valid code
    for (const auto& r : refined) {
        EXPECT_GT(r.getAddr(), 0u);
        EXPECT_GT(r.getSize(), 0u);
        EXPECT_EQ(r.getCode().size(), r.getSize());
        // First byte should be 0xE8 (call rel32 to VMPilot_Begin)
        EXPECT_EQ(r.getCode()[0], 0xE8);
    }
}

TEST_F(SegmentatorIntegrationX86, FullPipeline) {
    auto metadata = VMPilot::Common::get_file_metadata(binary_path);
    EXPECT_EQ(metadata.format, VMPilot::Common::FileFormat::ELF);
    EXPECT_EQ(metadata.mode, VMPilot::Common::FileMode::MODE_32);

    ELFFileHandlerStrategy elf(binary_path);
    auto text = elf.getTextSection();
    auto base = elf.getTextBaseAddr();
    auto symbols = elf.getNativeSymbolTable();

    X86Handler x86(metadata.mode, symbols);
    ASSERT_TRUE(x86.Load(text, base));
    auto regions = x86.getNativeFunctions();
    ASSERT_EQ(regions.size(), 2u);

    auto refined = VMPilot::SDK::RegionRefiner::refine(std::move(regions));
    EXPECT_EQ(refined.size(), 2u);

    for (const auto& r : refined) {
        EXPECT_EQ(r.getCode()[0], 0xE8);
    }
}

class SegmentatorIntegrationDarwinARM64 : public ::testing::Test {
   protected:
    const std::string binary_path =
        std::string(TEST_DATA_DIR) + "/basic_binary.Darwin.arm64";
};

TEST_F(SegmentatorIntegrationDarwinARM64, FullPipeline) {
    auto metadata = VMPilot::Common::get_file_metadata(binary_path);
    EXPECT_EQ(metadata.format, VMPilot::Common::FileFormat::MachO);

    MachOFileHandlerStrategy macho(binary_path);
    auto text = macho.getTextSection();
    auto base = macho.getTextBaseAddr();
    auto symbols = macho.getNativeSymbolTable();
    ASSERT_FALSE(text.empty());
    ASSERT_FALSE(symbols.empty());

    ARM64Handler arm64(metadata.mode, symbols);
    ASSERT_TRUE(arm64.Load(text, base));
    auto regions = arm64.getNativeFunctions();
    ASSERT_GE(regions.size(), 1u);

    auto refined = VMPilot::SDK::RegionRefiner::refine(std::move(regions));
    EXPECT_GE(refined.size(), 1u);

    for (const auto& r : refined) {
        EXPECT_GT(r.getAddr(), 0u);
        EXPECT_GT(r.getSize(), 0u);
        EXPECT_EQ(r.getCode().size(), r.getSize());
    }
}

class SegmentatorIntegrationWindowsX64 : public ::testing::Test {
   protected:
    const std::string binary_path =
        std::string(TEST_DATA_DIR) + "/basic_binary.Windows.x86_64.exe";
};

class SegmentatorIntegrationWindowsX86 : public ::testing::Test {
   protected:
    const std::string binary_path =
        std::string(TEST_DATA_DIR) + "/basic_binary.Windows.x86.exe";
};

TEST_F(SegmentatorIntegrationWindowsX64, FullPipeline) {
    auto metadata = VMPilot::Common::get_file_metadata(binary_path);
    EXPECT_EQ(metadata.format, VMPilot::Common::FileFormat::PE);

    PEFileHandlerStrategy pe(binary_path);
    auto text = pe.getTextSection();
    auto base = pe.getTextBaseAddr();
    auto symbols = pe.getNativeSymbolTable();
    ASSERT_FALSE(text.empty());
    ASSERT_FALSE(symbols.empty());

    X86Handler x86(metadata.mode, symbols);
    ASSERT_TRUE(x86.Load(text, base));
    auto regions = x86.getNativeFunctions();
    ASSERT_GE(regions.size(), 1u);

    auto refined = VMPilot::SDK::RegionRefiner::refine(std::move(regions));
    EXPECT_GE(refined.size(), 1u);

    for (const auto& r : refined) {
        EXPECT_GT(r.getAddr(), 0u);
        EXPECT_GT(r.getSize(), 0u);
        EXPECT_EQ(r.getCode().size(), r.getSize());
    }
}

TEST_F(SegmentatorIntegrationWindowsX86, FullPipeline) {
    auto metadata = VMPilot::Common::get_file_metadata(binary_path);
    EXPECT_EQ(metadata.format, VMPilot::Common::FileFormat::PE);

    PEFileHandlerStrategy pe(binary_path);
    auto text = pe.getTextSection();
    auto base = pe.getTextBaseAddr();
    auto symbols = pe.getNativeSymbolTable();
    ASSERT_FALSE(text.empty());
    ASSERT_FALSE(symbols.empty());

    X86Handler x86(metadata.mode, symbols);
    ASSERT_TRUE(x86.Load(text, base));
    auto regions = x86.getNativeFunctions();
    ASSERT_GE(regions.size(), 1u);

    auto refined = VMPilot::SDK::RegionRefiner::refine(std::move(regions));
    EXPECT_GE(refined.size(), 1u);

    for (const auto& r : refined) {
        EXPECT_GT(r.getAddr(), 0u);
        EXPECT_GT(r.getSize(), 0u);
        EXPECT_EQ(r.getCode().size(), r.getSize());
    }
}

TEST_F(SegmentatorIntegrationX64, FunctionNameExtraction) {
    auto metadata = VMPilot::Common::get_file_metadata(binary_path);
    ELFFileHandlerStrategy elf(binary_path);

    auto text = elf.getTextSection();
    auto base = elf.getTextBaseAddr();
    auto symbols = elf.getNativeSymbolTable();

    // Build CompilationContext with rodata
    CompilationContext ctx;
    ctx.symbols = elf.getNativeSymbolTable();
    ctx.arch = metadata.arch;
    ctx.mode = metadata.mode;
    ctx.sections = elf.getSections();

    X86Handler x86(metadata.mode, symbols);
    x86.setCompilationContext(std::move(ctx));
    ASSERT_TRUE(x86.Load(text, base));
    auto regions = x86.getNativeFunctions();
    ASSERT_EQ(regions.size(), 2u);

    // With rodata available, at least one region should have
    // a human-readable name (not the vmpilot_region_0x... fallback)
    bool has_named_region = false;
    for (const auto& r : regions) {
        if (r.getName().substr(0, 17) != "vmpilot_region_0x") {
            has_named_region = true;
            break;
        }
    }
    EXPECT_TRUE(has_named_region)
        << "Expected at least one region with __FUNCTION__ name, got: "
        << regions[0].getName() << ", " << regions[1].getName();
}

TEST_F(SegmentatorIntegrationX64, FunctionNameFallbackWithoutRodata) {
    auto metadata = VMPilot::Common::get_file_metadata(binary_path);
    ELFFileHandlerStrategy elf(binary_path);

    auto text = elf.getTextSection();
    auto base = elf.getTextBaseAddr();
    auto symbols = elf.getNativeSymbolTable();

    // No CompilationContext set → fallback naming
    X86Handler x86(metadata.mode, symbols);
    ASSERT_TRUE(x86.Load(text, base));
    auto regions = x86.getNativeFunctions();
    ASSERT_EQ(regions.size(), 2u);

    // All regions should have fallback names
    for (const auto& r : regions) {
        EXPECT_EQ(r.getName().substr(0, 17), "vmpilot_region_0x")
            << "Without CompilationContext, region should have fallback name";
    }
}

TEST_F(SegmentatorIntegrationX64, RegionsDontOverlap) {
    ELFFileHandlerStrategy elf(binary_path);
    auto metadata = VMPilot::Common::get_file_metadata(binary_path);
    X86Handler x86(metadata.mode, elf.getNativeSymbolTable());
    ASSERT_TRUE(x86.Load(elf.getTextSection(), elf.getTextBaseAddr()));
    auto regions = x86.getNativeFunctions();

    // Verify no two regions overlap
    for (size_t i = 0; i < regions.size(); ++i) {
        uint64_t a_end = regions[i].getAddr() + regions[i].getSize();
        for (size_t j = i + 1; j < regions.size(); ++j) {
            uint64_t b_start = regions[j].getAddr();
            uint64_t b_end = regions[j].getAddr() + regions[j].getSize();
            // Either a ends before b starts, or b ends before a starts
            bool no_overlap =
                (a_end <= b_start) || (b_end <= regions[i].getAddr());
            EXPECT_TRUE(no_overlap)
                << "Regions overlap: [" << std::hex << regions[i].getAddr()
                << ", " << a_end << ") and [" << b_start << ", " << b_end
                << ")";
        }
    }
}
