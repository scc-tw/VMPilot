#include <arm64.hpp>

#include <capstone/arm64.h>

namespace Capstone::ARM64 {

// --- Register classification ---

unsigned firstArgReg() { return ARM64_REG_X0; }

bool isFirstArgReg(unsigned reg) {
    return reg == ARM64_REG_X0 || reg == ARM64_REG_W0;
}

bool isCalleeSaved(unsigned reg) {
    return (reg >= ARM64_REG_X19 && reg <= ARM64_REG_X28) ||
           reg == ARM64_REG_X29 || reg == ARM64_REG_FP ||
           reg == ARM64_REG_X30 || reg == ARM64_REG_LR;
}

// --- Instruction pattern matching ---

bool isADRP(const Instruction& insn) {
    return insn.id == ARM64_INS_ADRP && insn.operands.size() >= 2 &&
           insn.operands[0].type == OpType::REG &&
           insn.operands[1].type == OpType::IMM;
}

bool isADR(const Instruction& insn) {
    return insn.id == ARM64_INS_ADR && insn.operands.size() >= 2 &&
           insn.operands[0].type == OpType::REG &&
           insn.operands[1].type == OpType::IMM;
}

bool isRegPlusImmADD(const Instruction& insn) {
    return insn.id == ARM64_INS_ADD && insn.operands.size() >= 3 &&
           insn.operands[0].type == OpType::REG &&
           insn.operands[1].type == OpType::REG &&
           insn.operands[2].type == OpType::IMM;
}

bool isRegToRegMov(const Instruction& insn) {
    // MOV xd, xm — direct MOV instruction
    if (insn.id == ARM64_INS_MOV && insn.operands.size() >= 2 &&
        insn.operands[0].type == OpType::REG &&
        insn.operands[1].type == OpType::REG) {
        return true;
    }
    // ORR xd, xzr, xm — MOV alias encoding
    if (insn.id == ARM64_INS_ORR && insn.operands.size() >= 3 &&
        insn.operands[0].type == OpType::REG &&
        insn.operands[1].type == OpType::REG &&
        insn.operands[2].type == OpType::REG &&
        insn.operands[1].reg == ARM64_REG_XZR) {
        return true;
    }
    return false;
}

unsigned getMovSource(const Instruction& insn) {
    // ORR xd, xzr, xm — source is operands[2]
    if (insn.id == ARM64_INS_ORR && insn.operands.size() >= 3)
        return insn.operands[2].reg;
    // MOV xd, xm — source is operands[1]
    return insn.operands[1].reg;
}

bool isRegMinusImmSUB(const Instruction& insn) {
    return insn.id == ARM64_INS_SUB && insn.operands.size() >= 3 &&
           insn.operands[0].type == OpType::REG &&
           insn.operands[1].type == OpType::REG &&
           insn.operands[2].type == OpType::IMM;
}

// --- Operand analysis ---

bool writesToReg(const Instruction& insn, unsigned reg) {
    if (insn.operands.empty())
        return false;
    const auto& dst = insn.operands[0];
    return dst.type == OpType::REG && dst.reg == reg;
}

}  // namespace Capstone::ARM64
