#include <PEPatcherTraits.hpp>

namespace VMPilot::Loader {

tl::expected<std::unique_ptr<BinaryContext>, Common::DiagnosticCode>
PEPatcherTraits::open(const std::string& /*path*/,
                      Common::DiagnosticCollector& diag) noexcept {
    diag.error("loader", Common::DiagnosticCode::PatchFormatUnsupported,
               "PE binary patching is not yet implemented");
    return tl::unexpected(Common::DiagnosticCode::PatchFormatUnsupported);
}

tl::expected<TextSectionInfo, Common::DiagnosticCode>
PEPatcherTraits::get_text_section(BinaryContext& /*ctx*/,
                                  Common::DiagnosticCollector& diag) noexcept {
    diag.error("loader", Common::DiagnosticCode::PatchFormatUnsupported, "PE not implemented");
    return tl::unexpected(Common::DiagnosticCode::PatchFormatUnsupported);
}

tl::expected<void, Common::DiagnosticCode>
PEPatcherTraits::overwrite_text(BinaryContext& /*ctx*/, uint64_t /*va*/,
                                const uint8_t* /*data*/, size_t /*len*/,
                                Common::DiagnosticCollector& diag) noexcept {
    diag.error("loader", Common::DiagnosticCode::PatchFormatUnsupported, "PE not implemented");
    return tl::unexpected(Common::DiagnosticCode::PatchFormatUnsupported);
}

tl::expected<NewSegmentInfo, Common::DiagnosticCode>
PEPatcherTraits::add_segment(BinaryContext& /*ctx*/, const std::string& /*name*/,
                             const std::vector<uint8_t>& /*payload*/, uint64_t /*alignment*/,
                             Common::DiagnosticCollector& diag) noexcept {
    diag.error("loader", Common::DiagnosticCode::PatchFormatUnsupported, "PE not implemented");
    return tl::unexpected(Common::DiagnosticCode::PatchFormatUnsupported);
}

tl::expected<void, Common::DiagnosticCode>
PEPatcherTraits::save(BinaryContext& /*ctx*/, const std::string& /*path*/,
                      Common::DiagnosticCollector& diag) noexcept {
    diag.error("loader", Common::DiagnosticCode::PatchFormatUnsupported, "PE not implemented");
    return tl::unexpected(Common::DiagnosticCode::PatchFormatUnsupported);
}

}  // namespace VMPilot::Loader
