/// Tests for the segment() top-level API across all supported platforms.

#include <segmentator.hpp>
#include <diagnostic.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <string>

using VMPilot::SDK::Segmentator::segment;
using VMPilot::SDK::Segmentator::SegmentationResult;
using VMPilot::SDK::RegionRefiner::ProtectedRegion;
using VMPilot::SDK::RegionRefiner::RegionSite;
using VMPilot::Common::DiagnosticCode;

class SegmentAPITest : public ::testing::Test {
   protected:
    static const std::vector<ProtectedRegion>& expectSuccess(
        const tl::expected<SegmentationResult, DiagnosticCode>& result,
        const std::string& label) {
        EXPECT_TRUE(result.has_value())
            << label << ": segment() failed with code "
            << static_cast<int>(result.error());
        return result->groups;
    }

    static bool hasNamedRegion(const std::vector<ProtectedRegion>& groups) {
        return std::any_of(groups.begin(), groups.end(),
                           [](const ProtectedRegion& g) {
                               return g.source_name.substr(0, 17) !=
                                      "vmpilot_region_0x";
                           });
    }

    static bool hasCanonicalSite(const std::vector<ProtectedRegion>& groups) {
        for (const auto& g : groups)
            for (const auto& s : g.sites)
                if (s.is_canonical) return true;
        return false;
    }

    static bool hasEnclosingSymbol(const std::vector<ProtectedRegion>& groups) {
        for (const auto& g : groups)
            for (const auto& s : g.sites)
                if (s.enclosing_symbol.has_value() &&
                    !s.enclosing_symbol->empty())
                    return true;
        return false;
    }

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

TEST_F(SegmentAPITest, SegmentErrorHandling) {
    auto result = segment("/nonexistent");
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::FileNotFound);
}

TEST_F(SegmentAPITest, LinuxX86_64FullPipeline) {
    auto result = segment(linuxX86_64());
    ASSERT_TRUE(result.has_value())
        << "segment() failed: code " << static_cast<int>(result.error());

    const auto& groups = result->groups;
    EXPECT_GE(groups.size(), 1u);
    EXPECT_TRUE(hasNamedRegion(groups));
    EXPECT_TRUE(hasEnclosingSymbol(groups));
}

TEST_F(SegmentAPITest, LinuxX86FullPipeline) {
    auto result = segment(linuxX86());
    ASSERT_TRUE(result.has_value())
        << "segment() failed: code " << static_cast<int>(result.error());

    const auto& groups = result->groups;
    EXPECT_GE(groups.size(), 1u);
    EXPECT_TRUE(hasNamedRegion(groups));
    EXPECT_TRUE(hasEnclosingSymbol(groups));
}

TEST_F(SegmentAPITest, DarwinARM64FullPipeline) {
    auto result = segment(darwinARM64());
    ASSERT_TRUE(result.has_value())
        << "segment() failed: code " << static_cast<int>(result.error());

    const auto& groups = result->groups;
    EXPECT_GE(groups.size(), 1u);
    EXPECT_TRUE(hasNamedRegion(groups));
    EXPECT_TRUE(hasEnclosingSymbol(groups));
}

TEST_F(SegmentAPITest, WindowsX86_64FullPipeline) {
    auto result = segment(windowsX86_64());
    ASSERT_TRUE(result.has_value())
        << "segment() failed: code " << static_cast<int>(result.error());

    const auto& groups = result->groups;
    EXPECT_GE(groups.size(), 1u);
    EXPECT_TRUE(hasNamedRegion(groups));
}

TEST_F(SegmentAPITest, WindowsX86FullPipeline) {
    auto result = segment(windowsX86());
    ASSERT_TRUE(result.has_value())
        << "segment() failed: code " << static_cast<int>(result.error());

    const auto& groups = result->groups;
    EXPECT_GE(groups.size(), 1u);
    EXPECT_TRUE(hasNamedRegion(groups));
}

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
            << p.label << ": segment() failed";

        EXPECT_TRUE(hasCanonicalSite(result->groups))
            << p.label << ": expected canonical site";
    }
}

TEST_F(SegmentAPITest, EnclosingSymbolPresent) {
    struct PlatformCase {
        std::string path;
        std::string label;
    };
    std::vector<PlatformCase> platforms = {
        {linuxX86_64(), "Linux.x86_64"},
        {linuxX86(), "Linux.x86"},
        {darwinARM64(), "Darwin.arm64"},
    };

    for (const auto& p : platforms) {
        auto result = segment(p.path);
        ASSERT_TRUE(result.has_value())
            << p.label << ": segment() failed";

        EXPECT_TRUE(hasEnclosingSymbol(result->groups))
            << p.label << ": expected enclosing symbol";
    }
}
