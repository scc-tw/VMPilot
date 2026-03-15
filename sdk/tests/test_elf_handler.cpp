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

    // Should contain VMPilot_Begin and VMPilot_End entries
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

TEST_F(ELFHandlerX64Test, SymbolTableHasPLTEntries) {
    auto table = handler.getNativeSymbolTable();
    bool has_plt = false;
    for (const auto& entry : table) {
        auto it = entry.additionalAttributes.find("entry_type");
        if (it != entry.additionalAttributes.end()) {
            try {
                if (std::get<std::string>(it->second) == "PLT") {
                    has_plt = true;
                    break;
                }
            } catch (...) {
            }
        }
    }
    EXPECT_TRUE(has_plt);
}

TEST_F(ELFHandlerX64Test, SymbolTableHasGOTEntries) {
    auto table = handler.getNativeSymbolTable();
    bool has_got = false;
    for (const auto& entry : table) {
        auto it = entry.additionalAttributes.find("entry_type");
        if (it != entry.additionalAttributes.end()) {
            try {
                if (std::get<std::string>(it->second) == "GOT") {
                    has_got = true;
                    break;
                }
            } catch (...) {
            }
        }
    }
    EXPECT_TRUE(has_got);
}

TEST_F(ELFHandlerX64Test, PLTAddrMatchesBeginEndAddr) {
    auto [begin, end] = handler.getBeginEndAddr();
    auto table = handler.getNativeSymbolTable();

    bool begin_found = false, end_found = false;
    for (const auto& entry : table) {
        auto it = entry.additionalAttributes.find("entry_type");
        if (it == entry.additionalAttributes.end()) continue;
        try {
            if (std::get<std::string>(it->second) != "PLT") continue;
        } catch (...) {
            continue;
        }
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
