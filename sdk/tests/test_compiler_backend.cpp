#include <CompilerBackend.hpp>
#include <SimpleBackend.hpp>
#include <CompilationUnit.hpp>
#include <instruction_t.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::BytecodeCompiler;
using VMPilot::SDK::Core::CompilationUnit;

static const std::string TEST_KEY =
    "01234567890123456789012345678901";  // 32-char key

class SimpleBackendTest : public ::testing::Test {
protected:
    void SetUp() override {
        backend = std::make_unique<SimpleBackend>(TEST_KEY);
    }

    CompilationUnit make_unit(const std::string& name,
                              std::vector<uint8_t> code,
                              uint64_t addr = 0x1000) {
        CompilationUnit unit;
        unit.name = name;
        unit.addr = addr;
        unit.size = code.size();
        unit.code = std::move(code);
        return unit;
    }

    std::unique_ptr<SimpleBackend> backend;
    CompileConfig config{TEST_KEY, false};
};

TEST_F(SimpleBackendTest, Name) {
    EXPECT_EQ(backend->name(), "simple");
}

TEST_F(SimpleBackendTest, EmptyCodeReturnsError) {
    auto unit = make_unit("empty", {});
    auto result = backend->compile_unit(unit, config);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error().code, CompileErrorCode::InvalidInput);
    EXPECT_EQ(result.error().unit_name, "empty");
}

TEST_F(SimpleBackendTest, SingleByte) {
    auto unit = make_unit("single", {0x90});
    auto result = backend->compile_unit(unit, config);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->name, "single");
    EXPECT_EQ(result->addr, 0x1000u);
    ASSERT_EQ(result->bytecodes.size(), 1u);
}

TEST_F(SimpleBackendTest, BytecodesPassChecksum) {
    auto unit = make_unit("checksum_test", {0x55, 0x89, 0xe5, 0xc3});
    auto result = backend->compile_unit(unit, config);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->bytecodes.size(), 4u);

    VMPilot::Common::Instruction instr_helper;
    for (const auto& inst : result->bytecodes) {
        EXPECT_TRUE(instr_helper.check(inst))
            << "Checksum verification failed for instruction";
    }
}

TEST_F(SimpleBackendTest, BytecodesEncodeCorrectValues) {
    std::vector<uint8_t> code = {0xAA, 0xBB, 0xCC};
    auto unit = make_unit("values", code);
    auto result = backend->compile_unit(unit, config);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->bytecodes.size(), 3u);

    for (size_t i = 0; i < code.size(); ++i) {
        EXPECT_EQ(result->bytecodes[i].left_operand, i);
        EXPECT_EQ(result->bytecodes[i].right_operand, code[i]);
    }
}

TEST_F(SimpleBackendTest, LargerCode) {
    std::vector<uint8_t> code(256);
    for (int i = 0; i < 256; ++i) code[i] = static_cast<uint8_t>(i);

    auto unit = make_unit("large", code);
    auto result = backend->compile_unit(unit, config);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->bytecodes.size(), 256u);
}

TEST(BackendFactory, SimpleCreation) {
    CompileConfig config{TEST_KEY, false};
    auto backend = create_backend("simple", config);
    ASSERT_NE(backend, nullptr);
    EXPECT_EQ(backend->name(), "simple");
}

TEST(BackendFactory, UnknownReturnsNull) {
    CompileConfig config{TEST_KEY, false};
    auto backend = create_backend("nonexistent", config);
    EXPECT_EQ(backend, nullptr);
}

TEST(BackendFactory, LLVMNotYetImplemented) {
    CompileConfig config{TEST_KEY, false};
    auto backend = create_backend("llvm", config);
    EXPECT_EQ(backend, nullptr);
}
