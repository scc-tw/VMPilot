#ifndef __LOADER_MACHO_PATCHER_HPP__
#define __LOADER_MACHO_PATCHER_HPP__
#pragma once

#include <BinaryPatcher.hpp>

namespace VMPilot::Loader {

/// Mach-O-specific binary patcher (stub — not yet implemented).
class MachOPatcher : public BinaryPatcher {
public:
    [[nodiscard]] tl::expected<PatchResult, Common::DiagnosticCode>
    patch(const PatchRequest& request,
          Common::DiagnosticCollector& diag) noexcept override;

    [[nodiscard]] std::string name() const noexcept override { return "MachO"; }
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_MACHO_PATCHER_HPP__
