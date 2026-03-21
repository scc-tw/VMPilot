#include <compile_pipeline.hpp>
#include <AnalysisContext.hpp>
#include <ReferenceAnalyzer.hpp>
#include <Serializer.hpp>
#include <segmentator.hpp>

#include <capstone.hpp>

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace {

/// Convert SDK arch enum to Capstone arch enum.
Capstone::Arch toCapstoneArch(VMPilot::Common::FileArch arch) {
    switch (arch) {
        case VMPilot::Common::FileArch::X86:   return Capstone::Arch::X86;
        case VMPilot::Common::FileArch::ARM64: return Capstone::Arch::ARM64;
        case VMPilot::Common::FileArch::ARM:   return Capstone::Arch::ARM;
        case VMPilot::Common::FileArch::MIPS:  return Capstone::Arch::MIPS;
        case VMPilot::Common::FileArch::PPC:   return Capstone::Arch::PPC;
        case VMPilot::Common::FileArch::RISCV: return Capstone::Arch::RISCV;
        default: return Capstone::Arch::X86;
    }
}

}  // namespace

namespace VMPilot::SDK::BytecodeCompiler {

using Common::DiagnosticCode;

tl::expected<CompilationResult, DiagnosticCode>
compile_binary(const std::string& binary_path,
               const CompileConfig& config,
               Common::DiagnosticCollector& diag,
               const std::string& backend_name) noexcept {
    // 1. Segment the binary
    auto seg_result = Segmentator::segment(binary_path, diag);
    if (!seg_result.has_value())
        return tl::unexpected(seg_result.error());

    // 2. Build CompilationUnits (single conversion point)
    auto units = Serializer::build_units(seg_result.value(), diag);

    // If build_units reported errors (orphan sites), stop
    if (diag.has_errors())
        return tl::unexpected(DiagnosticCode::OrphanSiteSkipped);

    // 2.5. Analyze data references for each unit
    try {
        Capstone::Capstone cs(
            toCapstoneArch(seg_result->context.arch),
            static_cast<Capstone::Mode>(seg_result->context.mode));

        // Build AnalysisContext once, share across all units
        auto analysis_ctx = ReferenceAnalyzer::AnalysisContext::build(
            seg_result->context.sections,
            seg_result->text_relocations,
            seg_result->context.symbols,
            seg_result->context.arch,
            seg_result->context.mode);

        for (auto& unit : units) {
            auto insns = cs.disasm(unit.code, unit.addr);
            auto refs = ReferenceAnalyzer::analyze(
                insns, unit.addr, unit.size, analysis_ctx);
            unit.data_references = std::move(refs);
        }
    } catch (...) {
        // Reference analysis failure is non-fatal — units remain usable
    }

    // 3. Debug mode: serialize to temp directory
    std::string temp_dir;
    if (config.debug_mode) {
        try {
            auto tmp = fs::temp_directory_path() / "vmpilot_compile_debug";
            fs::create_directories(tmp);
            temp_dir = tmp.string();
        } catch (const std::exception& e) {
            diag.error("compiler", DiagnosticCode::CompilerInternalError,
                       std::string("failed to create temp directory: ") +
                           e.what());
            return tl::unexpected(DiagnosticCode::CompilerInternalError);
        }

        auto dump_result = Serializer::dump(units, temp_dir, diag);
        if (!dump_result.has_value())
            return tl::unexpected(dump_result.error());
    }

    // 4. Create backend and compile
    auto backend = create_backend(backend_name, config);
    if (!backend) {
        diag.error("compiler", DiagnosticCode::BackendCreationFailed,
                   "unknown or failed backend: " + backend_name);
        return tl::unexpected(DiagnosticCode::BackendCreationFailed);
    }

    CompilationOrchestrator orchestrator(
        std::move(backend), config);
    auto comp_result = orchestrator.compile(units, diag);

    // 5. Cleanup: delete temp directory in non-debug mode
    if (!config.debug_mode && !temp_dir.empty()) {
        std::error_code ec;
        fs::remove_all(temp_dir, ec);
        if (ec) {
            diag.note("compiler", DiagnosticCode::None,
                      "failed to clean up temp directory: " + ec.message());
        }
    }

    return comp_result;
}

}  // namespace VMPilot::SDK::BytecodeCompiler
