#pragma once
#ifndef __COMMON_VM_INSN_HPP__
#define __COMMON_VM_INSN_HPP__

#include <cstdint>

namespace VMPilot::Common::VM {

/// Operand type constants (2-bit field).
constexpr uint8_t VM_OPERAND_NONE = 0;
constexpr uint8_t VM_OPERAND_REG  = 1;
constexpr uint8_t VM_OPERAND_POOL = 2;
constexpr uint8_t VM_OPERAND_MEM  = 3;

/// 8-byte fixed instruction format (ISA Design v1, Section 1).
///
/// Layout:
///   Byte 0-1: opcode   (16-bit)
///   Byte 2:   flags    (8-bit) -- operand_a type [7:6], operand_b type [5:4],
///                                  condition/width/variant [3:0]
///   Byte 3:   reg_pack (reg_a:4 | reg_b:4)
///   Byte 4-7: aux      (32-bit)
#pragma pack(push, 1)
struct VmInsn {
    uint16_t opcode;
    uint8_t  flags;
    uint8_t  reg_pack;
    uint32_t aux;

    /// Extract operand_a type from flags [7:6].
    [[nodiscard]] constexpr uint8_t operand_a_type() const noexcept {
        return (flags >> 6) & 0x03;
    }

    /// Extract operand_b type from flags [5:4].
    [[nodiscard]] constexpr uint8_t operand_b_type() const noexcept {
        return (flags >> 4) & 0x03;
    }

    /// Extract condition/width/variant from flags [3:0].
    [[nodiscard]] constexpr uint8_t condition() const noexcept {
        return flags & 0x0F;
    }

    /// Extract reg_a from reg_pack [7:4].
    [[nodiscard]] constexpr uint8_t reg_a() const noexcept {
        return (reg_pack >> 4) & 0x0F;
    }

    /// Extract reg_b from reg_pack [3:0].
    [[nodiscard]] constexpr uint8_t reg_b() const noexcept {
        return reg_pack & 0x0F;
    }
};
#pragma pack(pop)

static_assert(sizeof(VmInsn) == 8, "VmInsn must be exactly 8 bytes");

}  // namespace VMPilot::Common::VM

#endif  // __COMMON_VM_INSN_HPP__
