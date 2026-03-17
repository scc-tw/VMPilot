#include <NativeFunctionBase.hpp>
#include <RegionRefiner.hpp>

#include <gtest/gtest.h>

using NativeFunc = VMPilot::SDK::Segmentator::NativeFunctionBase;
using VMPilot::SDK::RegionRefiner::group;
using VMPilot::SDK::RegionRefiner::ProtectedRegion;

static std::unique_ptr<NativeFunc> make_region(
    uint64_t addr, uint64_t size, const std::string& name,
    const std::optional<std::string>& enclosing = std::nullopt) {
    std::vector<uint8_t> code(size, 0x90);
    auto nf = std::make_unique<NativeFunc>(addr, size, name, std::move(code));
    if (enclosing)
        nf->setEnclosingSymbol(*enclosing);
    return nf;
}

TEST(RegionGrouping, SingleRegionNoEnclosing) {
    std::vector<std::unique_ptr<NativeFunc>> regions;
    regions.push_back(make_region(0x1000, 50, "foo"));

    auto groups = group(regions);
    ASSERT_EQ(groups.size(), 1u);
    EXPECT_EQ(groups[0].source_name, "foo");
    ASSERT_EQ(groups[0].sites.size(), 1u);
    EXPECT_TRUE(groups[0].sites[0].is_canonical);
    EXPECT_EQ(groups[0].canonical_index, 0u);
}

TEST(RegionGrouping, TwoCopiesSameName) {
    std::vector<std::unique_ptr<NativeFunc>> regions;
    regions.push_back(make_region(0x1270, 41, "foo", "_Z3fooi"));
    regions.push_back(make_region(0x10e3, 13, "foo", "main"));

    auto groups = group(regions);
    ASSERT_EQ(groups.size(), 1u);
    EXPECT_EQ(groups[0].source_name, "foo");
    ASSERT_EQ(groups[0].sites.size(), 2u);

    // First site (in _Z3fooi) should be canonical
    EXPECT_TRUE(groups[0].sites[0].is_canonical);
    EXPECT_FALSE(groups[0].sites[1].is_canonical);
    EXPECT_EQ(groups[0].canonical_index, 0u);
}

TEST(RegionGrouping, CanonicalViaDemangle) {
    std::vector<std::unique_ptr<NativeFunc>> regions;
    // Inlined copy first, canonical second — canonical should still be found
    regions.push_back(make_region(0x10e3, 13, "foo", "main"));
    regions.push_back(make_region(0x1270, 41, "foo", "_Z3fooi"));

    auto groups = group(regions);
    ASSERT_EQ(groups.size(), 1u);

    // Second site (index 1, in _Z3fooi) should be canonical
    EXPECT_FALSE(groups[0].sites[0].is_canonical);
    EXPECT_TRUE(groups[0].sites[1].is_canonical);
    EXPECT_EQ(groups[0].canonical_index, 1u);
}

TEST(RegionGrouping, NoMatchFallback) {
    std::vector<std::unique_ptr<NativeFunc>> regions;
    // Both enclosing symbols don't match source_name
    regions.push_back(make_region(0x1000, 20, "bar", "unrelated_func1"));
    regions.push_back(make_region(0x2000, 20, "bar", "unrelated_func2"));

    auto groups = group(regions);
    ASSERT_EQ(groups.size(), 1u);

    // Fallback: first site is canonical
    EXPECT_TRUE(groups[0].sites[0].is_canonical);
    EXPECT_FALSE(groups[0].sites[1].is_canonical);
    EXPECT_EQ(groups[0].canonical_index, 0u);
}

TEST(RegionGrouping, MultipleGroups) {
    std::vector<std::unique_ptr<NativeFunc>> regions;
    // Group "foo": 2 copies
    regions.push_back(make_region(0x1270, 41, "foo", "_Z3fooi"));
    regions.push_back(make_region(0x10e3, 13, "foo", "main"));
    // Group "bar": 1 copy
    regions.push_back(make_region(0x2000, 30, "bar", "_Z3barv"));

    auto groups = group(regions);
    ASSERT_EQ(groups.size(), 2u);

    // Group "foo"
    EXPECT_EQ(groups[0].source_name, "foo");
    ASSERT_EQ(groups[0].sites.size(), 2u);

    // Group "bar"
    EXPECT_EQ(groups[1].source_name, "bar");
    ASSERT_EQ(groups[1].sites.size(), 1u);
    EXPECT_TRUE(groups[1].sites[0].is_canonical);
}
