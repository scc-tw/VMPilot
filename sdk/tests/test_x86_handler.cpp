#include <ELFHandler.hpp>
#include <X86Handler.hpp>
#include <file_type_parser.hpp>
#include <utilities.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::Segmentator;

class X86HandlerX64Test : public ::testing::Test {
   protected:
    void SetUp() override {
        std::string path =
            std::string(TEST_DATA_DIR) + "/basic_binary.Linux.x86_64";
        auto metadata = VMPilot::Common::get_file_metadata(path);
        ELFFileHandlerStrategy elf(path);
        symbols = elf.getNativeSymbolTable();
        text = elf.getTextSection();
        text_base = elf.getTextBaseAddr();
        mode = metadata.mode;
    }

    NativeSymbolTable symbols;
    std::vector<uint8_t> text;
    uint64_t text_base = 0;
    VMPilot::Common::FileMode mode;
};

class X86HandlerX86Test : public ::testing::Test {
   protected:
    void SetUp() override {
        std::string path =
            std::string(TEST_DATA_DIR) + "/basic_binary.Linux.x86";
        auto metadata = VMPilot::Common::get_file_metadata(path);
        ELFFileHandlerStrategy elf(path);
        symbols = elf.getNativeSymbolTable();
        text = elf.getTextSection();
        text_base = elf.getTextBaseAddr();
        mode = metadata.mode;
    }

    NativeSymbolTable symbols;
    std::vector<uint8_t> text;
    uint64_t text_base = 0;
    VMPilot::Common::FileMode mode;
};

TEST_F(X86HandlerX64Test, LoadSucceeds) {
    X86Handler handler(mode, symbols);
    EXPECT_TRUE(handler.Load(text, text_base));
}

TEST_F(X86HandlerX64Test, FindsProtectedRegions) {
    X86Handler handler(mode, symbols);
    ASSERT_TRUE(handler.Load(text, text_base));
    auto functions = handler.getNativeFunctions();
    EXPECT_EQ(functions.size(), 2u);
}

TEST_F(X86HandlerX64Test, RegionsHaveValidData) {
    X86Handler handler(mode, symbols);
    ASSERT_TRUE(handler.Load(text, text_base));
    auto functions = handler.getNativeFunctions();

    for (const auto& f : functions) {
        EXPECT_GT(f.getAddr(), 0u);
        EXPECT_GT(f.getSize(), 0u);
        EXPECT_FALSE(f.getCode().empty());
        EXPECT_EQ(f.getCode().size(), f.getSize());
    }
}

TEST_F(X86HandlerX64Test, CalledTwiceReturnsSameResult) {
    X86Handler handler(mode, symbols);
    ASSERT_TRUE(handler.Load(text, text_base));
    auto first = handler.getNativeFunctions();
    auto second = handler.getNativeFunctions();
    ASSERT_EQ(first.size(), second.size());
    for (size_t i = 0; i < first.size(); ++i) {
        EXPECT_EQ(first[i].getAddr(), second[i].getAddr());
        EXPECT_EQ(first[i].getSize(), second[i].getSize());
    }
}

TEST_F(X86HandlerX86Test, LoadSucceeds) {
    X86Handler handler(mode, symbols);
    EXPECT_TRUE(handler.Load(text, text_base));
}

TEST_F(X86HandlerX86Test, FindsProtectedRegions) {
    X86Handler handler(mode, symbols);
    ASSERT_TRUE(handler.Load(text, text_base));
    auto functions = handler.getNativeFunctions();
    EXPECT_EQ(functions.size(), 2u);
}

TEST(X86HandlerEdge, EmptySymbolTable) {
    NativeSymbolTable empty;
    X86Handler handler(VMPilot::Common::FileMode::MODE_64, empty);
    // Should load but find no regions
    std::vector<uint8_t> code = {0xE8, 0x00, 0x00, 0x00, 0x00, 0xC3};
    ASSERT_TRUE(handler.Load(code, 0x1000));
    auto functions = handler.getNativeFunctions();
    EXPECT_TRUE(functions.empty());
}

TEST(X86HandlerEdge, EmptyCode) {
    NativeSymbolTable symbols;
    X86Handler handler(VMPilot::Common::FileMode::MODE_64, symbols);
    EXPECT_FALSE(handler.Load({}, 0x1000));
}
