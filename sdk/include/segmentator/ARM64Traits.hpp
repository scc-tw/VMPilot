#ifndef __SDK_ARM64_TRAITS_HPP__
#define __SDK_ARM64_TRAITS_HPP__
#pragma once

#include <ArchHandlerCommon.hpp>
#include <RegValueResolver.hpp>

#include <arm64.hpp>

namespace VMPilot::SDK::Segmentator {

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

struct ARM64CallbackTraits {
    static std::optional<std::string> resolveCall(
        const Capstone::Instruction& insn, const AddrToSymbol& lookup);
    static std::optional<uint64_t> extractStringArg(
        size_t call_idx,
        const std::vector<Capstone::Instruction>& instructions,
        const AddrToSymbol& lookup);
};

}  // namespace VMPilot::SDK::Segmentator

#endif  // __SDK_ARM64_TRAITS_HPP__
