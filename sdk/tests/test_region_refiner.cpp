#include <NativeFunctionBase.hpp>
#include <RegionRefiner.hpp>

#include <cassert>
#include <cstdio>
#include <memory>
#include <vector>

using NativeFunc = VMPilot::SDK::Segmentator::NativeFunctionBase;
using VMPilot::SDK::RegionRefiner::refine;

static std::unique_ptr<NativeFunc> make_region(uint64_t addr, uint64_t size,
                                               const std::string& name = "") {
    std::vector<uint8_t> code(size, 0x90);  // fill with NOP
    return std::make_unique<NativeFunc>(addr, size,
                                       name.empty() ? "r_" + std::to_string(addr) : name,
                                       std::move(code));
}

static void test_no_overlap() {
    std::printf("  test_no_overlap... ");
    std::vector<std::unique_ptr<NativeFunc>> regions;
    regions.push_back(make_region(0x1000, 20));
    regions.push_back(make_region(0x2000, 30));
    regions.push_back(make_region(0x3000, 10));

    auto result = refine(std::move(regions));
    assert(result.size() == 3);
    assert(result[0]->getAddr() == 0x1000);
    assert(result[1]->getAddr() == 0x2000);
    assert(result[2]->getAddr() == 0x3000);
    std::printf("PASS\n");
}

static void test_full_containment() {
    // outer: [0x1000, 0x1064)  inner: [0x1020, 0x1040)
    std::printf("  test_full_containment... ");
    std::vector<std::unique_ptr<NativeFunc>> regions;
    regions.push_back(make_region(0x1000, 100));  // outer
    regions.push_back(make_region(0x1020, 32));   // inner — should be dropped

    auto result = refine(std::move(regions));
    assert(result.size() == 1);
    assert(result[0]->getAddr() == 0x1000);
    assert(result[0]->getSize() == 100);
    std::printf("PASS\n");
}

static void test_containment_reverse_order() {
    // Same as above but input order is inner first
    std::printf("  test_containment_reverse_order... ");
    std::vector<std::unique_ptr<NativeFunc>> regions;
    regions.push_back(make_region(0x1020, 32));   // inner first
    regions.push_back(make_region(0x1000, 100));  // outer second

    auto result = refine(std::move(regions));
    assert(result.size() == 1);
    assert(result[0]->getAddr() == 0x1000);
    assert(result[0]->getSize() == 100);
    std::printf("PASS\n");
}

static void test_partial_overlap() {
    // A: [0x1000, 0x1030)  B: [0x1020, 0x1050)
    std::printf("  test_partial_overlap... ");
    std::vector<std::unique_ptr<NativeFunc>> regions;
    regions.push_back(make_region(0x1000, 0x30));
    regions.push_back(make_region(0x1020, 0x30));

    auto result = refine(std::move(regions));
    assert(result.size() == 1);
    assert(result[0]->getAddr() == 0x1000);
    assert(result[0]->getSize() == 0x50);
    assert(result[0]->getCode().size() == 0x50);
    std::printf("PASS\n");
}

static void test_duplicate_address() {
    std::printf("  test_duplicate_address... ");
    std::vector<std::unique_ptr<NativeFunc>> regions;
    regions.push_back(make_region(0x1000, 20));
    regions.push_back(make_region(0x1000, 20));

    auto result = refine(std::move(regions));
    assert(result.size() == 1);
    std::printf("PASS\n");
}

static void test_multiple_nested() {
    // outer: [0x1000, 0x1100)
    // inner1: [0x1020, 0x1040)
    // inner2: [0x1060, 0x1080)
    // separate: [0x2000, 0x2020)
    std::printf("  test_multiple_nested... ");
    std::vector<std::unique_ptr<NativeFunc>> regions;
    regions.push_back(make_region(0x1060, 0x20));
    regions.push_back(make_region(0x2000, 0x20));
    regions.push_back(make_region(0x1000, 0x100));
    regions.push_back(make_region(0x1020, 0x20));

    auto result = refine(std::move(regions));
    assert(result.size() == 2);
    assert(result[0]->getAddr() == 0x1000);
    assert(result[0]->getSize() == 0x100);
    assert(result[1]->getAddr() == 0x2000);
    assert(result[1]->getSize() == 0x20);
    std::printf("PASS\n");
}

static void test_empty() {
    std::printf("  test_empty... ");
    std::vector<std::unique_ptr<NativeFunc>> regions;
    auto result = refine(std::move(regions));
    assert(result.empty());
    std::printf("PASS\n");
}

static void test_single() {
    std::printf("  test_single... ");
    std::vector<std::unique_ptr<NativeFunc>> regions;
    regions.push_back(make_region(0x1000, 50));
    auto result = refine(std::move(regions));
    assert(result.size() == 1);
    std::printf("PASS\n");
}

int main() {
    std::printf("=== RegionRefiner tests ===\n");
    test_empty();
    test_single();
    test_no_overlap();
    test_full_containment();
    test_containment_reverse_order();
    test_partial_overlap();
    test_duplicate_address();
    test_multiple_nested();
    std::printf("All tests passed.\n");
    return 0;
}
