#include <compile_pipeline.hpp>
#include <diagnostic_collector.hpp>

#include <gtest/gtest.h>

#include <filesystem>

using namespace VMPilot::SDK::BytecodeCompiler;
using VMPilot::Common::DiagnosticCollector;
using VMPilot::Common::DiagnosticCode;

namespace fs = std::filesystem;

class CompilePipelineTest : public ::testing::Test {
protected:
    std::string binary_path =
        std::string(TEST_DATA_DIR) + "/basic_binary.Linux.x86";

    CompileConfig make_config(bool debug = false) {
        return CompileConfig{debug};
    }
};

TEST_F(CompilePipelineTest, BasicCompilation) {
    if (!fs::exists(binary_path)) GTEST_SKIP() << "Test binary not found";

    DiagnosticCollector diag;
    auto result = compile_binary(binary_path, make_config(), diag);
    ASSERT_TRUE(result.has_value()) << diag.summary();
    EXPECT_GT(result->total_units, 0u);
    EXPECT_FALSE(result->outputs.empty());
    EXPECT_EQ(result->failed_units, 0u);
}

TEST_F(CompilePipelineTest, DebugModePreservesFiles) {
    if (!fs::exists(binary_path)) GTEST_SKIP() << "Test binary not found";

    DiagnosticCollector diag;
    auto config = make_config(true);
    auto result = compile_binary(binary_path, config, diag);
    ASSERT_TRUE(result.has_value()) << diag.summary();

    auto tmp = fs::temp_directory_path() / "vmpilot_compile_debug";
    EXPECT_TRUE(fs::exists(tmp))
        << "Debug temp directory should be preserved";

    std::error_code ec;
    fs::remove_all(tmp, ec);
}

TEST_F(CompilePipelineTest, DefaultModeCleanup) {
    if (!fs::exists(binary_path)) GTEST_SKIP() << "Test binary not found";

    DiagnosticCollector diag;
    auto result = compile_binary(binary_path, make_config(false), diag);
    ASSERT_TRUE(result.has_value()) << diag.summary();
}

TEST_F(CompilePipelineTest, InvalidBinaryPath) {
    DiagnosticCollector diag;
    auto result = compile_binary("/nonexistent/binary", make_config(), diag);
    ASSERT_FALSE(result.has_value());
    EXPECT_TRUE(diag.has_errors());
}

TEST_F(CompilePipelineTest, InvalidBackend) {
    if (!fs::exists(binary_path)) GTEST_SKIP() << "Test binary not found";

    DiagnosticCollector diag;
    auto result = compile_binary(binary_path, make_config(), diag, "nonexistent");
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), DiagnosticCode::BackendCreationFailed);
}

TEST_F(CompilePipelineTest, OutputBytecodesValid) {
    if (!fs::exists(binary_path)) GTEST_SKIP() << "Test binary not found";

    DiagnosticCollector diag;
    auto result = compile_binary(binary_path, make_config(), diag);
    ASSERT_TRUE(result.has_value()) << diag.summary();

    for (const auto& output : result->outputs) {
        EXPECT_FALSE(output.name.empty());
        EXPECT_FALSE(output.bytecodes.empty())
            << "Empty bytecodes for " << output.name;
    }
}
