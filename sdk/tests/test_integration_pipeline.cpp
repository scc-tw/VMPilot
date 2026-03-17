/// Integration tests using real compiled binaries.
///
/// Tests the full pipeline: segment → build_units → dump → load → compile
/// across multiple test data sets and platforms.

#include <Serializer.hpp>
#include <SerializationTraits.hpp>
#include <compile_pipeline.hpp>
#include <diagnostic_collector.hpp>
#include <instruction_t.hpp>
#include <segmentator.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

using namespace VMPilot::SDK;
using VMPilot::Common::DiagnosticCollector;
using VMPilot::Common::DiagnosticCode;

// ---------------------------------------------------------------------------
// Test data directories (set by CMake compile definitions)
// ---------------------------------------------------------------------------

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR ""
#endif
#ifndef TEST_DATA_MULTI_REGION
#define TEST_DATA_MULTI_REGION ""
#endif
#ifndef TEST_DATA_INLINE_HEAVY
#define TEST_DATA_INLINE_HEAVY ""
#endif
#ifndef TEST_DATA_NESTED_PROTECT
#define TEST_DATA_NESTED_PROTECT ""
#endif

static const std::string TEST_KEY =
    "01234567890123456789012345678901";

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/// Find a test binary in a data directory. Tries Linux x86 first, then others.
static std::string findBinary(const std::string& dir,
                              const std::string& base_name) {
    if (dir.empty()) return {};

    const char* suffixes[] = {
        ".Linux.x86_64", ".Linux.x86",
        ".Darwin.arm64",
        ".Windows.x86_64.exe", ".Windows.x86.exe",
    };
    for (const char* s : suffixes) {
        auto path = dir + "/" + base_name + s;
        if (fs::exists(path)) return path;
    }
    return {};
}

// ---------------------------------------------------------------------------
// basic: Single protected function
// ---------------------------------------------------------------------------

class BasicPipelineTest : public ::testing::Test {
protected:
    std::string binary = findBinary(TEST_DATA_DIR, "basic_binary");
};

TEST_F(BasicPipelineTest, FullPipelineEndToEnd) {
    if (binary.empty()) GTEST_SKIP() << "No basic test binary found";

    DiagnosticCollector diag;
    BytecodeCompiler::CompileConfig config{TEST_KEY, false};
    auto result = BytecodeCompiler::compile_binary(binary, config, diag);
    ASSERT_TRUE(result.has_value()) << diag.summary();
    EXPECT_GT(result->total_units, 0u);
    EXPECT_EQ(result->failed_units, 0u);

    // Every output should have valid bytecodes
    VMPilot::Common::Instruction instr_helper;
    for (const auto& out : result->outputs) {
        EXPECT_FALSE(out.bytecodes.empty()) << out.name;
        for (const auto& inst : out.bytecodes) {
            EXPECT_TRUE(instr_helper.check(inst)) << out.name;
        }
    }
}

TEST_F(BasicPipelineTest, SerializerRoundTrip) {
    if (binary.empty()) GTEST_SKIP() << "No basic test binary found";

    DiagnosticCollector diag;
    auto seg = Segmentator::segment(binary, diag);
    ASSERT_TRUE(seg.has_value()) << diag.summary();

    auto original = Serializer::build_units(*seg, diag);
    ASSERT_FALSE(original.empty());

    auto tmp = fs::temp_directory_path() / "vmpilot_basic_roundtrip";
    fs::remove_all(tmp);

    auto dump_ret = Serializer::dump(original, tmp.string(), diag);
    ASSERT_TRUE(dump_ret.has_value()) << static_cast<int>(dump_ret.error());

    auto loaded = Serializer::load(tmp.string(), diag);
    ASSERT_TRUE(loaded.has_value()) << static_cast<int>(loaded.error());

    EXPECT_EQ(original.size(), loaded->size());

    // Sort by addr for stable comparison
    auto by_addr = [](const Core::CompilationUnit& a,
                      const Core::CompilationUnit& b) {
        return a.addr < b.addr;
    };
    std::sort(original.begin(), original.end(), by_addr);
    std::sort(loaded->begin(), loaded->end(), by_addr);

    for (size_t i = 0; i < original.size(); ++i) {
        EXPECT_EQ(original[i].name, (*loaded)[i].name);
        EXPECT_EQ(original[i].addr, (*loaded)[i].addr);
        EXPECT_EQ(original[i].code, (*loaded)[i].code);
        EXPECT_EQ(original[i].is_canonical, (*loaded)[i].is_canonical);
        EXPECT_NE((*loaded)[i].context, nullptr);
    }

    fs::remove_all(tmp);
}

// ---------------------------------------------------------------------------
// multi_region: Three distinct protected functions
// ---------------------------------------------------------------------------

class MultiRegionTest : public ::testing::Test {
protected:
    std::string binary = findBinary(TEST_DATA_MULTI_REGION, "multi_region");
};

TEST_F(MultiRegionTest, ThreeGroupsDetected) {
    if (binary.empty()) GTEST_SKIP() << "No multi_region binary found";

    DiagnosticCollector diag;
    auto seg = Segmentator::segment(binary, diag);
    ASSERT_TRUE(seg.has_value()) << diag.summary();

    // Should have at least 3 groups (compute_alpha, compute_beta, compute_gamma)
    EXPECT_GE(seg->groups.size(), 3u)
        << "Expected 3 groups for 3 protected functions";

    // Each group should have at least 1 site
    for (const auto& g : seg->groups) {
        EXPECT_GE(g.sites.size(), 1u) << "Group '" << g.source_name << "'";
    }
}

TEST_F(MultiRegionTest, AllFunctionsCompile) {
    if (binary.empty()) GTEST_SKIP() << "No multi_region binary found";

    DiagnosticCollector diag;
    BytecodeCompiler::CompileConfig config{TEST_KEY, false};
    auto result = BytecodeCompiler::compile_binary(binary, config, diag);
    ASSERT_TRUE(result.has_value()) << diag.summary();

    EXPECT_GE(result->total_units, 3u);
    EXPECT_EQ(result->failed_units, 0u);
    EXPECT_EQ(result->outputs.size(), result->total_units);
}

TEST_F(MultiRegionTest, RoundTrip) {
    if (binary.empty()) GTEST_SKIP() << "No multi_region binary found";

    DiagnosticCollector diag;
    auto seg = Segmentator::segment(binary, diag);
    ASSERT_TRUE(seg.has_value());

    auto original = Serializer::build_units(*seg, diag);
    ASSERT_GE(original.size(), 3u);

    auto tmp = fs::temp_directory_path() / "vmpilot_multi_roundtrip";
    fs::remove_all(tmp);

    ASSERT_TRUE(Serializer::dump(original, tmp.string(), diag).has_value());
    auto loaded = Serializer::load(tmp.string(), diag);
    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(original.size(), loaded->size());

    fs::remove_all(tmp);
}

// ---------------------------------------------------------------------------
// inline_heavy: Inlined protected function → multiple sites per group
// ---------------------------------------------------------------------------

class InlineHeavyTest : public ::testing::Test {
protected:
    std::string binary = findBinary(TEST_DATA_INLINE_HEAVY, "inline_heavy");
};

TEST_F(InlineHeavyTest, MultipleSitesInGroup) {
    if (binary.empty()) GTEST_SKIP() << "No inline_heavy binary found";

    DiagnosticCollector diag;
    auto seg = Segmentator::segment(binary, diag);
    ASSERT_TRUE(seg.has_value()) << diag.summary();

    // Look for a group named "protected_op" with multiple sites
    const RegionRefiner::ProtectedRegion* target = nullptr;
    for (const auto& g : seg->groups) {
        if (g.source_name.find("protected_op") != std::string::npos) {
            target = &g;
            break;
        }
    }

    if (!target) {
        // Compiler might not have inlined — just check we have at least 1 group
        EXPECT_GE(seg->groups.size(), 1u)
            << "Expected at least 1 group in inline_heavy binary";
        return;
    }

    // If inlining happened, should have multiple sites
    EXPECT_GE(target->sites.size(), 2u)
        << "Expected multiple sites for inlined 'protected_op'";

    // Exactly one should be canonical
    int canonical_count = 0;
    for (const auto& s : target->sites) {
        if (s.is_canonical) ++canonical_count;
    }
    EXPECT_EQ(canonical_count, 1)
        << "Expected exactly 1 canonical site";
}

TEST_F(InlineHeavyTest, CompileAllCopies) {
    if (binary.empty()) GTEST_SKIP() << "No inline_heavy binary found";

    DiagnosticCollector diag;
    BytecodeCompiler::CompileConfig config{TEST_KEY, false};
    auto result = BytecodeCompiler::compile_binary(binary, config, diag);
    ASSERT_TRUE(result.has_value()) << diag.summary();

    EXPECT_GT(result->total_units, 0u);
    EXPECT_EQ(result->failed_units, 0u);

    // All outputs should have valid bytecodes
    VMPilot::Common::Instruction instr_helper;
    for (const auto& out : result->outputs) {
        for (const auto& inst : out.bytecodes) {
            EXPECT_TRUE(instr_helper.check(inst)) << out.name;
        }
    }
}

// ---------------------------------------------------------------------------
// nested_protect: Protected function calls another protected function
// ---------------------------------------------------------------------------

class NestedProtectTest : public ::testing::Test {
protected:
    std::string binary = findBinary(TEST_DATA_NESTED_PROTECT, "nested_protect");
};

TEST_F(NestedProtectTest, TwoDistinctGroups) {
    if (binary.empty()) GTEST_SKIP() << "No nested_protect binary found";

    DiagnosticCollector diag;
    auto seg = Segmentator::segment(binary, diag);
    ASSERT_TRUE(seg.has_value()) << diag.summary();

    // Should have at least 2 groups (inner_work + outer_work)
    EXPECT_GE(seg->groups.size(), 2u)
        << "Expected 2 groups for nested protected functions";

    // Regions should not overlap — each should have distinct addr ranges
    std::vector<std::pair<uint64_t, uint64_t>> ranges;
    for (const auto& r : seg->refined_regions) {
        ranges.push_back({r.getAddr(), r.getAddr() + r.getSize()});
    }
    std::sort(ranges.begin(), ranges.end());

    for (size_t i = 1; i < ranges.size(); ++i) {
        EXPECT_GE(ranges[i].first, ranges[i - 1].second)
            << "Regions overlap: [0x" << std::hex << ranges[i - 1].first
            << ", 0x" << ranges[i - 1].second << ") and [0x"
            << ranges[i].first << ", 0x" << ranges[i].second << ")";
    }
}

TEST_F(NestedProtectTest, BothFunctionsCompile) {
    if (binary.empty()) GTEST_SKIP() << "No nested_protect binary found";

    DiagnosticCollector diag;
    BytecodeCompiler::CompileConfig config{TEST_KEY, false};
    auto result = BytecodeCompiler::compile_binary(binary, config, diag);
    ASSERT_TRUE(result.has_value()) << diag.summary();

    EXPECT_GE(result->total_units, 2u);
    EXPECT_EQ(result->failed_units, 0u);
}

TEST_F(NestedProtectTest, RoundTrip) {
    if (binary.empty()) GTEST_SKIP() << "No nested_protect binary found";

    DiagnosticCollector diag;
    auto seg = Segmentator::segment(binary, diag);
    ASSERT_TRUE(seg.has_value());

    auto original = Serializer::build_units(*seg, diag);
    ASSERT_GE(original.size(), 2u);

    auto tmp = fs::temp_directory_path() / "vmpilot_nested_roundtrip";
    fs::remove_all(tmp);

    ASSERT_TRUE(Serializer::dump(original, tmp.string(), diag).has_value());
    auto loaded = Serializer::load(tmp.string(), diag);
    ASSERT_TRUE(loaded.has_value());
    EXPECT_EQ(original.size(), loaded->size());

    // Verify code bytes survived round-trip
    auto by_addr = [](const Core::CompilationUnit& a,
                      const Core::CompilationUnit& b) {
        return a.addr < b.addr;
    };
    std::sort(original.begin(), original.end(), by_addr);
    std::sort(loaded->begin(), loaded->end(), by_addr);

    for (size_t i = 0; i < original.size(); ++i) {
        EXPECT_EQ(original[i].code, (*loaded)[i].code)
            << "Code mismatch for unit " << original[i].name;
    }

    fs::remove_all(tmp);
}
