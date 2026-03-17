#ifndef __SDK_BYTECODE_COMPILER_COMPILATION_OUTPUT_HPP__
#define __SDK_BYTECODE_COMPILER_COMPILATION_OUTPUT_HPP__
#pragma once

#include <instruction_t.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace VMPilot::SDK::BytecodeCompiler {

/// Per-unit output from a compiler backend.
struct CompilationOutput {
    std::string name;
    uint64_t addr = 0;
    std::vector<Common::Instruction_t> bytecodes;
};

}  // namespace VMPilot::SDK::BytecodeCompiler

#endif  // __SDK_BYTECODE_COMPILER_COMPILATION_OUTPUT_HPP__
