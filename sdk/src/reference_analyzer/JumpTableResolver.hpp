#pragma once

#include "SectionLookup.hpp"

#include <DataReference.hpp>
#include <Section.hpp>
#include <capstone.hpp>

#include <cstdint>
#include <cstring>
#include <vector>

namespace VMPilot::SDK::ReferenceAnalyzer::JumpTable {

/// Describes the layout of a jump table in .rodata.
struct JumpTableLayout {
    uint64_t table_base = 0;       // Start address of the table in .rodata
    uint32_t entry_size = 0;       // 1 (byte-offset), 4, or 8 bytes per entry
    bool relative = false;         // Entries are signed offsets from resolve_base
    uint64_t resolve_base = 0;     // Base for relative entries (table_base or case_base)
    uint8_t shift = 0;             // For byte-offset: target = base + entry << shift
    uint32_t max_entries = 1024;   // From bounds check, or default limit
};

/// Read bytes from rodata_sections at a given VA.
/// Returns true if the read succeeded.
inline bool readRodataBytes(
    const std::vector<Core::Section>& rodata_sections,
    uint64_t va, void* buf, size_t len) noexcept {
    for (const auto& sec : rodata_sections) {
        if (!sec.has_data() || !sec.contains(va))
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

/// Resolve jump table entries using a unified layout descriptor.
/// Supports all variants:
///   - 8-byte absolute (x86-64 indexed)
///   - 4-byte absolute (x86-32 indexed)
///   - 4-byte relative (x86-64 lea+movsxd+add, ARM64 GCC)
///   - 1-byte offset with shift (ARM64 Apple Clang)
inline Core::JumpTableRef resolveEntries(
    const JumpTableLayout& layout,
    const std::vector<Core::Section>& rodata_sections,
    const SectionLookup& sections) noexcept {
    Core::JumpTableRef jt;
    jt.table_base = layout.table_base;
    jt.entry_size = layout.entry_size;
    jt.relative_entries = layout.relative;

    for (uint32_t i = 0; i < layout.max_entries; ++i) {
        uint64_t entry_va = layout.table_base +
                            static_cast<uint64_t>(i) * layout.entry_size;
        uint64_t target = 0;

        if (layout.entry_size == 1) {
            // Byte-offset: target = resolve_base + (entry << shift)
            uint8_t raw = 0;
            if (!readRodataBytes(rodata_sections, entry_va, &raw, 1))
                break;
            target = layout.resolve_base +
                     (static_cast<uint64_t>(raw) << layout.shift);
        } else if (layout.entry_size == 4) {
            uint32_t raw = 0;
            if (!readRodataBytes(rodata_sections, entry_va, &raw, 4))
                break;
            if (layout.relative) {
                auto signed_val = static_cast<int32_t>(raw);
                target = layout.resolve_base +
                         static_cast<uint64_t>(static_cast<int64_t>(signed_val));
            } else {
                target = raw;
            }
        } else if (layout.entry_size == 8) {
            uint64_t raw = 0;
            if (!readRodataBytes(rodata_sections, entry_va, &raw, 8))
                break;
            if (layout.relative) {
                auto signed_val = static_cast<int64_t>(raw);
                target = layout.resolve_base +
                         static_cast<uint64_t>(signed_val);
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
///   cmp reg, imm  followed by  ja/jae (x86) or b.hi/b.hs (ARM64)
/// Returns (imm + 1) as the entry count, or 0 if not found.
inline uint32_t findBoundsCheck(
    const std::vector<Capstone::Instruction>& insns,
    size_t insn_idx, bool is_arm64) noexcept {
    const size_t limit = (insn_idx > 10) ? insn_idx - 10 : 0;
    for (size_t i = insn_idx; i > limit; --i) {
        size_t check = i - 1;
        const auto& insn = insns[check];

        if (is_arm64) {
            if (insn.mnemonic == "cmp" && insn.operands.size() >= 2 &&
                insn.operands[1].type == Capstone::OpType::IMM) {
                if (check + 1 < insns.size()) {
                    const auto& next = insns[check + 1];
                    if (next.mnemonic == "b.hi" || next.mnemonic == "b.hs")
                        return static_cast<uint32_t>(insn.operands[1].imm) + 1;
                }
            }
        } else {
            if (insn.mnemonic == "cmp" && insn.operands.size() >= 2 &&
                insn.operands[1].type == Capstone::OpType::IMM) {
                if (check + 1 < insns.size()) {
                    const auto& next = insns[check + 1];
                    if (next.mnemonic == "ja" || next.mnemonic == "jae")
                        return static_cast<uint32_t>(insn.operands[1].imm) + 1;
                }
            }
        }
    }
    return 0;
}

}  // namespace VMPilot::SDK::ReferenceAnalyzer::JumpTable
