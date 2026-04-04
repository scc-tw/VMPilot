#include <X86Handler.hpp>

#include <ArchHandlerCommon.hpp>
#include <HandlerRegistry.hpp>
#include <RegValueResolver.hpp>
#include <X86Traits.hpp>

#include <x86.hpp>

#include <spdlog/spdlog.h>

using namespace VMPilot::SDK::Segmentator;

namespace VMPilot::SDK::Segmentator {
void registerX86Handler() {
    HandlerRegistry::instance().registerArchHandler(
        VMPilot::Common::FileArch::X86,
        [](VMPilot::Common::FileMode mode, const NativeSymbolTable& symbols) {
            return std::make_unique<X86Handler>(mode, symbols);
        });
}
}  // namespace VMPilot::SDK::Segmentator

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

// --- X86CallbackTraits method definitions ---

std::optional<std::string> X86CallbackTraits::resolveCall(
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

std::optional<uint64_t> X86CallbackTraits::extractStringArg(
    size_t call_idx,
    const std::vector<Capstone::Instruction>& instructions,
    const AddrToSymbol& lookup) {
    namespace CX = Capstone::X86;

    if (call_idx == 0)
        return std::nullopt;

    // Build a PIC thunk resolver for x86-32 GOT-relative addressing.
    // __x86.get_pc_thunk.XX sets register XX to the return address.
    CallValueResolver pic_resolver =
        [&lookup](const Capstone::Instruction& insn,
                  unsigned reg) -> std::optional<uint64_t> {
        uint64_t target = insn.getDirectTarget();
        if (target == 0)
            return std::nullopt;
        auto it = lookup.find(target);
        if (it == lookup.end())
            return std::nullopt;
        if (CX::isPcThunkForReg(it->second, reg))
            return insn.address + insn.size;  // return address
        return std::nullopt;
    };

    constexpr size_t kMaxWindow = 20;
    const size_t start =
        (call_idx > kMaxWindow) ? call_idx - kMaxWindow : 0;

    // --- x86-64: find the last write to any first-arg register
    //     (RDI/RCX) before the call, then resolve via constant prop.
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
                insn.operands[0].reg, i, instructions, start, 0,
                pic_resolver);
            if (va != 0)
                return va;
            break;  // found the write but couldn't resolve -- give up
        }
    }

    // --- x86-32 fallback: find the argument on the stack ---
    for (size_t i = call_idx; i-- > start;) {
        const auto& insn = instructions[i];

        // Stop at prior CALL -- its stack args aren't ours
        if (insn.isCall())
            break;

        // push imm32
        if (CX::isImmediatePush(insn))
            return static_cast<uint64_t>(insn.operands[0].imm);

        // mov [esp/rsp], imm
        if (CX::isImmediateStoreToStack(insn))
            return CX::getImmediateValue(insn);

        // push reg -> resolve reg's value
        if (CX::isRegPush(insn)) {
            uint64_t va = resolveRegValue<X86ArchTraits>(
                insn.operands[0].reg, i > 0 ? i - 1 : 0, instructions,
                start, 0, pic_resolver);
            if (va != 0)
                return va;
            break;
        }

        // mov [esp/rsp+off], reg -> resolve reg's value
        if (CX::isRegStoreToStack(insn)) {
            uint64_t va = resolveRegValue<X86ArchTraits>(
                insn.operands[1].reg, i > 0 ? i - 1 : 0, instructions,
                start, 0, pic_resolver);
            if (va != 0)
                return va;
            break;
        }
    }

    return std::nullopt;
}

std::vector<NativeFunctionBase>
X86Handler::doGetNativeFunctions() noexcept {
    return extractNativeFunctions<X86CallbackTraits>(*pImpl, m_compilation_ctx);
}
