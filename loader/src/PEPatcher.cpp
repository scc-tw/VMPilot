#include <PEPatcher.hpp>

namespace VMPilot::Loader {

tl::expected<PatchResult, Common::DiagnosticCode>
PEPatcher::patch(const PatchRequest& /*request*/,
                 Common::DiagnosticCollector& diag) noexcept {
    diag.error("loader", Common::DiagnosticCode::PatchFormatUnsupported,
               "PE binary patching is not yet implemented");
    return tl::unexpected(Common::DiagnosticCode::PatchFormatUnsupported);
}

}  // namespace VMPilot::Loader
