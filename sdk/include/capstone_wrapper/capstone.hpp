#ifndef __CAPSTONE_WRAPPER_HPP__
#define __CAPSTONE_WRAPPER_HPP__
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Capstone {

enum class Arch : uint8_t {
    ARM,
    ARM64,
    MIPS,
    X86,
    PPC,
    SPARC,
    SYSZ,
    XCORE,
    M68K,
    TMS320C64X,
    M680X,
    EVM,
    MOS65XX,
    WASM,
    BPF,
    RISCV,
    SH,
    TRICORE,
    MAX,
    ALL,
};

enum class Mode : uint32_t {
    MODE_LITTLE_ENDIAN = 0,
    MODE_ARM = 0,
    MODE_16 = 1 << 1,
    MODE_32 = 1 << 2,
    MODE_64 = 1 << 3,
    MODE_THUMB = 1 << 4,
    MODE_MCLASS = 1 << 5,
    MODE_V8 = 1 << 6,
    MODE_BIG_ENDIAN = 1U << 31,
};

enum class OpType : uint8_t {
    INVALID = 0,
    REG,
    IMM,
    MEM,
};

struct MemOp {
    unsigned int segment = 0;
    unsigned int base = 0;
    unsigned int index = 0;
    int scale = 0;
    int64_t disp = 0;

    /// Check if this is a RIP-relative memory operand
    bool isRipRelative() const noexcept;
};

struct Operand {
    OpType type = OpType::INVALID;
    unsigned int reg = 0;
    int64_t imm = 0;
    MemOp mem = {};
    uint8_t size = 0;
};

class Instruction {
   public:
    uint32_t id = 0;
    uint64_t address = 0;
    uint16_t size = 0;
    std::vector<uint8_t> bytes;
    std::string mnemonic;
    std::string op_str;

    // Architecture-neutral operand details
    std::vector<Operand> operands;

    // Instruction group membership
    std::vector<uint8_t> groups;

    /// Check if this instruction belongs to the CALL group
    bool isCall() const noexcept;

    /// Check if this instruction belongs to the JUMP group
    bool isJump() const noexcept;

    /// Check if this instruction belongs to the RET group
    bool isRet() const noexcept;

    /**
     * @brief For a direct call/jmp, return the immediate target address.
     * @return The target address, or 0 if not a direct call/jmp.
     */
    uint64_t getDirectTarget() const noexcept;

    /**
     * @brief For a RIP-relative indirect call/jmp (e.g. call [rip+disp]),
     *        compute the effective address being referenced.
     *        Effective addr = instruction address + instruction size + disp
     * @return The effective address, or 0 if not RIP-relative.
     */
    uint64_t getRipRelativeTarget() const noexcept;
};

class Capstone {
   public:
    Capstone(Arch arch, Mode mode);
    ~Capstone();

    Capstone(const Capstone&) = delete;
    Capstone& operator=(const Capstone&) = delete;
    Capstone(Capstone&& other) noexcept;
    Capstone& operator=(Capstone&& other) noexcept;

    /**
     * @brief Disassemble machine code.
     *
     * @param code Raw bytes to disassemble.
     * @param base_addr Virtual address of the first byte in code.
     * @return Disassembled instructions with full operand detail.
     */
    std::vector<Instruction> disasm(const std::vector<uint8_t>& code,
                                    uint64_t base_addr = 0) const;

   private:
    size_t handle_ = 0;
    Arch arch_ = Arch::X86;
};

}  // namespace Capstone

#endif  // __CAPSTONE_WRAPPER_HPP__
