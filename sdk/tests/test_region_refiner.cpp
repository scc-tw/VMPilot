#include <NativeFunctionBase.hpp>
#include <RegionRefiner.hpp>

#include <gtest/gtest.h>

using NativeFunc = VMPilot::SDK::Segmentator::NativeFunctionBase;
using VMPilot::SDK::RegionRefiner::refine;

static NativeFunc make_region(uint64_t addr, uint64_t size,
                              const std::string& name = "") {
    std::vector<uint8_t> code(size, 0x90);
    return NativeFunc(
        addr, size, name.empty() ? "r_" + std::to_string(addr) : name,
        std::move(code));
}

TEST(RegionRefiner, Empty) {
    std::vector<NativeFunc> regions;
    auto result = refine(std::move(regions));
    EXPECT_TRUE(result.empty());
}

TEST(RegionRefiner, Single) {
    std::vector<NativeFunc> regions;
    regions.push_back(make_region(0x1000, 50));
    auto result = refine(std::move(regions));
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].getAddr(), 0x1000u);
}

TEST(RegionRefiner, NoOverlap) {
    std::vector<NativeFunc> regions;
    regions.push_back(make_region(0x1000, 20));
    regions.push_back(make_region(0x2000, 30));
    regions.push_back(make_region(0x3000, 10));
    auto result = refine(std::move(regions));
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0].getAddr(), 0x1000u);
    EXPECT_EQ(result[1].getAddr(), 0x2000u);
    EXPECT_EQ(result[2].getAddr(), 0x3000u);
}

TEST(RegionRefiner, FullContainment) {
    std::vector<NativeFunc> regions;
    regions.push_back(make_region(0x1000, 100));
    regions.push_back(make_region(0x1020, 32));
    auto result = refine(std::move(regions));
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].getAddr(), 0x1000u);
    EXPECT_EQ(result[0].getSize(), 100u);
}

TEST(RegionRefiner, ContainmentReverseOrder) {
    std::vector<NativeFunc> regions;
    regions.push_back(make_region(0x1020, 32));
    regions.push_back(make_region(0x1000, 100));
    auto result = refine(std::move(regions));
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].getAddr(), 0x1000u);
    EXPECT_EQ(result[0].getSize(), 100u);
}

TEST(RegionRefiner, PartialOverlap) {
    std::vector<NativeFunc> regions;
    regions.push_back(make_region(0x1000, 0x30));
    regions.push_back(make_region(0x1020, 0x30));
    auto result = refine(std::move(regions));
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].getAddr(), 0x1000u);
    EXPECT_EQ(result[0].getSize(), 0x50u);
    EXPECT_EQ(result[0].getCode().size(), 0x50u);
}

TEST(RegionRefiner, DuplicateAddress) {
    std::vector<NativeFunc> regions;
    regions.push_back(make_region(0x1000, 20));
    regions.push_back(make_region(0x1000, 20));
    auto result = refine(std::move(regions));
    ASSERT_EQ(result.size(), 1u);
}

TEST(RegionRefiner, MultipleNested) {
    std::vector<NativeFunc> regions;
    regions.push_back(make_region(0x1060, 0x20));
    regions.push_back(make_region(0x2000, 0x20));
    regions.push_back(make_region(0x1000, 0x100));
    regions.push_back(make_region(0x1020, 0x20));
    auto result = refine(std::move(regions));
    ASSERT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0].getAddr(), 0x1000u);
    EXPECT_EQ(result[0].getSize(), 0x100u);
    EXPECT_EQ(result[1].getAddr(), 0x2000u);
}
