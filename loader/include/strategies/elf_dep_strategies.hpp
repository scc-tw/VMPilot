#ifndef __LOADER_STRATEGIES_ELF_DEP_HPP__
#define __LOADER_STRATEGIES_ELF_DEP_HPP__
#pragma once

/// @file strategies/elf_dep_strategies.hpp
/// @brief FallbackChain strategies for ELF DT_NEEDED injection.
///
/// Two strategies, tried in order:
///
///   1. StealDtNull — Find a spare DT_NULL padding slot in .dynamic and
///      overwrite it with a DT_NEEDED entry.  Preferred because it does
///      not grow the section or require ELFIO relayout.
///
///   2. GrowDynamic — Overwrite the DT_NULL terminator with DT_NEEDED,
///      then append a new DT_NULL.  Grows .dynamic by one entry; ELFIO
///      relayouts the file on save().
///
/// Usage:
///   using ElfDepInjector = Chain<StealDtNull, GrowDynamic>;
///   auto result = ElfDepInjector::execute(diag, elf_reader, lib_name);

#include <fallback_chain.hpp>
#include <diagnostic.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <string_view>

// Forward declaration — avoid pulling ELFIO into every TU
namespace ELFIO { class elfio; }

namespace VMPilot::Loader::strategies {

/// Layer 1: Steal a spare DT_NULL slot in .dynamic (no section growth).
///
/// The ELF .dynamic section typically has extra DT_NULL entries as padding.
/// If a spare slot exists (DT_NULL followed by another DT_NULL), this
/// strategy overwrites the first DT_NULL with a DT_NEEDED entry pointing
/// to the library soname in .dynstr.  The second DT_NULL serves as the
/// new terminator.
///
/// Advantages: no section growth, no ELFIO relayout, minimal mutation.
/// Fails if: no .dynamic, no spare DT_NULL, or malformed .dynamic.
struct StealDtNull {
    static constexpr const char* name = "StealDtNull";
    using result_type = tl::expected<void, Common::DiagnosticCode>;

    static result_type try_execute(
        Common::DiagnosticCollector& diag,
        ELFIO::elfio& reader,
        std::string_view soname) noexcept;
};

/// Layer 2: Grow .dynamic by appending a new entry after the DT_NULL terminator.
///
/// Overwrites the current DT_NULL with a DT_NEEDED entry, then appends a
/// fresh DT_NULL as the new terminator.  This grows .dynamic by one entry;
/// ELFIO recalculates all file offsets on save().
///
/// Advantages: always works if .dynamic has a DT_NULL terminator.
/// Disadvantages: modifies section size, requires ELFIO relayout.
/// Fails if: no .dynamic, no DT_NULL terminator, or missing .dynstr.
struct GrowDynamic {
    static constexpr const char* name = "GrowDynamic";
    using result_type = tl::expected<void, Common::DiagnosticCode>;

    static result_type try_execute(
        Common::DiagnosticCollector& diag,
        ELFIO::elfio& reader,
        std::string_view soname) noexcept;
};

/// Composed chain: try StealDtNull first, then GrowDynamic.
using ElfDepInjector = Chain<StealDtNull, GrowDynamic>;

}  // namespace VMPilot::Loader::strategies

#endif  // __LOADER_STRATEGIES_ELF_DEP_HPP__
