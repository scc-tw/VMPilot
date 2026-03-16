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
    if (it == entry.additionalAttributes.end())
        return "";
    try {
        return std::get<std::string>(it->second);
    } catch (...) {
        return "";
    }
}

// --- x86_64 tests ---

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

TEST_F(ELFHandlerX64Test, SymbolTableHasStubEntries) {
    auto table = handler.getNativeSymbolTable();
    bool found = false;
    for (const auto& entry : table) {
        if (getEntryType(entry) == "stub") {
            found = true;
            EXPECT_EQ(entry.type, SymbolType::FUNC);
            EXPECT_GT(entry.address, 0u);
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(ELFHandlerX64Test, SymbolTableHasPointerTableEntries) {
    auto table = handler.getNativeSymbolTable();
    bool found = false;
    for (const auto& entry : table) {
        if (getEntryType(entry) == "pointer_table") {
            found = true;
            EXPECT_EQ(entry.type, SymbolType::OBJECT);
            EXPECT_GT(entry.address, 0u);
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(ELFHandlerX64Test, StubEntriesHaveVMPilotMarkers) {
    auto table = handler.getNativeSymbolTable();

    bool begin_found = false, end_found = false;
    for (const auto& entry : table) {
        if (getEntryType(entry) != "stub")
            continue;
        if (entry.name.find("VMPilot_Begin") != std::string::npos) {
            begin_found = true;
            EXPECT_GT(entry.address, 0u);
        }
        if (entry.name.find("VMPilot_End") != std::string::npos) {
            end_found = true;
            EXPECT_GT(entry.address, 0u);
        }
    }
    EXPECT_TRUE(begin_found);
    EXPECT_TRUE(end_found);
}

TEST_F(ELFHandlerX64Test, StubAndPointerTableCountsMatch) {
    auto table = handler.getNativeSymbolTable();
    size_t stub_count = 0, pointer_table_count = 0;
    for (const auto& entry : table) {
        auto t = getEntryType(entry);
        if (t == "stub")
            ++stub_count;
        if (t == "pointer_table")
            ++pointer_table_count;
    }
    // Every PLT entry should have a matching GOT entry
    EXPECT_EQ(stub_count, pointer_table_count);
}

// --- x86 tests ---

TEST_F(ELFHandlerX86Test, GetTextSection) {
    auto text = handler.getTextSection();
    EXPECT_FALSE(text.empty());
}

TEST_F(ELFHandlerX86Test, GetTextBaseAddr) {
    auto base = handler.getTextBaseAddr();
    EXPECT_NE(base, static_cast<uint64_t>(-1));
    EXPECT_GT(base, 0u);
}

TEST_F(ELFHandlerX86Test, GetNativeSymbolTable) {
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

TEST_F(ELFHandlerX86Test, HasStubAndPointerTableEntries) {
    auto table = handler.getNativeSymbolTable();
    bool has_stub = false, has_pointer_table = false;
    for (const auto& entry : table) {
        auto t = getEntryType(entry);
        if (t == "stub")
            has_stub = true;
        if (t == "pointer_table")
            has_pointer_table = true;
    }
    EXPECT_TRUE(has_stub);
    EXPECT_TRUE(has_pointer_table);
}

TEST_F(ELFHandlerX86Test, StubEntriesHaveVMPilotMarkers) {
    auto table = handler.getNativeSymbolTable();

    bool begin_found = false, end_found = false;
    for (const auto& entry : table) {
        if (getEntryType(entry) != "stub")
            continue;
        if (entry.name.find("VMPilot_Begin") != std::string::npos) {
            begin_found = true;
            EXPECT_GT(entry.address, 0u);
        }
        if (entry.name.find("VMPilot_End") != std::string::npos) {
            end_found = true;
            EXPECT_GT(entry.address, 0u);
        }
    }
    EXPECT_TRUE(begin_found);
    EXPECT_TRUE(end_found);
}

TEST_F(ELFHandlerX86Test, StubAndPointerTableCountsMatch) {
    auto table = handler.getNativeSymbolTable();
    size_t stub_count = 0, pointer_table_count = 0;
    for (const auto& entry : table) {
        auto t = getEntryType(entry);
        if (t == "stub")
            ++stub_count;
        if (t == "pointer_table")
            ++pointer_table_count;
    }
    // Every PLT entry should have a matching GOT entry
    EXPECT_EQ(stub_count, pointer_table_count);
}
