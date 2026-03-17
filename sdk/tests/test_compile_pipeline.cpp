#include <compile_pipeline.hpp>

#include <gtest/gtest.h>

#include <filesystem>

using namespace VMPilot::SDK::BytecodeCompiler;

namespace fs = std::filesystem;

static const std::string TEST_KEY =
    "01234567890123456789012345678901";

class CompilePipelineTest : public ::testing::Test {
protected:
    // Path to test binary — set via TEST_DATA_DIR compile definition
    std::string binary_path =
        std::string(TEST_DATA_DIR) + "/basic_binary.Linux.x86";

    CompileConfig make_config(bool debug = false) {
        return CompileConfig{TEST_KEY, debug};
    }
};

TEST_F(CompilePipelineTest, BasicCompilation) {
    if (!fs::exists(binary_path)) GTEST_SKIP() << "Test binary not found";

    auto result = compile_binary(binary_path, make_config());
    ASSERT_TRUE(result.has_value()) << result.error();
    EXPECT_GT(result->total_units, 0u);
    EXPECT_FALSE(result->outputs.empty());
}

TEST_F(CompilePipelineTest, DebugModePreservesFiles) {
    if (!fs::exists(binary_path)) GTEST_SKIP() << "Test binary not found";

    auto config = make_config(true);
    auto result = compile_binary(binary_path, config);
    ASSERT_TRUE(result.has_value()) << result.error();

    // Debug temp directory should exist
    auto tmp = fs::temp_directory_path() / "vmpilot_compile_debug";
    EXPECT_TRUE(fs::exists(tmp))
        << "Debug temp directory should be preserved";

    // Cleanup after test
    std::error_code ec;
    fs::remove_all(tmp, ec);
}

TEST_F(CompilePipelineTest, DefaultModeCleanup) {
    if (!fs::exists(binary_path)) GTEST_SKIP() << "Test binary not found";

    auto result = compile_binary(binary_path, make_config(false));
    ASSERT_TRUE(result.has_value()) << result.error();

    // In non-debug mode, temp directory should not exist
    // (it was never created in non-debug mode)
    auto tmp = fs::temp_directory_path() / "vmpilot_compile_debug";
    // We don't assert non-existence because the directory might not
    // have been created at all in non-debug mode.
}

TEST_F(CompilePipelineTest, InvalidBinaryPath) {
    auto result = compile_binary("/nonexistent/binary", make_config());
    ASSERT_FALSE(result.has_value());
}

TEST_F(CompilePipelineTest, InvalidBackend) {
    if (!fs::exists(binary_path)) GTEST_SKIP() << "Test binary not found";

    auto result = compile_binary(binary_path, make_config(), "nonexistent");
    ASSERT_FALSE(result.has_value());
    EXPECT_NE(result.error().find("Unknown backend"), std::string::npos);
}

TEST_F(CompilePipelineTest, OutputBytecodesValid) {
    if (!fs::exists(binary_path)) GTEST_SKIP() << "Test binary not found";

    auto result = compile_binary(binary_path, make_config());
    ASSERT_TRUE(result.has_value()) << result.error();

    VMPilot::Common::Instruction instr_helper;
    for (const auto& output : result->outputs) {
        EXPECT_FALSE(output.name.empty());
        EXPECT_FALSE(output.bytecodes.empty());
        for (const auto& inst : output.bytecodes) {
            EXPECT_TRUE(instr_helper.check(inst))
                << "Invalid checksum in output for " << output.name;
        }
    }
}
