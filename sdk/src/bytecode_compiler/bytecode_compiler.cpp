#include <CompilerBackend.hpp>
#include <SimpleBackend.hpp>

namespace VMPilot::SDK::BytecodeCompiler {

std::unique_ptr<CompilerBackend>
create_backend(const std::string& name,
               const CompileConfig& config) noexcept {
    if (name == "simple") {
        try {
            return std::make_unique<SimpleBackend>(config.opcode_key);
        } catch (...) {
            return nullptr;
        }
    }
    return nullptr;
}

}  // namespace VMPilot::SDK::BytecodeCompiler
