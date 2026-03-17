#include <x86.hpp>

#include <capstone/x86.h>

namespace Capstone::X86 {

// --- Register classification ---

bool isFirstArgReg(unsigned reg) {
    return reg == X86_REG_RDI || reg == X86_REG_EDI ||
           reg == X86_REG_RCX || reg == X86_REG_ECX;
}

bool isCalleeSaved(unsigned reg) {
    switch (reg) {
        case X86_REG_RBX:
        case X86_REG_EBX:
        case X86_REG_RBP:
        case X86_REG_EBP:
        case X86_REG_RSP:
        case X86_REG_ESP:
        // ESI/EDI are callee-saved in x86-32 System V ABI.
        // In x86-64, RSI/RDI are caller-saved argument registers,
        // but the resolver traces RDI (not EDI) in 64-bit code,
        // so adding ESI/EDI here is safe for both modes.
        case X86_REG_ESI:
        case X86_REG_EDI:
        case X86_REG_R12:
        case X86_REG_R12D:
        case X86_REG_R13:
        case X86_REG_R13D:
        case X86_REG_R14:
        case X86_REG_R14D:
        case X86_REG_R15:
        case X86_REG_R15D:
            return true;
        default:
            return false;
    }
}

// --- Instruction pattern matching ---

bool isRipRelativeLEA(const Instruction& insn) {
    return insn.id == X86_INS_LEA && insn.operands.size() == 2 &&
           insn.operands[0].type == OpType::REG &&
           insn.operands[1].type == OpType::MEM &&
           insn.operands[1].mem.isRipRelative();
}

uint64_t computeRipRelativeVA(const Instruction& insn) {
    return static_cast<uint64_t>(
        static_cast<int64_t>(insn.address + insn.size) +
        insn.operands[1].mem.disp);
}

bool isImmediateLoad(const Instruction& insn) {
    if (insn.operands.size() != 2 ||
        insn.operands[0].type != OpType::REG ||
        insn.operands[1].type != OpType::IMM)
        return false;
    return insn.id == X86_INS_MOV || insn.id == X86_INS_MOVABS;
}

uint64_t getImmediateValue(const Instruction& insn) {
    return static_cast<uint64_t>(insn.operands[1].imm);
}

bool isRegToRegMov(const Instruction& insn) {
    return insn.id == X86_INS_MOV && insn.operands.size() == 2 &&
           insn.operands[0].type == OpType::REG &&
           insn.operands[1].type == OpType::REG;
}

unsigned getMovSource(const Instruction& insn) {
    return insn.operands[1].reg;
}

bool isImmediatePush(const Instruction& insn) {
    return insn.id == X86_INS_PUSH && insn.operands.size() >= 1 &&
           insn.operands[0].type == OpType::IMM;
}

bool isImmediateStoreToStack(const Instruction& insn) {
    if (insn.id != X86_INS_MOV || insn.operands.size() != 2)
        return false;
    const auto& dst = insn.operands[0];
    const auto& src = insn.operands[1];
    return dst.type == OpType::MEM &&
           (dst.mem.base == X86_REG_ESP || dst.mem.base == X86_REG_RSP) &&
           dst.mem.index == 0 && src.type == OpType::IMM;
}

bool isRegPush(const Instruction& insn) {
    return insn.id == X86_INS_PUSH && insn.operands.size() >= 1 &&
           insn.operands[0].type == OpType::REG;
}

bool isRegStoreToStack(const Instruction& insn) {
    if (insn.id != X86_INS_MOV || insn.operands.size() != 2)
        return false;
    const auto& dst = insn.operands[0];
    const auto& src = insn.operands[1];
    return dst.type == OpType::MEM &&
           (dst.mem.base == X86_REG_ESP || dst.mem.base == X86_REG_RSP) &&
           dst.mem.index == 0 && src.type == OpType::REG;
}

bool isNonRipLEA(const Instruction& insn) {
    return insn.id == X86_INS_LEA && insn.operands.size() == 2 &&
           insn.operands[0].type == OpType::REG &&
           insn.operands[1].type == OpType::MEM &&
           !insn.operands[1].mem.isRipRelative();
}

bool isImmediateADD(const Instruction& insn) {
    return insn.id == X86_INS_ADD && insn.operands.size() == 2 &&
           insn.operands[0].type == OpType::REG &&
           insn.operands[1].type == OpType::IMM;
}

bool isImmediateSUB(const Instruction& insn) {
    return insn.id == X86_INS_SUB && insn.operands.size() == 2 &&
           insn.operands[0].type == OpType::REG &&
           insn.operands[1].type == OpType::IMM;
}

// --- Operand analysis ---

bool isReadOnlyOp(const Instruction& insn) {
    switch (insn.id) {
        case X86_INS_PUSH:
        case X86_INS_CMP:
        case X86_INS_TEST:
            return true;
        default:
            return false;
    }
}

bool writesToReg(const Instruction& insn, unsigned reg) {
    if (isReadOnlyOp(insn) || insn.operands.empty())
        return false;
    const auto& dst = insn.operands[0];
    return dst.type == OpType::REG && dst.reg == reg;
}

bool isPcThunkForReg(const std::string& name, unsigned reg) {
    // Pattern: __x86.get_pc_thunk.XX  (GCC PIC thunks)
    auto pos = name.rfind(".get_pc_thunk.");
    if (pos == std::string::npos)
        return false;
    // Extract the register suffix after the last '.'
    auto dot = name.rfind('.');
    if (dot == std::string::npos || dot + 1 >= name.size())
        return false;
    auto suffix = name.substr(dot + 1);

    // Map suffix to capstone register ID
    unsigned target = 0;
    if (suffix == "ax")
        target = X86_REG_EAX;
    else if (suffix == "bx")
        target = X86_REG_EBX;
    else if (suffix == "cx")
        target = X86_REG_ECX;
    else if (suffix == "dx")
        target = X86_REG_EDX;
    else if (suffix == "si")
        target = X86_REG_ESI;
    else if (suffix == "di")
        target = X86_REG_EDI;
    else if (suffix == "bp")
        target = X86_REG_EBP;

    return target != 0 && target == reg;
}

}  // namespace Capstone::X86
