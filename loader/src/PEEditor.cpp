#include <PEEditor.hpp>

namespace VMPilot::Loader {

using DC = Common::DiagnosticCode;

static tl::unexpected<DC> not_impl(Common::DiagnosticCollector& diag) {
    diag.error("loader", DC::PatchFormatUnsupported,
               "PE binary patching is not yet implemented");
    return tl::unexpected(DC::PatchFormatUnsupported);
}

tl::expected<PEEditor, DC>
PEEditor::open(const std::string&, Common::DiagnosticCollector& diag) noexcept {
    return not_impl(diag);
}

TextSectionInfo PEEditor::text_section() const noexcept { return {}; }
uint64_t PEEditor::next_segment_va(uint64_t) const noexcept { return 0; }

tl::expected<void, DC>
PEEditor::overwrite_text(uint64_t, const uint8_t*, size_t,
                         Common::DiagnosticCollector& diag) noexcept {
    return not_impl(diag);
}

tl::expected<NewSegmentInfo, DC>
PEEditor::add_segment(std::string_view, const std::vector<uint8_t>&, uint64_t,
                      Common::DiagnosticCollector& diag) noexcept {
    return not_impl(diag);
}

tl::expected<void, DC>
PEEditor::add_runtime_dep(std::string_view, Common::DiagnosticCollector& diag) noexcept {
    return not_impl(diag);
}

void PEEditor::invalidate_signature() noexcept {
    // PE Authenticode: zero Certificate Table (index 4). Stubbed.
}

tl::expected<void, DC>
PEEditor::save(const std::string&, Common::DiagnosticCollector& diag) noexcept {
    return not_impl(diag);
}

}  // namespace VMPilot::Loader
