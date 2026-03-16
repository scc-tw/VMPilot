#include <MachOHandler.hpp>
#include <file_type_parser.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::Segmentator;

class MachOHandlerARM64Test : public ::testing::Test {
   protected:
    MachOFileHandlerStrategy handler{std::string(TEST_DATA_DIR) +
                                     "/basic_binary.Darwin.arm64"};
};

TEST_F(MachOHandlerARM64Test, GetBeginEndAddr) {
    auto [begin, end] = handler.getBeginEndAddr();
    EXPECT_NE(begin, static_cast<uint64_t>(-1));
    EXPECT_NE(end, static_cast<uint64_t>(-1));
    EXPECT_NE(begin, end);
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

TEST_F(MachOHandlerARM64Test, HasDirectCallTargets) {
    auto table = handler.getNativeSymbolTable();
    bool found = false;
    for (const auto& entry : table) {
        auto it = entry.additionalAttributes.find("entry_type");
        if (it != entry.additionalAttributes.end()) {
            try {
                if (std::get<std::string>(it->second) == "direct") {
                    found = true;
                    break;
                }
            } catch (...) {}
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(MachOHandlerARM64Test, HasIndirectCallTargets) {
    auto table = handler.getNativeSymbolTable();
    bool found = false;
    for (const auto& entry : table) {
        auto it = entry.additionalAttributes.find("entry_type");
        if (it != entry.additionalAttributes.end()) {
            try {
                if (std::get<std::string>(it->second) == "indirect") {
                    found = true;
                    break;
                }
            } catch (...) {}
        }
    }
    EXPECT_TRUE(found);
}

TEST_F(MachOHandlerARM64Test, StubAddrMatchesBeginEndAddr) {
    auto [begin, end] = handler.getBeginEndAddr();
    auto table = handler.getNativeSymbolTable();

    bool begin_found = false, end_found = false;
    for (const auto& entry : table) {
        auto it = entry.additionalAttributes.find("entry_type");
        if (it == entry.additionalAttributes.end()) continue;
        try {
            if (std::get<std::string>(it->second) != "direct") continue;
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
