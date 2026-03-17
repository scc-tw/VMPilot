#ifndef __SDK_CORE_SECTION_INFO_HPP__
#define __SDK_CORE_SECTION_INFO_HPP__
#pragma once

#include <cstdint>
#include <string>

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

struct SectionInfo {
    uint64_t base_addr = 0;
    uint64_t size = 0;
    SectionKind kind = SectionKind::Unknown;
    std::string name;

    bool contains(uint64_t va) const {
        return va >= base_addr && va < base_addr + size;
    }
};

}  // namespace VMPilot::SDK::Core

#endif  // __SDK_CORE_SECTION_INFO_HPP__
