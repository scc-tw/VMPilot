#include <MachOPatcher.hpp>

namespace VMPilot::Loader {

tl::expected<PatchResult, Common::DiagnosticCode>
MachOPatcher::patch(const PatchRequest& /*request*/,
                    Common::DiagnosticCollector& diag) noexcept {
    diag.error("loader", Common::DiagnosticCode::PatchFormatUnsupported,
               "Mach-O binary patching is not yet implemented");
    return tl::unexpected(Common::DiagnosticCode::PatchFormatUnsupported);
}

}  // namespace VMPilot::Loader
