#include <bytecode_compiler.hpp>
#include <x86_64_compiler.hpp>
#include <x86_compiler.hpp>

using namespace VMPilot::SDK::BytecodeCompiler;

std::unique_ptr<CompilerBase> CompilerFactory::CreateCompiler(
    Arch arch) noexcept {
    switch (arch) {
        case Arch::X86:
            return std::make_unique<X86Compiler>();
        case Arch::X86_64:
            return std::make_unique<X86_64Compiler>();
        case Arch::ARM:
        case Arch::ARM64:
            return nullptr;
    }
    return nullptr;
}
