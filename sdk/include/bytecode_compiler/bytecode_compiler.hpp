#ifndef __SDK_BYTECODE_COMPILER_HPP__
#define __SDK_BYTECODE_COMPILER_HPP__

#include <BytecodeCompileRecipe.hpp>

#include <cstdint>
#include <memory>
#include <vector>

namespace VMPilot::SDK::BytecodeCompiler {

enum class Arch : uint8_t {
    X86,
    X86_64,
    ARM,
    ARM64,
};

class CompilerBase {
    Arch m_arch;

   public:
    virtual std::vector<uint8_t> Compile(
        const BytecodeCompileRecipe& script) = 0;

    Arch GetArch() const noexcept { return m_arch; }

    CompilerBase(Arch arch) : m_arch(arch) {}
    virtual ~CompilerBase() = default;
};

class CompilerFactory {
   public:
    static std::unique_ptr<CompilerBase> CreateCompiler(Arch arch) noexcept;
};

}  // namespace VMPilot::SDK::BytecodeCompiler

#endif
