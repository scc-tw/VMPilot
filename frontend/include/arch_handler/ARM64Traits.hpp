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

        // --- Memory load patterns ---
        // LDR / LDUR family (excluding atomic loads: ldar, ldxr, ldaxr).
        // Detects base-register + displacement loads without index register.
        if (insn.operands.size() >= 2 &&
            insn.operands[0].type == Capstone::OpType::REG &&
            insn.operands[1].type == Capstone::OpType::MEM &&
            insn.operands[1].mem.base != 0 &&
            insn.operands[1].mem.index == 0 &&
            insn.mnemonic.size() >= 3 &&
            (insn.mnemonic.compare(0, 4, "ldur") == 0 ||
             insn.mnemonic.compare(0, 3, "ldr") == 0)) {
            return MemoryLoad{insn.operands[1].mem.base,
                              insn.operands[1].mem.disp,
                              insn.operands[1].size};
        }

        // --- System register read ---
        // MRS: Move from system register (e.g. mrs x0, tpidr_el0)
        if (insn.mnemonic == "mrs" &&
            !insn.operands.empty() &&
            insn.operands[0].type == Capstone::OpType::REG) {
            return OpaqueSource{OpaqueSource::Kind::SystemReg, 0, 0};
        }

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
