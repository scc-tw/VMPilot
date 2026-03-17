#include <capstone.hpp>

#include <capstone/arm64.h>
#include <capstone/capstone.h>
#include <capstone/x86.h>

#include <algorithm>
#include <stdexcept>

namespace Capstone {

// --- MemOp ---

bool MemOp::isRipRelative() const noexcept {
    return base == X86_REG_RIP;
}

// --- Instruction helpers ---

static bool hasGroup(const std::vector<uint8_t>& groups, uint8_t group) {
    return std::find(groups.begin(), groups.end(), group) != groups.end();
}

bool Instruction::isCall() const noexcept {
    return hasGroup(groups, CS_GRP_CALL);
}

bool Instruction::isJump() const noexcept {
    return hasGroup(groups, CS_GRP_JUMP);
}

bool Instruction::isRet() const noexcept {
    return hasGroup(groups, CS_GRP_RET);
}

uint64_t Instruction::getDirectTarget() const noexcept {
    if (operands.size() == 1 && operands[0].type == OpType::IMM) {
        return static_cast<uint64_t>(operands[0].imm);
    }
    return 0;
}

uint64_t Instruction::getRipRelativeTarget() const noexcept {
    if (operands.size() == 1 && operands[0].type == OpType::MEM &&
        operands[0].mem.isRipRelative()) {
        // RIP-relative: effective addr = next instruction addr + displacement
        // next instruction addr = this instruction's address + size
        return static_cast<uint64_t>(static_cast<int64_t>(address + size) +
                                     operands[0].mem.disp);
    }
    return 0;
}

// --- Arch/Mode mapping to capstone C enums ---

// Capstone 6.x renamed CS_ARCH_ARM64 → CS_ARCH_AARCH64
//                  and CS_ARCH_SYSZ  → CS_ARCH_SYSTEMZ
#ifndef CS_ARCH_ARM64
#define CS_ARCH_ARM64 CS_ARCH_AARCH64
#endif
#ifndef CS_ARCH_AARCH64
#define CS_ARCH_AARCH64 CS_ARCH_ARM64
#endif
#ifndef CS_ARCH_SYSZ
#define CS_ARCH_SYSZ CS_ARCH_SYSTEMZ
#endif
#ifndef CS_ARCH_SYSTEMZ
#define CS_ARCH_SYSTEMZ CS_ARCH_SYSZ
#endif

static cs_arch toCSArch(Arch arch) {
    switch (arch) {
        case Arch::ARM:
            return CS_ARCH_ARM;
        case Arch::ARM64:
            return CS_ARCH_ARM64;
        case Arch::MIPS:
            return CS_ARCH_MIPS;
        case Arch::X86:
            return CS_ARCH_X86;
        case Arch::PPC:
            return CS_ARCH_PPC;
        case Arch::SPARC:
            return CS_ARCH_SPARC;
        case Arch::SYSZ:
            return CS_ARCH_SYSZ;
        case Arch::XCORE:
            return CS_ARCH_XCORE;
        case Arch::M68K:
            return CS_ARCH_M68K;
        case Arch::TMS320C64X:
            return CS_ARCH_TMS320C64X;
        case Arch::M680X:
            return CS_ARCH_M680X;
        case Arch::EVM:
            return CS_ARCH_EVM;
        case Arch::WASM:
            return CS_ARCH_WASM;
        case Arch::BPF:
            return CS_ARCH_BPF;
        case Arch::RISCV:
            return CS_ARCH_RISCV;
        case Arch::SH:
            return CS_ARCH_SH;
        case Arch::TRICORE:
            return CS_ARCH_TRICORE;
        default:
            return CS_ARCH_X86;
    }
}

// --- Capstone class ---

Capstone::Capstone(Arch arch, Mode mode) : arch_(arch) {
    auto c_arch = toCSArch(arch);
    auto c_mode = static_cast<enum cs_mode>(static_cast<uint32_t>(mode));

    cs_err err = cs_open(c_arch, c_mode, reinterpret_cast<csh*>(&handle_));
    if (err != CS_ERR_OK) {
        throw std::runtime_error(std::string("cs_open failed: ") +
                                 cs_strerror(err));
    }

    // Enable detail mode to get operand information
    cs_option(handle_, CS_OPT_DETAIL, CS_OPT_ON);
}

Capstone::~Capstone() {
    if (handle_ != 0) {
        csh h = static_cast<csh>(handle_);
        cs_close(&h);
        handle_ = 0;
    }
}

Capstone::Capstone(Capstone&& other) noexcept
    : handle_(other.handle_), arch_(other.arch_) {
    other.handle_ = 0;
}

Capstone& Capstone::operator=(Capstone&& other) noexcept {
    if (this != &other) {
        if (handle_ != 0) {
            csh h = static_cast<csh>(handle_);
            cs_close(&h);
        }
        handle_ = other.handle_;
        other.handle_ = 0;
    }
    return *this;
}

std::vector<Instruction> Capstone::disasm(const std::vector<uint8_t>& code,
                                          uint64_t base_addr) const {
    cs_insn* insn = nullptr;
    size_t count = cs_disasm(static_cast<csh>(handle_), code.data(),
                             code.size(), base_addr, 0, &insn);

    std::vector<Instruction> result;
    result.reserve(count);

    for (size_t i = 0; i < count; ++i) {
        Instruction inst;
        inst.id = insn[i].id;
        inst.address = insn[i].address;
        inst.size = insn[i].size;
        inst.bytes.assign(insn[i].bytes, insn[i].bytes + insn[i].size);
        inst.mnemonic = insn[i].mnemonic;
        inst.op_str = insn[i].op_str;

        // Extract detail if available
        if (insn[i].detail) {
            const auto& detail = *insn[i].detail;

            // Groups
            inst.groups.assign(detail.groups,
                               detail.groups + detail.groups_count);

            // Architecture-specific operands
            if (arch_ == Arch::X86) {
                const auto& x86 = detail.x86;
                std::copy(x86.prefix, x86.prefix + 4, inst.x86_prefix);
                inst.operands.reserve(x86.op_count);
                for (uint8_t j = 0; j < x86.op_count; ++j) {
                    const auto& src = x86.operands[j];
                    Operand op;
                    op.size = src.size;
                    switch (src.type) {
                        case X86_OP_REG:
                            op.type = OpType::REG;
                            op.reg = src.reg;
                            break;
                        case X86_OP_IMM:
                            op.type = OpType::IMM;
                            op.imm = src.imm;
                            break;
                        case X86_OP_MEM:
                            op.type = OpType::MEM;
                            op.mem.segment = src.mem.segment;
                            op.mem.base = src.mem.base;
                            op.mem.index = src.mem.index;
                            op.mem.scale = src.mem.scale;
                            op.mem.disp = src.mem.disp;
                            break;
                        default:
                            op.type = OpType::INVALID;
                            break;
                    }
                    inst.operands.push_back(std::move(op));
                }
            } else if (arch_ == Arch::ARM64) {
                const auto& arm64 = detail.arm64;
                inst.operands.reserve(arm64.op_count);
                for (uint8_t j = 0; j < arm64.op_count; ++j) {
                    const auto& src = arm64.operands[j];
                    Operand op;
                    op.size = 0;
                    switch (src.type) {
                        case ARM64_OP_REG:
                            op.type = OpType::REG;
                            op.reg = src.reg;
                            break;
                        case ARM64_OP_IMM:
                            op.type = OpType::IMM;
                            op.imm = src.imm;
                            break;
                        case ARM64_OP_MEM:
                            op.type = OpType::MEM;
                            op.mem.base = src.mem.base;
                            op.mem.index = src.mem.index;
                            op.mem.disp = src.mem.disp;
                            break;
                        default:
                            op.type = OpType::INVALID;
                            break;
                    }
                    inst.operands.push_back(std::move(op));
                }
            }
        }

        result.push_back(std::move(inst));
    }

    cs_free(insn, count);
    return result;
}

}  // namespace Capstone
