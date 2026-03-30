#ifndef __LOADER_MACHO_EDITOR_HPP__
#define __LOADER_MACHO_EDITOR_HPP__
#pragma once

#include <LoaderTypes.hpp>
#include <MachOStructs.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace VMPilot::Loader {

/// Mach-O binary editor — reads a 64-bit Mach-O file into memory and
/// provides mutation operations for binary patching.
///
/// No virtual inheritance, no opaque base class.  Fully self-contained.
class MachOEditor {
public:
    /// Open and parse a Mach-O 64-bit binary.
    /// Returns the editor on success, diagnostic code on failure.
    [[nodiscard]] static tl::expected<MachOEditor, Common::DiagnosticCode>
    open(const std::string& path,
         Common::DiagnosticCollector& diag) noexcept;

    /// Text section virtual address and size.
    [[nodiscard]] TextSectionInfo text_section() const noexcept;

    /// Overwrite bytes in __text at the given virtual address.
    [[nodiscard]] tl::expected<void, Common::DiagnosticCode>
    overwrite_text(uint64_t va, const uint8_t* data, size_t len,
                   Common::DiagnosticCollector& diag) noexcept;

    /// Add a new loadable segment with the given payload.
    /// Inserts LC_SEGMENT_64 into header padding, appends data at EOF.
    /// Returns the virtual address where the segment was placed.
    [[nodiscard]] tl::expected<NewSegmentInfo, Common::DiagnosticCode>
    add_segment(std::string_view name, const std::vector<uint8_t>& payload,
                uint64_t alignment,
                Common::DiagnosticCollector& diag) noexcept;

    /// Overwrite bytes at any VA in any loaded segment (including __VMPILOT).
    [[nodiscard]] tl::expected<void, Common::DiagnosticCode>
    overwrite_segment(uint64_t va, const uint8_t* data, size_t len,
                      Common::DiagnosticCollector& diag) noexcept;

    /// Write the modified binary to disk.
    [[nodiscard]] tl::expected<void, Common::DiagnosticCode>
    save(const std::string& path,
         Common::DiagnosticCollector& diag) noexcept;

private:
    MachOEditor() = default;

    // --- Typed buffer access ---
    template <typename T>
    [[nodiscard]] T read_at(size_t offset) const {
        T val{};
        std::memcpy(&val, buf_.data() + offset, sizeof(T));
        return val;
    }
    template <typename T>
    void write_at(size_t offset, const T& val) {
        std::memcpy(buf_.data() + offset, &val, sizeof(T));
    }

    // --- State ---
    std::vector<uint8_t> buf_;

    MachO::mach_header_64 header_{};
    uint64_t text_va_       = 0;
    uint64_t text_size_     = 0;
    uint32_t text_file_off_ = 0;
    size_t   lcmds_end_     = 0;     // byte past last load command
    uint32_t first_sect_off_= 0;     // earliest section file offset (for padding)

    struct SegEntry { uint64_t vmaddr, vmsize; };
    std::vector<SegEntry> segments_;

    [[nodiscard]] uint64_t highest_va() const noexcept;
    [[nodiscard]] size_t header_padding() const noexcept;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_MACHO_EDITOR_HPP__
