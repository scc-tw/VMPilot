#include <Serializer.hpp>
#include <segmentator.hpp>

#include <VMPilot_crypto.hpp>
#include <vmpilot.pb.h>

#include <toml++/toml.hpp>

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <set>
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

// ---------------------------------------------------------------------------
// Multiple groups
// ---------------------------------------------------------------------------

TEST_F(SerializerTest, MultipleGroups) {
    Segmentator::SegmentationResult result;
    result.context.arch = Segmentator::Arch::X86;
    result.context.mode = Segmentator::Mode::MODE_64;
    result.binary_path = "multi.elf";

    std::vector<uint8_t> code_a = {0xc3};
    std::vector<uint8_t> code_b = {0x90, 0xc3};
    std::vector<uint8_t> code_c = {0xcc};

    result.refined_regions.emplace_back(0x1000, code_a.size(), "alpha", code_a);
    result.refined_regions.back().setEnclosingSymbol("main");
    result.refined_regions.emplace_back(0x2000, code_b.size(), "beta", code_b);
    result.refined_regions.back().setEnclosingSymbol("main");
    result.refined_regions.emplace_back(0x3000, code_c.size(), "gamma", code_c);
    result.refined_regions.back().setEnclosingSymbol("helper");

    auto makeGroup = [](const std::string& name, uint64_t addr, uint64_t size,
                        const std::string& enclosing) {
        RegionRefiner::ProtectedRegion g;
        g.source_name = name;
        g.canonical_index = 0;
        RegionRefiner::RegionSite s;
        s.source_name = name;
        s.enclosing_symbol = enclosing;
        s.is_canonical = true;
        s.addr = addr;
        s.size = size;
        g.sites.push_back(std::move(s));
        return g;
    };

    result.groups.push_back(makeGroup("alpha", 0x1000, 1, "main"));
    result.groups.push_back(makeGroup("beta", 0x2000, 2, "main"));
    result.groups.push_back(makeGroup("gamma", 0x3000, 1, "helper"));

    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    EXPECT_TRUE(fs::is_directory(test_dir / "alpha.group"));
    EXPECT_TRUE(fs::is_directory(test_dir / "beta.group"));
    EXPECT_TRUE(fs::is_directory(test_dir / "gamma.group"));

    // Each group directory should have exactly one .unit.pb
    for (const char* name : {"alpha.group", "beta.group", "gamma.group"}) {
        int count = 0;
        for (const auto& e : fs::directory_iterator(test_dir / name))
            if (e.path().extension() == ".pb") ++count;
        EXPECT_EQ(count, 1) << "Expected 1 unit in " << name;
    }
}

// ---------------------------------------------------------------------------
// Inline (non-canonical) unit verification
// ---------------------------------------------------------------------------

TEST_F(SerializerTest, InlineUnitFields) {
    auto result = makeSyntheticResult();
    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    bool found_inline = false;
    for (const auto& entry : fs::directory_iterator(test_dir / "foo.group")) {
        if (entry.path().extension() != ".pb") continue;

        auto bytes = readFileBytes(entry.path());
        vmpilot::CompilationUnit unit;
        ASSERT_TRUE(unit.ParseFromArray(bytes.data(),
                                        static_cast<int>(bytes.size())));

        if (!unit.is_canonical()) {
            found_inline = true;
            EXPECT_EQ(unit.addr(), 0x2000u);
            EXPECT_EQ(unit.size(), 3u);
            EXPECT_EQ(unit.enclosing_symbol(), "_Z3fooi");

            auto code = unit.code();
            std::vector<uint8_t> code_vec(code.begin(), code.end());
            EXPECT_EQ(code_vec, (std::vector<uint8_t>{0x90, 0x90, 0xc3}));
        }
    }
    EXPECT_TRUE(found_inline) << "Expected to find an inline unit";
}

// ---------------------------------------------------------------------------
// Long filename truncation
// ---------------------------------------------------------------------------

TEST_F(SerializerTest, LongNestedTemplateName) {
    // Simulate a deeply nested C++ template name that produces an absurdly
    // long mangled symbol — the kind that breaks naive filename generation.
    // e.g. boost::variant<vector<vector<vector<...>>>>
    std::string nested_template =
        "_ZN5boost6detail14variant_accessINSt6vectorINS2_INS2_IdSaIdEE"
        "SaIS4_EESaIS6_EESaIS8_EESaISA_EESaISC_EESaISE_EESaISG_EESaI"
        "SI_EESaISK_EESaISM_EESaISO_EESaISQ_EESaISS_EESaISU_EESaISW_"
        "EESaISY_EEE14some_long_inner_template_specialization_name";  // ~300 chars

    auto result = makeSyntheticResult();

    // Use the long template as both source name and enclosing symbol
    result.groups[0].source_name = nested_template;
    for (auto& site : result.groups[0].sites) {
        site.source_name = nested_template;
        site.enclosing_symbol = nested_template;
    }

    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    // Group directory must exist and its name must be <= 255 chars (NAME_MAX)
    bool found_group = false;
    for (const auto& entry : fs::directory_iterator(test_dir)) {
        auto name = entry.path().filename().string();
        if (name.find(".group") != std::string::npos) {
            found_group = true;
            EXPECT_LE(name.size(), 255u) << "Directory name too long: " << name;
        }
    }
    EXPECT_TRUE(found_group) << "No .group directory created";

    // Every unit file must exist and have a filename <= 255 chars
    int unit_count = 0;
    for (const auto& dir_entry : fs::directory_iterator(test_dir)) {
        if (!dir_entry.is_directory()) continue;
        for (const auto& f : fs::directory_iterator(dir_entry.path())) {
            if (f.path().extension() != ".pb") continue;
            ++unit_count;
            auto fname = f.path().filename().string();
            EXPECT_LE(fname.size(), 120u) << "Filename too long: " << fname;

            // An address must still appear in the filename (never truncated)
            EXPECT_NE(fname.find("0x"), std::string::npos)
                << "Address missing from filename: " << fname;
        }
    }
    EXPECT_EQ(unit_count, 2);
}

TEST_F(SerializerTest, TwoLongNamesStayUnique) {
    // Two different 300-char names that share a long prefix.
    // After truncation, they must still produce different filenames
    // thanks to the FNV hash suffix.
    std::string prefix(280, 'X');
    std::string name_a = prefix + "_variant_A_ending";
    std::string name_b = prefix + "_variant_B_ending";

    Segmentator::SegmentationResult result;
    result.context.arch = Segmentator::Arch::X86;
    result.context.mode = Segmentator::Mode::MODE_64;

    std::vector<uint8_t> code = {0xc3};
    result.refined_regions.emplace_back(0xA000, code.size(), "fn", code);
    result.refined_regions.back().setEnclosingSymbol(name_a);
    result.refined_regions.emplace_back(0xB000, code.size(), "fn", code);
    result.refined_regions.back().setEnclosingSymbol(name_b);

    RegionRefiner::ProtectedRegion g;
    g.source_name = "fn";
    g.canonical_index = 0;

    RegionRefiner::RegionSite s1;
    s1.source_name = "fn";
    s1.enclosing_symbol = name_a;
    s1.is_canonical = true;
    s1.addr = 0xA000;
    s1.size = 1;

    RegionRefiner::RegionSite s2;
    s2.source_name = "fn";
    s2.enclosing_symbol = name_b;
    s2.is_canonical = false;
    s2.addr = 0xB000;
    s2.size = 1;

    g.sites.push_back(std::move(s1));
    g.sites.push_back(std::move(s2));
    result.groups.push_back(std::move(g));

    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    // Should produce 2 distinct unit files
    std::set<std::string> filenames;
    for (const auto& dir_entry : fs::directory_iterator(test_dir)) {
        if (!dir_entry.is_directory()) continue;
        for (const auto& f : fs::directory_iterator(dir_entry.path())) {
            if (f.path().extension() == ".pb")
                filenames.insert(f.path().filename().string());
        }
    }
    EXPECT_EQ(filenames.size(), 2u) << "Truncated names collided!";
}

// ---------------------------------------------------------------------------
// Missing enclosing symbol (nullopt — PE format scenario)
// ---------------------------------------------------------------------------

TEST_F(SerializerTest, MissingEnclosingSymbol) {
    Segmentator::SegmentationResult result;
    result.context.arch = Segmentator::Arch::X86;
    result.context.mode = Segmentator::Mode::MODE_32;
    result.binary_path = "test.exe";

    std::vector<uint8_t> code = {0xc3};
    result.refined_regions.emplace_back(0x401000, code.size(), "func", code);
    // No setEnclosingSymbol — simulates PE

    RegionRefiner::ProtectedRegion g;
    g.source_name = "func";
    g.canonical_index = 0;
    RegionRefiner::RegionSite s;
    s.source_name = "func";
    // enclosing_symbol left as nullopt
    s.is_canonical = true;
    s.addr = 0x401000;
    s.size = 1;
    g.sites.push_back(std::move(s));
    result.groups.push_back(std::move(g));

    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    // Unit should have "unknown" as enclosing_symbol
    for (const auto& e : fs::directory_iterator(test_dir / "func.group")) {
        if (e.path().extension() != ".pb") continue;
        auto bytes = readFileBytes(e.path());
        vmpilot::CompilationUnit unit;
        ASSERT_TRUE(unit.ParseFromArray(bytes.data(),
                                        static_cast<int>(bytes.size())));
        EXPECT_EQ(unit.enclosing_symbol(), "unknown");
    }

    // Filename should contain "unknown"
    auto manifest = readFileString(test_dir / "manifest.toml");
    EXPECT_NE(manifest.find("unknown"), std::string::npos);
}

// ---------------------------------------------------------------------------
// Symbol entry_type roundtrip (stub, pointer_table)
// ---------------------------------------------------------------------------

TEST_F(SerializerTest, SymbolEntryTypeRoundTrip) {
    auto result = makeSyntheticResult();

    Segmentator::NativeSymbolTableEntry stub_sym;
    stub_sym.name = "printf";
    stub_sym.address = 0x5000;
    stub_sym.size = 16;
    stub_sym.type = Segmentator::SymbolType::FUNC;
    stub_sym.isGlobal = true;
    stub_sym.setAttribute("entry_type", std::string("stub"));
    result.context.symbols.push_back(std::move(stub_sym));

    Segmentator::NativeSymbolTableEntry pt_sym;
    pt_sym.name = "puts";
    pt_sym.address = 0x6000;
    pt_sym.size = 8;
    pt_sym.type = Segmentator::SymbolType::OBJECT;
    pt_sym.isGlobal = true;
    pt_sym.setAttribute("entry_type", std::string("pointer_table"));
    result.context.symbols.push_back(std::move(pt_sym));

    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    auto ctx_bytes = readFileBytes(test_dir / "context.pb");
    vmpilot::CompilationContext ctx_pb;
    ASSERT_TRUE(ctx_pb.ParseFromArray(ctx_bytes.data(),
                                      static_cast<int>(ctx_bytes.size())));

    // Original "main" + "printf" (stub) + "puts" (pointer_table)
    ASSERT_EQ(ctx_pb.symbols_size(), 3);

    bool found_stub = false, found_pt = false;
    for (int i = 0; i < ctx_pb.symbols_size(); ++i) {
        const auto& s = ctx_pb.symbols(i);
        if (s.name() == "printf") {
            found_stub = true;
            EXPECT_EQ(s.entry_type(), "stub");
            EXPECT_EQ(s.address(), 0x5000u);
            EXPECT_TRUE(s.is_global());
        }
        if (s.name() == "puts") {
            found_pt = true;
            EXPECT_EQ(s.entry_type(), "pointer_table");
            EXPECT_EQ(s.address(), 0x6000u);
        }
    }
    EXPECT_TRUE(found_stub);
    EXPECT_TRUE(found_pt);
}

// ---------------------------------------------------------------------------
// Rodata bytes survive serialization exactly
// ---------------------------------------------------------------------------

TEST_F(SerializerTest, RodataRoundTrip) {
    auto result = makeSyntheticResult();
    // Replace rodata with a known pattern including nulls
    result.context.rodata_sections.clear();
    Segmentator::ReadOnlySection sec;
    sec.base_addr = 0xDEAD;
    sec.data = {0x00, 0xFF, 0x41, 0x42, 0x00, 0x43};
    result.context.rodata_sections.push_back(std::move(sec));

    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    auto ctx_bytes = readFileBytes(test_dir / "context.pb");
    vmpilot::CompilationContext ctx_pb;
    ASSERT_TRUE(ctx_pb.ParseFromArray(ctx_bytes.data(),
                                      static_cast<int>(ctx_bytes.size())));

    ASSERT_EQ(ctx_pb.rodata_sections_size(), 1);
    EXPECT_EQ(ctx_pb.rodata_sections(0).base_addr(), 0xDEADu);
    auto data = ctx_pb.rodata_sections(0).data();
    std::vector<uint8_t> actual(data.begin(), data.end());
    EXPECT_EQ(actual,
              (std::vector<uint8_t>{0x00, 0xFF, 0x41, 0x42, 0x00, 0x43}));
}

// ---------------------------------------------------------------------------
// Determinism — same input produces identical output
// ---------------------------------------------------------------------------

TEST_F(SerializerTest, Determinism) {
    auto result = makeSyntheticResult();

    fs::path dir1 = test_dir / "run1";
    fs::path dir2 = test_dir / "run2";

    auto ret1 = Serializer::dump(result, dir1.string());
    auto ret2 = Serializer::dump(result, dir2.string());
    ASSERT_TRUE(ret1.has_value()) << ret1.error();
    ASSERT_TRUE(ret2.has_value()) << ret2.error();

    // context.pb should be byte-identical
    EXPECT_EQ(readFileBytes(dir1 / "context.pb"),
              readFileBytes(dir2 / "context.pb"));

    // manifest.toml should be identical
    EXPECT_EQ(readFileString(dir1 / "manifest.toml"),
              readFileString(dir2 / "manifest.toml"));
}

// ---------------------------------------------------------------------------
// Overwrite — dumping to same directory twice succeeds
// ---------------------------------------------------------------------------

TEST_F(SerializerTest, OverwriteExistingOutput) {
    auto result = makeSyntheticResult();

    auto ret1 = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret1.has_value()) << ret1.error();

    // Change compiler_info and dump again to same directory
    result.compiler_info = "Clang 17.0.0";
    auto ret2 = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret2.has_value()) << ret2.error();

    auto manifest = readFileString(test_dir / "manifest.toml");
    EXPECT_NE(manifest.find("Clang 17.0.0"), std::string::npos);
}

// ---------------------------------------------------------------------------
// Manifest is valid TOML (parse it back)
// ---------------------------------------------------------------------------

TEST_F(SerializerTest, ManifestIsParsableTOML) {
    auto result = makeSyntheticResult();
    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    auto content = readFileString(test_dir / "manifest.toml");
    auto parsed = toml::parse(content);

    // [binary]
    ASSERT_TRUE(parsed["binary"].is_table());
    EXPECT_EQ(parsed["binary"]["path"].value_or(std::string()),
              "test_binary.elf");

    // [context]
    ASSERT_TRUE(parsed["context"].is_table());
    EXPECT_EQ(parsed["context"]["file"].value_or(std::string()),
              "context.pb");
    EXPECT_FALSE(
        parsed["context"]["sha256"].value_or(std::string()).empty());

    // [[groups]]
    ASSERT_TRUE(parsed["groups"].is_array());
    auto* groups = parsed["groups"].as_array();
    ASSERT_EQ(groups->size(), 1u);

    auto* g0 = (*groups)[0].as_table();
    ASSERT_NE(g0, nullptr);
    EXPECT_EQ((*g0)["name"].value_or(std::string()), "foo");

    // [[groups.units]]
    ASSERT_TRUE((*g0)["units"].is_array());
    auto* units = (*g0)["units"].as_array();
    ASSERT_EQ(units->size(), 2u);

    // Verify unit fields
    auto* u0 = (*units)[0].as_table();
    ASSERT_NE(u0, nullptr);
    EXPECT_FALSE((*u0)["file"].value_or(std::string()).empty());
    EXPECT_FALSE((*u0)["addr"].value_or(std::string()).empty());
    EXPECT_TRUE((*u0)["size"].is_integer());
    EXPECT_TRUE((*u0)["is_canonical"].is_boolean());
    EXPECT_FALSE(
        (*u0)["enclosing_symbol"].value_or(std::string()).empty());
}

// ---------------------------------------------------------------------------
// Unit file count matches manifest entries
// ---------------------------------------------------------------------------

TEST_F(SerializerTest, UnitCountMatchesManifest) {
    auto result = makeSyntheticResult();
    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    auto content = readFileString(test_dir / "manifest.toml");
    auto parsed = toml::parse(content);

    int manifest_units = 0;
    auto* groups = parsed["groups"].as_array();
    for (auto& g : *groups) {
        auto* units = g.as_table()->at("units").as_array();
        manifest_units += static_cast<int>(units->size());
    }

    // Count actual .unit.pb files on disk
    int disk_units = 0;
    for (const auto& dir_entry : fs::directory_iterator(test_dir)) {
        if (!dir_entry.is_directory()) continue;
        for (const auto& f : fs::directory_iterator(dir_entry.path())) {
            if (f.path().string().find(".unit.pb") != std::string::npos)
                ++disk_units;
        }
    }

    EXPECT_EQ(manifest_units, disk_units);
}

// ---------------------------------------------------------------------------
// Site addr not in refined_regions — graceful skip
// ---------------------------------------------------------------------------

TEST_F(SerializerTest, OrphanSiteSkipped) {
    auto result = makeSyntheticResult();

    // Add a site whose addr does not exist in refined_regions
    RegionRefiner::RegionSite orphan;
    orphan.source_name = "foo";
    orphan.enclosing_symbol = "ghost";
    orphan.is_canonical = false;
    orphan.addr = 0xDEAD;
    orphan.size = 1;
    result.groups[0].sites.push_back(std::move(orphan));

    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    // Should still have exactly 2 unit files (the orphan is skipped)
    int count = 0;
    for (const auto& e : fs::directory_iterator(test_dir / "foo.group"))
        if (e.path().extension() == ".pb") ++count;
    EXPECT_EQ(count, 2);
}

// ---------------------------------------------------------------------------
// Context SHA-256 in manifest matches recomputed hash
// ---------------------------------------------------------------------------

TEST_F(SerializerTest, ManifestSHA256MatchesRecomputed) {
    auto result = makeSyntheticResult();
    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    // Recompute hash from context.pb on disk
    auto ctx_bytes = readFileBytes(test_dir / "context.pb");
    auto hash = VMPilot::Crypto::SHA256(ctx_bytes, {});

    // Build hex string
    std::ostringstream oss;
    for (auto b : hash)
        oss << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(b);
    std::string expected_hex = oss.str();

    // Parse manifest and compare
    auto content = readFileString(test_dir / "manifest.toml");
    auto parsed = toml::parse(content);
    auto manifest_hex =
        parsed["context"]["sha256"].value_or(std::string(""));

    EXPECT_EQ(manifest_hex, expected_hex);
    EXPECT_EQ(manifest_hex.size(), 64u);  // SHA-256 = 32 bytes = 64 hex chars
}

// ---------------------------------------------------------------------------
// All unsafe filesystem chars are sanitized
// ---------------------------------------------------------------------------

TEST_F(SerializerTest, AllUnsafeCharsSanitized) {
    auto result = makeSyntheticResult();
    result.groups[0].source_name = R"(a?b<c>d:e"f|g*h\i/j)";
    for (auto& site : result.groups[0].sites)
        site.source_name = result.groups[0].source_name;

    auto ret = Serializer::dump(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << ret.error();

    // All unsafe chars should become underscore
    EXPECT_TRUE(fs::is_directory(test_dir / "a_b_c_d_e_f_g_h_i_j.group"));
}

// ---------------------------------------------------------------------------
// getCompilerInfo() integration — verify segment() populates it
// ---------------------------------------------------------------------------

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR ""
#endif

TEST_F(SerializerTest, SegmentPopulatesNewFields) {
    const std::string data_dir = TEST_DATA_DIR;
    if (data_dir.empty()) GTEST_SKIP() << "TEST_DATA_DIR not set";

    // Linux x86 — ELF with .comment section
    auto elf_path = fs::path(data_dir) / "basic_binary.Linux.x86";
    if (!fs::exists(elf_path)) GTEST_SKIP() << "Missing " << elf_path;

    auto seg = Segmentator::segment(elf_path.string());
    ASSERT_TRUE(seg.has_value()) << Segmentator::to_string(seg.error());

    EXPECT_FALSE(seg->refined_regions.empty())
        << "refined_regions should be populated";
    EXPECT_EQ(seg->binary_path, elf_path.string());
    // ELF .comment usually contains "GCC"
    EXPECT_FALSE(seg->compiler_info.empty())
        << "compiler_info should be non-empty for ELF";
}

TEST_F(SerializerTest, CompilerInfoPE) {
    const std::string data_dir = TEST_DATA_DIR;
    if (data_dir.empty()) GTEST_SKIP() << "TEST_DATA_DIR not set";

    auto pe_path = fs::path(data_dir) / "basic_binary.Windows.x86_64.exe";
    if (!fs::exists(pe_path)) GTEST_SKIP() << "Missing " << pe_path;

    auto seg = Segmentator::segment(pe_path.string());
    ASSERT_TRUE(seg.has_value()) << Segmentator::to_string(seg.error());

    // PE should report "MSVC Linker X.Y"
    EXPECT_NE(seg->compiler_info.find("MSVC Linker"), std::string::npos)
        << "compiler_info = '" << seg->compiler_info << "'";
}

TEST_F(SerializerTest, CompilerInfoMachO) {
    const std::string data_dir = TEST_DATA_DIR;
    if (data_dir.empty()) GTEST_SKIP() << "TEST_DATA_DIR not set";

    auto macho_path = fs::path(data_dir) / "basic_binary.Darwin.arm64";
    if (!fs::exists(macho_path)) GTEST_SKIP() << "Missing " << macho_path;

    auto seg = Segmentator::segment(macho_path.string());
    ASSERT_TRUE(seg.has_value()) << Segmentator::to_string(seg.error());

    EXPECT_NE(seg->compiler_info.find("Mach-O"), std::string::npos)
        << "compiler_info = '" << seg->compiler_info << "'";
}

// ---------------------------------------------------------------------------
// Cross-platform integration: segment → dump → verify for all binaries
// ---------------------------------------------------------------------------

class SerializerCrossPlatformTest
    : public SerializerTest,
      public ::testing::WithParamInterface<const char*> {};

TEST_P(SerializerCrossPlatformTest, SegmentAndDump) {
    const std::string data_dir = TEST_DATA_DIR;
    if (data_dir.empty()) GTEST_SKIP() << "TEST_DATA_DIR not set";

    auto bin_path = fs::path(data_dir) / GetParam();
    if (!fs::exists(bin_path))
        GTEST_SKIP() << "Missing " << bin_path;

    auto seg = Segmentator::segment(bin_path.string());
    ASSERT_TRUE(seg.has_value())
        << GetParam() << ": " << Segmentator::to_string(seg.error());

    auto ret = Serializer::dump(*seg, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << GetParam() << ": " << ret.error();

    // Basic structural checks
    EXPECT_TRUE(fs::exists(test_dir / "context.pb"));
    EXPECT_TRUE(fs::exists(test_dir / "manifest.toml"));

    // context.pb must be valid protobuf
    auto ctx_bytes = readFileBytes(test_dir / "context.pb");
    vmpilot::CompilationContext ctx_pb;
    EXPECT_TRUE(ctx_pb.ParseFromArray(ctx_bytes.data(),
                                      static_cast<int>(ctx_bytes.size())));
    EXPECT_GT(ctx_pb.symbols_size(), 0);

    // Every .unit.pb must be a valid CompilationUnit
    int total_units = 0;
    for (const auto& dir_entry : fs::directory_iterator(test_dir)) {
        if (!dir_entry.is_directory()) continue;
        for (const auto& f : fs::directory_iterator(dir_entry.path())) {
            if (f.path().string().find(".unit.pb") == std::string::npos)
                continue;
            ++total_units;
            auto bytes = readFileBytes(f.path());
            vmpilot::CompilationUnit unit;
            EXPECT_TRUE(unit.ParseFromArray(
                bytes.data(), static_cast<int>(bytes.size())))
                << "Invalid protobuf: " << f.path();
            EXPECT_GT(unit.size(), 0u);
            EXPECT_FALSE(unit.code().empty());
            EXPECT_EQ(unit.context_file(), "context.pb");
            EXPECT_EQ(unit.context_hash().size(), 32u);  // SHA-256
        }
    }
    EXPECT_GT(total_units, 0) << "No units produced for " << GetParam();

    // manifest.toml must be parsable
    auto manifest_str = readFileString(test_dir / "manifest.toml");
    EXPECT_NO_THROW(toml::parse(manifest_str));
}

INSTANTIATE_TEST_SUITE_P(
    AllPlatforms, SerializerCrossPlatformTest,
    ::testing::Values("basic_binary.Linux.x86",
                      "basic_binary.Linux.x86_64",
                      "basic_binary.Darwin.arm64",
                      "basic_binary.Windows.x86.exe",
                      "basic_binary.Windows.x86_64.exe"));
