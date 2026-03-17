#include <ReferenceAnalyzer.hpp>

#include "InsnAnalyzer.hpp"
#include "PatternAnalyzer.hpp"
#include "RelocationAnalyzer.hpp"
#include "SectionLookup.hpp"

#include <algorithm>
#include <unordered_map>

namespace VMPilot::SDK::ReferenceAnalyzer {

namespace {

/// Merge results: L1 wins -> L2 fills gaps -> L3 upgrades.
/// Key: insn_offset (instruction address).
std::vector<Core::DataReference> mergeResults(
    std::vector<Core::DataReference> l1_refs,
    std::vector<Core::DataReference> l2_refs,
    std::vector<Core::DataReference> l3_refs) {
    // Build map keyed by insn_offset. L1 is authoritative.
    std::unordered_map<uint64_t, Core::DataReference> merged;

    // L1 wins — insert all relocation-based refs
    for (auto& ref : l1_refs) {
        merged[ref.insn_offset] = std::move(ref);
    }

    // L2 fills gaps — only insert if no L1 entry exists
    for (auto& ref : l2_refs) {
        if (merged.find(ref.insn_offset) == merged.end()) {
            merged[ref.insn_offset] = std::move(ref);
        }
    }

    // L3 upgrades — upgrade kind/tls_model for existing entries,
    // or insert new entries for patterns not covered by L1/L2
    for (auto& ref : l3_refs) {
        auto it = merged.find(ref.insn_offset);
        if (it != merged.end()) {
            // Upgrade: if L3 identifies TLS, upgrade the kind/model
            if (ref.kind == Core::DataRefKind::TlsVar &&
                it->second.kind != Core::DataRefKind::TlsVar) {
                it->second.kind = Core::DataRefKind::TlsVar;
                it->second.tls_model = ref.tls_model;
            }
            // If existing is already TLS but model is None, upgrade model
            if (ref.tls_model != Core::TlsModel::None &&
                it->second.tls_model == Core::TlsModel::None) {
                it->second.tls_model = ref.tls_model;
            }
            // Upgrade: if L3 identifies JumpTable, upgrade ReadOnlyData
            if (ref.kind == Core::DataRefKind::JumpTable &&
                it->second.kind == Core::DataRefKind::ReadOnlyData) {
                it->second.kind = Core::DataRefKind::JumpTable;
                it->second.jump_table = std::move(ref.jump_table);
            }
        } else {
            merged[ref.insn_offset] = std::move(ref);
        }
    }

    // Collect into sorted vector
    std::vector<Core::DataReference> result;
    result.reserve(merged.size());
    for (auto& [_, ref] : merged) {
        result.push_back(std::move(ref));
    }

    std::sort(result.begin(), result.end(),
              [](const Core::DataReference& a, const Core::DataReference& b) {
                  return a.insn_offset < b.insn_offset;
              });

    return result;
}

}  // namespace

std::vector<Core::DataReference> analyze(
    const std::vector<Capstone::Instruction>& insns,
    uint64_t region_addr, uint64_t region_size,
    const std::vector<Core::SectionInfo>& sections,
    const std::vector<Core::RelocationEntry>& text_relocations,
    const Segmentator::NativeSymbolTable& symbols,
    const std::vector<Segmentator::ReadOnlySection>& rodata_sections,
    Segmentator::Arch arch, Segmentator::Mode mode) noexcept {
    SectionLookup lookup(sections);

    // Layer 1: Relocation analysis
    auto l1 = analyzeRelocations(text_relocations, region_addr, region_size,
                                  lookup);

    // Layer 2: Instruction analysis
    auto l2 = analyzeInstructions(insns, region_addr, region_size, lookup,
                                   symbols, arch, mode);

    // Layer 3: Pattern detection (TLS + jump tables)
    auto l3 = analyzePatterns(insns, region_addr, region_size, symbols, lookup,
                               rodata_sections, arch, mode);

    // Merge: L1 wins -> L2 fills -> L3 upgrades
    return mergeResults(std::move(l1), std::move(l2), std::move(l3));
}

}  // namespace VMPilot::SDK::ReferenceAnalyzer
