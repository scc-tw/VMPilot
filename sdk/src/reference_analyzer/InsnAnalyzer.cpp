#include "InsnAnalyzer.hpp"
#include "SectionLookup.hpp"

#include <ARM64Traits.hpp>
#include <RegValueResolver.hpp>
#include <X86Traits.hpp>
#include <x86.hpp>

#include <capstone/x86.h>

#include <unordered_map>

namespace VMPilot::SDK::ReferenceAnalyzer {

namespace {

// Stack register IDs (from capstone x86.h)
constexpr unsigned kX86_RSP = X86_REG_RSP;
constexpr unsigned kX86_RBP = X86_REG_RBP;
constexpr unsigned kX86_ESP = X86_REG_ESP;
constexpr unsigned kX86_EBP = X86_REG_EBP;
constexpr unsigned kX86_SP = X86_REG_SP;
constexpr unsigned kX86_BP = X86_REG_BP;

bool isStackReg(unsigned reg) {
    return reg == kX86_RSP || reg == kX86_RBP || reg == kX86_ESP ||
           reg == kX86_EBP || reg == kX86_SP || reg == kX86_BP;
}

bool isLEA(const Capstone::Instruction& insn) {
    return insn.mnemonic == "lea";
}

Core::DataRefKind classifyBySection(Core::SectionKind sk) {
    switch (sk) {
        case Core::SectionKind::Data:
        case Core::SectionKind::Bss:
            return Core::DataRefKind::GlobalVar;
        case Core::SectionKind::Rodata:
            return Core::DataRefKind::ReadOnlyData;
        case Core::SectionKind::Got:
            return Core::DataRefKind::GotLoad;
        case Core::SectionKind::Tls:
            return Core::DataRefKind::TlsVar;
        default:
            return Core::DataRefKind::Unknown;
    }
}

/// Determine if a memory operand is a write (destination)
bool isMemWrite(const Capstone::Instruction& insn, size_t op_idx) {
    // For most x86 instructions, operand 0 is destination
    // Exceptions: CMP, TEST, PUSH are read-only on their first operand
    if (insn.mnemonic == "cmp" || insn.mnemonic == "test" ||
        insn.mnemonic == "push")
        return false;

    // For two-operand instructions, op 0 is typically the destination
    return op_idx == 0;
}

/// Build symbol address lookup
using AddrToName = std::unordered_map<uint64_t, std::string>;

AddrToName buildSymbolLookup(
    const Segmentator::NativeSymbolTable& symbols) {
    AddrToName result;
    for (const auto& sym : symbols) {
        if (!sym.name.empty() && sym.address != 0)
            result[sym.address] = sym.name;
    }
    return result;
}

void analyzeX86Insns(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const SectionLookup& sections,
    const AddrToName& sym_lookup,
    std::vector<Core::DataReference>& refs) {
    const uint64_t region_end = region_addr + region_size;

    for (size_t idx = 0; idx < insns.size(); ++idx) {
        const auto& insn = insns[idx];

        // Only consider instructions within our region
        if (insn.address < region_addr || insn.address >= region_end)
            continue;

        for (size_t op_idx = 0; op_idx < insn.operands.size(); ++op_idx) {
            const auto& op = insn.operands[op_idx];
            if (op.type != Capstone::OpType::MEM)
                continue;

            // Skip LEA — just address computation, not a memory access
            if (isLEA(insn))
                continue;

            // Skip stack-relative
            if (isStackReg(op.mem.base))
                continue;

            uint64_t target_va = 0;
            bool resolved = false;

            // RIP-relative: target = insn.address + insn.size + disp
            if (op.mem.isRipRelative()) {
                target_va = insn.address + insn.size + op.mem.disp;
                resolved = true;
            }
            // Register-indirect: try to resolve base register
            else if (op.mem.base != 0 && op.mem.index == 0) {
                size_t from = idx > 0 ? idx - 1 : 0;
                uint64_t base_val =
                    Segmentator::resolveRegValue<Segmentator::X86ArchTraits>(
                        op.mem.base, from, insns, 0);
                if (base_val != 0) {
                    target_va = base_val + op.mem.disp;
                    resolved = true;
                }
            }

            if (!resolved)
                continue;

            // Classify by section
            auto sk = sections.classify(target_va);
            if (sk == Core::SectionKind::Text ||
                sk == Core::SectionKind::Plt ||
                sk == Core::SectionKind::Unknown)
                continue;

            Core::DataReference ref;
            ref.insn_offset = insn.address;
            ref.target_va = target_va;
            ref.kind = classifyBySection(sk);
            ref.source = Core::DataRefSource::InsnAnalysis;
            ref.access_size = op.size;
            ref.is_write = isMemWrite(insn, op_idx);
            ref.is_pc_relative = op.mem.isRipRelative();

            // Try symbol lookup
            auto sym_it = sym_lookup.find(target_va);
            if (sym_it != sym_lookup.end())
                ref.target_symbol = sym_it->second;

            refs.push_back(std::move(ref));
        }
    }
}

void analyzeARM64Insns(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const SectionLookup& sections,
    const AddrToName& sym_lookup,
    std::vector<Core::DataReference>& refs) {
    const uint64_t region_end = region_addr + region_size;

    for (size_t idx = 0; idx < insns.size(); ++idx) {
        const auto& insn = insns[idx];

        if (insn.address < region_addr || insn.address >= region_end)
            continue;

        // Skip instructions without MEM operand
        bool has_mem = false;
        for (const auto& op : insn.operands) {
            if (op.type == Capstone::OpType::MEM) {
                has_mem = true;
                break;
            }
        }
        if (!has_mem)
            continue;

        // Skip stack-relative (SP = X31)
        for (size_t op_idx = 0; op_idx < insn.operands.size(); ++op_idx) {
            const auto& op = insn.operands[op_idx];
            if (op.type != Capstone::OpType::MEM)
                continue;

            // ARM64 SP register is typically reg 31 in capstone
            // Skip if base is SP (stack pointer)
            // We use the capstone ARM64 register ID for SP
            constexpr unsigned ARM64_REG_SP_ID = 2;  // ARM64_REG_SP
            if (op.mem.base == ARM64_REG_SP_ID)
                continue;

            // Try to resolve base register via ADRP+ADD pattern
            uint64_t target_va = 0;
            bool resolved = false;

            if (op.mem.base != 0 && idx > 0) {
                size_t from = idx > 0 ? idx - 1 : 0;
                uint64_t base_val =
                    Segmentator::resolveRegValue<Segmentator::ARM64ArchTraits>(
                        op.mem.base, from, insns, 0);
                if (base_val != 0) {
                    target_va = base_val + op.mem.disp;
                    resolved = true;
                }
            }

            if (!resolved)
                continue;

            auto sk = sections.classify(target_va);
            if (sk == Core::SectionKind::Text ||
                sk == Core::SectionKind::Plt ||
                sk == Core::SectionKind::Unknown)
                continue;

            Core::DataReference ref;
            ref.insn_offset = insn.address;
            ref.target_va = target_va;
            ref.kind = classifyBySection(sk);
            ref.source = Core::DataRefSource::InsnAnalysis;
            ref.access_size = op.size;
            // ARM64: For store instructions (str), operand 1 is the MEM dest
            ref.is_write =
                (insn.mnemonic.substr(0, 3) == "str" ||
                 insn.mnemonic.substr(0, 3) == "stp");

            auto sym_it = sym_lookup.find(target_va);
            if (sym_it != sym_lookup.end())
                ref.target_symbol = sym_it->second;

            refs.push_back(std::move(ref));
        }
    }
}

}  // namespace

std::vector<Core::DataReference> analyzeInstructions(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const SectionLookup& sections,
    const Segmentator::NativeSymbolTable& symbols,
    Segmentator::Arch arch, Segmentator::Mode /*mode*/) noexcept {
    std::vector<Core::DataReference> refs;

    auto sym_lookup = buildSymbolLookup(symbols);

    using FileArch = VMPilot::Common::FileArch;
    if (arch == FileArch::X86) {
        analyzeX86Insns(insns, region_addr, region_size, sections, sym_lookup,
                        refs);
    } else if (arch == FileArch::ARM64) {
        analyzeARM64Insns(insns, region_addr, region_size, sections, sym_lookup,
                          refs);
    }

    return refs;
}

}  // namespace VMPilot::SDK::ReferenceAnalyzer
