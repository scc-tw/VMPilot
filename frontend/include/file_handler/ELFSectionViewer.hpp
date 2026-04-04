#ifndef __ELF_SECTION_VIEWER_HPP__
#define __ELF_SECTION_VIEWER_HPP__
#pragma once

#include <cstddef>
#include <cstdint>

namespace VMPilot::SDK::Segmentator {

/**
 * @brief Lightweight section reference: stores the index into the
 *        elfio-modern editor's section list.
 */
class ELFSectionViewer {
   public:
    ELFSectionViewer() = default;
    explicit ELFSectionViewer(std::size_t idx) : section_idx(idx) {}

    ELFSectionViewer(const ELFSectionViewer&) = default;
    ELFSectionViewer& operator=(const ELFSectionViewer&) = default;
    ELFSectionViewer(ELFSectionViewer&&) = default;
    ELFSectionViewer& operator=(ELFSectionViewer&&) = default;
    ~ELFSectionViewer() = default;

    std::size_t getSectionIndex() const noexcept { return section_idx; }

   private:
    std::size_t section_idx = 0;
};

}  // namespace VMPilot::SDK::Segmentator

#endif  // __ELF_SECTION_VIEWER_HPP__
