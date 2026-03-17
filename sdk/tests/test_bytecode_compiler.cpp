#include <bytecode_compiler.hpp>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::BytecodeCompiler;

static const std::string TEST_KEY =
    "01234567890123456789012345678901";

TEST(BackendFactory, CreateSimple) {
    CompileConfig config{TEST_KEY, false};
    auto backend = create_backend("simple", config);
    ASSERT_NE(backend, nullptr);
    EXPECT_EQ(backend->name(), "simple");
}

TEST(BackendFactory, CreateUnknown) {
    CompileConfig config{TEST_KEY, false};
    auto backend = create_backend("unknown", config);
    EXPECT_EQ(backend, nullptr);
}

TEST(CompileConfigDefaults, DebugModeDefaultFalse) {
    CompileConfig config;
    EXPECT_FALSE(config.debug_mode);
    EXPECT_TRUE(config.opcode_key.empty());
}
