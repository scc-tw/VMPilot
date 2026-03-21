#pragma once

#include <DataReference.hpp>
#include <cstdint>
#include <vector>

namespace VMPilot::SDK::ReferenceAnalyzer {

/// Generic relocation analysis -- arch-independent algorithm.
/// Traits must provide: static RelocationClassification classify(uint32_t type) noexcept;
template <typename RelocTraits>
std::vector<Core::DataReference> analyzeRelocations(
    const std::vector<Core::RelocationEntry>& text_relocations,
    uint64_t region_addr,
    uint64_t region_size) noexcept
{
    std::vector<Core::DataReference> result;
    const uint64_t region_end = region_addr + region_size;

    for (const auto& reloc : text_relocations) {
        if (reloc.offset < region_addr || reloc.offset >= region_end)
            continue;

        auto cls = RelocTraits::classify(reloc.type);
        if (cls.skip)
            continue;

        Core::DataReference ref;
        ref.insn_offset = reloc.offset;
        ref.source = Core::DataRefSource::Relocation;
        ref.relocation = reloc;
        ref.target_symbol = reloc.symbol_name;
        ref.kind = cls.kind;
        ref.tls_model = cls.tls_model;
        ref.is_pc_relative = cls.is_pc_relative;

        result.push_back(std::move(ref));
    }

    return result;
}

}  // namespace VMPilot::SDK::ReferenceAnalyzer
