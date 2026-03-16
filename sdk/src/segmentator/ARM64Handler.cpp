#include <ARM64Handler.hpp>

#include "ArchHandlerCommon.hpp"

#include <spdlog/spdlog.h>

using namespace VMPilot::SDK::Segmentator;

// Self-register ARM64 arch handler
static ArchHandlerRegistrar arm64_registrar(
    VMPilot::Common::FileArch::ARM64,
    [](VMPilot::Common::FileMode mode, const NativeSymbolTable& symbols) {
        return std::make_unique<ARM64Handler>(mode, symbols);
    });

struct ARM64Handler::Impl : ArchHandlerImplBase {
    using ArchHandlerImplBase::ArchHandlerImplBase;
};

ARM64Handler::ARM64Handler(Mode mode, const NativeSymbolTable& symbols)
    : ArchHandlerStrategy(Arch::ARM64, mode),
      pImpl(make_arm64_handler_impl(mode, symbols)) {}

std::unique_ptr<ARM64Handler::Impl>
VMPilot::SDK::Segmentator::make_arm64_handler_impl(
    Mode mode, const NativeSymbolTable& symbols) {
    return std::make_unique<ARM64Handler::Impl>(
        Capstone::Capstone(Capstone::Arch::ARM64,
                           static_cast<Capstone::Mode>(mode)),
        buildAddrLookup(symbols));
}

ARM64Handler::~ARM64Handler() = default;

bool ARM64Handler::doLoad(const std::vector<uint8_t>& code,
                          const uint64_t base_addr) noexcept {
    pImpl->base_addr = base_addr;
    pImpl->instructions = pImpl->cs.disasm(code, base_addr);
    return !pImpl->instructions.empty();
}

// ARM64-specific call resolver: handles direct bl only
static std::optional<std::string> arm64ResolveCallTarget(
    const Capstone::Instruction& insn, const AddrToSymbol& lookup) {
    uint64_t target = insn.getDirectTarget();
    if (target != 0) {
        auto it = lookup.find(target);
        if (it != lookup.end()) {
            return it->second;
        }
    }
    return std::nullopt;
}

std::vector<std::unique_ptr<NativeFunctionBase>>
ARM64Handler::doGetNativeFunctions() noexcept {
    return extractNativeFunctions(*pImpl, arm64ResolveCallTarget);
}
