#ifndef __SDK_X86_64_COMPILER_HPP__
#define __SDK_X86_64_COMPILER_HPP__

#include <bytecode_compiler.hpp>

namespace VMPilot::SDK::BytecodeCompiler {

class X86_64Compiler : public CompilerBase {
   public:
    std::vector<uint8_t> Compile(const BytecodeCompileRecipe& script) override;
    X86_64Compiler() : CompilerBase(Arch::X86_64) {}
};

}  // namespace VMPilot::SDK::BytecodeCompiler

#endif  // __SDK_X86_64_COMPILER_HPP__
