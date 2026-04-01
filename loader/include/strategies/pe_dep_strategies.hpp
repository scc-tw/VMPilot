#ifndef __LOADER_STRATEGIES_PE_DEP_HPP__
#define __LOADER_STRATEGIES_PE_DEP_HPP__
#pragma once

/// @file strategies/pe_dep_strategies.hpp
/// @brief FallbackChain strategies for PE import injection.
///
/// One strategy:
///   1. CoffiImportInject — Use coffi-modern coff_editor::imports() to
///      inject a new import entry into the import directory.

#include <fallback_chain.hpp>
#include <diagnostic.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <string_view>

namespace coffi {
struct pe32_traits;
template <typename Traits> class coff_editor;
}  // namespace coffi

namespace VMPilot::Loader::strategies {

/// PE import injection via coffi-modern coff_editor.
///
/// Uses coff_editor::imports() builder to add a new import entry
/// for the runtime DLL.  All existing imports are preserved.
struct CoffiImportInject {
    static constexpr const char* name = "CoffiImportInject";
    using result_type = tl::expected<void, Common::DiagnosticCode>;

    static result_type try_execute(
        Common::DiagnosticCollector& diag,
        coffi::coff_editor<coffi::pe32_traits>& editor,
        std::string_view dll_name) noexcept;
};

/// PE dep injection chain (single strategy — PE has no graceful fallback).
using PeDepInjector = Chain<CoffiImportInject>;

}  // namespace VMPilot::Loader::strategies

#endif  // __LOADER_STRATEGIES_PE_DEP_HPP__
