#include <NativeSymbolTable.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::Segmentator;

TEST(NativeSymbolTableEntry, DefaultConstruction) {
    NativeSymbolTableEntry entry;
    EXPECT_EQ(entry.name, "");
    EXPECT_EQ(entry.address, 0u);
    EXPECT_EQ(entry.size, 0u);
    EXPECT_EQ(entry.type, SymbolType::NOTYPE);
    EXPECT_FALSE(entry.isGlobal);
}

TEST(NativeSymbolTableEntry, SetAndGetAttribute) {
    NativeSymbolTableEntry entry;
    entry.setAttribute("entry_type", std::string("PLT"));
    EXPECT_EQ(entry.getAttribute<std::string>("entry_type", ""), "PLT");
}

TEST(NativeSymbolTableEntry, GetAttributeDefault) {
    NativeSymbolTableEntry entry;
    EXPECT_EQ(entry.getAttribute<int>("nonexistent", -1), -1);
    EXPECT_EQ(entry.getAttribute<std::string>("nonexistent", "default"),
              "default");
}

TEST(NativeSymbolTableEntry, GetAttributeTypeMismatch) {
    NativeSymbolTableEntry entry;
    entry.setAttribute("key", 42);
    // Asking for string when it's int should return default
    EXPECT_EQ(entry.getAttribute<std::string>("key", "fallback"), "fallback");
}

TEST(NativeSymbolTableEntry, SectionNumber) {
    NativeSymbolTableEntry entry;
    entry.setSectionNumber(5);
    EXPECT_EQ(entry.getSectionNumber(), 5);
}

TEST(NativeSymbolTableEntry, StorageClass) {
    NativeSymbolTableEntry entry;
    entry.setStorageClass(2);
    EXPECT_EQ(entry.getStorageClass(), 2);
}

TEST(NativeSymbolTableEntry, AuxSymbols) {
    NativeSymbolTableEntry entry;
    EXPECT_EQ(entry.getNumberOfAuxSymbols(), 0);
    entry.setNumberOfAuxSymbols(3);
    EXPECT_EQ(entry.getNumberOfAuxSymbols(), 3);
}

TEST(NativeSymbolTable, IsVector) {
    NativeSymbolTable table;
    EXPECT_TRUE(table.empty());

    NativeSymbolTableEntry entry;
    entry.name = "test";
    entry.address = 0x1000;
    table.push_back(entry);

    EXPECT_EQ(table.size(), 1u);
    EXPECT_EQ(table[0].name, "test");
}
