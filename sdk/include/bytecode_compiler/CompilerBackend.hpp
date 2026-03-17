#ifndef __SDK_BYTECODE_COMPILER_COMPILER_BACKEND_HPP__
#define __SDK_BYTECODE_COMPILER_COMPILER_BACKEND_HPP__
#pragma once

#include <CompilationOutput.hpp>
#include <CompilationUnit.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <memory>
#include <string>

namespace VMPilot::SDK::BytecodeCompiler {

/// Configuration passed to every backend invocation.
struct CompileConfig {
    std::string opcode_key;     // BLAKE3 key for opcode obfuscation
    bool debug_mode = false;    // preserve intermediate files for diagnosis
};

/// Abstract compiler backend (Strategy pattern).
///
/// Implementations must be thread-safe: compile_unit() is called from
/// pool workers concurrently on different units.
class CompilerBackend {
public:
    virtual ~CompilerBackend() = default;

    [[nodiscard]] virtual tl::expected<CompilationOutput, Common::DiagnosticCode>
    compile_unit(const Core::CompilationUnit& unit,
                 const CompileConfig& config,
                 Common::DiagnosticCollector& diag) noexcept = 0;

    [[nodiscard]] virtual std::string name() const noexcept = 0;
};

/// Factory.  Currently supports: "simple".  Future: "llvm".
/// @param name   Backend name (e.g. "simple").
/// @param config Compile configuration — some backends need the opcode key
///               at construction time for table generation.
[[nodiscard]] std::unique_ptr<CompilerBackend>
create_backend(const std::string& name,
               const CompileConfig& config) noexcept;

}  // namespace VMPilot::SDK::BytecodeCompiler

#endif  // __SDK_BYTECODE_COMPILER_COMPILER_BACKEND_HPP__
