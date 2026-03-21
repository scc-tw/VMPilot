#include "MemoryModel.hpp"

#include <algorithm>

namespace VMPilot::SDK::ReferenceAnalyzer {

MemoryModel::MemoryModel(std::vector<SectionData> sections) {
    entries_.reserve(sections.size());
    for (auto& s : sections) {
        if (s.data == nullptr || s.size == 0)
            continue;
        entries_.push_back({s.base_addr, s.size, s.kind, s.is_mutable, s.data});
    }
    std::sort(entries_.begin(), entries_.end(),
              [](const Entry& a, const Entry& b) {
                  return a.base_addr < b.base_addr;
              });
}

const MemoryModel::Entry* MemoryModel::find(uint64_t va) const noexcept {
    // Binary search: find the last entry with base_addr <= va
    auto it = std::upper_bound(
        entries_.begin(), entries_.end(), va,
        [](uint64_t addr, const Entry& e) { return addr < e.base_addr; });

    if (it == entries_.begin())
        return nullptr;
    --it;
    if (it->contains(va))
        return &(*it);
    return nullptr;
}

std::optional<ReadResult> MemoryModel::read(uint64_t va,
                                            uint8_t size) const noexcept {
    if (size != 1 && size != 2 && size != 4 && size != 8)
        return std::nullopt;

    const auto* entry = find(va);
    if (!entry)
        return std::nullopt;

    // Check that the entire read fits within the section
    uint64_t end_va = va + size;
    if (end_va > entry->base_addr + entry->size)
        return std::nullopt;

    size_t offset = static_cast<size_t>(va - entry->base_addr);

    uint64_t value = 0;
    std::memcpy(&value, entry->data + offset, size);

    return ReadResult{value, entry->kind, entry->is_mutable};
}

Core::SectionKind MemoryModel::classify(uint64_t va) const noexcept {
    const auto* entry = find(va);
    return entry ? entry->kind : Core::SectionKind::Unknown;
}

bool MemoryModel::contains(uint64_t va) const noexcept {
    return find(va) != nullptr;
}

}  // namespace VMPilot::SDK::ReferenceAnalyzer
