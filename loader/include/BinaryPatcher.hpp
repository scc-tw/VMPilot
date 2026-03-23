#ifndef __LOADER_BINARY_PATCHER_HPP__
#define __LOADER_BINARY_PATCHER_HPP__
#pragma once

#include <LoaderTypes.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <memory>
#include <string>

namespace VMPilot::Loader {

/// Abstract binary patcher (Strategy pattern, parallels CompilerBackend).
///
/// Implementations are format-specific: ELF, PE, Mach-O.
/// The patcher reads the original binary, applies patches (JMP stubs,
/// bytecode blob injection), and writes the output binary.
class BinaryPatcher {
public:
    virtual ~BinaryPatcher() = default;

    /// Patch the binary described by `request`.
    [[nodiscard]] virtual tl::expected<PatchResult, Common::DiagnosticCode>
    patch(const PatchRequest& request,
          Common::DiagnosticCollector& diag) noexcept = 0;

    /// Human-readable name of this patcher (e.g. "ELF", "PE", "MachO").
    [[nodiscard]] virtual std::string name() const noexcept = 0;
};

/// Factory.  Creates the appropriate patcher for the given format.
/// Currently supports: ELF.  PE and Mach-O return NotImplemented stubs.
[[nodiscard]] std::unique_ptr<BinaryPatcher>
create_patcher(Common::FileFormat format) noexcept;

}  // namespace VMPilot::Loader

#endif  // __LOADER_BINARY_PATCHER_HPP__
