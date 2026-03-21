#ifndef __SDK_SEGMENTATOR_COMPILATION_CONTEXT_HPP__
#define __SDK_SEGMENTATOR_COMPILATION_CONTEXT_HPP__
#pragma once

#include <ArchEnum.hpp>
#include <DataReference.hpp>
#include <ModeEnum.hpp>
#include <NativeSymbolTable.hpp>
#include <Section.hpp>
#include <file_type_parser.hpp>

#include <optional>
#include <string>
#include <vector>

namespace VMPilot::SDK::Segmentator {

/// Shared context for all regions extracted from the same binary.
/// Populated by Segmentator, consumed by:
///   - ArchHandler (__FUNCTION__ name resolution)
///   - ReferenceAnalyzer (data/TLS reference detection)
///   - BytecodeCompiler (constant folding, data reference resolution)
struct CompilationContext {
    // ── Binary identity ──
    Common::FileFormat format = Common::FileFormat::Unknown;
    Arch arch = {};
    Mode mode = {};
    uint64_t image_base = 0;
    uint64_t text_base = 0;
    std::string compiler_info;

    // ── Sections (unified representation) ──
    std::vector<Core::Section> sections;

    // ── Symbols ──
    NativeSymbolTable symbols;

    // ── Relocations ──
    std::vector<Core::RelocationEntry> text_relocations;

    // ── Convenience accessors ──

    /// Find the section containing a virtual address.
    const Core::Section* findSection(uint64_t va) const noexcept {
        for (const auto& s : sections)
            if (s.contains(va))
                return &s;
        return nullptr;
    }

    /// Find the first section with a given kind.
    const Core::Section* findSectionByKind(
        Core::SectionKind kind) const noexcept {
        for (const auto& s : sections)
            if (s.kind == kind)
                return &s;
        return nullptr;
    }

    /// Find all sections with a given kind.
    std::vector<const Core::Section*> findSectionsByKind(
        Core::SectionKind kind) const {
        std::vector<const Core::Section*> result;
        for (const auto& s : sections)
            if (s.kind == kind)
                result.push_back(&s);
        return result;
    }

    /// Classify a virtual address by the section it falls in.
    Core::SectionKind classifyAddress(uint64_t va) const noexcept {
        auto* s = findSection(va);
        return s ? s->kind : Core::SectionKind::Unknown;
    }

    /// Read a typed value (1/2/4/8 bytes) from any section.
    std::optional<uint64_t> readValue(uint64_t va,
                                      uint8_t size) const noexcept {
        auto* s = findSection(va);
        return s ? s->readValue(va, size) : std::nullopt;
    }

    /// Read a null-terminated C string from any section.
    std::string readCString(uint64_t va) const {
        auto* s = findSection(va);
        return s ? s->readCString(va) : std::string{};
    }
};

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_SEGMENTATOR_COMPILATION_CONTEXT_HPP__
