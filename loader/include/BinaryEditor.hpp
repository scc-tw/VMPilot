#ifndef __LOADER_BINARY_EDITOR_HPP__
#define __LOADER_BINARY_EDITOR_HPP__
#pragma once

/// @file BinaryEditor.hpp
/// @brief Variant-based type-erased editor and factory for binary mutation.
///
/// Each platform editor (ELFEditor, PEEditor, MachOEditor) inherits from
/// EditorBase<Derived> (CRTP).  At the factory boundary, runtime format
/// selection is done via std::variant<ELFEditor, PEEditor, MachOEditor>.
///
/// Source: D13 D1-D3 (binary patching / loader).

#include <LoaderTypes.hpp>
#include <diagnostic_collector.hpp>

#include <ELFEditor.hpp>
#include <MachOEditor.hpp>
#include <PEEditor.hpp>

#include <tl/expected.hpp>

#include <variant>

namespace VMPilot::Loader {

/// Runtime-polymorphic editor handle.  std::visit dispatches to the
/// concrete CRTP editor without vtable indirection.
using AnyEditor = std::variant<ELFEditor, PEEditor, MachOEditor>;

/// Factory. Opens a binary file and returns the appropriate editor
/// wrapped in the AnyEditor variant.
[[nodiscard]] tl::expected<AnyEditor, Common::DiagnosticCode>
open_binary(const std::string& path, Common::FileFormat format,
            Common::DiagnosticCollector& diag) noexcept;

}  // namespace VMPilot::Loader

#endif  // __LOADER_BINARY_EDITOR_HPP__
