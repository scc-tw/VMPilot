#ifndef __SDK_BYTECODE_COMPILER_COMPILE_ERROR_HPP__
#define __SDK_BYTECODE_COMPILER_COMPILE_ERROR_HPP__
#pragma once

#include <cstdint>
#include <string>

namespace VMPilot::SDK::BytecodeCompiler {

enum class CompileErrorCode : uint8_t {
    Unknown,
    InvalidInput,
    UnsupportedArch,
    UnsupportedInstruction,
    InternalError,
    NotImplemented,
};

struct CompileError {
    CompileErrorCode code = CompileErrorCode::Unknown;
    std::string message;
    std::string unit_name;
    uint64_t unit_addr = 0;
};

}  // namespace VMPilot::SDK::BytecodeCompiler

#endif  // __SDK_BYTECODE_COMPILER_COMPILE_ERROR_HPP__
