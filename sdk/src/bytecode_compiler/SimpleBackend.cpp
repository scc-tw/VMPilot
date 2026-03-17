#include <SimpleBackend.hpp>
#include <instruction_t.hpp>
#include <opcode_enum.hpp>

#include <random>

namespace VMPilot::SDK::BytecodeCompiler {

using Common::DiagnosticCode;

SimpleBackend::SimpleBackend(const std::string& opcode_key) {
    Common::Opcode_table_generator gen(opcode_key);
    buildtime_ot_ = gen.Get_RealOp_to_OID();
}

tl::expected<CompilationOutput, DiagnosticCode>
SimpleBackend::compile_unit(const Core::CompilationUnit& unit,
                            [[maybe_unused]] const CompileConfig& config,
                            Common::DiagnosticCollector& diag) noexcept {
    if (unit.code.empty()) {
        diag.error("compiler", DiagnosticCode::InvalidInput,
                   "empty code in compilation unit",
                   unit.name, unit.addr);
        return tl::unexpected(DiagnosticCode::InvalidInput);
    }

    CompilationOutput output;
    output.name = unit.name;
    output.addr = unit.addr;
    output.bytecodes.reserve(unit.code.size());

    const auto mov_real = static_cast<Common::Opcode_t>(
        Common::Opcode::Enum::DataMovement::MOV);
    auto it = buildtime_ot_.find(mov_real);
    if (it == buildtime_ot_.end()) {
        diag.error("compiler", DiagnosticCode::CompilerInternalError,
                   "MOV opcode not found in buildtime opcode table",
                   unit.name, unit.addr);
        return tl::unexpected(DiagnosticCode::CompilerInternalError);
    }
    const auto mov_oid = it->second;

    thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<uint32_t> nonce_dist;

    Common::Instruction instr_helper;

    for (size_t i = 0; i < unit.code.size(); ++i) {
        Common::Instruction_t inst{};
        inst.opcode = mov_oid;
        inst.left_operand = i;
        inst.right_operand = unit.code[i];
        inst.nounce = nonce_dist(rng);
        instr_helper.update_checksum(inst);

        output.bytecodes.push_back(inst);
    }

    return output;
}

}  // namespace VMPilot::SDK::BytecodeCompiler
