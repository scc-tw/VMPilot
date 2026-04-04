#include <Section.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::Core;

// --- contains() ---

TEST(Section, ContainsValidAddress) {
    Section section;
    section.base_addr = 0x1000;
    section.size = 4;
    section.data = {0x41, 0x42, 0x43, 0x00};

    EXPECT_TRUE(section.contains(0x1000));
    EXPECT_TRUE(section.contains(0x1003));
    EXPECT_FALSE(section.contains(0x1004));
    EXPECT_FALSE(section.contains(0x0FFF));
}

TEST(Section, ContainsEmptySection) {
    Section section;
    section.base_addr = 0x1000;
    section.size = 0;
    EXPECT_FALSE(section.contains(0x1000));
}

TEST(Section, ContainsBssSection) {
    Section section;
    section.base_addr = 0x1000;
    section.size = 0x100;
    section.kind = SectionKind::Bss;
    // .bss has size but no data
    EXPECT_TRUE(section.contains(0x1000));
    EXPECT_TRUE(section.contains(0x10FF));
    EXPECT_FALSE(section.contains(0x1100));
    EXPECT_FALSE(section.has_data());
}

// --- readCString() ---

TEST(Section, ReadCStringBasic) {
    Section section;
    section.base_addr = 0x2000;
    section.data = {'h', 'e', 'l', 'l', 'o', '\0',
                    'w', 'o', 'r', 'l', 'd', '\0'};
    section.size = section.data.size();

    EXPECT_EQ(section.readCString(0x2000), "hello");
    EXPECT_EQ(section.readCString(0x2006), "world");
}

TEST(Section, ReadCStringAtOffset) {
    Section section;
    section.base_addr = 0x3000;
    section.data = {'a', 'b', 'c', '\0', 'x', 'y', '\0'};
    section.size = section.data.size();

    EXPECT_EQ(section.readCString(0x3000), "abc");
    EXPECT_EQ(section.readCString(0x3001), "bc");
    EXPECT_EQ(section.readCString(0x3003), "");
    EXPECT_EQ(section.readCString(0x3004), "xy");
}

TEST(Section, ReadCStringOutOfRange) {
    Section section;
    section.base_addr = 0x4000;
    section.data = {'a', '\0'};
    section.size = section.data.size();

    EXPECT_EQ(section.readCString(0x3FFF), "");
    EXPECT_EQ(section.readCString(0x4002), "");
}

TEST(Section, ReadCStringNoNullTerminator) {
    Section section;
    section.base_addr = 0x5000;
    section.data = {'a', 'b', 'c'};
    section.size = section.data.size();

    EXPECT_EQ(section.readCString(0x5000), "abc");
}

TEST(Section, ReadCStringNoData) {
    Section section;
    section.base_addr = 0x6000;
    section.size = 0x100;
    // No data loaded (.bss style)
    EXPECT_EQ(section.readCString(0x6000), "");
}

// --- readValue() ---

TEST(Section, ReadValue8) {
    Section section;
    section.base_addr = 0x1000;
    section.data = {0x42, 0xFF};
    section.size = section.data.size();

    auto val = section.readValue(0x1000, 1);
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 0x42u);
}

TEST(Section, ReadValue32) {
    Section section;
    section.base_addr = 0x1000;
    section.data = {0x78, 0x56, 0x34, 0x12};
    section.size = section.data.size();

    auto val = section.readValue(0x1000, 4);
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 0x12345678u);
}

TEST(Section, ReadValue64) {
    Section section;
    section.base_addr = 0x1000;
    section.data = {0xEF, 0xCD, 0xAB, 0x90, 0x78, 0x56, 0x34, 0x12};
    section.size = section.data.size();

    auto val = section.readValue(0x1000, 8);
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 0x1234567890ABCDEFu);
}

TEST(Section, ReadValueOutOfBounds) {
    Section section;
    section.base_addr = 0x1000;
    section.data = {0x42, 0x43};
    section.size = section.data.size();

    EXPECT_FALSE(section.readValue(0x1001, 4).has_value());
    EXPECT_FALSE(section.readValue(0x0FFF, 1).has_value());
    EXPECT_FALSE(section.readValue(0x1002, 1).has_value());
}

TEST(Section, ReadValueInvalidSize) {
    Section section;
    section.base_addr = 0x1000;
    section.data = {0x42, 0x43, 0x44};
    section.size = section.data.size();

    EXPECT_FALSE(section.readValue(0x1000, 3).has_value());
    EXPECT_FALSE(section.readValue(0x1000, 0).has_value());
}

TEST(Section, ReadValueNoData) {
    Section section;
    section.base_addr = 0x1000;
    section.size = 0x100;
    EXPECT_FALSE(section.readValue(0x1000, 4).has_value());
}

// --- is_mutable() ---

TEST(Section, IsMutableData) {
    Section section;
    section.kind = SectionKind::Data;
    EXPECT_TRUE(section.is_mutable());
}

TEST(Section, IsMutableBss) {
    Section section;
    section.kind = SectionKind::Bss;
    EXPECT_TRUE(section.is_mutable());
}

TEST(Section, IsMutableRodata) {
    Section section;
    section.kind = SectionKind::Rodata;
    EXPECT_FALSE(section.is_mutable());
}

TEST(Section, IsMutableText) {
    Section section;
    section.kind = SectionKind::Text;
    EXPECT_FALSE(section.is_mutable());
}
