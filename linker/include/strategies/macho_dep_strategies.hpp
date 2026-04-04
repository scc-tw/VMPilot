#ifndef __LOADER_STRATEGIES_MACHO_DEP_HPP__
#define __LOADER_STRATEGIES_MACHO_DEP_HPP__
#pragma once

/// @file strategies/macho_dep_strategies.hpp
/// @brief FallbackChain strategies for Mach-O LC_LOAD_DYLIB injection.
///
/// Two strategies:
///   1. UseHeaderPadding — Insert LC_LOAD_DYLIB in the gap between the
///      last existing load command and the first section data.  Preferred
///      because it does not shift any section data.
///
///   2. ShiftSectionData — If header padding is insufficient, expand the
///      header by shifting all section data forward.  All file offsets in
///      load commands are recalculated.

#include <fallback_chain.hpp>
#include <diagnostic.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <string_view>
#include <vector>

namespace VMPilot::Loader::strategies {

/// Layer 1: Use existing header padding for LC_LOAD_DYLIB.
struct UseHeaderPadding {
    static constexpr const char* name = "UseHeaderPadding";
    using result_type = tl::expected<void, Common::DiagnosticCode>;

    static result_type try_execute(
        Common::DiagnosticCollector& diag,
        std::vector<uint8_t>& buf,
        uint32_t& ncmds,
        uint32_t& sizeofcmds,
        size_t first_sect_off,
        std::string_view install_name) noexcept;
};

/// Layer 2: Shift section data to make room for LC_LOAD_DYLIB.
struct ShiftSectionData {
    static constexpr const char* name = "ShiftSectionData";
    using result_type = tl::expected<void, Common::DiagnosticCode>;

    static result_type try_execute(
        Common::DiagnosticCollector& diag,
        std::vector<uint8_t>& buf,
        uint32_t& ncmds,
        uint32_t& sizeofcmds,
        size_t first_sect_off,
        std::string_view install_name) noexcept;
};

/// MachO dep injection chain: try header padding, then shift.
using MachoDepInjector = Chain<UseHeaderPadding, ShiftSectionData>;

}  // namespace VMPilot::Loader::strategies

#endif  // __LOADER_STRATEGIES_MACHO_DEP_HPP__
