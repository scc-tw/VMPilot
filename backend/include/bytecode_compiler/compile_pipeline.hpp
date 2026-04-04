#ifndef __SDK_BYTECODE_COMPILER_COMPILE_PIPELINE_HPP__
#define __SDK_BYTECODE_COMPILER_COMPILE_PIPELINE_HPP__
#pragma once

#include <CompilationOrchestrator.hpp>
#include <CompilerBackend.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <string>

namespace VMPilot::SDK::BytecodeCompiler {

/// Top-level entry point: segment → build_units → serialize (debug) → compile → cleanup.
///
/// @param binary_path   Path to the input binary file.
/// @param config        Compile configuration (opcode key, debug mode).
/// @param diag          Diagnostic collector for all pipeline stages.
/// @param backend_name  Backend to use ("simple", future: "llvm").
/// @return CompilationResult on success, DiagnosticCode on failure.
[[nodiscard]] tl::expected<CompilationResult, Common::DiagnosticCode>
compile_binary(const std::string& binary_path,
               const CompileConfig& config,
               Common::DiagnosticCollector& diag =
                   Common::DiagnosticCollector::noop(),
               const std::string& backend_name = "simple") noexcept;

}  // namespace VMPilot::SDK::BytecodeCompiler

#endif  // __SDK_BYTECODE_COMPILER_COMPILE_PIPELINE_HPP__
