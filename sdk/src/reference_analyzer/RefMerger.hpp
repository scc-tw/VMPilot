#pragma once

#include <DataReference.hpp>

#include <algorithm>
#include <unordered_map>
#include <vector>

namespace VMPilot::SDK::ReferenceAnalyzer {

/// Merge policy for the three analysis layers.
/// L1 (relocation) wins → L2 (instruction) fills gaps → L3 (pattern) upgrades.
class RefMerger {
public:
    /// Merge three layers of DataReferences into a single sorted vector.
    static std::vector<Core::DataReference> merge(
        std::vector<Core::DataReference> l1_refs,
        std::vector<Core::DataReference> l2_refs,
        std::vector<Core::DataReference> l3_refs) noexcept
    {
        std::unordered_map<uint64_t, Core::DataReference> merged;

        // L1 wins — insert all relocation-based refs
        for (auto& ref : l1_refs)
            merged[ref.insn_offset] = std::move(ref);

        // L2 fills gaps — only insert if no L1 entry exists
        for (auto& ref : l2_refs) {
            if (merged.find(ref.insn_offset) == merged.end())
                merged[ref.insn_offset] = std::move(ref);
        }

        // L3 upgrades — upgrade existing entries or insert new
        for (auto& ref : l3_refs) {
            auto it = merged.find(ref.insn_offset);
            if (it != merged.end()) {
                tryUpgrade(it->second, std::move(ref));
            } else {
                merged[ref.insn_offset] = std::move(ref);
            }
        }

        // Collect into sorted vector
        std::vector<Core::DataReference> result;
        result.reserve(merged.size());
        for (auto& [_, ref] : merged)
            result.push_back(std::move(ref));

        std::sort(result.begin(), result.end(),
                  [](const Core::DataReference& a,
                     const Core::DataReference& b) {
                      return a.insn_offset < b.insn_offset;
                  });

        return result;
    }

private:
    /// Apply L3 upgrade rules to an existing entry.
    static void tryUpgrade(Core::DataReference& existing,
                           Core::DataReference upgrade) noexcept {
        // Upgrade: L3 identifies TLS on a non-TLS entry
        if (upgrade.kind == Core::DataRefKind::TlsVar &&
            existing.kind != Core::DataRefKind::TlsVar) {
            existing.kind = Core::DataRefKind::TlsVar;
            existing.tls_model = upgrade.tls_model;
        }
        // Upgrade TLS model if existing has None
        if (upgrade.tls_model != Core::TlsModel::None &&
            existing.tls_model == Core::TlsModel::None) {
            existing.tls_model = upgrade.tls_model;
        }
        // Upgrade: L3 identifies JumpTable on ReadOnlyData entry
        if (upgrade.kind == Core::DataRefKind::JumpTable &&
            existing.kind == Core::DataRefKind::ReadOnlyData) {
            existing.kind = Core::DataRefKind::JumpTable;
            existing.jump_table = std::move(upgrade.jump_table);
        }
    }
};

}  // namespace VMPilot::SDK::ReferenceAnalyzer
