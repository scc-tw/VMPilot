#include <X86Handler.hpp>

#include "ArchHandlerCommon.hpp"
#include "RegValueResolver.hpp"

#include <x86.hpp>

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
    uint64_t target = insn.getDirectTarget();
    if (target != 0) {
        auto it = lookup.find(target);
        if (it != lookup.end())
            return it->second;
    }
    target = insn.getRipRelativeTarget();
    if (target != 0) {
        auto it = lookup.find(target);
        if (it != lookup.end())
            return it->second;
    }
    return std::nullopt;
}

// --- X86 arch traits for generic backward constant propagation ---
struct X86ArchTraits {
    static bool isCalleeSaved(unsigned reg) {
        return Capstone::X86::isCalleeSaved(reg);
    }
    static bool writesToReg(const Capstone::Instruction& insn, unsigned reg) {
        return Capstone::X86::writesToReg(insn, reg);
    }
    static WriteClassification classifyWrite(const Capstone::Instruction& insn,
                                             unsigned /*reg*/) {
        namespace CX = Capstone::X86;

        // --- Terminal patterns (produce a constant) ---
        if (CX::isRipRelativeLEA(insn))
            return ResolvedConstant{CX::computeRipRelativeVA(insn)};

        if (CX::isImmediateLoad(insn))
            return ResolvedConstant{CX::getImmediateValue(insn)};

        // --- Forwarding patterns (trace the source) ---
        if (CX::isRegToRegMov(insn))
            return RegisterForward{CX::getMovSource(insn)};

        // lea reg, [base+disp] (non-RIP; includes lea reg, [disp])
        if (CX::isNonRipLEA(insn)) {
            unsigned base = insn.operands[1].mem.base;
            unsigned idx = insn.operands[1].mem.index;
            int64_t disp = insn.operands[1].mem.disp;

            if (idx != 0)
                return Unresolvable{};
            if (base == 0)
                return ResolvedConstant{static_cast<uint64_t>(disp)};
            return ArithmeticAdjust{base, disp};
        }

        if (CX::isImmediateADD(insn))
            return ArithmeticAdjust{insn.operands[0].reg,
                                    insn.operands[1].imm};

        if (CX::isImmediateSUB(insn))
            return ArithmeticAdjust{insn.operands[0].reg,
                                    -insn.operands[1].imm};

        return Unresolvable{};
    }
};

// Extract the VA of the first string argument near a call instruction.
// Uses backward constant propagation to handle arbitrary compiler code
// generation patterns including instruction scheduling, register spilling,
// and arithmetic address adjustments.
static std::optional<uint64_t> x86ExtractStringArg(
    size_t call_idx,
    const std::vector<Capstone::Instruction>& instructions) {
    namespace CX = Capstone::X86;

    if (call_idx == 0)
        return std::nullopt;

    constexpr size_t kMaxWindow = 20;
    const size_t start = (call_idx > kMaxWindow) ? call_idx - kMaxWindow : 0;

    // --- x86-64: find the last write to any first-arg register (RDI/RCX)
    //     before the call, then resolve its value via constant propagation.
    for (size_t i = call_idx; i-- > start;) {
        const auto& insn = instructions[i];

        // Arg registers are caller-saved; stop at call/ret boundary
        if (insn.isCall() || insn.isRet())
            break;

        // Found an instruction that sets a first-arg register
        if (!insn.operands.empty() &&
            insn.operands[0].type == Capstone::OpType::REG &&
            CX::isFirstArgReg(insn.operands[0].reg) &&
            !CX::isReadOnlyOp(insn)) {
            uint64_t va = resolveRegValue<X86ArchTraits>(
                insn.operands[0].reg, i, instructions, start);
            if (va != 0)
                return va;
            break;  // found the write but couldn't resolve — give up
        }
    }

    // --- x86-32 fallback: find the argument on the stack ---
    for (size_t i = call_idx; i-- > start;) {
        const auto& insn = instructions[i];

        // Stop at prior CALL — its stack args aren't ours
        if (insn.isCall())
            break;

        // push imm32
        if (CX::isImmediatePush(insn))
            return static_cast<uint64_t>(insn.operands[0].imm);

        // mov [esp/rsp], imm
        if (CX::isImmediateStoreToStack(insn))
            return CX::getImmediateValue(insn);

        // push reg → resolve reg's value
        if (CX::isRegPush(insn)) {
            uint64_t va = resolveRegValue<X86ArchTraits>(
                insn.operands[0].reg, i > 0 ? i - 1 : 0, instructions,
                start);
            if (va != 0)
                return va;
            break;
        }

        // mov [esp/rsp+off], reg → resolve reg's value
        if (CX::isRegStoreToStack(insn)) {
            uint64_t va = resolveRegValue<X86ArchTraits>(
                insn.operands[1].reg, i > 0 ? i - 1 : 0, instructions,
                start);
            if (va != 0)
                return va;
            break;
        }
    }

    return std::nullopt;
}

std::vector<std::unique_ptr<NativeFunctionBase>>
X86Handler::doGetNativeFunctions() noexcept {
    pImpl->compilation_ctx = &m_compilation_ctx;
    return extractNativeFunctions(*pImpl, x86ResolveCallTarget,
                                  x86ExtractStringArg);
}
