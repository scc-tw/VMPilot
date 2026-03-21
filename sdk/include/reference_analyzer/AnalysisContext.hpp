#pragma once

#include <ArchEnum.hpp>
#include <DataReference.hpp>
#include <ModeEnum.hpp>
#include <NativeSymbolTable.hpp>
#include <ReadOnlySection.hpp>
#include <SectionInfo.hpp>

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace VMPilot::SDK::ReferenceAnalyzer {

/// Shared analysis context — built once, used by all layers.
///
/// NOTE: Single-threaded. If multi-thread analysis is needed in the
/// future, this design must be revisited.
struct AnalysisContext {
    std::vector<Core::SectionInfo> sections;
    std::vector<Core::RelocationEntry> text_relocations;
    Segmentator::NativeSymbolTable symbols;
    std::vector<Segmentator::ReadOnlySection> rodata_sections;
    Segmentator::Arch arch;
    Segmentator::Mode mode;

    // Pre-built lookups (constructed once)
    std::unordered_map<uint64_t, std::string> symbol_lookup;

    /// Build the context and pre-compute lookups.
    static AnalysisContext build(
        std::vector<Core::SectionInfo> sections,
        std::vector<Core::RelocationEntry> text_relocations,
        Segmentator::NativeSymbolTable symbols,
        std::vector<Segmentator::ReadOnlySection> rodata_sections,
        Segmentator::Arch arch,
        Segmentator::Mode mode)
    {
        AnalysisContext ctx;
        ctx.sections = std::move(sections);
        ctx.text_relocations = std::move(text_relocations);
        ctx.rodata_sections = std::move(rodata_sections);
        ctx.arch = arch;
        ctx.mode = mode;

        // Build symbol lookup once
        for (const auto& sym : symbols) {
            if (!sym.name.empty() && sym.address != 0)
                ctx.symbol_lookup[sym.address] = sym.name;
        }
        ctx.symbols = std::move(symbols);

        return ctx;
    }
};

}  // namespace VMPilot::SDK::ReferenceAnalyzer
