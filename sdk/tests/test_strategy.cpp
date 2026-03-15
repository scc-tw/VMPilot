#include <Strategy.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::Segmentator;

TEST(FileHandlerStrategy, DefaultGetBeginEndAddr) {
    FileHandlerStrategy handler;
    auto [begin, end] = handler.getBeginEndAddr();
    EXPECT_EQ(begin, static_cast<uint64_t>(-1));
    EXPECT_EQ(end, static_cast<uint64_t>(-1));
}

TEST(FileHandlerStrategy, DefaultGetTextSection) {
    FileHandlerStrategy handler;
    auto text = handler.getTextSection();
    EXPECT_TRUE(text.empty());
}

TEST(FileHandlerStrategy, DefaultGetTextBaseAddr) {
    FileHandlerStrategy handler;
    auto base = handler.getTextBaseAddr();
    EXPECT_EQ(base, static_cast<uint64_t>(-1));
}

TEST(FileHandlerStrategy, DefaultGetNativeSymbolTable) {
    FileHandlerStrategy handler;
    auto table = handler.getNativeSymbolTable();
    EXPECT_TRUE(table.empty());
}

TEST(ArchHandlerStrategy, DefaultConstructor) {
    ArchHandlerStrategy handler;
    // Default is X86 MODE_64
    std::vector<uint8_t> code = {0x90};
    EXPECT_FALSE(handler.Load(code, 0x1000));
}

TEST(ArchHandlerStrategy, DefaultGetNativeFunctions) {
    ArchHandlerStrategy handler;
    auto functions = handler.getNativeFunctions();
    EXPECT_TRUE(functions.empty());
}
