#include <compile_pipeline.hpp>
#include <Serializer.hpp>
#include <segmentator.hpp>

#include <cstdlib>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace VMPilot::SDK::BytecodeCompiler {

tl::expected<CompilationResult, std::string>
compile_binary(const std::string& binary_path,
               const CompileConfig& config,
               const std::string& backend_name) noexcept {
    // 1. Segment the binary
    auto seg_result = Segmentator::segment(binary_path);
    if (!seg_result.has_value()) {
        return tl::unexpected(
            std::string("Segmentation failed: ") +
            Segmentator::to_string(seg_result.error()));
    }

    // 2. Debug mode: serialize to temp directory
    std::string temp_dir;
    if (config.debug_mode) {
        try {
            auto tmp = fs::temp_directory_path() / "vmpilot_compile_debug";
            fs::create_directories(tmp);
            temp_dir = tmp.string();
        } catch (const std::exception& e) {
            return tl::unexpected(
                std::string("Failed to create temp directory: ") + e.what());
        }

        auto dump_result =
            Serializer::dump(seg_result.value(), temp_dir);
        if (!dump_result.has_value()) {
            return tl::unexpected(
                std::string("Serialization failed: ") +
                dump_result.error());
        }
    }

    // 3. Create backend
    auto backend = create_backend(backend_name, config);
    if (!backend) {
        return tl::unexpected(
            std::string("Unknown backend: ") + backend_name);
    }

    // 4. Compile
    CompilationOrchestrator orchestrator(
        std::move(backend), config);
    auto comp_result = orchestrator.compile(seg_result.value());

    // 5. Cleanup: delete temp directory in non-debug mode
    if (!config.debug_mode && !temp_dir.empty()) {
        std::error_code ec;
        fs::remove_all(temp_dir, ec);
    }

    return comp_result;
}

}  // namespace VMPilot::SDK::BytecodeCompiler
