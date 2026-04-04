#include <CompilerBackend.hpp>
#include <SimpleBackend.hpp>
#include <CompilationUnit.hpp>
#include <diagnostic_collector.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::BytecodeCompiler;
using VMPilot::SDK::Core::CompilationUnit;
using VMPilot::Common::DiagnosticCollector;
using VMPilot::Common::DiagnosticCode;

class SimpleBackendTest : public ::testing::Test {
protected:
    void SetUp() override {
        backend = std::make_unique<SimpleBackend>();
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
    CompileConfig config{false};
    DiagnosticCollector diag;
};

TEST_F(SimpleBackendTest, Name) {
    EXPECT_EQ(backend->name(), "simple");
}

TEST_F(SimpleBackendTest, EmptyCodeReturnsError) {
    auto unit = make_unit("empty", {});
    auto result = backend->compile_unit(unit, config, diag);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::InvalidInput);
    EXPECT_TRUE(diag.has_errors());
}

TEST_F(SimpleBackendTest, SingleByte) {
    auto unit = make_unit("single", {0x90});
    auto result = backend->compile_unit(unit, config, diag);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->name, "single");
    EXPECT_EQ(result->addr, 0x1000u);
    ASSERT_EQ(result->bytecodes.size(), 1u);
    EXPECT_EQ(result->bytecodes[0], 0x90);
}

TEST_F(SimpleBackendTest, BytecodesMatchInputCode) {
    std::vector<uint8_t> code = {0xAA, 0xBB, 0xCC};
    auto unit = make_unit("values", code);
    auto result = backend->compile_unit(unit, config, diag);
    ASSERT_TRUE(result.has_value());
    ASSERT_EQ(result->bytecodes.size(), 3u);

    for (size_t i = 0; i < code.size(); ++i) {
        EXPECT_EQ(result->bytecodes[i], code[i]);
    }
}

TEST_F(SimpleBackendTest, LargerCode) {
    std::vector<uint8_t> code(256);
    for (size_t i = 0; i < 256; ++i) code[i] = static_cast<uint8_t>(i);

    auto unit = make_unit("large", code);
    auto result = backend->compile_unit(unit, config, diag);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->bytecodes.size(), 256u);
    EXPECT_EQ(result->bytecodes, code);
}

TEST(BackendFactory, SimpleCreation) {
    auto backend = create_backend("simple");
    ASSERT_NE(backend, nullptr);
    EXPECT_EQ(backend->name(), "simple");
}

TEST(BackendFactory, UnknownReturnsNull) {
    auto backend = create_backend("nonexistent");
    EXPECT_EQ(backend, nullptr);
}

TEST(BackendFactory, LLVMNotYetImplemented) {
    auto backend = create_backend("llvm");
    EXPECT_EQ(backend, nullptr);
}
