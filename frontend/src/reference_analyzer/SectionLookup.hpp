#ifndef __SDK_REFERENCE_ANALYZER_SECTION_LOOKUP_HPP__
#define __SDK_REFERENCE_ANALYZER_SECTION_LOOKUP_HPP__
#pragma once

#include <Section.hpp>

#include <algorithm>
#include <vector>

namespace VMPilot::SDK::ReferenceAnalyzer {

/// Sorted-vector wrapper for O(log n) VA -> Section lookup.
class SectionLookup {
   public:
    explicit SectionLookup(const std::vector<Core::Section>& sections)
        : sections_(sections) {
        std::sort(sections_.begin(), sections_.end(),
                  [](const Core::Section& a, const Core::Section& b) {
                      return a.base_addr < b.base_addr;
                  });
    }

    /// Classify a VA by finding the section it belongs to.
    /// Returns SectionKind::Unknown if no section contains the VA.
    Core::SectionKind classify(uint64_t va) const {
        // Binary search: find last section with base_addr <= va
        auto it = std::upper_bound(
            sections_.begin(), sections_.end(), va,
            [](uint64_t addr, const Core::Section& sec) {
                return addr < sec.base_addr;
            });

        if (it != sections_.begin()) {
            --it;
            if (it->contains(va))
                return it->kind;
        }
        return Core::SectionKind::Unknown;
    }

    /// Find the section containing a VA (or nullptr).
    const Core::Section* find(uint64_t va) const {
        auto it = std::upper_bound(
            sections_.begin(), sections_.end(), va,
            [](uint64_t addr, const Core::Section& sec) {
                return addr < sec.base_addr;
            });

        if (it != sections_.begin()) {
            --it;
            if (it->contains(va))
                return &(*it);
        }
        return nullptr;
    }

   private:
    std::vector<Core::Section> sections_;
};

}  // namespace VMPilot::SDK::ReferenceAnalyzer

#endif  // __SDK_REFERENCE_ANALYZER_SECTION_LOOKUP_HPP__
