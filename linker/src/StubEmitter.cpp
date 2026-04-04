#include <file_type_parser.hpp>
#include <StubEmitter.hpp>

#include <memory>

namespace VMPilot::Loader {

// Forward-declare factory functions defined in each emitter TU.
std::unique_ptr<StubEmitter> make_x86_64_emitter();   // X86_64StubEmitter.cpp
std::unique_ptr<StubEmitter> make_x86_32_emitter();   // X86_32StubEmitter.cpp
std::unique_ptr<StubEmitter> make_arm64_emitter();     // ARM64StubEmitter.cpp

std::unique_ptr<StubEmitter>
create_emitter(Common::FileArch arch, Common::FileMode mode) noexcept {
    using FA = Common::FileArch;
    using FM = Common::FileMode;

    if (arch == FA::X86) {
        const auto m = static_cast<uint32_t>(mode);
        if (m & static_cast<uint32_t>(FM::MODE_64))
            return make_x86_64_emitter();
        if (m & static_cast<uint32_t>(FM::MODE_32))
            return make_x86_32_emitter();
    }

    if (arch == FA::ARM64)
        return make_arm64_emitter();

    return nullptr;
}

}  // namespace VMPilot::Loader
