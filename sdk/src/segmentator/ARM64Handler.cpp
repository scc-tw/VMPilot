#include <ARM64Handler.hpp>

#include "ArchHandlerCommon.hpp"
#include "ARM64Traits.hpp"
#include "RegValueResolver.hpp"

#include <arm64.hpp>

#include <spdlog/spdlog.h>

using namespace VMPilot::SDK::Segmentator;

namespace VMPilot::SDK::Segmentator {
void registerARM64Handler() {
    HandlerRegistry::instance().registerArchHandler(
        VMPilot::Common::FileArch::ARM64,
        [](VMPilot::Common::FileMode mode, const NativeSymbolTable& symbols) {
            return std::make_unique<ARM64Handler>(mode, symbols);
        });
}
}  // namespace VMPilot::SDK::Segmentator

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

// --- ARM64CallbackTraits method definitions ---

std::optional<std::string> ARM64CallbackTraits::resolveCall(
    const Capstone::Instruction& insn, const AddrToSymbol& lookup) {
    uint64_t target = insn.getDirectTarget();
    if (target != 0) {
        auto it = lookup.find(target);
        if (it != lookup.end())
            return it->second;
    }
    return std::nullopt;
}

std::optional<uint64_t> ARM64CallbackTraits::extractStringArg(
    size_t call_idx,
    const std::vector<Capstone::Instruction>& instructions,
    const AddrToSymbol& /*lookup*/) {
    if (call_idx == 0)
        return std::nullopt;

    constexpr size_t kMaxWindow = 20;
    const size_t start =
        (call_idx > kMaxWindow) ? call_idx - kMaxWindow : 0;

    // Resolve X0 (AAPCS64 first argument register) at the call site.
    uint64_t va = resolveRegValue<ARM64ArchTraits>(
        Capstone::ARM64::firstArgReg(), call_idx - 1, instructions, start);
    if (va != 0)
        return va;
    return std::nullopt;
}

std::vector<NativeFunctionBase>
ARM64Handler::doGetNativeFunctions() noexcept {
    return extractNativeFunctions<ARM64CallbackTraits>(*pImpl, m_compilation_ctx);
}
