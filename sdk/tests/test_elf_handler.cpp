#include <ELFHandler.hpp>
#include <file_type_parser.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::Segmentator;

class ELFHandlerX64Test : public ::testing::Test {
   protected:
    ELFFileHandlerStrategy handler{std::string(TEST_DATA_DIR) +
                                   "/basic_binary.Linux.x86_64"};
};

class ELFHandlerX86Test : public ::testing::Test {
   protected:
    ELFFileHandlerStrategy handler{std::string(TEST_DATA_DIR) +
                                   "/basic_binary.Linux.x86"};
};

// --- Helper to find entry_type attribute ---
static std::string getEntryType(const NativeSymbolTableEntry& entry) {
    auto it = entry.additionalAttributes.find("entry_type");
    if (it == entry.additionalAttributes.end()) return "";
    try {
        return std::get<std::string>(it->second);
    } catch (...) {
        return "";
    }
}

// --- x86_64 tests ---

TEST_F(ELFHandlerX64Test, GetBeginEndAddr) {
    auto [begin, end] = handler.getBeginEndAddr();
    EXPECT_NE(begin, static_cast<uint64_t>(-1));
    EXPECT_NE(end, static_cast<uint64_t>(-1));
    EXPECT_NE(begin, end);
}

TEST_F(ELFHandlerX64Test, GetTextSection) {
    auto text = handler.getTextSection();
    EXPECT_FALSE(text.empty());
}

TEST_F(ELFHandlerX64Test, GetTextBaseAddr) {
    auto base = handler.getTextBaseAddr();
    EXPECT_NE(base, static_cast<uint64_t>(-1));
    EXPECT_GT(base, 0u);
}

TEST_F(ELFHandlerX64Test, GetNativeSymbolTable) {
    auto table = handler.getNativeSymbolTable();
    EXPECT_FALSE(table.empty());

    bool found_begin = false, found_end = false;
    for (const auto& entry : table) {
        if (entry.name.find("VMPilot_Begin") != std::string::npos)
            found_begin = true;
        if (entry.name.find("VMPilot_End") != std::string::npos)
            found_end = true;
    }
    EXPECT_TRUE(found_begin);
    EXPECT_TRUE(found_end);
}

TEST_F(ELFHandlerX64Test, SymbolTableHasDirectEntries) {
    auto table = handler.getNativeSymbolTable();
    bool found = false;
    for (const auto& entry : table) {
        if (getEntryType(entry) == "direct") {
            found = true;
            EXPECT_EQ(entry.type, SymbolType::FUNC);
            EXPECT_GT(entry.address, 0u);
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(ELFHandlerX64Test, SymbolTableHasIndirectEntries) {
    auto table = handler.getNativeSymbolTable();
    bool found = false;
    for (const auto& entry : table) {
        if (getEntryType(entry) == "indirect") {
            found = true;
            EXPECT_EQ(entry.type, SymbolType::OBJECT);
            EXPECT_GT(entry.address, 0u);
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(ELFHandlerX64Test, DirectCallTargetMatchesBeginEndAddr) {
    auto [begin, end] = handler.getBeginEndAddr();
    auto table = handler.getNativeSymbolTable();

    bool begin_found = false, end_found = false;
    for (const auto& entry : table) {
        if (getEntryType(entry) != "direct") continue;
        if (entry.name.find("VMPilot_Begin") != std::string::npos &&
            entry.address == begin)
            begin_found = true;
        if (entry.name.find("VMPilot_End") != std::string::npos &&
            entry.address == end)
            end_found = true;
    }
    EXPECT_TRUE(begin_found);
    EXPECT_TRUE(end_found);
}

TEST_F(ELFHandlerX64Test, DirectAndIndirectCountsMatch) {
    auto table = handler.getNativeSymbolTable();
    size_t direct_count = 0, indirect_count = 0;
    for (const auto& entry : table) {
        auto t = getEntryType(entry);
        if (t == "direct") ++direct_count;
        if (t == "indirect") ++indirect_count;
    }
    // Every PLT entry should have a matching GOT entry
    EXPECT_EQ(direct_count, indirect_count);
}

// --- x86 tests ---

TEST_F(ELFHandlerX86Test, GetBeginEndAddr) {
    auto [begin, end] = handler.getBeginEndAddr();
    EXPECT_NE(begin, static_cast<uint64_t>(-1));
    EXPECT_NE(end, static_cast<uint64_t>(-1));
}

TEST_F(ELFHandlerX86Test, GetTextSection) {
    auto text = handler.getTextSection();
    EXPECT_FALSE(text.empty());
}

TEST_F(ELFHandlerX86Test, GetNativeSymbolTable) {
    auto table = handler.getNativeSymbolTable();
    EXPECT_FALSE(table.empty());
}

TEST_F(ELFHandlerX86Test, HasDirectAndIndirectEntries) {
    auto table = handler.getNativeSymbolTable();
    bool has_direct = false, has_indirect = false;
    for (const auto& entry : table) {
        auto t = getEntryType(entry);
        if (t == "direct") has_direct = true;
        if (t == "indirect") has_indirect = true;
    }
    EXPECT_TRUE(has_direct);
    EXPECT_TRUE(has_indirect);
}
