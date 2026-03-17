#include <Serializer.hpp>
#include <segmentator.hpp>

#include <VMPilot_crypto.hpp>
#include <vmpilot.pb.h>

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

using namespace VMPilot::SDK;

namespace {

/// Read an entire file into a byte vector.
std::vector<uint8_t> readFileBytes(const fs::path& path) {
    std::ifstream in(path, std::ios::binary);
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(in),
                                std::istreambuf_iterator<char>());
}

/// Read an entire file into a string.
std::string readFileString(const fs::path& path) {
    std::ifstream in(path, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(in),
                       std::istreambuf_iterator<char>());
}

/// Build a minimal synthetic SegmentationResult for testing.
Segmentator::SegmentationResult makeSyntheticResult() {
    Segmentator::SegmentationResult result;

    // Build two refined regions
    std::vector<uint8_t> code1 = {0x55, 0x48, 0x89, 0xe5, 0x5d, 0xc3};
    std::vector<uint8_t> code2 = {0x90, 0x90, 0xc3};

    Segmentator::NativeFunctionBase r1(0x1000, code1.size(), "foo", code1);
    r1.setEnclosingSymbol("main");

    Segmentator::NativeFunctionBase r2(0x2000, code2.size(), "foo", code2);
    r2.setEnclosingSymbol("_Z3fooi");

    result.refined_regions.push_back(std::move(r1));
    result.refined_regions.push_back(std::move(r2));

    // Build one group with two sites
    RegionRefiner::ProtectedRegion group;
    group.source_name = "foo";
    group.canonical_index = 0;

    RegionRefiner::RegionSite site1;
    site1.source_name = "foo";
    site1.enclosing_symbol = "main";
    site1.is_canonical = true;
    site1.addr = 0x1000;
    site1.size = code1.size();

    RegionRefiner::RegionSite site2;
    site2.source_name = "foo";
    site2.enclosing_symbol = "_Z3fooi";
    site2.is_canonical = false;
    site2.addr = 0x2000;
    site2.size = code2.size();

    group.sites.push_back(std::move(site1));
    group.sites.push_back(std::move(site2));
    result.groups.push_back(std::move(group));

    // Context
    result.context.arch = Segmentator::Arch::X86;
    result.context.mode = Segmentator::Mode::MODE_64;

    Segmentator::NativeSymbolTableEntry sym;
    sym.name = "main";
    sym.address = 0x1000;
    sym.size = 100;
    sym.type = Segmentator::SymbolType::FUNC;
    sym.isGlobal = true;
    result.context.symbols.push_back(std::move(sym));

    Segmentator::ReadOnlySection rodata;
    rodata.base_addr = 0x3000;
    rodata.data = {0x68, 0x65, 0x6c, 0x6c, 0x6f};
    result.context.rodata_sections.push_back(std::move(rodata));

    result.binary_path = "test_binary.elf";
    result.compiler_info = "GCC: (test) 11.0.0";

    return result;
}

class SerializerTest : public ::testing::Test {
   protected:
    fs::path test_dir;

    void SetUp() override {
        test_dir = fs::temp_directory_path() / "vmpilot_serializer_test";
        fs::remove_all(test_dir);
    }

    void TearDown() override { fs::remove_all(test_dir); }
};

}  // namespace

TEST_F(SerializerTest, RoundTrip) {
    auto result = makeSyntheticResult();
    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    // Verify context.pb exists and is a valid protobuf
    auto ctx_bytes = readFileBytes(test_dir / "context.pb");
    ASSERT_FALSE(ctx_bytes.empty());

    vmpilot::CompilationContext ctx_pb;
    ASSERT_TRUE(ctx_pb.ParseFromArray(ctx_bytes.data(),
                                      static_cast<int>(ctx_bytes.size())));
    EXPECT_EQ(ctx_pb.arch(),
              static_cast<uint32_t>(Segmentator::Arch::X86));
    EXPECT_EQ(ctx_pb.mode(),
              static_cast<uint32_t>(Segmentator::Mode::MODE_64));
    EXPECT_EQ(ctx_pb.symbols_size(), 1);
    EXPECT_EQ(ctx_pb.symbols(0).name(), "main");
    EXPECT_EQ(ctx_pb.rodata_sections_size(), 1);
}

TEST_F(SerializerTest, DirectoryStructure) {
    auto result = makeSyntheticResult();
    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    EXPECT_TRUE(fs::exists(test_dir / "manifest.toml"));
    EXPECT_TRUE(fs::exists(test_dir / "context.pb"));
    EXPECT_TRUE(fs::is_directory(test_dir / "foo.group"));

    // Should have two .unit.pb files in foo.group/
    int unit_count = 0;
    for (const auto& entry : fs::directory_iterator(test_dir / "foo.group")) {
        if (entry.path().extension() == ".pb") ++unit_count;
    }
    EXPECT_EQ(unit_count, 2);
}

TEST_F(SerializerTest, ManifestContainsExpectedFields) {
    auto result = makeSyntheticResult();
    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    auto manifest = readFileString(test_dir / "manifest.toml");
    EXPECT_NE(manifest.find("test_binary.elf"), std::string::npos);
    EXPECT_NE(manifest.find("GCC: (test) 11.0.0"), std::string::npos);
    EXPECT_NE(manifest.find("context.pb"), std::string::npos);
    EXPECT_NE(manifest.find("foo"), std::string::npos);
    EXPECT_NE(manifest.find("X86"), std::string::npos);
}

TEST_F(SerializerTest, ContextHashConsistency) {
    auto result = makeSyntheticResult();
    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    // Read context.pb and compute SHA-256
    auto ctx_bytes = readFileBytes(test_dir / "context.pb");
    auto expected_hash =
        VMPilot::Crypto::SHA256(ctx_bytes, /*salt=*/{});

    // Read a unit.pb and verify its context_hash matches
    for (const auto& entry : fs::directory_iterator(test_dir / "foo.group")) {
        if (entry.path().extension() != ".pb") continue;

        auto unit_bytes = readFileBytes(entry.path());
        vmpilot::CompilationUnit unit_pb;
        ASSERT_TRUE(unit_pb.ParseFromArray(
            unit_bytes.data(), static_cast<int>(unit_bytes.size())));

        auto hash_str = unit_pb.context_hash();
        std::vector<uint8_t> actual_hash(hash_str.begin(), hash_str.end());
        EXPECT_EQ(actual_hash, expected_hash);
    }
}

TEST_F(SerializerTest, UnitProtobufRoundTrip) {
    auto result = makeSyntheticResult();
    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    // Find the canonical unit (main)
    bool found_canonical = false;
    for (const auto& entry : fs::directory_iterator(test_dir / "foo.group")) {
        if (entry.path().extension() != ".pb") continue;

        auto unit_bytes = readFileBytes(entry.path());
        vmpilot::CompilationUnit unit_pb;
        ASSERT_TRUE(unit_pb.ParseFromArray(
            unit_bytes.data(), static_cast<int>(unit_bytes.size())));

        if (unit_pb.is_canonical()) {
            found_canonical = true;
            EXPECT_EQ(unit_pb.addr(), 0x1000u);
            EXPECT_EQ(unit_pb.size(), 6u);
            EXPECT_EQ(unit_pb.enclosing_symbol(), "main");
            EXPECT_EQ(unit_pb.context_file(), "context.pb");

            auto code = unit_pb.code();
            std::vector<uint8_t> code_vec(code.begin(), code.end());
            std::vector<uint8_t> expected = {0x55, 0x48, 0x89, 0xe5,
                                             0x5d, 0xc3};
            EXPECT_EQ(code_vec, expected);
        }
    }
    EXPECT_TRUE(found_canonical);
}

TEST_F(SerializerTest, FilenameSanitization) {
    // Test with a source name containing filesystem-unsafe characters
    auto result = makeSyntheticResult();
    result.groups[0].source_name = "foo<bar>:baz";
    for (auto& site : result.groups[0].sites)
        site.source_name = "foo<bar>:baz";

    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    // Directory name should be sanitized
    EXPECT_TRUE(fs::is_directory(test_dir / "foo_bar__baz.group"));
}

TEST_F(SerializerTest, EmptyResult) {
    Segmentator::SegmentationResult result;
    result.context.arch = Segmentator::Arch::X86;
    result.context.mode = Segmentator::Mode::MODE_32;

    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    // Should still produce context.pb and manifest.toml
    EXPECT_TRUE(fs::exists(test_dir / "context.pb"));
    EXPECT_TRUE(fs::exists(test_dir / "manifest.toml"));
}

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR ""
#endif

TEST_F(SerializerTest, IntegrationWithRealBinary) {
    const std::string data_dir = TEST_DATA_DIR;
    if (data_dir.empty()) {
        GTEST_SKIP() << "TEST_DATA_DIR not set";
    }

    // Try to find a test binary
    std::string test_binary;
    for (const char* name :
         {"basic_binary.Linux.x86", "basic_binary.Linux.x64"}) {
        auto path = fs::path(data_dir) / name;
        if (fs::exists(path)) {
            test_binary = path.string();
            break;
        }
    }

    if (test_binary.empty()) {
        GTEST_SKIP() << "No test binary found in " << data_dir;
    }

    auto seg_result = Segmentator::segment(test_binary);
    if (!seg_result.has_value()) {
        GTEST_SKIP() << "Segmentation failed: "
                     << Segmentator::to_string(seg_result.error());
    }

    auto ret = Serializer::dump(*seg_result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    EXPECT_TRUE(fs::exists(test_dir / "context.pb"));
    EXPECT_TRUE(fs::exists(test_dir / "manifest.toml"));

    // Verify at least one group directory was created
    int group_count = 0;
    for (const auto& entry : fs::directory_iterator(test_dir)) {
        if (entry.is_directory() &&
            entry.path().filename().string().find(".group") !=
                std::string::npos) {
            ++group_count;
        }
    }
    EXPECT_GT(group_count, 0);
}
