#pragma once

#include <SectionInfo.hpp>

#include <cstdint>
#include <cstring>
#include <optional>
#include <vector>

namespace VMPilot::SDK::ReferenceAnalyzer {

/// Result of reading from the memory model.
struct ReadResult {
    uint64_t value;
    Core::SectionKind from_section;
    bool is_mutable;  // true for .data/.bss, false for .rodata/.got
};

/// Non-owning view of a binary section's contents.
struct SectionData {
    uint64_t base_addr;
    uint64_t size;
    Core::SectionKind kind;
    const uint8_t* data;  // Non-owning pointer — lifetime managed by caller
    bool is_mutable;
};

/// Provides typed reads from binary sections.
/// Built once from section data, used by SymbolicResolver and analysis layers.
///
/// NOTE: Single-threaded. The section data pointers must remain valid
/// for the lifetime of this object.
class MemoryModel {
public:
    /// Build from a list of section data views.
    /// Entries are sorted by base_addr for O(log n) lookup.
    explicit MemoryModel(std::vector<SectionData> sections);

    /// Default constructor — empty model, all reads return nullopt.
    MemoryModel() = default;

    /// Read a value from a known section.
    /// size must be 1, 2, 4, or 8.
    /// Returns nullopt if address is not in any known section or read
    /// would cross a section boundary.
    std::optional<ReadResult> read(uint64_t va, uint8_t size) const noexcept;

    /// Classify an address by the section it falls in.
    Core::SectionKind classify(uint64_t va) const noexcept;

    /// Check if an address falls within any known section.
    bool contains(uint64_t va) const noexcept;

    /// Number of sections in the model.
    size_t section_count() const noexcept { return entries_.size(); }

private:
    struct Entry {
        uint64_t base_addr;
        uint64_t size;
        Core::SectionKind kind;
        bool is_mutable;
        const uint8_t* data;

        bool contains(uint64_t va) const noexcept {
            return va >= base_addr && va < base_addr + size;
        }
    };
    std::vector<Entry> entries_;  // sorted by base_addr

    const Entry* find(uint64_t va) const noexcept;
};

}  // namespace VMPilot::SDK::ReferenceAnalyzer
