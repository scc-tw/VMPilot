#include <MachOHandler.hpp>
#include <file_type_parser.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::Segmentator;

class MachOHandlerARM64Test : public ::testing::Test {
   protected:
    MachOFileHandlerStrategy handler{std::string(TEST_DATA_DIR) +
                                     "/basic_binary.Darwin.arm64"};
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

TEST_F(MachOHandlerARM64Test, GetTextSection) {
    auto text = handler.getTextSection();
    EXPECT_FALSE(text.empty());
}

TEST_F(MachOHandlerARM64Test, GetTextBaseAddr) {
    auto base = handler.getTextBaseAddr();
    EXPECT_NE(base, static_cast<uint64_t>(-1));
    EXPECT_GT(base, 0u);
}

TEST_F(MachOHandlerARM64Test, GetNativeSymbolTable) {
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

TEST_F(MachOHandlerARM64Test, HasStubCallTargets) {
    auto table = handler.getNativeSymbolTable();
    bool found = false;
    for (const auto& entry : table) {
        if (getEntryType(entry) == "stub") {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(MachOHandlerARM64Test, HasPointerTableTargets) {
    auto table = handler.getNativeSymbolTable();
    bool found = false;
    for (const auto& entry : table) {
        if (getEntryType(entry) == "pointer_table") {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(MachOHandlerARM64Test, StubEntriesHaveVMPilotMarkers) {
    auto table = handler.getNativeSymbolTable();

    bool begin_found = false, end_found = false;
    for (const auto& entry : table) {
        if (getEntryType(entry) != "stub") continue;
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
