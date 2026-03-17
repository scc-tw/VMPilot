#include <X86Handler.hpp>

#include "ArchHandlerCommon.hpp"

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

// ---------------------------------------------------------------------------
// Backward constant propagation: resolve the constant value held in `reg`
// by scanning instructions backwards and following data flow through:
//   - LEA [rip+disp]              (PIC/PIE address)
//   - LEA [base+disp]             (base-relative, resolves base recursively)
//   - LEA [disp]                  (absolute address, non-PIE)
//   - MOV/MOVABS reg, imm         (absolute address)
//   - MOV reg, reg                (register forwarding, arbitrary chain length)
//   - ADD reg, imm / SUB reg, imm (arithmetic adjustment on address)
// Respects CALL/RET boundaries via callee-saved register awareness.
// ---------------------------------------------------------------------------
static uint64_t resolveRegValue(
    unsigned reg, size_t from_idx,
    const std::vector<Capstone::Instruction>& instructions, size_t min_idx,
    int depth = 0) {
    namespace CX = Capstone::X86;

    constexpr int kMaxDepth = 5;
    if (depth >= kMaxDepth || from_idx >= instructions.size())
        return 0;

    for (size_t i = from_idx + 1; i-- > min_idx;) {
        const auto& insn = instructions[i];

        // CALL/RET clobber caller-saved registers
        if (insn.isCall() || insn.isRet()) {
            if (!CX::isCalleeSaved(reg))
                return 0;
            continue;
        }

        // Skip instructions that don't write to our register
        if (!CX::writesToReg(insn, reg))
            continue;

        // --- Terminal patterns (produce a constant) ---

        // lea reg, [rip+disp]
        if (CX::isRipRelativeLEA(insn))
            return CX::computeRipRelativeVA(insn);

        // mov/movabs reg, imm
        if (CX::isImmediateLoad(insn))
            return CX::getImmediateValue(insn);

        // --- Forwarding patterns (trace the source) ---

        // mov reg, src_reg
        if (CX::isRegToRegMov(insn))
            return resolveRegValue(CX::getMovSource(insn), i > 0 ? i - 1 : 0,
                                   instructions, min_idx, depth + 1);

        // lea reg, [base+disp] (non-RIP; includes lea reg, [disp])
        if (CX::isNonRipLEA(insn)) {
            unsigned base = insn.operands[1].mem.base;
            unsigned idx = insn.operands[1].mem.index;
            int64_t disp = insn.operands[1].mem.disp;

            // Reject SIB with index register — too complex
            if (idx != 0)
                return 0;

            // lea reg, [disp] — no base, absolute address
            if (base == 0)
                return static_cast<uint64_t>(disp);

            // lea reg, [base+disp] — resolve base recursively
            uint64_t base_val = resolveRegValue(
                base, i > 0 ? i - 1 : 0, instructions, min_idx, depth + 1);
            if (base_val != 0)
                return static_cast<uint64_t>(
                    static_cast<int64_t>(base_val) + disp);
            return 0;
        }

        // add reg, imm — address adjustment
        if (CX::isImmediateADD(insn)) {
            int64_t imm = insn.operands[1].imm;
            uint64_t prev = resolveRegValue(reg, i > 0 ? i - 1 : 0,
                                            instructions, min_idx, depth + 1);
            if (prev != 0)
                return static_cast<uint64_t>(
                    static_cast<int64_t>(prev) + imm);
            return 0;
        }

        // sub reg, imm — address adjustment
        if (CX::isImmediateSUB(insn)) {
            int64_t imm = insn.operands[1].imm;
            uint64_t prev = resolveRegValue(reg, i > 0 ? i - 1 : 0,
                                            instructions, min_idx, depth + 1);
            if (prev != 0)
                return static_cast<uint64_t>(
                    static_cast<int64_t>(prev) - imm);
            return 0;
        }

        // Unknown write to register → can't resolve
        return 0;
    }

    return 0;
}

// Extract the VA of the first string argument near a call instruction.
// Uses backward constant propagation to handle arbitrary compiler code
// generation patterns including instruction scheduling, register spilling,
// and arithmetic address adjustments.
static uint64_t x86ExtractStringArg(
    size_t call_idx,
    const std::vector<Capstone::Instruction>& instructions) {
    namespace CX = Capstone::X86;

    if (call_idx == 0)
        return 0;

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
            insn.operands[0].type == CX::OpType::REG &&
            CX::isFirstArgReg(insn.operands[0].reg) &&
            !CX::isReadOnlyOp(insn)) {
            uint64_t va = resolveRegValue(insn.operands[0].reg, i,
                                          instructions, start);
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
            uint64_t va = resolveRegValue(insn.operands[0].reg,
                                          i > 0 ? i - 1 : 0, instructions,
                                          start);
            if (va != 0)
                return va;
            break;
        }

        // mov [esp/rsp+off], reg → resolve reg's value
        if (CX::isRegStoreToStack(insn)) {
            uint64_t va = resolveRegValue(insn.operands[1].reg,
                                          i > 0 ? i - 1 : 0, instructions,
                                          start);
            if (va != 0)
                return va;
            break;
        }
    }

    return 0;
}

std::vector<std::unique_ptr<NativeFunctionBase>>
X86Handler::doGetNativeFunctions() noexcept {
    pImpl->compilation_ctx = &m_compilation_ctx;
    return extractNativeFunctions(*pImpl, x86ResolveCallTarget,
                                  x86ExtractStringArg);
}
