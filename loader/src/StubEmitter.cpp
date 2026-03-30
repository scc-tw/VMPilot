#include <file_type_parser.hpp>
#include <StubEmitter.hpp>

#include <memory>

namespace VMPilot::Loader {

// Forward-declare factory functions defined in each emitter TU.
std::unique_ptr<StubEmitter> make_x86_64_emitter();   // X86_64StubEmitter.cpp
std::unique_ptr<StubEmitter> make_arm64_emitter();     // ARM64StubEmitter.cpp

std::unique_ptr<StubEmitter>
create_emitter(Common::FileArch arch, Common::FileMode mode) noexcept {
    using FA = Common::FileArch;
    using FM = Common::FileMode;

    if (arch == FA::X86 && mode == FM::MODE_64)
        return make_x86_64_emitter();

    if (arch == FA::ARM64 && mode == FM::MODE_LITTLE_ENDIAN)
        return make_arm64_emitter();

    return nullptr;
}

}  // namespace VMPilot::Loader
