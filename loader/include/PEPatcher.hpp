#ifndef __LOADER_PE_PATCHER_HPP__
#define __LOADER_PE_PATCHER_HPP__
#pragma once

#include <BinaryPatcher.hpp>

namespace VMPilot::Loader {

/// PE-specific binary patcher (stub — not yet implemented).
class PEPatcher : public BinaryPatcher {
public:
    [[nodiscard]] tl::expected<PatchResult, Common::DiagnosticCode>
    patch(const PatchRequest& request,
          Common::DiagnosticCollector& diag) noexcept override;

    [[nodiscard]] std::string name() const noexcept override { return "PE"; }
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_PE_PATCHER_HPP__
