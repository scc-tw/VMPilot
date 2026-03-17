#ifndef __SDK_BYTECODE_COMPILER_COMPILE_PIPELINE_HPP__
#define __SDK_BYTECODE_COMPILER_COMPILE_PIPELINE_HPP__
#pragma once

#include <CompilationOrchestrator.hpp>
#include <CompilerBackend.hpp>

#include <tl/expected.hpp>

#include <string>

namespace VMPilot::SDK::BytecodeCompiler {

/// Top-level entry point: segment → serialize (debug) → compile → cleanup.
///
/// @param binary_path   Path to the input binary file.
/// @param config        Compile configuration (opcode key, debug mode).
/// @param backend_name  Backend to use ("simple", future: "llvm").
/// @return CompilationResult on success, error string on failure.
[[nodiscard]] tl::expected<CompilationResult, std::string>
compile_binary(const std::string& binary_path,
               const CompileConfig& config,
               const std::string& backend_name = "simple") noexcept;

}  // namespace VMPilot::SDK::BytecodeCompiler

#endif  // __SDK_BYTECODE_COMPILER_COMPILE_PIPELINE_HPP__
