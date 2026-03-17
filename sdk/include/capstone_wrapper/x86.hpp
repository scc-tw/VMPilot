#ifndef __CAPSTONE_WRAPPER_X86_HPP__
#define __CAPSTONE_WRAPPER_X86_HPP__
#pragma once

#include <capstone.hpp>

#include <cstdint>

namespace Capstone::X86 {

// Note: OpType, MemOp, and Operand are defined in <capstone.hpp>.
// This header provides architecture-specific helpers for instruction
// analysis without exposing raw capstone constants.

// --- Register classification ---

/// True if reg is a first-argument register in x86-64 calling conventions.
/// Checks both SysV (RDI/EDI) and MSVC (RCX/ECX).
bool isFirstArgReg(unsigned reg);

/// True if reg is callee-saved in both SysV and MSVC x86-64 ABIs,
/// meaning its value is preserved across function calls.
bool isCalleeSaved(unsigned reg);

// --- Instruction pattern matching ---

/// True if this is a LEA with a RIP-relative memory operand.
/// Pattern: lea <reg>, [rip + disp]
bool isRipRelativeLEA(const Instruction& insn);

/// Compute the effective VA from a RIP-relative LEA instruction.
/// Precondition: isRipRelativeLEA(insn) is true.
uint64_t computeRipRelativeVA(const Instruction& insn);

/// True if this is a MOV that loads an immediate value into a register.
/// Pattern: mov <reg>, <imm>  (includes movabs for 64-bit immediates)
bool isImmediateLoad(const Instruction& insn);

/// Get the immediate value from a MOV-immediate instruction.
/// Precondition: isImmediateLoad(insn) is true.
uint64_t getImmediateValue(const Instruction& insn);

/// True if this is a register-to-register MOV.
/// Pattern: mov <dst_reg>, <src_reg>
bool isRegToRegMov(const Instruction& insn);

/// Get the source register from a register-to-register MOV.
/// Precondition: isRegToRegMov(insn) is true.
unsigned getMovSource(const Instruction& insn);

/// True if this is a PUSH with an immediate operand.
/// Pattern: push <imm32>
bool isImmediatePush(const Instruction& insn);

/// True if this is a MOV that stores an immediate to a stack location.
/// Pattern: mov [esp/rsp + offset], <imm>
bool isImmediateStoreToStack(const Instruction& insn);

/// True if this is a PUSH of a register value.
/// Pattern: push <reg>
bool isRegPush(const Instruction& insn);

/// True if this is a MOV that stores a register to a stack location.
/// Pattern: mov [esp/rsp + offset], <reg>
bool isRegStoreToStack(const Instruction& insn);

/// True if this is a LEA with a non-RIP-relative memory operand.
/// Pattern: lea <reg>, [base + index*scale + disp]  (base != RIP)
bool isNonRipLEA(const Instruction& insn);

/// True if this is an ADD with a register and immediate operand.
/// Pattern: add <reg>, <imm>
bool isImmediateADD(const Instruction& insn);

/// True if this is a SUB with a register and immediate operand.
/// Pattern: sub <reg>, <imm>
bool isImmediateSUB(const Instruction& insn);

// --- Operand analysis ---

/// True if the instruction does NOT modify its first (destination) operand.
/// Instructions like PUSH, CMP, TEST read but don't write their operand.
bool isReadOnlyOp(const Instruction& insn);

/// True if the instruction writes to the given register via its
/// first (destination) operand.
bool writesToReg(const Instruction& insn, unsigned reg);

/// True if `name` is a PIC thunk (e.g., __x86.get_pc_thunk.bx) that
/// targets the given register.  Used to resolve x86-32 GOT-relative
/// addressing through `call thunk; add reg, imm` patterns.
bool isPcThunkForReg(const std::string& name, unsigned reg);

}  // namespace Capstone::X86

#endif  // __CAPSTONE_WRAPPER_X86_HPP__
