#include "PatternAnalyzer.hpp"
#include "SectionLookup.hpp"

#include <ARM64Traits.hpp>
#include <RegValueResolver.hpp>
#include <X86Traits.hpp>

#include <capstone/arm64.h>
#include <capstone/x86.h>

#include <algorithm>
#include <cstring>
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

}

// ---------------------------------------------------------------------------
// Jump table helpers (shared across architectures)
// ---------------------------------------------------------------------------

/// Read bytes from rodata_sections at a given VA.
/// Returns true if the read succeeded.
bool readRodataBytes(
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
Core::JumpTableRef resolveJumpTableEntries(
    uint64_t table_base, uint32_t entry_size, bool relative_entries,
    uint32_t entry_count_hint,
    const std::vector<Segmentator::ReadOnlySection>& rodata_sections,
    const SectionLookup& sections) {
    Core::JumpTableRef jt;
    jt.table_base = table_base;
    jt.entry_size = entry_size;
    jt.relative_entries = relative_entries;

    // If no bounds check found, scan until invalid entry (max 1024)
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

        // Validate: target must be in .text
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
uint32_t findBoundsCheck(
    const std::vector<Capstone::Instruction>& insns,
    size_t insn_idx, bool is_arm64) {
    // Search up to 10 instructions backward
    const size_t limit = (insn_idx > 10) ? insn_idx - 10 : 0;
    for (size_t i = insn_idx; i > limit; --i) {
        size_t check = i - 1;
        const auto& insn = insns[check];

        if (is_arm64) {
            // ARM64: look for "cmp wN, #imm" then "b.hi"
            if (insn.mnemonic == "cmp" && insn.operands.size() >= 2 &&
                insn.operands[1].type == Capstone::OpType::IMM) {
                // Verify next instruction is b.hi
                if (check + 1 < insns.size()) {
                    const auto& next = insns[check + 1];
                    if (next.mnemonic == "b.hi" || next.mnemonic == "b.hs") {
                        return static_cast<uint32_t>(insn.operands[1].imm) + 1;
                    }
                }
            }
        } else {
            // x86: look for "cmp reg, imm" then "ja/jae"
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

// ---------------------------------------------------------------------------
// x86/x86-64 Jump Table Detection
// ---------------------------------------------------------------------------

void detectX86JumpTablePatterns(
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

        // --- Pattern A: jmp [mem] with index register (indexed memory jump)
        // x86-64: jmp qword ptr [rip+TABLE+rax*8]
        // x86-32: jmp dword ptr [TABLE+eax*4]
        if (insn.operands.size() == 1 &&
            insn.operands[0].type == Capstone::OpType::MEM &&
            insn.operands[0].mem.index != 0) {
            const auto& mem = insn.operands[0].mem;

            uint64_t table_base = 0;
            uint32_t entry_size = 0;
            bool resolved = false;

            if (is_64bit && mem.isRipRelative()) {
                // x86-64: rip-relative with index*scale
                table_base = insn.address + insn.size + mem.disp;
                entry_size = static_cast<uint32_t>(mem.scale > 0 ? mem.scale
                                                                  : 8);
                resolved = true;
            } else if (!is_64bit && mem.base == 0) {
                // x86-32: absolute address with index*scale
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

        // --- Pattern B: jmp reg (x86-64, relative entries via LEA+movsxd+add)
        // jmp rax  where rax was computed from:
        //   lea rcx, [rip+TABLE]
        //   movsxd rax, [rcx+rax*4]
        //   add rax, rcx
        //   jmp rax
        if (is_64bit && insn.operands.size() == 1 &&
            insn.operands[0].type == Capstone::OpType::REG) {
            // Backtrace to find `lea Rx, [rip+disp]` pointing to .rodata
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
                // x86-64 relative: movsxd loads 32-bit signed offset
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

// ---------------------------------------------------------------------------
// ARM64 Jump Table Detection
// ---------------------------------------------------------------------------

void detectARM64JumpTablePatterns(
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

        // ARM64 indirect branch: "br Xn"
        if (insn.mnemonic != "br")
            continue;

        // Backtrace to find ADRP+ADD pattern pointing to .rodata
        uint64_t table_base = 0;
        const size_t limit = (idx > 10) ? idx - 10 : 0;
        for (size_t i = idx; i > limit; --i) {
            const auto& prev = insns[i - 1];
            // Look for ADD with immediate that follows ADRP
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
        // ARM64 uses 32-bit relative offsets (ldr w + sxtw + add)
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

}  // namespace

std::vector<Core::DataReference> analyzePatterns(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const Segmentator::NativeSymbolTable& symbols,
    const SectionLookup& sections,
    const std::vector<Segmentator::ReadOnlySection>& rodata_sections,
    Segmentator::Arch arch, Segmentator::Mode mode) noexcept {
    std::vector<Core::DataReference> refs;

    AddrToName sym_lookup;
    for (const auto& sym : symbols) {
        if (!sym.name.empty() && sym.address != 0)
            sym_lookup[sym.address] = sym.name;
    }

    using FileArch = VMPilot::Common::FileArch;
    using FileMode = VMPilot::Common::FileMode;
    if (arch == FileArch::X86) {
        detectX86TlsPatterns(insns, region_addr, region_size, sym_lookup, refs);
        bool is_64bit = (mode == FileMode::MODE_64);
        detectX86JumpTablePatterns(insns, region_addr, region_size, sections,
                                   rodata_sections, is_64bit, refs);
    } else if (arch == FileArch::ARM64) {
        detectARM64TlsPatterns(insns, region_addr, region_size, sym_lookup,
                               refs);
        detectARM64JumpTablePatterns(insns, region_addr, region_size, sections,
                                     rodata_sections, refs);
    }

    return refs;
}

}  // namespace VMPilot::SDK::ReferenceAnalyzer
