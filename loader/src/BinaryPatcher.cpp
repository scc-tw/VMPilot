#include <BinaryPatcher.hpp>
#include <ELFPatcher.hpp>
#include <PEPatcher.hpp>
#include <MachOPatcher.hpp>

namespace VMPilot::Loader {

std::unique_ptr<BinaryPatcher>
create_patcher(Common::FileFormat format) noexcept {
    switch (format) {
        case Common::FileFormat::ELF:
            return std::make_unique<ELFPatcher>("ELF");
        case Common::FileFormat::PE:
            return std::make_unique<PEPatcher>("PE");
        case Common::FileFormat::MachO:
            return std::make_unique<MachOPatcher>("MachO");
        default:
            return nullptr;
    }
}

}  // namespace VMPilot::Loader
