#ifndef __SDK_ARCH_ENUM_HPP__
#define __SDK_ARCH_ENUM_HPP__
#pragma once

#include <file_type_parser.hpp>

#include <cstdint>

namespace VMPilot::SDK::Segmentator {
using Arch = VMPilot::Common::FileArch;

inline const char* to_string(Arch arch) noexcept {
    switch (arch) {
        case Arch::X86:   return "X86";
        case Arch::ARM64: return "ARM64";
        case Arch::ARM:   return "ARM";
        case Arch::MIPS:  return "MIPS";
        case Arch::PPC:   return "PPC";
        case Arch::RISCV: return "RISCV";
        default:          return "UNKNOWN";
    }
}
}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_ARCH_ENUM_HPP__