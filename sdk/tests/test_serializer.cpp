#include <Serializer.hpp>
#include <SerializationTraits.hpp>
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
namespace Common = VMPilot::Common;

namespace {

std::vector<uint8_t> readFileBytes(const fs::path& path) {
    std::ifstream in(path, std::ios::binary);
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(in),
                                std::istreambuf_iterator<char>());
}

std::string readFileString(const fs::path& path) {
    std::ifstream in(path, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(in),
                       std::istreambuf_iterator<char>());
}

/// Build a minimal synthetic SegmentationResult for testing.
Segmentator::SegmentationResult makeSyntheticResult() {
    Segmentator::SegmentationResult result;

    std::vector<uint8_t> code1 = {0x55, 0x48, 0x89, 0xe5, 0x5d, 0xc3};
    std::vector<uint8_t> code2 = {0x90, 0x90, 0xc3};

    Segmentator::NativeFunctionBase r1(0x1000, code1.size(), "foo", code1);
    r1.setEnclosingSymbol("main");

    Segmentator::NativeFunctionBase r2(0x2000, code2.size(), "foo", code2);
    r2.setEnclosingSymbol("_Z3fooi");

    result.refined_regions.push_back(std::move(r1));
    result.refined_regions.push_back(std::move(r2));

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

/// Helper: build_units + dump in one call.
tl::expected<void, VMPilot::Common::DiagnosticCode> dumpFromResult(
    const Segmentator::SegmentationResult& result,
    const std::string& output_dir) {
    auto units = Serializer::build_units(result);
    return Serializer::dump(units, output_dir);
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

// ===========================================================================
// build_units tests
// ===========================================================================

TEST_F(SerializerTest, BuildUnitsFromSyntheticResult) {
    auto result = makeSyntheticResult();
    auto units = Serializer::build_units(result);

    ASSERT_EQ(units.size(), 2u);

    // All units share the same context
    EXPECT_NE(units[0].context, nullptr);
    EXPECT_EQ(units[0].context.get(), units[1].context.get());
    EXPECT_EQ(units[0].context->arch, Segmentator::Arch::X86);

    // Check first unit (canonical)
    bool found_canonical = false;
    bool found_inline = false;
    for (const auto& u : units) {
        if (u.is_canonical) {
            found_canonical = true;
            EXPECT_EQ(u.name, "foo");
            EXPECT_EQ(u.addr, 0x1000u);
            EXPECT_EQ(u.enclosing_symbol, "main");
        } else {
            found_inline = true;
            EXPECT_EQ(u.name, "foo");
            EXPECT_EQ(u.addr, 0x2000u);
            EXPECT_EQ(u.enclosing_symbol, "_Z3fooi");
        }
    }
    EXPECT_TRUE(found_canonical);
    EXPECT_TRUE(found_inline);
}

TEST_F(SerializerTest, BuildUnitsEmptyResult) {
    Segmentator::SegmentationResult result;
    result.context.arch = Segmentator::Arch::X86;
    auto units = Serializer::build_units(result);
    EXPECT_TRUE(units.empty());
}

// ===========================================================================
// SerializationTraits round-trip tests
// ===========================================================================

TEST_F(SerializerTest, ContextTraitsRoundTrip) {
    Segmentator::CompilationContext ctx;
    ctx.arch = Segmentator::Arch::X86;
    ctx.mode = Segmentator::Mode::MODE_64;

    Segmentator::NativeSymbolTableEntry sym;
    sym.name = "test_sym";
    sym.address = 0x1234;
    sym.size = 42;
    sym.isGlobal = true;
    sym.setAttribute("entry_type", std::string("stub"));
    ctx.symbols.push_back(std::move(sym));

    Segmentator::ReadOnlySection sec;
    sec.base_addr = 0x5000;
    sec.data = {0xAA, 0xBB, 0x00, 0xCC};
    ctx.rodata_sections.push_back(std::move(sec));

    auto bytes = Serializer::serialize(ctx);
    ASSERT_TRUE(bytes.has_value()) << bytes.error();

    auto restored = Serializer::deserialize<Segmentator::CompilationContext>(*bytes);
    ASSERT_TRUE(restored.has_value()) << restored.error();

    EXPECT_EQ(restored->arch, ctx.arch);
    EXPECT_EQ(restored->mode, ctx.mode);
    ASSERT_EQ(restored->symbols.size(), 1u);
    EXPECT_EQ(restored->symbols[0].name, "test_sym");
    EXPECT_EQ(restored->symbols[0].address, 0x1234u);
    EXPECT_TRUE(restored->symbols[0].isGlobal);
    ASSERT_EQ(restored->rodata_sections.size(), 1u);
    EXPECT_EQ(restored->rodata_sections[0].base_addr, 0x5000u);
    EXPECT_EQ(restored->rodata_sections[0].data,
              (std::vector<uint8_t>{0xAA, 0xBB, 0x00, 0xCC}));
}

TEST_F(SerializerTest, UnitTraitsRoundTrip) {
    auto ctx = std::make_shared<const Segmentator::CompilationContext>(
        Segmentator::CompilationContext{
            {}, {}, {}, Segmentator::Arch::X86, Segmentator::Mode::MODE_64});

    Core::CompilationUnit unit;
    unit.name = "my_func";
    unit.addr = 0xABCD;
    unit.size = 4;
    unit.code = {0x55, 0x89, 0xe5, 0xc3};
    unit.enclosing_symbol = "_Z7my_funcv";
    unit.is_canonical = true;
    unit.context = ctx;

    auto bytes = Serializer::serialize(unit);
    ASSERT_TRUE(bytes.has_value()) << bytes.error();

    auto restored = Serializer::deserialize<Core::CompilationUnit>(*bytes, ctx);
    ASSERT_TRUE(restored.has_value()) << restored.error();

    EXPECT_EQ(restored->name, "my_func");
    EXPECT_EQ(restored->addr, 0xABCDu);
    EXPECT_EQ(restored->size, 4u);
    EXPECT_EQ(restored->code, unit.code);
    EXPECT_EQ(restored->enclosing_symbol, "_Z7my_funcv");
    EXPECT_TRUE(restored->is_canonical);
    EXPECT_EQ(restored->context.get(), ctx.get());
}

// ===========================================================================
// dump + load round-trip tests
// ===========================================================================

TEST_F(SerializerTest, DumpAndLoadRoundTrip) {
    auto result = makeSyntheticResult();
    auto units = Serializer::build_units(result);
    auto ret = Serializer::dump(units, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

    auto loaded = Serializer::load(test_dir.string());
    ASSERT_TRUE(loaded.has_value()) << static_cast<int>(loaded.error());
    ASSERT_EQ(loaded->size(), units.size());

    // Verify loaded units have correct data
    for (const auto& u : *loaded) {
        EXPECT_EQ(u.name, "foo");
        EXPECT_NE(u.context, nullptr);
        EXPECT_EQ(u.context->arch, Segmentator::Arch::X86);
        EXPECT_FALSE(u.code.empty());
    }
}

TEST_F(SerializerTest, LoadUnitSingle) {
    auto result = makeSyntheticResult();
    auto units = Serializer::build_units(result);
    auto ret = Serializer::dump(units, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

    // Find a .unit.pb file
    std::string unit_path;
    for (const auto& dir : fs::directory_iterator(test_dir)) {
        if (!dir.is_directory()) continue;
        for (const auto& f : fs::directory_iterator(dir.path())) {
            if (f.path().extension() == ".pb") {
                unit_path = f.path().string();
                break;
            }
        }
        if (!unit_path.empty()) break;
    }
    ASSERT_FALSE(unit_path.empty()) << "No .unit.pb found";

    auto ctx_path = (test_dir / "context.pb").string();
    auto loaded = Serializer::load_unit(unit_path, ctx_path);
    ASSERT_TRUE(loaded.has_value()) << static_cast<int>(loaded.error());
    EXPECT_EQ(loaded->name, "foo");
    EXPECT_NE(loaded->context, nullptr);
    EXPECT_FALSE(loaded->code.empty());
}

// ===========================================================================
// Original dump tests (adapted to new API)
// ===========================================================================

TEST_F(SerializerTest, RoundTrip) {
    auto result = makeSyntheticResult();
    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

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
    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

    EXPECT_TRUE(fs::exists(test_dir / "manifest.toml"));
    EXPECT_TRUE(fs::exists(test_dir / "context.pb"));
    EXPECT_TRUE(fs::is_directory(test_dir / "foo.group"));

    int unit_count = 0;
    for (const auto& entry : fs::directory_iterator(test_dir / "foo.group")) {
        if (entry.path().extension() == ".pb") ++unit_count;
    }
    EXPECT_EQ(unit_count, 2);
}

TEST_F(SerializerTest, ManifestContainsExpectedFields) {
    auto result = makeSyntheticResult();
    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

    auto manifest = readFileString(test_dir / "manifest.toml");
    EXPECT_NE(manifest.find("context.pb"), std::string::npos);
    EXPECT_NE(manifest.find("foo"), std::string::npos);
    EXPECT_NE(manifest.find("X86"), std::string::npos);
}

TEST_F(SerializerTest, ContextHashConsistency) {
    auto result = makeSyntheticResult();
    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

    auto ctx_bytes = readFileBytes(test_dir / "context.pb");
    auto expected_hash =
        VMPilot::Crypto::SHA256(ctx_bytes, /*salt=*/{});

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
    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

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
    // Build a result with unsafe chars in the name
    Segmentator::SegmentationResult result;
    result.context.arch = Segmentator::Arch::X86;
    result.context.mode = Segmentator::Mode::MODE_64;

    std::string unsafe_name = "foo<bar>:baz";
    std::vector<uint8_t> code = {0xc3};
    result.refined_regions.emplace_back(0x1000, code.size(), unsafe_name, code);
    result.refined_regions.back().setEnclosingSymbol("main");

    RegionRefiner::ProtectedRegion g;
    g.source_name = unsafe_name;
    g.canonical_index = 0;
    RegionRefiner::RegionSite s;
    s.source_name = unsafe_name;
    s.enclosing_symbol = "main";
    s.is_canonical = true;
    s.addr = 0x1000;
    s.size = 1;
    g.sites.push_back(std::move(s));
    result.groups.push_back(std::move(g));

    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

    EXPECT_TRUE(fs::is_directory(test_dir / "foo_bar__baz.group"));
}

TEST_F(SerializerTest, MultipleGroups) {
    Segmentator::SegmentationResult result;
    result.context.arch = Segmentator::Arch::X86;
    result.context.mode = Segmentator::Mode::MODE_64;

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

    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

    EXPECT_TRUE(fs::is_directory(test_dir / "alpha.group"));
    EXPECT_TRUE(fs::is_directory(test_dir / "beta.group"));
    EXPECT_TRUE(fs::is_directory(test_dir / "gamma.group"));

    for (const char* name : {"alpha.group", "beta.group", "gamma.group"}) {
        int count = 0;
        for (const auto& e : fs::directory_iterator(test_dir / name))
            if (e.path().extension() == ".pb") ++count;
        EXPECT_EQ(count, 1) << "Expected 1 unit in " << name;
    }
}

TEST_F(SerializerTest, InlineUnitFields) {
    auto result = makeSyntheticResult();
    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

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

TEST_F(SerializerTest, LongNestedTemplateName) {
    std::string nested_template =
        "_ZN5boost6detail14variant_accessINSt6vectorINS2_INS2_IdSaIdEE"
        "SaIS4_EESaIS6_EESaIS8_EESaISA_EESaISC_EESaISE_EESaISG_EESaI"
        "SI_EESaISK_EESaISM_EESaISO_EESaISQ_EESaISS_EESaISU_EESaISW_"
        "EESaISY_EEE14some_long_inner_template_specialization_name";

    Segmentator::SegmentationResult result;
    result.context.arch = Segmentator::Arch::X86;
    result.context.mode = Segmentator::Mode::MODE_64;

    std::vector<uint8_t> code1 = {0x55, 0x48, 0x89, 0xe5, 0x5d, 0xc3};
    std::vector<uint8_t> code2 = {0x90, 0x90, 0xc3};
    result.refined_regions.emplace_back(0x1000, code1.size(), nested_template, code1);
    result.refined_regions.back().setEnclosingSymbol(nested_template);
    result.refined_regions.emplace_back(0x2000, code2.size(), nested_template, code2);
    result.refined_regions.back().setEnclosingSymbol(nested_template);

    RegionRefiner::ProtectedRegion group;
    group.source_name = nested_template;
    group.canonical_index = 0;
    RegionRefiner::RegionSite s1;
    s1.source_name = nested_template;
    s1.enclosing_symbol = nested_template;
    s1.is_canonical = true;
    s1.addr = 0x1000;
    s1.size = code1.size();
    RegionRefiner::RegionSite s2;
    s2.source_name = nested_template;
    s2.enclosing_symbol = nested_template;
    s2.is_canonical = false;
    s2.addr = 0x2000;
    s2.size = code2.size();
    group.sites.push_back(std::move(s1));
    group.sites.push_back(std::move(s2));
    result.groups.push_back(std::move(group));

    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

    bool found_group = false;
    for (const auto& entry : fs::directory_iterator(test_dir)) {
        auto name = entry.path().filename().string();
        if (name.find(".group") != std::string::npos) {
            found_group = true;
            EXPECT_LE(name.size(), 255u) << "Directory name too long: " << name;
        }
    }
    EXPECT_TRUE(found_group) << "No .group directory created";

    int unit_count = 0;
    for (const auto& dir_entry : fs::directory_iterator(test_dir)) {
        if (!dir_entry.is_directory()) continue;
        for (const auto& f : fs::directory_iterator(dir_entry.path())) {
            if (f.path().extension() != ".pb") continue;
            ++unit_count;
            auto fname = f.path().filename().string();
            EXPECT_LE(fname.size(), 120u) << "Filename too long: " << fname;
            EXPECT_NE(fname.find("0x"), std::string::npos)
                << "Address missing from filename: " << fname;
        }
    }
    EXPECT_EQ(unit_count, 2);
}

TEST_F(SerializerTest, TwoLongNamesStayUnique) {
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

    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

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

TEST_F(SerializerTest, MissingEnclosingSymbol) {
    Segmentator::SegmentationResult result;
    result.context.arch = Segmentator::Arch::X86;
    result.context.mode = Segmentator::Mode::MODE_32;

    std::vector<uint8_t> code = {0xc3};
    result.refined_regions.emplace_back(0x401000, code.size(), "func", code);

    RegionRefiner::ProtectedRegion g;
    g.source_name = "func";
    g.canonical_index = 0;
    RegionRefiner::RegionSite s;
    s.source_name = "func";
    s.is_canonical = true;
    s.addr = 0x401000;
    s.size = 1;
    g.sites.push_back(std::move(s));
    result.groups.push_back(std::move(g));

    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

    for (const auto& e : fs::directory_iterator(test_dir / "func.group")) {
        if (e.path().extension() != ".pb") continue;
        auto bytes = readFileBytes(e.path());
        vmpilot::CompilationUnit unit;
        ASSERT_TRUE(unit.ParseFromArray(bytes.data(),
                                        static_cast<int>(bytes.size())));
        // Empty enclosing_symbol maps to "unknown" in dump
        EXPECT_TRUE(unit.enclosing_symbol() == "unknown" ||
                    unit.enclosing_symbol().empty());
    }
}

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

    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

    auto ctx_bytes = readFileBytes(test_dir / "context.pb");
    vmpilot::CompilationContext ctx_pb;
    ASSERT_TRUE(ctx_pb.ParseFromArray(ctx_bytes.data(),
                                      static_cast<int>(ctx_bytes.size())));

    ASSERT_EQ(ctx_pb.symbols_size(), 3);

    bool found_stub = false, found_pt = false;
    for (int i = 0; i < ctx_pb.symbols_size(); ++i) {
        const auto& s = ctx_pb.symbols(i);
        if (s.name() == "printf") {
            found_stub = true;
            EXPECT_EQ(s.entry_type(), "stub");
        }
        if (s.name() == "puts") {
            found_pt = true;
            EXPECT_EQ(s.entry_type(), "pointer_table");
        }
    }
    EXPECT_TRUE(found_stub);
    EXPECT_TRUE(found_pt);
}

TEST_F(SerializerTest, RodataRoundTrip) {
    auto result = makeSyntheticResult();
    result.context.rodata_sections.clear();
    Segmentator::ReadOnlySection sec;
    sec.base_addr = 0xDEAD;
    sec.data = {0x00, 0xFF, 0x41, 0x42, 0x00, 0x43};
    result.context.rodata_sections.push_back(std::move(sec));

    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

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

TEST_F(SerializerTest, Determinism) {
    auto result = makeSyntheticResult();

    fs::path dir1 = test_dir / "run1";
    fs::path dir2 = test_dir / "run2";

    auto ret1 = dumpFromResult(result, dir1.string());
    auto ret2 = dumpFromResult(result, dir2.string());
    ASSERT_TRUE(ret1.has_value()) << static_cast<int>(ret1.error());
    ASSERT_TRUE(ret2.has_value()) << static_cast<int>(ret2.error());

    EXPECT_EQ(readFileBytes(dir1 / "context.pb"),
              readFileBytes(dir2 / "context.pb"));
}

TEST_F(SerializerTest, ManifestIsParsableTOML) {
    auto result = makeSyntheticResult();
    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

    auto content = readFileString(test_dir / "manifest.toml");
    auto parsed = toml::parse(content);

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

    ASSERT_TRUE((*g0)["units"].is_array());
    auto* units = (*g0)["units"].as_array();
    ASSERT_EQ(units->size(), 2u);
}

TEST_F(SerializerTest, UnitCountMatchesManifest) {
    auto result = makeSyntheticResult();
    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

    auto content = readFileString(test_dir / "manifest.toml");
    auto parsed = toml::parse(content);

    int manifest_units = 0;
    auto* groups = parsed["groups"].as_array();
    for (auto& g : *groups) {
        auto* units = g.as_table()->at("units").as_array();
        manifest_units += static_cast<int>(units->size());
    }

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

TEST_F(SerializerTest, OrphanSiteSkipped) {
    auto result = makeSyntheticResult();

    RegionRefiner::RegionSite orphan;
    orphan.source_name = "foo";
    orphan.enclosing_symbol = "ghost";
    orphan.is_canonical = false;
    orphan.addr = 0xDEAD;
    orphan.size = 1;
    result.groups[0].sites.push_back(std::move(orphan));

    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

    int count = 0;
    for (const auto& e : fs::directory_iterator(test_dir / "foo.group"))
        if (e.path().extension() == ".pb") ++count;
    EXPECT_EQ(count, 2);
}

TEST_F(SerializerTest, ManifestSHA256MatchesRecomputed) {
    auto result = makeSyntheticResult();
    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

    auto ctx_bytes = readFileBytes(test_dir / "context.pb");
    auto hash = VMPilot::Crypto::SHA256(ctx_bytes, {});

    std::ostringstream oss;
    for (auto b : hash)
        oss << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(b);
    std::string expected_hex = oss.str();

    auto content = readFileString(test_dir / "manifest.toml");
    auto parsed = toml::parse(content);
    auto manifest_hex =
        parsed["context"]["sha256"].value_or(std::string(""));

    EXPECT_EQ(manifest_hex, expected_hex);
    EXPECT_EQ(manifest_hex.size(), 64u);
}

TEST_F(SerializerTest, AllUnsafeCharsSanitized) {
    std::string unsafe_name = R"(a?b<c>d:e"f|g*h\i/j)";

    Segmentator::SegmentationResult result;
    result.context.arch = Segmentator::Arch::X86;
    result.context.mode = Segmentator::Mode::MODE_64;

    std::vector<uint8_t> code = {0xc3};
    result.refined_regions.emplace_back(0x1000, code.size(), unsafe_name, code);
    result.refined_regions.back().setEnclosingSymbol("main");

    RegionRefiner::ProtectedRegion g;
    g.source_name = unsafe_name;
    g.canonical_index = 0;
    RegionRefiner::RegionSite s;
    s.source_name = unsafe_name;
    s.enclosing_symbol = "main";
    s.is_canonical = true;
    s.addr = 0x1000;
    s.size = 1;
    g.sites.push_back(std::move(s));
    result.groups.push_back(std::move(g));

    auto ret = dumpFromResult(result, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << static_cast<int>(ret.error());

    EXPECT_TRUE(fs::is_directory(test_dir / "a_b_c_d_e_f_g_h_i_j.group"));
}

// ===========================================================================
// Integration tests with real binaries
// ===========================================================================

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR ""
#endif

TEST_F(SerializerTest, SegmentPopulatesNewFields) {
    const std::string data_dir = TEST_DATA_DIR;
    if (data_dir.empty()) GTEST_SKIP() << "TEST_DATA_DIR not set";

    auto elf_path = fs::path(data_dir) / "basic_binary.Linux.x86";
    if (!fs::exists(elf_path)) GTEST_SKIP() << "Missing " << elf_path;

    auto seg = Segmentator::segment(elf_path.string());
    ASSERT_TRUE(seg.has_value()) << static_cast<int>(seg.error());

    EXPECT_FALSE(seg->refined_regions.empty());
    EXPECT_EQ(seg->binary_path, elf_path.string());
    EXPECT_FALSE(seg->compiler_info.empty());
}

TEST_F(SerializerTest, CompilerInfoPE) {
    const std::string data_dir = TEST_DATA_DIR;
    if (data_dir.empty()) GTEST_SKIP() << "TEST_DATA_DIR not set";

    auto pe_path = fs::path(data_dir) / "basic_binary.Windows.x86_64.exe";
    if (!fs::exists(pe_path)) GTEST_SKIP() << "Missing " << pe_path;

    auto seg = Segmentator::segment(pe_path.string());
    ASSERT_TRUE(seg.has_value()) << static_cast<int>(seg.error());

    EXPECT_NE(seg->compiler_info.find("MSVC Linker"), std::string::npos)
        << "compiler_info = '" << seg->compiler_info << "'";
}

TEST_F(SerializerTest, CompilerInfoMachO) {
    const std::string data_dir = TEST_DATA_DIR;
    if (data_dir.empty()) GTEST_SKIP() << "TEST_DATA_DIR not set";

    auto macho_path = fs::path(data_dir) / "basic_binary.Darwin.arm64";
    if (!fs::exists(macho_path)) GTEST_SKIP() << "Missing " << macho_path;

    auto seg = Segmentator::segment(macho_path.string());
    ASSERT_TRUE(seg.has_value()) << static_cast<int>(seg.error());

    EXPECT_NE(seg->compiler_info.find("Mach-O"), std::string::npos)
        << "compiler_info = '" << seg->compiler_info << "'";
}

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
        << GetParam() << ": " << static_cast<int>(seg.error());

    auto units = Serializer::build_units(*seg);
    auto ret = Serializer::dump(units, test_dir.string());
    ASSERT_TRUE(ret.has_value()) << GetParam() << ": " << static_cast<int>(ret.error());

    EXPECT_TRUE(fs::exists(test_dir / "context.pb"));
    EXPECT_TRUE(fs::exists(test_dir / "manifest.toml"));

    auto ctx_bytes = readFileBytes(test_dir / "context.pb");
    vmpilot::CompilationContext ctx_pb;
    EXPECT_TRUE(ctx_pb.ParseFromArray(ctx_bytes.data(),
                                      static_cast<int>(ctx_bytes.size())));
    EXPECT_GT(ctx_pb.symbols_size(), 0);

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
            EXPECT_EQ(unit.context_hash().size(), 32u);
        }
    }
    EXPECT_GT(total_units, 0) << "No units produced for " << GetParam();

    auto manifest_str = readFileString(test_dir / "manifest.toml");
    EXPECT_NO_THROW(static_cast<void>(toml::parse(manifest_str)));

    // Round-trip: load back and verify
    auto loaded = Serializer::load(test_dir.string());
    ASSERT_TRUE(loaded.has_value()) << static_cast<int>(loaded.error());
    EXPECT_EQ(loaded->size(), static_cast<size_t>(total_units));
}

INSTANTIATE_TEST_SUITE_P(
    AllPlatforms, SerializerCrossPlatformTest,
    ::testing::Values("basic_binary.Linux.x86",
                      "basic_binary.Linux.x86_64",
                      "basic_binary.Darwin.arm64",
                      "basic_binary.Windows.x86.exe",
                      "basic_binary.Windows.x86_64.exe"));
