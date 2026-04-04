#pragma once

#include "SectionLookup.hpp"
#include "AtomicTraits.hpp"

#include <ARM64Traits.hpp>
#include <DataReference.hpp>
#include <NativeSymbolTable.hpp>
#include <RegValueResolver.hpp>
#include <X86Traits.hpp>
#include <capstone.hpp>

#include <cstdint>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace VMPilot::SDK::ReferenceAnalyzer {

/// Map SectionKind -> DataRefKind (shared by all architectures).
inline Core::DataRefKind classifyBySection(Core::SectionKind sk) noexcept {
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

/// InsnTraits concept (documented, not enforced with C++20 concepts):
///   - ArchTraits: same traits as for resolveRegValue
///   - static bool isStackRelative(unsigned base_reg)
///   - static bool isAddressComputation(const Instruction& insn)
///   - static bool isFenceInstruction(const Instruction& insn)
///   - classifyAtomic / isMemWrite (signatures vary by arch)

struct X86InsnTraits {
    using ArchTraits = Segmentator::X86ArchTraits;

    static bool isStackRelative(unsigned base_reg) noexcept;
    static bool isAddressComputation(
        const Capstone::Instruction& insn) noexcept;
    static bool isFenceInstruction(
        const Capstone::Instruction& insn) noexcept;

    static AtomicInfo classifyAtomic(const Capstone::Instruction& insn,
                                     bool has_mem_operand) noexcept;
    static bool isMemWrite(const Capstone::Instruction& insn,
                           size_t op_idx) noexcept;
};

struct ARM64InsnTraits {
    using ArchTraits = Segmentator::ARM64ArchTraits;

    static bool isStackRelative(unsigned base_reg) noexcept;
    static bool isAddressComputation(
        const Capstone::Instruction& insn) noexcept;
    static bool isFenceInstruction(
        const Capstone::Instruction& insn) noexcept;

    static AtomicInfo classifyAtomic(
        const Capstone::Instruction& insn) noexcept;
    static bool isMemWrite(const Capstone::Instruction& insn,
                           const AtomicInfo& atomic) noexcept;
};

/// Generic instruction analysis algorithm.
/// Replaces analyzeX86Insns() and analyzeARM64Insns().
template <typename Traits>
std::vector<Core::DataReference> analyzeInsns(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const SectionLookup& sections,
    const Segmentator::NativeSymbolTable& symbols) noexcept {
    using ArchTraits = typename Traits::ArchTraits;

    std::vector<Core::DataReference> refs;
    const uint64_t region_end = region_addr + region_size;

    // Build symbol address lookup once
    std::unordered_map<uint64_t, std::string> sym_lookup;
    for (const auto& sym : symbols) {
        if (!sym.name.empty() && sym.address != 0)
            sym_lookup[sym.address] = sym.name;
    }

    for (size_t idx = 0; idx < insns.size(); ++idx) {
        const auto& insn = insns[idx];
        if (insn.address < region_addr || insn.address >= region_end)
            continue;

        // --- Architecture-specific fence detection ---
        if (Traits::isFenceInstruction(insn)) {
            AtomicInfo fence_ai;
            if constexpr (std::is_same_v<Traits, X86InsnTraits>) {
                fence_ai = Traits::classifyAtomic(insn, false);
                // x86 fence insns create ref directly
                fence_ai.op = Core::AtomicOp::Fence;
                fence_ai.ordering = Core::AtomicOrdering::AcqRel;
                fence_ai.width = Core::AtomicWidth::None;
            } else {
                fence_ai = Traits::classifyAtomic(insn);
            }

            if (fence_ai.op == Core::AtomicOp::Fence) {
                Core::DataReference ref;
                ref.insn_offset = insn.address;
                ref.target_va = 0;
                ref.kind = Core::DataRefKind::Unknown;
                ref.source = Core::DataRefSource::InsnAnalysis;
                ref.atomic_op = fence_ai.op;
                ref.atomic_ordering = fence_ai.ordering;
                ref.atomic_width = Core::AtomicWidth::None;
                refs.push_back(std::move(ref));
            }
            continue;
        }

        // --- Per-operand memory reference analysis ---
        // For ARM64: classify atomic BEFORE operand loop (mnemonic-based)
        AtomicInfo arm64_atomic;
        if constexpr (std::is_same_v<Traits, ARM64InsnTraits>) {
            arm64_atomic = Traits::classifyAtomic(insn);
        }

        for (size_t op_idx = 0; op_idx < insn.operands.size(); ++op_idx) {
            const auto& op = insn.operands[op_idx];
            if (op.type != Capstone::OpType::MEM)
                continue;

            if (Traits::isAddressComputation(insn))
                continue;

            if (Traits::isStackRelative(op.mem.base))
                continue;

            // Resolve target address
            uint64_t target_va = 0;
            bool resolved = false;

            if (op.mem.isRipRelative()) {
                // x86 RIP-relative (ARM64 doesn't use this)
                target_va = insn.address + insn.size + op.mem.disp;
                resolved = true;
            } else if (op.mem.base != 0 && op.mem.index == 0) {
                size_t from = idx > 0 ? idx - 1 : 0;
                uint64_t base_val =
                    Segmentator::resolveRegValue<ArchTraits>(
                        op.mem.base, from, insns, 0);
                if (base_val != 0) {
                    target_va = base_val + op.mem.disp;
                    resolved = true;
                }
            } else if (op.mem.base != 0 && op.mem.index != 0 &&
                       !Traits::isStackRelative(op.mem.index)) {
                // Scaled-index: resolve base for element-0 classification
                size_t from = idx > 0 ? idx - 1 : 0;
                uint64_t base_val =
                    Segmentator::resolveRegValue<ArchTraits>(
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
            ref.is_pc_relative = op.mem.isRipRelative();
            ref.is_mutable_target =
                (sk == Core::SectionKind::Data ||
                 sk == Core::SectionKind::Bss);

            // Write detection + atomic classification
            if constexpr (std::is_same_v<Traits, X86InsnTraits>) {
                ref.is_write = Traits::isMemWrite(insn, op_idx);
                auto atomic = Traits::classifyAtomic(insn, true);
                if (atomic.op != Core::AtomicOp::None) {
                    ref.atomic_op = atomic.op;
                    ref.atomic_ordering = atomic.ordering;
                    auto special = atomicWidthForCmpxchg(insn.id);
                    ref.atomic_width =
                        (special != Core::AtomicWidth::None)
                            ? special
                            : atomicWidthFromSize(op.size);
                }
            } else {
                ref.is_write = Traits::isMemWrite(insn, arm64_atomic);
                if (arm64_atomic.op != Core::AtomicOp::None) {
                    ref.atomic_op = arm64_atomic.op;
                    ref.atomic_ordering = arm64_atomic.ordering;
                    ref.atomic_width = arm64_atomic.width;
                }
            }

            // Symbol lookup
            auto sym_it = sym_lookup.find(target_va);
            if (sym_it != sym_lookup.end())
                ref.target_symbol = sym_it->second;

            refs.push_back(std::move(ref));
        }
    }
    return refs;
}

}  // namespace VMPilot::SDK::ReferenceAnalyzer
