#ifndef __LOADER_PLATFORM_TRAITS_HPP__
#define __LOADER_PLATFORM_TRAITS_HPP__
#pragma once

/// @file PlatformTraits.hpp
/// @brief Compile-time per-format constants + runtime bridge.
///
/// Supports cross-compilation: all traits compile on every host.
/// A Linux host can produce patched PE / Mach-O binaries.
///
/// Source: D13§D1 (section naming), [DESIGN] (runtime lib naming).

#include <file_type_parser.hpp>

#include <cstdint>
#include <string_view>

namespace VMPilot::Loader {

// ---------------------------------------------------------------------------
// Compile-time traits per target format
// ---------------------------------------------------------------------------

template <Common::FileFormat F>
struct PlatformTraits;

template <>
struct PlatformTraits<Common::FileFormat::MachO> {
    static constexpr const char* runtime_lib  = "@rpath/libvmpilot_runtime.dylib";
    static constexpr const char* section_name = ".vmpilot";    // user decision
    static constexpr const char* segment_name = "__VMPILOT";
    static constexpr uint64_t    page_align   = 0x4000;        // 16KB ARM64 macOS
};

template <>
struct PlatformTraits<Common::FileFormat::ELF> {
    static constexpr const char* runtime_lib  = "libvmpilot_runtime.so";
    static constexpr const char* section_name = ".vmpilot";
    static constexpr const char* segment_name = nullptr;        // ELF segments unnamed
    static constexpr uint64_t    page_align   = 0x1000;        // 4KB
};

template <>
struct PlatformTraits<Common::FileFormat::PE> {
    static constexpr const char* runtime_lib  = "vmpilot_runtime.dll";
    static constexpr const char* section_name = ".vmpltt";      // 8-char PE limit
    static constexpr const char* segment_name = nullptr;
    static constexpr uint64_t    page_align   = 0x1000;        // 4KB
};

// ---------------------------------------------------------------------------
// Runtime bridge (format known at runtime, not compile time)
// ---------------------------------------------------------------------------

struct FormatConfig {
    std::string_view runtime_lib;
    std::string_view section_name;
    uint64_t page_align;

    static FormatConfig for_format(Common::FileFormat f) noexcept {
        switch (f) {
            case Common::FileFormat::MachO: return from<Common::FileFormat::MachO>();
            case Common::FileFormat::ELF:   return from<Common::FileFormat::ELF>();
            case Common::FileFormat::PE:    return from<Common::FileFormat::PE>();
            default:                        return {"", "", 0x1000};
        }
    }

private:
    template <Common::FileFormat F>
    static constexpr FormatConfig from() {
        return {
            PlatformTraits<F>::runtime_lib,
            PlatformTraits<F>::section_name,
            PlatformTraits<F>::page_align,
        };
    }
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_PLATFORM_TRAITS_HPP__
