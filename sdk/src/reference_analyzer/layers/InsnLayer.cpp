#include "InsnLayer.hpp"
#include "AtomicTraits.hpp"

#include <capstone/arm64.h>
#include <capstone/x86.h>

namespace VMPilot::SDK::ReferenceAnalyzer {

// --- X86InsnTraits ---

bool X86InsnTraits::isStackRelative(unsigned base_reg) noexcept {
    return base_reg == X86_REG_RSP || base_reg == X86_REG_RBP ||
           base_reg == X86_REG_ESP || base_reg == X86_REG_EBP ||
           base_reg == X86_REG_SP  || base_reg == X86_REG_BP;
}

bool X86InsnTraits::isAddressComputation(
    const Capstone::Instruction& insn) noexcept {
    return insn.mnemonic == "lea";
}

bool X86InsnTraits::isFenceInstruction(
    const Capstone::Instruction& insn) noexcept {
    return insn.id == X86_INS_MFENCE || insn.id == X86_INS_SFENCE ||
           insn.id == X86_INS_LFENCE;
}

AtomicInfo X86InsnTraits::classifyAtomic(
    const Capstone::Instruction& insn, bool has_mem_operand) noexcept {
    return X86AtomicTraits::classify(insn, has_mem_operand);
}

bool X86InsnTraits::isMemWrite(const Capstone::Instruction& insn,
                                size_t op_idx) noexcept {
    return X86AtomicTraits::isMemWrite(insn, op_idx);
}

// --- ARM64InsnTraits ---

bool ARM64InsnTraits::isStackRelative(unsigned base_reg) noexcept {
    return base_reg == ARM64_REG_SP || base_reg == ARM64_REG_FP;
}

bool ARM64InsnTraits::isAddressComputation(
    const Capstone::Instruction& /*insn*/) noexcept {
    // ARM64 doesn't have LEA equivalent for address computation
    return false;
}

bool ARM64InsnTraits::isFenceInstruction(
    const Capstone::Instruction& insn) noexcept {
    return insn.mnemonic == "dmb" || insn.mnemonic == "dsb" ||
           insn.mnemonic == "isb";
}

AtomicInfo ARM64InsnTraits::classifyAtomic(
    const Capstone::Instruction& insn) noexcept {
    return ARM64AtomicTraits::classify(insn);
}

bool ARM64InsnTraits::isMemWrite(const Capstone::Instruction& insn,
                                  const AtomicInfo& atomic) noexcept {
    return ARM64AtomicTraits::isMemWrite(insn, atomic);
}

}  // namespace VMPilot::SDK::ReferenceAnalyzer
