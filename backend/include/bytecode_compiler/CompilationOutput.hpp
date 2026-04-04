#ifndef __SDK_BYTECODE_COMPILER_COMPILATION_OUTPUT_HPP__
#define __SDK_BYTECODE_COMPILER_COMPILATION_OUTPUT_HPP__
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace VMPilot::SDK::BytecodeCompiler {

/// Per-unit output from a compiler backend.
struct CompilationOutput {
    std::string name;
    uint64_t addr = 0;

    /// Raw VM bytecode blob (header + encrypted VmInsn stream + integrity table).
    /// Format defined by doc 09 §2: "emission phase produces VmBytecodeBlob, not
    /// individual Instruction_t structs."
    std::vector<uint8_t> bytecodes;
};

}  // namespace VMPilot::SDK::BytecodeCompiler

#endif  // __SDK_BYTECODE_COMPILER_COMPILATION_OUTPUT_HPP__
