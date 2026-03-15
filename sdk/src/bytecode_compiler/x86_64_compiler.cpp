#include <x86_64_compiler.hpp>

#include <stdexcept>

using namespace VMPilot::SDK::BytecodeCompiler;

std::vector<uint8_t> X86_64Compiler::Compile(
    [[maybe_unused]] const BytecodeCompileRecipe& script) {
    throw std::runtime_error("X86_64Compiler not implemented yet");
}
