#ifndef __SDK_BYTECODE_COMPILER_COMPILATION_ORCHESTRATOR_HPP__
#define __SDK_BYTECODE_COMPILER_COMPILATION_ORCHESTRATOR_HPP__
#pragma once

#include <CompilationOutput.hpp>
#include <CompileError.hpp>
#include <CompilerBackend.hpp>
#include <segmentator.hpp>

#include <tl/expected.hpp>

#include <memory>
#include <string>
#include <vector>

namespace VMPilot::SDK::BytecodeCompiler {

/// Aggregated result from compiling all units.
struct CompilationResult {
    std::vector<CompilationOutput> outputs;
    std::vector<CompileError> errors;
    size_t total_units = 0;
};

/// Dispatches compilation units to a backend via a thread pool.
class CompilationOrchestrator {
public:
    /// @param backend   Compiler backend to use (takes ownership).
    /// @param config    Shared compile configuration.
    /// @param num_threads  Worker thread count. 0 = hardware_concurrency.
    CompilationOrchestrator(std::unique_ptr<CompilerBackend> backend,
                            CompileConfig config,
                            size_t num_threads = 0);

    /// Build CompilationUnits from segmentation results and compile in parallel.
    [[nodiscard]] tl::expected<CompilationResult, std::string>
    compile(const Segmentator::SegmentationResult& result) noexcept;

private:
    std::unique_ptr<CompilerBackend> backend_;
    CompileConfig config_;
    size_t num_threads_;
};

}  // namespace VMPilot::SDK::BytecodeCompiler

#endif  // __SDK_BYTECODE_COMPILER_COMPILATION_ORCHESTRATOR_HPP__
