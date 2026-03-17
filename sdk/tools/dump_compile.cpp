/// dump_compile: Developer debug tool — dump full pipeline state at every stage.
///
/// Usage: dump_compile <binary> [opcode_key]
///
/// Shows detailed output for each pipeline stage:
///   1. Segmentation: regions found, addresses, sizes, code hex
///   2. Grouping: groups, sites, canonical detection, enclosing symbols
///   3. Build units: CompilationUnit fields
///   4. Data references: per-unit reference analysis results
///   5. Compilation: per-unit bytecodes with opcode/operand/checksum detail
///   6. Diagnostics: full diagnostic report

#include <compile_pipeline.hpp>
#include <Serializer.hpp>
#include <diagnostic_collector.hpp>
#include <instruction_t.hpp>
#include <segmentator.hpp>

#include <cinttypes>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace VMPilot::SDK;
using VMPilot::Common::DiagnosticCollector;
using VMPilot::Common::DiagnosticCode;

static std::string hexdump(const std::vector<uint8_t>& data, size_t max = 64) {
    std::ostringstream ss;
    for (size_t i = 0; i < data.size() && i < max; ++i) {
        ss << std::hex << std::setfill('0') << std::setw(2)
           << static_cast<int>(data[i]);
        if (i + 1 < data.size() && i + 1 < max) ss << ' ';
    }
    if (data.size() > max) ss << " ... (" << data.size() << " bytes total)";
    return ss.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <binary> [opcode_key]\n", argv[0]);
        return 1;
    }

    std::string opcode_key =
        (argc >= 3) ? argv[2] : "01234567890123456789012345678901";

    DiagnosticCollector diag;

    // ── Stage 1: Segmentation ──
    printf("===========================================================\n");
    printf("  Stage 1: Segmentation\n");
    printf("===========================================================\n\n");

    auto seg = Segmentator::segment(argv[1], diag);
    if (!seg.has_value()) {
        fprintf(stderr, "Segmentation FAILED: %s\n",
                VMPilot::Common::to_string(seg.error()));
        if (diag.size() > 0)
            fprintf(stderr, "\n%s", diag.summary().c_str());
        return 1;
    }

    printf("Binary:        %s\n", seg->binary_path.c_str());
    printf("Compiler info: %s\n", seg->compiler_info.c_str());
    printf("Arch:          %s\n", Segmentator::to_string(seg->context.arch));
    printf("Mode:          %s\n", Segmentator::to_string(seg->context.mode));
    printf("Symbols:       %zu\n", seg->context.symbols.size());
    printf("RO sections:   %zu\n", seg->context.rodata_sections.size());
    printf("Regions:       %zu (after refine)\n",
           seg->refined_regions.size());
    printf("\n");

    for (size_t i = 0; i < seg->refined_regions.size(); ++i) {
        const auto& r = seg->refined_regions[i];
        printf("  Region [%zu] \"%s\"\n", i, r.getName().c_str());
        printf("    addr: 0x%" PRIx64 "  size: %" PRIu64 "\n",
               r.getAddr(), r.getSize());
        printf("    code: %s\n", hexdump(r.getCode()).c_str());
        if (r.getEnclosingSymbol().has_value())
            printf("    enclosing: %s\n", r.getEnclosingSymbol()->c_str());
        printf("\n");
    }

    // ── Stage 2: Grouping ──
    printf("===========================================================\n");
    printf("  Stage 2: Grouping (%zu groups)\n", seg->groups.size());
    printf("===========================================================\n\n");

    for (size_t gi = 0; gi < seg->groups.size(); ++gi) {
        const auto& g = seg->groups[gi];
        printf("  Group [%zu] \"%s\"  (%zu site%s)\n", gi,
               g.source_name.c_str(), g.sites.size(),
               g.sites.size() == 1 ? "" : "s");

        for (size_t si = 0; si < g.sites.size(); ++si) {
            const auto& s = g.sites[si];
            printf("    [%zu] %s  addr=0x%" PRIx64 "  size=%" PRIu64,
                   si, s.is_canonical ? "CANONICAL" : "inline   ",
                   s.addr, s.size);
            if (s.enclosing_symbol)
                printf("  in %s", s.enclosing_symbol->c_str());
            printf("\n");
        }
        printf("\n");
    }

    // ── Stage 3: Build units ──
    auto units = Serializer::build_units(*seg, diag);

    printf("===========================================================\n");
    printf("  Stage 3: Build units (%zu units)\n", units.size());
    printf("===========================================================\n\n");

    for (size_t i = 0; i < units.size(); ++i) {
        const auto& u = units[i];
        printf("  Unit [%zu] \"%s\"\n", i, u.name.c_str());
        printf("    addr:      0x%" PRIx64 "\n", u.addr);
        printf("    size:      %" PRIu64 "\n", u.size);
        printf("    canonical: %s\n", u.is_canonical ? "yes" : "no");
        printf("    enclosing: %s\n",
               u.enclosing_symbol.empty() ? "(none)" :
               u.enclosing_symbol.c_str());
        printf("    context:   %s\n", u.context ? "present" : "NULL");
        printf("    code:      %s\n", hexdump(u.code).c_str());
        if (!u.data_references.empty())
            printf("    data refs: %zu\n", u.data_references.size());
        printf("\n");
    }

    if (diag.has_errors()) {
        fprintf(stderr, "Build units reported errors - stopping.\n");
        fprintf(stderr, "\n%s", diag.summary().c_str());
        return 1;
    }

    // ── Stage 4: Compilation ──
    BytecodeCompiler::CompileConfig config{opcode_key, false};
    auto backend = BytecodeCompiler::create_backend("simple", config);
    if (!backend) {
        fprintf(stderr, "Failed to create backend\n");
        return 1;
    }

    BytecodeCompiler::CompilationOrchestrator orch(
        std::move(backend), config);
    auto result = orch.compile(units, diag);

    printf("===========================================================\n");
    printf("  Stage 4: Compilation (%s)\n",
           result.has_value() ? "OK" : "FAILED");
    printf("===========================================================\n\n");

    if (!result.has_value()) {
        fprintf(stderr, "Compilation FAILED: %s\n",
                VMPilot::Common::to_string(result.error()));
        if (diag.size() > 0)
            fprintf(stderr, "\n%s", diag.summary().c_str());
        return 1;
    }

    printf("  Total units:  %zu\n", result->total_units);
    printf("  Succeeded:    %zu\n", result->outputs.size());
    printf("  Failed:       %zu\n\n", result->failed_units);

    VMPilot::Common::Instruction instr_helper;

    for (size_t i = 0; i < result->outputs.size(); ++i) {
        const auto& out = result->outputs[i];
        printf("  Output [%zu] \"%s\"  addr=0x%" PRIx64
               "  bytecodes=%zu\n",
               i, out.name.c_str(), out.addr, out.bytecodes.size());

        // Show first few bytecodes in detail
        size_t show = std::min(out.bytecodes.size(), size_t(5));
        for (size_t j = 0; j < show; ++j) {
            const auto& inst = out.bytecodes[j];
            bool valid = instr_helper.check(inst);
            printf("    [%3zu] opcode=0x%04x  L=%" PRIu64
                   "  R=0x%02" PRIx64
                   "  nonce=0x%08x  checksum=0x%04x  %s\n",
                   j, inst.opcode, inst.left_operand,
                   inst.right_operand, inst.nounce,
                   inst.checksum, valid ? "OK" : "FAIL");
        }
        if (out.bytecodes.size() > show)
            printf("    ... %zu more bytecodes\n",
                   out.bytecodes.size() - show);
        printf("\n");
    }

    // ── Diagnostics ──
    if (diag.size() > 0) {
        printf("===========================================================\n");
        printf("  Diagnostics\n");
        printf("===========================================================\n\n");
        printf("%s", diag.summary().c_str());
    }

    return 0;
}
