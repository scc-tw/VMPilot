#include <NativeFunctionBase.hpp>

#include <gtest/gtest.h>

using VMPilot::SDK::Segmentator::NativeFunctionBase;

TEST(NativeFunctionBase, ConstructAndGetters) {
    std::vector<uint8_t> code = {0x90, 0xCC, 0xC3};
    NativeFunctionBase nf(0x1000, 3, "test_func", code);

    EXPECT_EQ(nf.getAddr(), 0x1000u);
    EXPECT_EQ(nf.getSize(), 3u);
    EXPECT_EQ(nf.getName(), "test_func");
    EXPECT_EQ(nf.getCode(), code);
}

TEST(NativeFunctionBase, CopyConstructor) {
    std::vector<uint8_t> code = {0x90, 0x90};
    NativeFunctionBase original(0x2000, 2, "orig", code);
    NativeFunctionBase copy(original);

    EXPECT_EQ(copy.getAddr(), original.getAddr());
    EXPECT_EQ(copy.getSize(), original.getSize());
    EXPECT_EQ(copy.getName(), original.getName());
    EXPECT_EQ(copy.getCode(), original.getCode());
}

TEST(NativeFunctionBase, MoveConstructor) {
    std::vector<uint8_t> code = {0xC3};
    NativeFunctionBase original(0x3000, 1, "movable", code);
    NativeFunctionBase moved(std::move(original));

    EXPECT_EQ(moved.getAddr(), 0x3000u);
    EXPECT_EQ(moved.getName(), "movable");
    EXPECT_EQ(moved.getCode(), code);
}

TEST(NativeFunctionBase, CopyAssignment) {
    std::vector<uint8_t> code1 = {0x90};
    std::vector<uint8_t> code2 = {0xCC, 0xC3};
    NativeFunctionBase a(0x1000, 1, "a", code1);
    NativeFunctionBase b(0x2000, 2, "b", code2);

    a = b;
    EXPECT_EQ(a.getAddr(), 0x2000u);
    EXPECT_EQ(a.getName(), "b");
    EXPECT_EQ(a.getCode(), code2);
}

TEST(NativeFunctionBase, MoveAssignment) {
    std::vector<uint8_t> code = {0xCC};
    NativeFunctionBase a(0x1000, 1, "a", {0x90});
    NativeFunctionBase b(0x2000, 1, "b", code);

    a = std::move(b);
    EXPECT_EQ(a.getAddr(), 0x2000u);
    EXPECT_EQ(a.getName(), "b");
}

TEST(NativeFunctionBase, SelfAssignment) {
    std::vector<uint8_t> code = {0x90, 0xC3};
    NativeFunctionBase nf(0x1000, 2, "self", code);
    nf = nf;
    EXPECT_EQ(nf.getName(), "self");
    EXPECT_EQ(nf.getCode(), code);
}

TEST(NativeFunctionBase, EmptyCode) {
    NativeFunctionBase nf(0, 0, "", {});
    EXPECT_TRUE(nf.getCode().empty());
    EXPECT_TRUE(nf.getName().empty());
}
