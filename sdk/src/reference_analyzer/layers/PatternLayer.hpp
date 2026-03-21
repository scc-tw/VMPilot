#pragma once

#include "SymExpr.hpp"
#include "SymbolicResolver.hpp"
#include "SectionLookup.hpp"

#include <ARM64Traits.hpp>
#include <ModeEnum.hpp>
#include <DataReference.hpp>
#include <NativeSymbolTable.hpp>
#include <ReadOnlySection.hpp>
#include <X86Traits.hpp>
#include <capstone.hpp>

#include <capstone/arm64.h>
#include <capstone/x86.h>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace VMPilot::SDK::ReferenceAnalyzer {

// ============================================================================
// Jump table helpers (inline, copied from PatternAnalyzer.cpp for Phase 4d)
// ============================================================================

/// Read bytes from rodata_sections at a given VA.
/// Returns true if the read succeeded.
inline bool readRodataBytes(
    const std::vector<Segmentator::ReadOnlySection>& rodata_sections,
    uint64_t va, void* buf, size_t len) {
    for (const auto& sec : rodata_sections) {
        if (!sec.contains(va))
            continue;
        uint64_t end_va = va + len;
        if (end_va > sec.base_addr + sec.data.size())
            return false;
        size_t offset = static_cast<size_t>(va - sec.base_addr);
        std::memcpy(buf, sec.data.data() + offset, len);
        return true;
    }
    return false;
}

/// Resolve jump table entries from rodata, validating each target is in .text.
inline Core::JumpTableRef resolveJumpTableEntries(
    uint64_t table_base, uint32_t entry_size, bool relative_entries,
    uint32_t entry_count_hint,
    const std::vector<Segmentator::ReadOnlySection>& rodata_sections,
    const SectionLookup& sections) {
    Core::JumpTableRef jt;
    jt.table_base = table_base;
    jt.entry_size = entry_size;
    jt.relative_entries = relative_entries;

    const uint32_t max_entries =
        (entry_count_hint > 0) ? entry_count_hint : 1024;

    for (uint32_t i = 0; i < max_entries; ++i) {
        uint64_t entry_va = table_base + static_cast<uint64_t>(i) * entry_size;
        uint64_t target = 0;

        if (entry_size == 4) {
            uint32_t raw = 0;
            if (!readRodataBytes(rodata_sections, entry_va, &raw, 4))
                break;
            if (relative_entries) {
                auto signed_val = static_cast<int32_t>(raw);
                target = table_base + static_cast<uint64_t>(
                             static_cast<int64_t>(signed_val));
            } else {
                target = raw;
            }
        } else if (entry_size == 8) {
            uint64_t raw = 0;
            if (!readRodataBytes(rodata_sections, entry_va, &raw, 8))
                break;
            if (relative_entries) {
                auto signed_val = static_cast<int64_t>(raw);
                target = table_base + static_cast<uint64_t>(signed_val);
            } else {
                target = raw;
            }
        } else {
            break;
        }

        if (sections.classify(target) != Core::SectionKind::Text)
            break;

        jt.targets.push_back(target);
    }

    jt.entry_count = static_cast<uint32_t>(jt.targets.size());
    return jt;
}

/// Look backward from insn_idx for a bounds check pattern:
///   cmp reg, imm  followed by  ja/jae (x86) or b.hi (ARM64)
/// Returns (imm + 1) as the entry count, or 0 if not found.
inline uint32_t findBoundsCheck(
    const std::vector<Capstone::Instruction>& insns,
    size_t insn_idx, bool is_arm64) {
    const size_t limit = (insn_idx > 10) ? insn_idx - 10 : 0;
    for (size_t i = insn_idx; i > limit; --i) {
        size_t check = i - 1;
        const auto& insn = insns[check];

        if (is_arm64) {
            if (insn.mnemonic == "cmp" && insn.operands.size() >= 2 &&
                insn.operands[1].type == Capstone::OpType::IMM) {
                if (check + 1 < insns.size()) {
                    const auto& next = insns[check + 1];
                    if (next.mnemonic == "b.hi" || next.mnemonic == "b.hs") {
                        return static_cast<uint32_t>(insn.operands[1].imm) + 1;
                    }
                }
            }
        } else {
            if (insn.mnemonic == "cmp" && insn.operands.size() >= 2 &&
                insn.operands[1].type == Capstone::OpType::IMM) {
                if (check + 1 < insns.size()) {
                    const auto& next = insns[check + 1];
                    if (next.mnemonic == "ja" || next.mnemonic == "jae") {
                        return static_cast<uint32_t>(insn.operands[1].imm) + 1;
                    }
                }
            }
        }
    }
    return 0;
}

/// Resolve byte-offset jump table entries (Apple Clang ARM64 variant).
/// Each entry is a 1-byte unsigned offset; target = case_base + entry * shift.
inline Core::JumpTableRef resolveByteOffsetJumpTable(
    uint64_t table_base, uint64_t case_base, unsigned shift,
    uint32_t entry_count_hint,
    const std::vector<Segmentator::ReadOnlySection>& rodata_sections,
    const SectionLookup& sections) {
    Core::JumpTableRef jt;
    jt.table_base = table_base;
    jt.entry_size = 1;
    jt.relative_entries = true;

    const uint32_t max_entries =
        (entry_count_hint > 0) ? entry_count_hint : 1024;

    for (uint32_t i = 0; i < max_entries; ++i) {
        uint8_t raw = 0;
        if (!readRodataBytes(rodata_sections, table_base + i, &raw, 1))
            break;
        uint64_t target = case_base + (static_cast<uint64_t>(raw) << shift);
        if (sections.classify(target) != Core::SectionKind::Text)
            break;
        jt.targets.push_back(target);
    }

    jt.entry_count = static_cast<uint32_t>(jt.targets.size());
    return jt;
}

// ============================================================================
// X86 Jump Table Detection (copied from PatternAnalyzer.cpp, Phase 4d)
// ============================================================================

inline void detectX86JumpTablePatterns(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const SectionLookup& sections,
    const std::vector<Segmentator::ReadOnlySection>& rodata_sections,
    bool is_64bit,
    std::vector<Core::DataReference>& refs) {
    const uint64_t region_end = region_addr + region_size;

    for (size_t idx = 0; idx < insns.size(); ++idx) {
        const auto& insn = insns[idx];
        if (insn.address < region_addr || insn.address >= region_end)
            continue;
        if (!insn.isJump())
            continue;

        // Pattern A: jmp [mem] with index register (indexed memory jump)
        if (insn.operands.size() == 1 &&
            insn.operands[0].type == Capstone::OpType::MEM &&
            insn.operands[0].mem.index != 0) {
            const auto& mem = insn.operands[0].mem;

            uint64_t table_base = 0;
            uint32_t entry_size = 0;
            bool resolved = false;

            if (is_64bit && mem.isRipRelative()) {
                table_base = insn.address + insn.size + mem.disp;
                entry_size = static_cast<uint32_t>(mem.scale > 0 ? mem.scale
                                                                  : 8);
                resolved = true;
            } else if (!is_64bit && mem.base == 0) {
                table_base = static_cast<uint64_t>(
                    static_cast<uint32_t>(mem.disp));
                entry_size = static_cast<uint32_t>(mem.scale > 0 ? mem.scale
                                                                  : 4);
                resolved = true;
            }

            if (resolved &&
                sections.classify(table_base) == Core::SectionKind::Rodata) {
                uint32_t count = findBoundsCheck(insns, idx, false);

                auto jt = resolveJumpTableEntries(
                    table_base, entry_size, /*relative_entries=*/false, count,
                    rodata_sections, sections);

                if (!jt.targets.empty()) {
                    Core::DataReference ref;
                    ref.insn_offset = insn.address;
                    ref.target_va = table_base;
                    ref.kind = Core::DataRefKind::JumpTable;
                    ref.source = Core::DataRefSource::PatternMatch;
                    ref.is_pc_relative = mem.isRipRelative();
                    ref.jump_table = std::move(jt);
                    refs.push_back(std::move(ref));
                }
            }
            continue;
        }

        // Pattern B: jmp reg (x86-64, relative entries via LEA+movsxd+add)
        if (is_64bit && insn.operands.size() == 1 &&
            insn.operands[0].type == Capstone::OpType::REG) {
            uint64_t table_base = 0;
            const size_t limit = (idx > 10) ? idx - 10 : 0;
            for (size_t i = idx; i > limit; --i) {
                const auto& prev = insns[i - 1];
                if (prev.mnemonic == "lea" && prev.operands.size() == 2 &&
                    prev.operands[1].type == Capstone::OpType::MEM &&
                    prev.operands[1].mem.isRipRelative()) {
                    uint64_t candidate =
                        prev.address + prev.size + prev.operands[1].mem.disp;
                    if (sections.classify(candidate) ==
                        Core::SectionKind::Rodata) {
                        table_base = candidate;
                        break;
                    }
                }
            }

            if (table_base != 0) {
                uint32_t count = findBoundsCheck(insns, idx, false);
                auto jt = resolveJumpTableEntries(
                    table_base, /*entry_size=*/4, /*relative_entries=*/true,
                    count, rodata_sections, sections);

                if (!jt.targets.empty()) {
                    Core::DataReference ref;
                    ref.insn_offset = insn.address;
                    ref.target_va = table_base;
                    ref.kind = Core::DataRefKind::JumpTable;
                    ref.source = Core::DataRefSource::PatternMatch;
                    ref.is_pc_relative = true;
                    ref.jump_table = std::move(jt);
                    refs.push_back(std::move(ref));
                }
            }
        }
    }
}

// ============================================================================
// ARM64 Jump Table Detection (copied from PatternAnalyzer.cpp, Phase 4d)
// ============================================================================

inline void detectARM64JumpTablePatterns(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const SectionLookup& sections,
    const std::vector<Segmentator::ReadOnlySection>& rodata_sections,
    std::vector<Core::DataReference>& refs) {
    const uint64_t region_end = region_addr + region_size;

    for (size_t idx = 0; idx < insns.size(); ++idx) {
        const auto& insn = insns[idx];
        if (insn.address < region_addr || insn.address >= region_end)
            continue;

        if (insn.mnemonic != "br")
            continue;

        uint64_t table_base = 0;
        const size_t limit = (idx > 10) ? idx - 10 : 0;

        // Pattern A: Apple Clang byte-offset jump table
        {
            uint64_t case_base = 0;
            unsigned shift = 2;

            for (size_t i = idx; i > limit; --i) {
                const auto& prev = insns[i - 1];

                if (prev.mnemonic == "adr" && prev.operands.size() >= 2 &&
                    prev.operands[0].type == Capstone::OpType::REG &&
                    prev.operands[1].type == Capstone::OpType::IMM) {
                    case_base =
                        static_cast<uint64_t>(prev.operands[1].imm);
                }

                if (prev.mnemonic == "add" && prev.operands.size() >= 3 &&
                    prev.operands[0].type == Capstone::OpType::REG &&
                    prev.operands[2].type == Capstone::OpType::REG) {
                    auto pos = prev.op_str.find("lsl #");
                    if (pos != std::string::npos) {
                        shift = static_cast<unsigned>(
                            std::strtoul(prev.op_str.c_str() + pos + 5,
                                         nullptr, 10));
                    }
                }

                if (prev.mnemonic == "add" && prev.operands.size() >= 3 &&
                    prev.operands[2].type == Capstone::OpType::IMM &&
                    i >= 2) {
                    const auto& prev2 = insns[i - 2];
                    if (prev2.mnemonic == "adrp" &&
                        prev2.operands.size() >= 2 &&
                        prev2.operands[0].reg == prev.operands[1].reg) {
                        uint64_t candidate =
                            static_cast<uint64_t>(prev2.operands[1].imm) +
                            static_cast<uint64_t>(prev.operands[2].imm);
                        if (sections.classify(candidate) ==
                            Core::SectionKind::Rodata) {
                            table_base = candidate;
                        }
                    }
                }
            }

            if (table_base != 0 && case_base != 0) {
                uint32_t count = findBoundsCheck(insns, idx, true);
                auto jt = resolveByteOffsetJumpTable(
                    table_base, case_base, shift, count,
                    rodata_sections, sections);

                if (!jt.targets.empty()) {
                    Core::DataReference ref;
                    ref.insn_offset = insn.address;
                    ref.target_va = table_base;
                    ref.kind = Core::DataRefKind::JumpTable;
                    ref.source = Core::DataRefSource::PatternMatch;
                    ref.jump_table = std::move(jt);
                    refs.push_back(std::move(ref));
                }
                continue;
            }
        }

        // Pattern B: GCC/Linux ARM64 4-byte relative offset
        for (size_t i = idx; i > limit; --i) {
            const auto& prev = insns[i - 1];
            if (prev.mnemonic == "add" && prev.operands.size() >= 3 &&
                prev.operands[2].type == Capstone::OpType::IMM && i >= 2) {
                const auto& prev2 = insns[i - 2];
                if (prev2.mnemonic == "adrp" && prev2.operands.size() >= 2 &&
                    prev2.operands[0].reg == prev.operands[1].reg) {
                    uint64_t candidate =
                        static_cast<uint64_t>(prev2.operands[1].imm) +
                        static_cast<uint64_t>(prev.operands[2].imm);
                    if (sections.classify(candidate) ==
                        Core::SectionKind::Rodata) {
                        table_base = candidate;
                        break;
                    }
                }
            }
        }

        if (table_base == 0)
            continue;

        uint32_t count = findBoundsCheck(insns, idx, true);
        auto jt = resolveJumpTableEntries(
            table_base, /*entry_size=*/4, /*relative_entries=*/true, count,
            rodata_sections, sections);

        if (!jt.targets.empty()) {
            Core::DataReference ref;
            ref.insn_offset = insn.address;
            ref.target_va = table_base;
            ref.kind = Core::DataRefKind::JumpTable;
            ref.source = Core::DataRefSource::PatternMatch;
            ref.jump_table = std::move(jt);
            refs.push_back(std::move(ref));
        }
    }
}

// ============================================================================
// Main entry point: analyzePatterns (template version)
// ============================================================================

/// Analyze instruction patterns using symbolic resolution.
/// Architecture-independent TLS detection uses resolveSymbolic + SymExpr
/// predicates. Jump table detection uses architecture dispatch via
/// if constexpr on ArchTraits.
template <typename ArchTraits>
std::vector<Core::DataReference> analyzePatterns(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const Segmentator::NativeSymbolTable& symbols,
    const SectionLookup& sections,
    const std::vector<Segmentator::ReadOnlySection>& rodata_sections,
    Segmentator::Mode mode = Segmentator::Mode::MODE_64) noexcept {
    std::vector<Core::DataReference> refs;
    const uint64_t region_end = region_addr + region_size;

    // Build symbol lookup once
    std::unordered_map<uint64_t, std::string> addr_to_name;
    for (const auto& sym : symbols) {
        if (!sym.name.empty() && sym.address != 0)
            addr_to_name[sym.address] = sym.name;
    }

    // Symbol lookup function for resolveSymbolic
    SymbolLookup sym_lookup = [&](uint64_t addr) -> std::optional<std::string> {
        auto it = addr_to_name.find(addr);
        if (it != addr_to_name.end())
            return it->second;
        return std::nullopt;
    };

    for (size_t idx = 0; idx < insns.size(); ++idx) {
        const auto& insn = insns[idx];
        if (insn.address < region_addr || insn.address >= region_end)
            continue;

        // =============================================================
        // TLS Detection (SymExpr-based, architecture-independent)
        // =============================================================

        // Check memory operands for segment/system register provenance
        bool tls_found = false;
        for (const auto& op : insn.operands) {
            if (op.type != Capstone::OpType::MEM) continue;

            // x86 segment override (FS/GS) -> TLS LocalExec
            // These may have base=0 (e.g., mov eax, fs:[0x28])
            if (op.mem.segment == X86_REG_FS ||
                op.mem.segment == X86_REG_GS) {
                Core::DataReference ref;
                ref.insn_offset = insn.address;
                ref.target_va = static_cast<uint64_t>(op.mem.disp);
                ref.kind = Core::DataRefKind::TlsVar;
                ref.tls_model = Core::TlsModel::LocalExec;
                ref.source = Core::DataRefSource::PatternMatch;
                ref.access_size = op.size;
                refs.push_back(std::move(ref));
                tls_found = true;
                break;
            }

            // For operands with a base register, use resolveSymbolic
            // to check for system register provenance (ARM64 TPIDR_EL0)
            if (op.mem.base != 0) {
                auto addr_expr = resolveSymbolic<ArchTraits>(
                    op.mem.base, idx > 0 ? idx - 1 : 0,
                    insns, 0, sym_lookup);

                // Segment base from resolveSymbolic (x86 base-register
                // chains involving fs/gs)
                if (addr_expr.involvesOpaque(
                        SymExpr::OpaqueKind::SegmentBase)) {
                    Core::DataReference ref;
                    ref.insn_offset = insn.address;
                    ref.target_va = static_cast<uint64_t>(op.mem.disp);
                    ref.kind = Core::DataRefKind::TlsVar;
                    ref.tls_model = Core::TlsModel::LocalExec;
                    ref.source = Core::DataRefSource::PatternMatch;
                    ref.access_size = op.size;
                    refs.push_back(std::move(ref));
                    tls_found = true;
                    break;
                }

                // System register (ARM64 TPIDR_EL0) -> TLS LocalExec
                if (addr_expr.involvesOpaque(
                        SymExpr::OpaqueKind::SystemReg)) {
                    Core::DataReference ref;
                    ref.insn_offset = insn.address;
                    ref.kind = Core::DataRefKind::TlsVar;
                    ref.tls_model = Core::TlsModel::LocalExec;
                    ref.source = Core::DataRefSource::PatternMatch;
                    refs.push_back(std::move(ref));
                    tls_found = true;
                    break;
                }
            }
        }
        if (tls_found) continue;

        // Call-based TLS: __tls_get_addr or macOS TLV resolver
        if (insn.isCall() || insn.mnemonic == "blr") {
            // Resolve the call target
            uint64_t direct_target = insn.getDirectTarget();
            if (direct_target == 0)
                direct_target = insn.getRipRelativeTarget();

            // Check for __tls_get_addr (direct call)
            if (direct_target != 0) {
                auto it = addr_to_name.find(direct_target);
                if (it != addr_to_name.end() &&
                    it->second == "__tls_get_addr") {
                    Core::DataReference ref;
                    ref.insn_offset = insn.address;
                    ref.kind = Core::DataRefKind::TlsVar;
                    ref.tls_model = Core::TlsModel::GeneralDynamic;
                    ref.source = Core::DataRefSource::PatternMatch;

                    // Try to find the target VA from the preceding LEA
                    // (x86) or ADRP+ADD (ARM64)
                    if (idx > 0) {
                        const auto& prev = insns[idx - 1];
                        if (prev.operands.size() == 2 &&
                            prev.operands[1].type == Capstone::OpType::MEM &&
                            prev.operands[1].mem.isRipRelative()) {
                            ref.target_va = prev.address + prev.size +
                                            prev.operands[1].mem.disp;
                        }
                    }

                    refs.push_back(std::move(ref));
                    continue;
                }
            }

            // macOS TLV resolver: blr xN where xN = load from .tls section
            if (insn.mnemonic == "blr" && !insn.operands.empty() &&
                insn.operands[0].type == Capstone::OpType::REG) {
                auto call_expr = resolveSymbolic<ArchTraits>(
                    insn.operands[0].reg, idx > 0 ? idx - 1 : 0,
                    insns, 0, sym_lookup);

                if (call_expr.involvesLoad()) {
                    auto* load_addr = call_expr.findLoadAddr();
                    if (load_addr) {
                        auto addr_val = load_addr->tryEval();
                        if (addr_val &&
                            sections.classify(*addr_val) ==
                                Core::SectionKind::Tls) {
                            Core::DataReference ref;
                            ref.insn_offset = insn.address;
                            ref.target_va = *addr_val;
                            ref.kind = Core::DataRefKind::TlsVar;
                            ref.tls_model = Core::TlsModel::LocalDynamic;
                            ref.source = Core::DataRefSource::PatternMatch;
                            refs.push_back(std::move(ref));
                            continue;
                        }
                    }
                }
            }
        }
    }

    // =============================================================
    // Jump Table Detection (architecture-dispatched via if constexpr)
    // =============================================================
    if constexpr (std::is_same_v<ArchTraits, Segmentator::ARM64ArchTraits>) {
        detectARM64JumpTablePatterns(insns, region_addr, region_size,
                                     sections, rodata_sections, refs);
    } else {
        // X86ArchTraits — need to determine 64-bit mode.
        // For the template version, we always assume 64-bit since the
        // ArchTraits doesn't carry mode info. The caller can pass
        // rodata_sections to enable jump table detection regardless.
        // In practice, x86 code in VMPilot is always 64-bit.
        bool is_64bit = (mode == Segmentator::Mode::MODE_64);
        detectX86JumpTablePatterns(insns, region_addr, region_size,
                                   sections, rodata_sections,
                                   is_64bit, refs);
    }

    return refs;
}

}  // namespace VMPilot::SDK::ReferenceAnalyzer
