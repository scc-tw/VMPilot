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
#include <ReferenceAnalyzer.hpp>
#include <Serializer.hpp>
#include <Section.hpp>
#include <capstone.hpp>
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

static const char* refKindStr(Core::DataRefKind k) {
    switch (k) {
        case Core::DataRefKind::Unknown:      return "Unknown";
        case Core::DataRefKind::GlobalVar:    return "GlobalVar";
        case Core::DataRefKind::ReadOnlyData: return "ReadOnly";
        case Core::DataRefKind::GotLoad:      return "GOT";
        case Core::DataRefKind::TlsVar:       return "TLS";
        case Core::DataRefKind::JumpTable:    return "JumpTable";
        default:                               return "?";
    }
}

static const char* refSourceStr(Core::DataRefSource s) {
    switch (s) {
        case Core::DataRefSource::Relocation:   return "reloc";
        case Core::DataRefSource::InsnAnalysis:  return "insn";
        case Core::DataRefSource::PatternMatch:  return "pattern";
        default:                                  return "?";
    }
}

static const char* tlsModelStr(Core::TlsModel m) {
    switch (m) {
        case Core::TlsModel::None:           return "";
        case Core::TlsModel::LocalExec:      return "LocalExec";
        case Core::TlsModel::InitialExec:    return "InitialExec";
        case Core::TlsModel::LocalDynamic:   return "LocalDynamic";
        case Core::TlsModel::GeneralDynamic: return "GeneralDynamic";
        default:                              return "?";
    }
}

static const char* atomicOpStr(Core::AtomicOp op) {
    switch (op) {
        case Core::AtomicOp::None:           return "";
        case Core::AtomicOp::LoadExclusive:  return "LdExcl";
        case Core::AtomicOp::StoreExclusive: return "StExcl";
        case Core::AtomicOp::CompareSwap:    return "CAS";
        case Core::AtomicOp::Swap:           return "Swap";
        case Core::AtomicOp::FetchAdd:       return "FetchAdd";
        case Core::AtomicOp::RMW:            return "RMW";
        case Core::AtomicOp::LoadAcquire:    return "LdAcq";
        case Core::AtomicOp::StoreRelease:   return "StRel";
        case Core::AtomicOp::Fence:          return "Fence";
        default:                              return "?";
    }
}

static const char* atomicOrderStr(Core::AtomicOrdering o) {
    switch (o) {
        case Core::AtomicOrdering::None:    return "";
        case Core::AtomicOrdering::Relaxed: return "relaxed";
        case Core::AtomicOrdering::Acquire: return "acquire";
        case Core::AtomicOrdering::Release: return "release";
        case Core::AtomicOrdering::AcqRel:  return "acq_rel";
        default:                             return "?";
    }
}

static const char* atomicWidthStr(Core::AtomicWidth w) {
    switch (w) {
        case Core::AtomicWidth::None:      return "";
        case Core::AtomicWidth::Atomic8:   return "8";
        case Core::AtomicWidth::Atomic16:  return "16";
        case Core::AtomicWidth::Atomic32:  return "32";
        case Core::AtomicWidth::Atomic64:  return "64";
        case Core::AtomicWidth::Atomic128: return "128";
        default:                            return "?";
    }
}

static const char* sectionKindStr(Core::SectionKind k) {
    switch (k) {
        case Core::SectionKind::Text:    return "Text";
        case Core::SectionKind::Rodata:  return "Rodata";
        case Core::SectionKind::Data:    return "Data";
        case Core::SectionKind::Bss:     return "Bss";
        case Core::SectionKind::Tls:     return "Tls";
        case Core::SectionKind::Got:     return "Got";
        case Core::SectionKind::Plt:     return "Plt";
        case Core::SectionKind::Unknown: return "Unknown";
        default:                          return "?";
    }
}

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
    printf("Sections:      %zu\n", seg->context.sections.size());
    printf("Regions:       %zu (after refine)\n",
           seg->refined_regions.size());
    printf("\n");

    // Section map — critical for understanding ReferenceAnalyzer classification
    if (!seg->context.sections.empty()) {
        printf("  Section Map (what ReferenceAnalyzer sees):\n");
        for (const auto& sec : seg->context.sections) {
            printf("    0x%" PRIx64 " - 0x%" PRIx64 "  %-7s  %s\n",
                   sec.base_addr, sec.base_addr + sec.size,
                   sectionKindStr(sec.kind), sec.name.c_str());
        }
        printf("\n");
    }

    // Set up capstone for disassembly
    Capstone::Arch cs_arch = Capstone::Arch::X86;
    Capstone::Mode cs_mode = Capstone::Mode::MODE_64;
    using FileArch = VMPilot::Common::FileArch;
    using FileMode = VMPilot::Common::FileMode;
    if (seg->context.arch == FileArch::ARM64) {
        cs_arch = Capstone::Arch::ARM64;
        cs_mode = Capstone::Mode::MODE_ARM;
    } else if (seg->context.mode == FileMode::MODE_32) {
        cs_mode = Capstone::Mode::MODE_32;
    }
    Capstone::Capstone cs(cs_arch, cs_mode);

    for (size_t i = 0; i < seg->refined_regions.size(); ++i) {
        const auto& r = seg->refined_regions[i];
        printf("  Region [%zu] \"%s\"\n", i, r.getName().c_str());
        printf("    addr: 0x%" PRIx64 "  size: %" PRIu64 "\n",
               r.getAddr(), r.getSize());
        if (r.getEnclosingSymbol().has_value())
            printf("    enclosing: %s\n", r.getEnclosingSymbol()->c_str());

        // Disassembly of the protected region
        auto insns = cs.disasm(r.getCode(), r.getAddr());
        printf("    disasm: (%zu insns)\n", insns.size());
        for (const auto& insn : insns) {
            printf("      0x%" PRIx64 ":  %-8s %s\n",
                   insn.address, insn.mnemonic.c_str(),
                   insn.op_str.c_str());
        }
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

    // ── Stage 3: Build units + reference analysis ──
    auto units = Serializer::build_units(*seg, diag);

    // Run reference analysis (same as compile_pipeline.cpp step 2.5)
    for (auto& unit : units) {
        auto unit_insns = cs.disasm(unit.code, unit.addr);
        unit.data_references = ReferenceAnalyzer::analyze(
            unit_insns, unit.addr, unit.size,
            unit.context ? unit.context->sections
                         : std::vector<Core::Section>{},
            seg->text_relocations,
            unit.context ? unit.context->symbols
                         : Segmentator::NativeSymbolTable{},
            seg->context.arch, seg->context.mode);
    }

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
        printf("    data refs: %zu\n", u.data_references.size());
        for (size_t j = 0; j < u.data_references.size(); ++j) {
            const auto& ref = u.data_references[j];
            printf("      [%zu] 0x%" PRIx64 " → 0x%" PRIx64
                   "  %-9s  src=%-7s  %uB  %s%s",
                   j, ref.insn_offset, ref.target_va,
                   refKindStr(ref.kind), refSourceStr(ref.source),
                   ref.access_size,
                   ref.is_write ? "W" : "R",
                   ref.is_pc_relative ? " PC-rel" : "");
            if (!ref.target_symbol.empty())
                printf("  sym=%s", ref.target_symbol.c_str());
            if (ref.tls_model != Core::TlsModel::None)
                printf("  tls=%s", tlsModelStr(ref.tls_model));
            if (ref.atomic_op != Core::AtomicOp::None) {
                printf("  atomic{%s %s",
                       atomicOpStr(ref.atomic_op),
                       atomicOrderStr(ref.atomic_ordering));
                if (ref.atomic_width != Core::AtomicWidth::None)
                    printf(" %sbit", atomicWidthStr(ref.atomic_width));
                printf("}");
            }
            if (ref.jump_table.has_value()) {
                printf("  jt{base=0x%" PRIx64 " %u×%uB %s %zu targets}",
                       ref.jump_table->table_base,
                       ref.jump_table->entry_count,
                       ref.jump_table->entry_size,
                       ref.jump_table->relative_entries ? "rel" : "abs",
                       ref.jump_table->targets.size());
            }
            printf("\n");
        }
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
