#ifndef __SDK_SEGMENTATOR_READONLY_SECTION_HPP__
#define __SDK_SEGMENTATOR_READONLY_SECTION_HPP__
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace VMPilot::SDK::Segmentator {

/// Read-only section for string/data resolution.
/// Used by __FUNCTION__ extraction (readCString) and by future
/// BytecodeCompiler for jump table / RIP-relative data access.
struct ReadOnlySection {
    std::vector<uint8_t> data;
    uint64_t base_addr = static_cast<uint64_t>(-1);

    bool contains(uint64_t va) const {
        if (base_addr == static_cast<uint64_t>(-1) || data.empty())
            return false;
        return va >= base_addr && va < base_addr + data.size();
    }

    std::string readCString(uint64_t va) const {
        if (!contains(va))
            return {};
        const size_t offset = va - base_addr;
        const auto* start =
            reinterpret_cast<const char*>(data.data() + offset);
        const size_t max_len = data.size() - offset;
        size_t len = 0;
        while (len < max_len && start[len] != '\0')
            ++len;
        return std::string(start, len);
    }
};

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_SEGMENTATOR_READONLY_SECTION_HPP__
