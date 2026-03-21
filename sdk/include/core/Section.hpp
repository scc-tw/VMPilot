#ifndef __SDK_CORE_SECTION_HPP__
#define __SDK_CORE_SECTION_HPP__
#pragma once

#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <vector>

namespace VMPilot::SDK::Core {

enum class SectionKind : uint8_t {
    Text,
    Rodata,
    Data,
    Bss,
    Tls,
    Got,
    Plt,
    Unknown,
};

/// Unified binary section representation.
///
/// Replaces the former ReadOnlySection (had data bytes but no kind/size)
/// and SectionInfo (had kind/size but no data bytes).
///
/// - `.bss`: data is empty, size is non-zero (zero-initialized at runtime)
/// - `.text`: data may be empty (code bytes live in CompilationUnit.code)
/// - All other loadable sections: data is populated
struct Section {
    uint64_t base_addr = 0;
    uint64_t size = 0;
    SectionKind kind = SectionKind::Unknown;
    std::string name;
    std::vector<uint8_t> data;

    bool has_data() const noexcept { return !data.empty(); }

    bool is_mutable() const noexcept {
        return kind == SectionKind::Data || kind == SectionKind::Bss;
    }

    bool contains(uint64_t va) const noexcept {
        return va >= base_addr && va < base_addr + size;
    }

    /// Read a typed value (1, 2, 4, or 8 bytes) at a virtual address.
    /// Returns nullopt if no data loaded, out of bounds, or invalid size.
    std::optional<uint64_t> readValue(uint64_t va,
                                      uint8_t byte_count) const noexcept {
        if (!has_data() || !contains(va))
            return std::nullopt;
        if (byte_count != 1 && byte_count != 2 &&
            byte_count != 4 && byte_count != 8)
            return std::nullopt;

        const uint64_t offset = va - base_addr;
        if (offset + byte_count > data.size())
            return std::nullopt;

        uint64_t value = 0;
        std::memcpy(&value, data.data() + offset, byte_count);
        return value;
    }

    /// Read a null-terminated C string at a virtual address.
    /// Returns empty string if no data loaded or out of bounds.
    std::string readCString(uint64_t va) const {
        if (!has_data() || !contains(va))
            return {};
        const uint64_t offset = va - base_addr;
        if (offset >= data.size())
            return {};
        const auto* start =
            reinterpret_cast<const char*>(data.data() + offset);
        const size_t max_len = data.size() - offset;
        size_t len = 0;
        while (len < max_len && start[len] != '\0')
            ++len;
        return std::string(start, len);
    }
};

}  // namespace VMPilot::SDK::Core

#endif  // __SDK_CORE_SECTION_HPP__
