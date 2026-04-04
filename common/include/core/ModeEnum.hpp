#ifndef __SDK_MODE_ENUM_HPP__
#define __SDK_MODE_ENUM_HPP__
#pragma once

#include <file_type_parser.hpp>

#include <cstdint>

namespace VMPilot::SDK::Segmentator {
using Mode = VMPilot::Common::FileMode;

inline const char* to_string(Mode mode) noexcept {
    auto m = static_cast<uint32_t>(mode);
    if (m & static_cast<uint32_t>(Mode::MODE_64)) return "MODE_64";
    if (m & static_cast<uint32_t>(Mode::MODE_32)) return "MODE_32";
    if (m & static_cast<uint32_t>(Mode::MODE_16)) return "MODE_16";
    return "MODE_LITTLE_ENDIAN";
}
}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_MODE_ENUM_HPP__