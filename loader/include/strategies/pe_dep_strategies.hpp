#ifndef __LOADER_STRATEGIES_PE_DEP_HPP__
#define __LOADER_STRATEGIES_PE_DEP_HPP__
#pragma once

/// @file strategies/pe_dep_strategies.hpp
/// @brief FallbackChain strategies for PE import injection.
///
/// One strategy:
///   1. CoffiImportInject — Use COFFI import_section_accessor to reconstruct
///      the import directory and inject a new import entry.

#include <fallback_chain.hpp>
#include <diagnostic.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <string_view>

namespace COFFI { class coffi; }

namespace VMPilot::Loader::strategies {

/// PE import injection via COFFI import_section_accessor.
///
/// Reconstructs the entire .idata section (IDT + ILT + IAT) with the
/// new import entry appended.  Preserves all existing imports.
struct CoffiImportInject {
    static constexpr const char* name = "CoffiImportInject";
    using result_type = tl::expected<void, Common::DiagnosticCode>;

    static result_type try_execute(
        Common::DiagnosticCollector& diag,
        COFFI::coffi& reader,
        std::string_view dll_name) noexcept;
};

/// PE dep injection chain (single strategy — PE has no graceful fallback).
using PeDepInjector = Chain<CoffiImportInject>;

}  // namespace VMPilot::Loader::strategies

#endif  // __LOADER_STRATEGIES_PE_DEP_HPP__
