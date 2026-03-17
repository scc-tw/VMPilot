#include <ARM64Handler.hpp>

#include "ArchHandlerCommon.hpp"
#include "RegValueResolver.hpp"

#include <arm64.hpp>

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
        if (it != lookup.end())
            return it->second;
    }
    return std::nullopt;
}

// --- ARM64 arch traits for generic backward constant propagation ---
struct ARM64ArchTraits {
    static bool isCalleeSaved(unsigned reg) {
        return Capstone::ARM64::isCalleeSaved(reg);
    }
    static bool writesToReg(const Capstone::Instruction& insn, unsigned reg) {
        return Capstone::ARM64::writesToReg(insn, reg);
    }
    static WriteClassification classifyWrite(const Capstone::Instruction& insn,
                                             unsigned /*reg*/) {
        namespace CA = Capstone::ARM64;

        // --- Terminal patterns ---
        if (CA::isADRP(insn))
            return ResolvedConstant{
                static_cast<uint64_t>(insn.operands[1].imm)};

        if (CA::isADR(insn))
            return ResolvedConstant{
                static_cast<uint64_t>(insn.operands[1].imm)};

        // --- Forwarding patterns ---
        if (CA::isRegToRegMov(insn))
            return RegisterForward{CA::getMovSource(insn)};

        if (CA::isRegPlusImmADD(insn))
            return ArithmeticAdjust{insn.operands[1].reg,
                                    insn.operands[2].imm};

        if (CA::isRegMinusImmSUB(insn))
            return ArithmeticAdjust{insn.operands[1].reg,
                                    -insn.operands[2].imm};

        return Unresolvable{};
    }
};

// Extract the VA of the first string argument near a BL instruction.
// Uses backward constant propagation to handle arbitrary compiler code
// generation patterns including instruction scheduling, register spilling,
// ADRP+ADD pairs with intervening instructions, and cross-register variants.
static std::optional<uint64_t> arm64ExtractStringArg(
    size_t call_idx,
    const std::vector<Capstone::Instruction>& instructions) {
    if (call_idx == 0)
        return std::nullopt;

    constexpr size_t kMaxWindow = 20;
    const size_t start = (call_idx > kMaxWindow) ? call_idx - kMaxWindow : 0;

    // Resolve X0 (AAPCS64 first argument register) at the call site.
    uint64_t va = resolveRegValue<ARM64ArchTraits>(
        Capstone::ARM64::firstArgReg(), call_idx - 1, instructions, start);
    if (va != 0)
        return va;
    return std::nullopt;
}

std::vector<std::unique_ptr<NativeFunctionBase>>
ARM64Handler::doGetNativeFunctions() noexcept {
    pImpl->compilation_ctx = &m_compilation_ctx;
    return extractNativeFunctions(*pImpl, arm64ResolveCallTarget,
                                  arm64ExtractStringArg);
}
