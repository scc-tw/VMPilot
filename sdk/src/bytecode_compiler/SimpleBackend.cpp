#include <SimpleBackend.hpp>

namespace VMPilot::SDK::BytecodeCompiler {

tl::expected<CompilationOutput, Common::DiagnosticCode>
SimpleBackend::compile_unit(const Core::CompilationUnit& unit,
                            const CompileConfig&,
                            Common::DiagnosticCollector& diag) noexcept {
    if (unit.code.empty()) {
        diag.error("compiler", Common::DiagnosticCode::InvalidInput,
                   "empty code in compilation unit", unit.name, unit.addr);
        return tl::unexpected(Common::DiagnosticCode::InvalidInput);
    }

    CompilationOutput output;
    output.name = unit.name;
    output.addr = unit.addr;
    // Stub: copy raw native bytes as-is (real backend will emit encrypted VmInsn blob)
    output.bytecodes.assign(unit.code.begin(), unit.code.end());
    return output;
}

}  // namespace VMPilot::SDK::BytecodeCompiler
