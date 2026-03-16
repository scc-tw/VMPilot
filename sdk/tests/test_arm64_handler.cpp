#include <ARM64Handler.hpp>
#include <MachOHandler.hpp>
#include <file_type_parser.hpp>
#include <utilities.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::Segmentator;

class ARM64HandlerTest : public ::testing::Test {
   protected:
    void SetUp() override {
        std::string path =
            std::string(TEST_DATA_DIR) + "/basic_binary.Darwin.arm64";
        auto metadata = VMPilot::Common::get_file_metadata(path);
        MachOFileHandlerStrategy macho(path);
        symbols = macho.getNativeSymbolTable();
        text = macho.getTextSection();
        text_base = macho.getTextBaseAddr();
        mode = metadata.mode;
    }

    NativeSymbolTable symbols;
    std::vector<uint8_t> text;
    uint64_t text_base = 0;
    VMPilot::Common::FileMode mode;
};

TEST_F(ARM64HandlerTest, LoadSucceeds) {
    ARM64Handler handler(mode, symbols);
    EXPECT_TRUE(handler.Load(text, text_base));
}

TEST_F(ARM64HandlerTest, FindsProtectedRegions) {
    ARM64Handler handler(mode, symbols);
    ASSERT_TRUE(handler.Load(text, text_base));
    auto functions = handler.getNativeFunctions();
    // foo() has one pair + main() may have inlined pair
    EXPECT_GE(functions.size(), 1u);
}

TEST_F(ARM64HandlerTest, RegionsHaveValidData) {
    ARM64Handler handler(mode, symbols);
    ASSERT_TRUE(handler.Load(text, text_base));
    auto functions = handler.getNativeFunctions();

    for (const auto& f : functions) {
        EXPECT_GT(f->getAddr(), 0u);
        EXPECT_GT(f->getSize(), 0u);
        EXPECT_FALSE(f->getCode().empty());
        EXPECT_EQ(f->getCode().size(), f->getSize());
    }
}

TEST_F(ARM64HandlerTest, CalledTwiceReturnsSameResult) {
    ARM64Handler handler(mode, symbols);
    ASSERT_TRUE(handler.Load(text, text_base));
    auto first = handler.getNativeFunctions();
    auto second = handler.getNativeFunctions();
    ASSERT_EQ(first.size(), second.size());
    for (size_t i = 0; i < first.size(); ++i) {
        EXPECT_EQ(first[i]->getAddr(), second[i]->getAddr());
        EXPECT_EQ(first[i]->getSize(), second[i]->getSize());
    }
}

TEST(ARM64HandlerEdge, EmptySymbolTable) {
    NativeSymbolTable empty;
    ARM64Handler handler(VMPilot::Common::FileMode::MODE_ARM, empty);
    std::vector<uint8_t> code = {0x00, 0x00, 0x00, 0x94, 0xC0, 0x03, 0x5F, 0xD6};
    ASSERT_TRUE(handler.Load(code, 0x1000));
    auto functions = handler.getNativeFunctions();
    EXPECT_TRUE(functions.empty());
}

TEST(ARM64HandlerEdge, EmptyCode) {
    NativeSymbolTable symbols;
    ARM64Handler handler(VMPilot::Common::FileMode::MODE_ARM, symbols);
    EXPECT_FALSE(handler.Load({}, 0x1000));
}
