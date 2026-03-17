#ifndef __SDK_BYTECODE_COMPILER_SIMPLE_BACKEND_HPP__
#define __SDK_BYTECODE_COMPILER_SIMPLE_BACKEND_HPP__
#pragma once

#include <CompilerBackend.hpp>
#include <opcode_table.hpp>

namespace VMPilot::SDK::BytecodeCompiler {

/// Structural stub backend that emits MOV instructions for each code byte.
///
/// Produces valid Instruction_t values that pass Instruction::check(),
/// enabling end-to-end pipeline testing.  Thread-safe: the cached opcode
/// table is read-only after construction.
class SimpleBackend final : public CompilerBackend {
public:
    explicit SimpleBackend(const std::string& opcode_key);

    [[nodiscard]] tl::expected<CompilationOutput, CompileError>
    compile_unit(const Core::CompilationUnit& unit,
                 const CompileConfig& config) noexcept override;

    [[nodiscard]] std::string name() const noexcept override {
        return "simple";
    }

private:
    Common::Buildtime_OT buildtime_ot_;
};

}  // namespace VMPilot::SDK::BytecodeCompiler

#endif  // __SDK_BYTECODE_COMPILER_SIMPLE_BACKEND_HPP__
