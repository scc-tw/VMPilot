/// Tests for the segment() top-level API across all supported platforms.
/// Validates the full pipeline: file parsing, disassembly, region extraction,
/// refinement, and grouping with canonical site detection.

#include <segmentator.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <string>

using VMPilot::SDK::Segmentator::segment;
using VMPilot::SDK::Segmentator::SegmentError;
using VMPilot::SDK::Segmentator::SegmentationResult;
using VMPilot::SDK::RegionRefiner::ProtectedRegion;
using VMPilot::SDK::RegionRefiner::RegionSite;

// ---------------------------------------------------------------------------
// Test fixture with common helpers
// ---------------------------------------------------------------------------

class SegmentAPITest : public ::testing::Test {
   protected:
    /// Check that the result succeeded and return groups for further checks.
    static const std::vector<ProtectedRegion>& expectSuccess(
        const tl::expected<SegmentationResult, SegmentError>& result,
        const std::string& label) {
        EXPECT_TRUE(result.has_value())
            << label << ": segment() failed with "
            << (result.has_value()
                    ? "success"
                    : VMPilot::SDK::Segmentator::to_string(result.error()));
        return result->groups;
    }

    /// Returns true if at least one group has a non-fallback source name.
    static bool hasNamedRegion(const std::vector<ProtectedRegion>& groups) {
        return std::any_of(groups.begin(), groups.end(),
                           [](const ProtectedRegion& g) {
                               return g.source_name.substr(0, 17) !=
                                      "vmpilot_region_0x";
                           });
    }

    /// Returns true if at least one group has is_canonical on one of its sites.
    static bool hasCanonicalSite(const std::vector<ProtectedRegion>& groups) {
        for (const auto& g : groups) {
            for (const auto& s : g.sites) {
                if (s.is_canonical)
                    return true;
            }
        }
        return false;
    }

    /// Returns true if at least one site across all groups has an enclosing symbol.
    static bool hasEnclosingSymbol(const std::vector<ProtectedRegion>& groups) {
        for (const auto& g : groups) {
            for (const auto& s : g.sites) {
                if (s.enclosing_symbol.has_value() &&
                    !s.enclosing_symbol->empty())
                    return true;
            }
        }
        return false;
    }

    // Binary paths
    static std::string linuxX86_64() {
        return std::string(TEST_DATA_DIR) + "/basic_binary.Linux.x86_64";
    }
    static std::string linuxX86() {
        return std::string(TEST_DATA_DIR) + "/basic_binary.Linux.x86";
    }
    static std::string darwinARM64() {
        return std::string(TEST_DATA_DIR) + "/basic_binary.Darwin.arm64";
    }
    static std::string windowsX86_64() {
        return std::string(TEST_DATA_DIR) + "/basic_binary.Windows.x86_64.exe";
    }
    static std::string windowsX86() {
        return std::string(TEST_DATA_DIR) + "/basic_binary.Windows.x86.exe";
    }
};

// ---------------------------------------------------------------------------
// Error handling
// ---------------------------------------------------------------------------

TEST_F(SegmentAPITest, SegmentErrorHandling) {
    auto result = segment("/nonexistent");
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), SegmentError::FileNotFound);
}

// ---------------------------------------------------------------------------
// Full pipeline tests per platform
// ---------------------------------------------------------------------------

TEST_F(SegmentAPITest, LinuxX86_64FullPipeline) {
    auto result = segment(linuxX86_64());
    ASSERT_TRUE(result.has_value())
        << "segment() failed: "
        << VMPilot::SDK::Segmentator::to_string(result.error());

    const auto& groups = result->groups;
    EXPECT_GE(groups.size(), 1u) << "Expected at least one region group";

    // Regions should have human-readable names (not fallback)
    EXPECT_TRUE(hasNamedRegion(groups))
        << "Expected at least one group with a non-fallback source_name";

    // Enclosing symbols should be populated for ELF
    EXPECT_TRUE(hasEnclosingSymbol(groups))
        << "Expected at least one site with an enclosing_symbol (ELF)";
}

TEST_F(SegmentAPITest, LinuxX86FullPipeline) {
    auto result = segment(linuxX86());
    ASSERT_TRUE(result.has_value())
        << "segment() failed: "
        << VMPilot::SDK::Segmentator::to_string(result.error());

    const auto& groups = result->groups;
    EXPECT_GE(groups.size(), 1u) << "Expected at least one region group";

    EXPECT_TRUE(hasNamedRegion(groups))
        << "Expected at least one group with a non-fallback source_name";

    EXPECT_TRUE(hasEnclosingSymbol(groups))
        << "Expected at least one site with an enclosing_symbol (ELF)";
}

TEST_F(SegmentAPITest, DarwinARM64FullPipeline) {
    auto result = segment(darwinARM64());
    ASSERT_TRUE(result.has_value())
        << "segment() failed: "
        << VMPilot::SDK::Segmentator::to_string(result.error());

    const auto& groups = result->groups;
    EXPECT_GE(groups.size(), 1u) << "Expected at least one region group";

    EXPECT_TRUE(hasNamedRegion(groups))
        << "Expected at least one group with a non-fallback source_name";

    // Mach-O should have enclosing symbols too
    EXPECT_TRUE(hasEnclosingSymbol(groups))
        << "Expected at least one site with an enclosing_symbol (Mach-O)";
}

TEST_F(SegmentAPITest, WindowsX86_64FullPipeline) {
    auto result = segment(windowsX86_64());
    ASSERT_TRUE(result.has_value())
        << "segment() failed: "
        << VMPilot::SDK::Segmentator::to_string(result.error());

    const auto& groups = result->groups;
    EXPECT_GE(groups.size(), 1u) << "Expected at least one region group";

    EXPECT_TRUE(hasNamedRegion(groups))
        << "Expected at least one group with a non-fallback source_name";
}

TEST_F(SegmentAPITest, WindowsX86FullPipeline) {
    auto result = segment(windowsX86());
    ASSERT_TRUE(result.has_value())
        << "segment() failed: "
        << VMPilot::SDK::Segmentator::to_string(result.error());

    const auto& groups = result->groups;
    EXPECT_GE(groups.size(), 1u) << "Expected at least one region group";

    EXPECT_TRUE(hasNamedRegion(groups))
        << "Expected at least one group with a non-fallback source_name";
}

// ---------------------------------------------------------------------------
// Cross-platform canonical site detection
// ---------------------------------------------------------------------------

TEST_F(SegmentAPITest, AllPlatformsHaveCanonicalSites) {
    struct PlatformCase {
        std::string path;
        std::string label;
    };
    std::vector<PlatformCase> platforms = {
        {linuxX86_64(), "Linux.x86_64"},
        {linuxX86(), "Linux.x86"},
        {darwinARM64(), "Darwin.arm64"},
        {windowsX86_64(), "Windows.x86_64"},
        {windowsX86(), "Windows.x86"},
    };

    for (const auto& p : platforms) {
        auto result = segment(p.path);
        ASSERT_TRUE(result.has_value())
            << p.label << ": segment() failed: "
            << VMPilot::SDK::Segmentator::to_string(result.error());

        EXPECT_TRUE(hasCanonicalSite(result->groups))
            << p.label
            << ": expected at least one group with is_canonical == true";
    }
}

// ---------------------------------------------------------------------------
// Enclosing symbol presence (ELF and Mach-O)
// ---------------------------------------------------------------------------

TEST_F(SegmentAPITest, EnclosingSymbolPresent) {
    struct PlatformCase {
        std::string path;
        std::string label;
    };
    // ELF and Mach-O binaries should have enclosing symbols
    std::vector<PlatformCase> platforms = {
        {linuxX86_64(), "Linux.x86_64"},
        {linuxX86(), "Linux.x86"},
        {darwinARM64(), "Darwin.arm64"},
    };

    for (const auto& p : platforms) {
        auto result = segment(p.path);
        ASSERT_TRUE(result.has_value())
            << p.label << ": segment() failed: "
            << VMPilot::SDK::Segmentator::to_string(result.error());

        EXPECT_TRUE(hasEnclosingSymbol(result->groups))
            << p.label
            << ": expected at least one site with enclosing_symbol populated";
    }
}
