#ifndef __SDK_X86_TRAITS_HPP__
#define __SDK_X86_TRAITS_HPP__
#pragma once

#include "ArchHandlerCommon.hpp"
#include "RegValueResolver.hpp"

#include <x86.hpp>

namespace VMPilot::SDK::Segmentator {

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

struct X86CallbackTraits {
    static std::optional<std::string> resolveCall(
        const Capstone::Instruction& insn, const AddrToSymbol& lookup);
    static std::optional<uint64_t> extractStringArg(
        size_t call_idx,
        const std::vector<Capstone::Instruction>& instructions,
        const AddrToSymbol& lookup);
};

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_X86_TRAITS_HPP__
