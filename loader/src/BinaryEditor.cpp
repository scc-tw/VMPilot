#include <BinaryEditor.hpp>
#include <ELFEditor.hpp>
#include <MachOEditor.hpp>
#include <PEEditor.hpp>

namespace VMPilot::Loader {

tl::expected<std::unique_ptr<BinaryEditor>, Common::DiagnosticCode>
open_binary(const std::string& path, Common::FileFormat format,
            Common::DiagnosticCollector& diag) noexcept {
    using DC = Common::DiagnosticCode;

    switch (format) {
        case Common::FileFormat::MachO: {
            auto ed = MachOEditor::open(path, diag);
            if (!ed) return tl::unexpected(ed.error());
            return std::make_unique<MachOEditor>(std::move(*ed));
        }
        case Common::FileFormat::ELF: {
            auto ed = ELFEditor::open(path, diag);
            if (!ed) return tl::unexpected(ed.error());
            return std::make_unique<ELFEditor>(std::move(*ed));
        }
        case Common::FileFormat::PE: {
            auto ed = PEEditor::open(path, diag);
            if (!ed) return tl::unexpected(ed.error());
            return std::make_unique<PEEditor>(std::move(*ed));
        }
        default:
            diag.error("loader", DC::PatchFormatUnsupported, "unknown binary format");
            return tl::unexpected(DC::PatchFormatUnsupported);
    }
}

}  // namespace VMPilot::Loader
