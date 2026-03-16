#include <X86Handler.hpp>

#include "ArchHandlerCommon.hpp"

#include <spdlog/spdlog.h>

using namespace VMPilot::SDK::Segmentator;

static ArchHandlerRegistrar x86_registrar(
    VMPilot::Common::FileArch::X86,
    [](VMPilot::Common::FileMode mode, const NativeSymbolTable& symbols) {
        return std::make_unique<X86Handler>(mode, symbols);
    });

struct X86Handler::Impl : ArchHandlerImplBase {
    using ArchHandlerImplBase::ArchHandlerImplBase;
};

X86Handler::X86Handler(Mode mode, const NativeSymbolTable& symbols)
    : ArchHandlerStrategy(Arch::X86, mode),
      pImpl(make_x86_handler_impl(mode, symbols)) {}

std::unique_ptr<X86Handler::Impl>
VMPilot::SDK::Segmentator::make_x86_handler_impl(
    Mode mode, const NativeSymbolTable& symbols) {
    return std::make_unique<X86Handler::Impl>(
        Capstone::Capstone(Capstone::Arch::X86,
                           static_cast<Capstone::Mode>(mode)),
        buildAddrLookup(symbols));
}

X86Handler::~X86Handler() = default;

bool X86Handler::doLoad(const std::vector<uint8_t>& code,
                        const uint64_t base_addr) noexcept {
    pImpl->base_addr = base_addr;
    pImpl->instructions = pImpl->cs.disasm(code, base_addr);
    return !pImpl->instructions.empty();
}

// X86-specific call resolver: handles direct calls and RIP-relative indirect
static std::optional<std::string> x86ResolveCallTarget(
    const Capstone::Instruction& insn, const AddrToSymbol& lookup) {
    // Case 1: direct call/jmp with immediate operand
    uint64_t target = insn.getDirectTarget();
    if (target != 0) {
        auto it = lookup.find(target);
        if (it != lookup.end()) {
            return it->second;
        }
    }

    // Case 2: RIP-relative indirect call/jmp (e.g. -fno-plt)
    target = insn.getRipRelativeTarget();
    if (target != 0) {
        auto it = lookup.find(target);
        if (it != lookup.end()) {
            return it->second;
        }
    }

    return std::nullopt;
}

std::vector<std::unique_ptr<NativeFunctionBase>>
X86Handler::doGetNativeFunctions() noexcept {
    return extractNativeFunctions(*pImpl, x86ResolveCallTarget);
}
