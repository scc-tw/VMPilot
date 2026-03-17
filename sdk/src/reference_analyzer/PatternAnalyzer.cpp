#include "PatternAnalyzer.hpp"

#include <capstone/x86.h>

#include <algorithm>
#include <unordered_map>

namespace VMPilot::SDK::ReferenceAnalyzer {

namespace {

// Segment register IDs
constexpr unsigned kX86_REG_FS = X86_REG_FS;
constexpr unsigned kX86_REG_GS = X86_REG_GS;

using AddrToName = std::unordered_map<uint64_t, std::string>;

void detectX86TlsPatterns(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const AddrToName& sym_lookup,
    std::vector<Core::DataReference>& refs) {
    const uint64_t region_end = region_addr + region_size;

    for (size_t idx = 0; idx < insns.size(); ++idx) {
        const auto& insn = insns[idx];
        if (insn.address < region_addr || insn.address >= region_end)
            continue;

        // Pattern 1: fs: or gs: segment override → TLS LocalExec
        for (const auto& op : insn.operands) {
            if (op.type != Capstone::OpType::MEM)
                continue;

            if (op.mem.segment == kX86_REG_FS ||
                op.mem.segment == kX86_REG_GS) {
                Core::DataReference ref;
                ref.insn_offset = insn.address;
                ref.target_va = op.mem.disp;  // offset from TLS base
                ref.kind = Core::DataRefKind::TlsVar;
                ref.tls_model = Core::TlsModel::LocalExec;
                ref.source = Core::DataRefSource::PatternMatch;
                ref.access_size = op.size;
                refs.push_back(std::move(ref));
            }
        }

        // Pattern 2: call __tls_get_addr → General Dynamic
        if (insn.isCall()) {
            uint64_t target = insn.getDirectTarget();
            if (target == 0)
                target = insn.getRipRelativeTarget();

            if (target != 0) {
                auto it = sym_lookup.find(target);
                if (it != sym_lookup.end() &&
                    it->second == "__tls_get_addr") {
                    Core::DataReference ref;
                    ref.insn_offset = insn.address;
                    ref.kind = Core::DataRefKind::TlsVar;
                    ref.tls_model = Core::TlsModel::GeneralDynamic;
                    ref.source = Core::DataRefSource::PatternMatch;

                    // Look backward for preceding lea rdi, [rip+X@TLSGD]
                    // to capture the target symbol
                    if (idx > 0) {
                        const auto& prev = insns[idx - 1];
                        if (prev.mnemonic == "lea" &&
                            prev.operands.size() == 2 &&
                            prev.operands[1].type == Capstone::OpType::MEM &&
                            prev.operands[1].mem.isRipRelative()) {
                            ref.target_va = prev.address + prev.size +
                                            prev.operands[1].mem.disp;
                        }
                    }

                    refs.push_back(std::move(ref));
                }
            }
        }
    }
}

void detectARM64TlsPatterns(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const AddrToName& sym_lookup,
    std::vector<Core::DataReference>& refs) {
    const uint64_t region_end = region_addr + region_size;

    for (size_t idx = 0; idx < insns.size(); ++idx) {
        const auto& insn = insns[idx];
        if (insn.address < region_addr || insn.address >= region_end)
            continue;

        // Pattern: mrs Xn, TPIDR_EL0 → TLS LocalExec
        if (insn.mnemonic == "mrs") {
            // Check op_str for tpidr_el0
            std::string lower_ops = insn.op_str;
            std::transform(lower_ops.begin(), lower_ops.end(),
                           lower_ops.begin(), ::tolower);
            if (lower_ops.find("tpidr_el0") != std::string::npos) {
                Core::DataReference ref;
                ref.insn_offset = insn.address;
                ref.kind = Core::DataRefKind::TlsVar;
                ref.tls_model = Core::TlsModel::LocalExec;
                ref.source = Core::DataRefSource::PatternMatch;
                refs.push_back(std::move(ref));
            }
        }

        // Pattern: call __tls_get_addr (ARM64: bl __tls_get_addr)
        if (insn.isCall()) {
            uint64_t target = insn.getDirectTarget();
            if (target != 0) {
                auto it = sym_lookup.find(target);
                if (it != sym_lookup.end() &&
                    it->second == "__tls_get_addr") {
                    Core::DataReference ref;
                    ref.insn_offset = insn.address;
                    ref.kind = Core::DataRefKind::TlsVar;
                    ref.tls_model = Core::TlsModel::GeneralDynamic;
                    ref.source = Core::DataRefSource::PatternMatch;
                    refs.push_back(std::move(ref));
                }
            }
        }
    }

    // TODO: Jump table detection — detect indirect jumps through .rodata
    // TODO: Atomic detection — need Instruction to expose x86 prefix bytes
}

}  // namespace

std::vector<Core::DataReference> analyzePatterns(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const Segmentator::NativeSymbolTable& symbols,
    Segmentator::Arch arch, Segmentator::Mode /*mode*/) noexcept {
    std::vector<Core::DataReference> refs;

    AddrToName sym_lookup;
    for (const auto& sym : symbols) {
        if (!sym.name.empty() && sym.address != 0)
            sym_lookup[sym.address] = sym.name;
    }

    using FileArch = VMPilot::Common::FileArch;
    if (arch == FileArch::X86) {
        detectX86TlsPatterns(insns, region_addr, region_size, sym_lookup, refs);
    } else if (arch == FileArch::ARM64) {
        detectARM64TlsPatterns(insns, region_addr, region_size, sym_lookup,
                               refs);
    }

    return refs;
}

}  // namespace VMPilot::SDK::ReferenceAnalyzer
