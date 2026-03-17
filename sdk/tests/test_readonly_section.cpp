#include <ReadOnlySection.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::Segmentator;

TEST(ReadOnlySection, ContainsValidAddress) {
    ReadOnlySection section;
    section.base_addr = 0x1000;
    section.data = {0x41, 0x42, 0x43, 0x00};  // "ABC\0"

    EXPECT_TRUE(section.contains(0x1000));
    EXPECT_TRUE(section.contains(0x1003));
    EXPECT_FALSE(section.contains(0x1004));  // past end
    EXPECT_FALSE(section.contains(0x0FFF));  // before start
}

TEST(ReadOnlySection, ContainsEmptySection) {
    ReadOnlySection section;
    section.base_addr = 0x1000;
    // data is empty
    EXPECT_FALSE(section.contains(0x1000));
}

TEST(ReadOnlySection, ContainsSentinelBaseAddr) {
    ReadOnlySection section;
    // base_addr defaults to -1 (sentinel)
    section.data = {0x41};
    EXPECT_FALSE(section.contains(0x0));
    EXPECT_FALSE(section.contains(static_cast<uint64_t>(-1)));
}

TEST(ReadOnlySection, ReadCStringBasic) {
    ReadOnlySection section;
    section.base_addr = 0x2000;
    // "hello\0world\0"
    section.data = {'h', 'e', 'l', 'l', 'o', '\0',
                    'w', 'o', 'r', 'l', 'd', '\0'};

    EXPECT_EQ(section.readCString(0x2000), "hello");
    EXPECT_EQ(section.readCString(0x2006), "world");
}

TEST(ReadOnlySection, ReadCStringAtOffset) {
    ReadOnlySection section;
    section.base_addr = 0x3000;
    section.data = {'a', 'b', 'c', '\0', 'x', 'y', '\0'};

    EXPECT_EQ(section.readCString(0x3000), "abc");
    EXPECT_EQ(section.readCString(0x3001), "bc");
    EXPECT_EQ(section.readCString(0x3003), "");  // points at null
    EXPECT_EQ(section.readCString(0x3004), "xy");
}

TEST(ReadOnlySection, ReadCStringOutOfRange) {
    ReadOnlySection section;
    section.base_addr = 0x4000;
    section.data = {'a', '\0'};

    EXPECT_EQ(section.readCString(0x3FFF), "");  // before section
    EXPECT_EQ(section.readCString(0x4002), "");  // past end
}

TEST(ReadOnlySection, ReadCStringNoNullTerminator) {
    ReadOnlySection section;
    section.base_addr = 0x5000;
    section.data = {'a', 'b', 'c'};  // no null terminator

    // Should read up to end of data
    EXPECT_EQ(section.readCString(0x5000), "abc");
}

TEST(ReadOnlySection, ReadCStringEmpty) {
    ReadOnlySection section;
    section.base_addr = 0x6000;
    section.data = {'\0', 'a'};

    EXPECT_EQ(section.readCString(0x6000), "");  // null at start
    EXPECT_EQ(section.readCString(0x6001), "a");
}
