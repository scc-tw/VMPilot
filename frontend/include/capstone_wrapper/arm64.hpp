#ifndef __CAPSTONE_WRAPPER_ARM64_HPP__
#define __CAPSTONE_WRAPPER_ARM64_HPP__
#pragma once

#include <capstone.hpp>

#include <cstdint>

namespace Capstone::ARM64 {

// --- Register classification ---

/// Returns the register ID for the first argument register (X0 in AAPCS64).
unsigned firstArgReg();

/// True if reg is the first argument register (X0 or W0) in AAPCS64.
bool isFirstArgReg(unsigned reg);

/// True if reg is callee-saved in AAPCS64 (X19-X28, FP/X29, LR/X30).
bool isCalleeSaved(unsigned reg);

// --- Instruction pattern matching ---

/// True if this is an ADRP instruction (address of 4KB page).
/// Pattern: adrp <reg>, <page_addr>
bool isADRP(const Instruction& insn);

/// True if this is an ADR instruction (single-instruction PC-relative).
/// Pattern: adr <reg>, <target_addr>
bool isADR(const Instruction& insn);

/// True if this is an ADD with a register source and immediate offset.
/// Pattern: add <dst_reg>, <src_reg>, #<imm>
bool isRegPlusImmADD(const Instruction& insn);

/// True if this is a SUB with a register source and immediate offset.
/// Pattern: sub <dst_reg>, <src_reg>, #<imm>
bool isRegMinusImmSUB(const Instruction& insn);

/// True if this is a register-to-register MOV.
/// Handles both the MOV mnemonic and its ORR alias encoding.
/// Pattern: mov <dst_reg>, <src_reg>
bool isRegToRegMov(const Instruction& insn);

/// Get the source register from a register-to-register MOV.
/// Handles both MOV and ORR alias encodings.
/// Precondition: isRegToRegMov(insn) is true.
unsigned getMovSource(const Instruction& insn);

// --- Operand analysis ---

/// True if the instruction writes to the given register via its
/// first (destination) operand.
bool writesToReg(const Instruction& insn, unsigned reg);

}  // namespace Capstone::ARM64

#endif  // __CAPSTONE_WRAPPER_ARM64_HPP__
