#ifndef __SDK_BYTECODE_COMPILER_SIMPLE_BACKEND_HPP__
#define __SDK_BYTECODE_COMPILER_SIMPLE_BACKEND_HPP__
#pragma once

#include <CompilerBackend.hpp>

namespace VMPilot::SDK::BytecodeCompiler {

/// Trivial stub backend that copies native bytes into the output blob.
///
/// The real backend will emit an encrypted VmInsn stream; this stub
/// exists to keep the pipeline end-to-end testable.
class SimpleBackend final : public CompilerBackend {
public:
    SimpleBackend() = default;

    [[nodiscard]] tl::expected<CompilationOutput, Common::DiagnosticCode>
    compile_unit(const Core::CompilationUnit& unit,
                 const CompileConfig& config,
                 Common::DiagnosticCollector& diag) noexcept override;

    [[nodiscard]] std::string name() const noexcept override { return "simple"; }
};

}  // namespace VMPilot::SDK::BytecodeCompiler

#endif  // __SDK_BYTECODE_COMPILER_SIMPLE_BACKEND_HPP__
