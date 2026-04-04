#include <gtest/gtest.h>

#include "SymExpr.hpp"
#include "SymbolicResolver.hpp"

#include <ARM64Traits.hpp>
#include <X86Traits.hpp>
#include <capstone.hpp>

#include <capstone/arm64.h>
#include <capstone/capstone.h>
#include <capstone/x86.h>

#include <string>
#include <vector>

using namespace VMPilot::SDK::ReferenceAnalyzer;
using namespace VMPilot::SDK::Segmentator;

// ---- Helpers to build synthetic instructions ----

namespace {

Capstone::Instruction makeInsn(uint32_t id, const std::string& mnemonic,
                               uint64_t addr = 0x1000,
                               uint16_t size = 4) {
    Capstone::Instruction insn;
    insn.id = id;
    insn.mnemonic = mnemonic;
    insn.address = addr;
    insn.size = size;
    return insn;
}

Capstone::Operand regOp(unsigned reg, uint8_t sz = 8) {
    Capstone::Operand op;
    op.type = Capstone::OpType::REG;
    op.reg = reg;
    op.size = sz;
    return op;
}

Capstone::Operand memOp(unsigned base, int64_t disp, uint8_t sz = 8,
                         unsigned segment = 0, unsigned index = 0) {
    Capstone::Operand op;
    op.type = Capstone::OpType::MEM;
    op.mem.base = base;
    op.mem.index = index;
    op.mem.disp = disp;
    op.mem.segment = segment;
    op.size = sz;
    return op;
}

Capstone::Operand immOp(int64_t val, uint8_t sz = 8) {
    Capstone::Operand op;
    op.type = Capstone::OpType::IMM;
    op.imm = val;
    op.size = sz;
    return op;
}

/// Make a CALL instruction (add CS_GRP_CALL to groups).
Capstone::Instruction makeCall(uint64_t target, uint64_t addr = 0x1000) {
    auto insn = makeInsn(X86_INS_CALL, "call", addr, 5);
    insn.groups.push_back(CS_GRP_CALL);
    insn.operands.push_back(immOp(static_cast<int64_t>(target)));
    return insn;
}

}  // namespace

// ---- Tests ----

// ARM64: adrp x0, #page; add x0, x0, #off -> Constant(page + off)
TEST(SymbolicResolver, AdrpAddSequence) {
    const uint64_t page = 0x410000;
    const int64_t off = 0x120;

    std::vector<Capstone::Instruction> insns;

    // adrp x0, #page
    {
        auto insn = makeInsn(ARM64_INS_ADRP, "adrp", 0x1000, 4);
        insn.operands.push_back(regOp(ARM64_REG_X0));
        insn.operands.push_back(immOp(static_cast<int64_t>(page)));
        insns.push_back(std::move(insn));
    }
    // add x0, x0, #off
    {
        auto insn = makeInsn(ARM64_INS_ADD, "add", 0x1004, 4);
        insn.operands.push_back(regOp(ARM64_REG_X0));
        insn.operands.push_back(regOp(ARM64_REG_X0));
        insn.operands.push_back(immOp(off));
        insns.push_back(std::move(insn));
    }

    auto expr = resolveSymbolic<ARM64ArchTraits>(
        ARM64_REG_X0, 1, insns, 0);

    // Should constant-fold to page + off
    ASSERT_TRUE(expr.isConstant());
    EXPECT_EQ(*expr.tryEval(), page + static_cast<uint64_t>(off));
}

// X86: mov rax, [rip+0x1234] at addr 0x1000, size 7
// This is a RIP-relative LEA check (but actually it's a MOV from memory).
// Since classifyWrite for x86 doesn't classify memory loads (returns
// Unresolvable), the resolver should return Unknown.
// However, if it's a RIP-relative LEA: lea rax, [rip+0x1234], that returns
// ResolvedConstant with the computed VA.
TEST(SymbolicResolver, X86RipRelativeLEA) {
    std::vector<Capstone::Instruction> insns;

    // lea rax, [rip+0x1234] at addr 0x1000, size 7
    {
        auto insn = makeInsn(X86_INS_LEA, "lea", 0x1000, 7);
        insn.operands.push_back(regOp(X86_REG_RAX));
        insn.operands.push_back(memOp(X86_REG_RIP, 0x1234));
        insns.push_back(std::move(insn));
    }

    auto expr = resolveSymbolic<X86ArchTraits>(
        X86_REG_RAX, 0, insns, 0);

    // computeRipRelativeVA = addr + size + disp = 0x1000 + 7 + 0x1234 = 0x223B
    ASSERT_TRUE(expr.isConstant());
    EXPECT_EQ(*expr.tryEval(), 0x223Bu);
}

// X86: mov rax, [rip+0x1234] -- a RIP-relative memory load.
// Phase 1 extended X86ArchTraits to classify this as MemoryLoad.
// The resolver produces Load(Const(computed_address)).
TEST(SymbolicResolver, X86RipRelativeMemoryLoad) {
    std::vector<Capstone::Instruction> insns;

    // mov rax, [rip+0x1234] at addr 0x1000, size 7
    // effective addr = 0x1000 + 7 + 0x1234 = 0x223B
    {
        auto insn = makeInsn(X86_INS_MOV, "mov", 0x1000, 7);
        insn.operands.push_back(regOp(X86_REG_RAX));
        insn.operands.push_back(memOp(X86_REG_RIP, 0x1234));
        insns.push_back(std::move(insn));
    }

    auto expr = resolveSymbolic<X86ArchTraits>(
        X86_REG_RAX, 0, insns, 0);

    // MemoryLoad with base_reg=0, offset=0x223B -> Load(Add(Const(0), Const(0x223B)))
    // Constant folding: Add(0, 0x223B) = Const(0x223B)
    // Result: Load(Const(0x223B))
    ASSERT_TRUE(expr.involvesLoad());
    auto* load_addr = expr.findLoadAddr();
    ASSERT_NE(load_addr, nullptr);
    ASSERT_TRUE(load_addr->isConstant());
    EXPECT_EQ(*load_addr->tryEval(), 0x223Bu);
}

// ARM64: mrs x0, tpidr_el0 -- Phase 1 extended ARM64ArchTraits to
// classify this as OpaqueSource{SystemReg}.
TEST(SymbolicResolver, ARM64MrsSystemReg) {
    std::vector<Capstone::Instruction> insns;

    {
        auto insn = makeInsn(ARM64_INS_MRS, "mrs", 0x1000, 4);
        insn.operands.push_back(regOp(ARM64_REG_X0));
        insn.operands.push_back(immOp(0xDE82));  // TPIDR_EL0 encoding
        insns.push_back(std::move(insn));
    }

    auto expr = resolveSymbolic<ARM64ArchTraits>(
        ARM64_REG_X0, 0, insns, 0);

    // OpaqueSource{SystemReg} → Opaque(SystemReg)
    ASSERT_EQ(expr.kind, SymExpr::Kind::Opaque);
    EXPECT_EQ(expr.opaque_kind, SymExpr::OpaqueKind::SystemReg);
}

// Depth limit: chain of register forwards exceeding kMaxSymResolveDepth
TEST(SymbolicResolver, DepthLimit) {
    // Create a chain: mov x1, x0; mov x2, x1; mov x3, x2; ... (10 moves)
    // Each MOV creates a RegisterForward, recursing deeper.
    // kMaxSymResolveDepth = 8, so depth >= 8 returns Unknown.
    std::vector<Capstone::Instruction> insns;

    const int chain_len = 10;
    for (int j = 0; j < chain_len; ++j) {
        unsigned dst = ARM64_REG_X0 + static_cast<unsigned>(j + 1);
        unsigned src = ARM64_REG_X0 + static_cast<unsigned>(j);
        auto insn = makeInsn(ARM64_INS_MOV, "mov",
                             0x1000 + static_cast<uint64_t>(j) * 4, 4);
        insn.operands.push_back(regOp(dst));
        insn.operands.push_back(regOp(src));
        insns.push_back(std::move(insn));
    }

    // Resolve the last register (x10) at the last instruction
    unsigned last_reg = ARM64_REG_X0 + chain_len;
    auto expr = resolveSymbolic<ARM64ArchTraits>(
        last_reg, insns.size() - 1, insns, 0);

    // Depth limit exceeded -> Unknown
    EXPECT_TRUE(expr.isUnknown());
}

// Empty instruction vector
TEST(SymbolicResolver, EmptyInsns) {
    std::vector<Capstone::Instruction> insns;
    auto expr = resolveSymbolic<X86ArchTraits>(
        X86_REG_RAX, 0, insns, 0);
    EXPECT_TRUE(expr.isUnknown());
}

// Register forward chain resolving to a constant:
// mov rax, 0x1000; mov rbx, rax; lea rcx, [rbx+8]
// Resolving rcx -> Add(Forward(rbx->Forward(rax->Const(0x1000))), 8)
// = Const(0x1008) via constant folding
TEST(SymbolicResolver, RegisterForwardChain) {
    std::vector<Capstone::Instruction> insns;

    // mov rax, 0x1000
    {
        auto insn = makeInsn(X86_INS_MOV, "mov", 0x1000, 7);
        insn.operands.push_back(regOp(X86_REG_RAX));
        insn.operands.push_back(immOp(0x1000));
        insns.push_back(std::move(insn));
    }
    // mov rbx, rax
    {
        auto insn = makeInsn(X86_INS_MOV, "mov", 0x1007, 3);
        insn.operands.push_back(regOp(X86_REG_RBX));
        insn.operands.push_back(regOp(X86_REG_RAX));
        insns.push_back(std::move(insn));
    }
    // lea rcx, [rbx+8]
    {
        auto insn = makeInsn(X86_INS_LEA, "lea", 0x100A, 4);
        insn.operands.push_back(regOp(X86_REG_RCX));
        insn.operands.push_back(memOp(X86_REG_RBX, 8));
        insns.push_back(std::move(insn));
    }

    auto expr = resolveSymbolic<X86ArchTraits>(
        X86_REG_RCX, 2, insns, 0);

    // Should fold to Const(0x1000 + 8 = 0x1008)
    ASSERT_TRUE(expr.isConstant());
    EXPECT_EQ(*expr.tryEval(), 0x1008u);
}

// CALL clobbers caller-saved register with sym_lookup
TEST(SymbolicResolver, CallWithSymLookup) {
    std::vector<Capstone::Instruction> insns;

    // call 0x2000
    insns.push_back(makeCall(0x2000, 0x1000));
    // (any instruction using RAX after the call)

    // RAX is caller-saved on x86-64, so the call clobbers it.
    // With a sym_lookup, we should get Opaque(CallResult, "my_func").
    SymbolLookup lookup = [](uint64_t addr) -> std::optional<std::string> {
        if (addr == 0x2000)
            return "my_func";
        return std::nullopt;
    };

    auto expr = resolveSymbolic<X86ArchTraits>(
        X86_REG_RAX, 0, insns, 0, lookup);

    EXPECT_EQ(expr.kind, SymExpr::Kind::Opaque);
    EXPECT_EQ(expr.opaque_kind, SymExpr::OpaqueKind::CallResult);
    EXPECT_EQ(expr.symbol, "my_func");
}

// CALL clobbers caller-saved register without sym_lookup -> Unknown
TEST(SymbolicResolver, CallWithoutSymLookup) {
    std::vector<Capstone::Instruction> insns;

    // call 0x2000
    insns.push_back(makeCall(0x2000, 0x1000));

    auto expr = resolveSymbolic<X86ArchTraits>(
        X86_REG_RAX, 0, insns, 0);

    EXPECT_TRUE(expr.isUnknown());
}

// CALL does not clobber callee-saved register
TEST(SymbolicResolver, CallPreservesCalleeSaved) {
    std::vector<Capstone::Instruction> insns;

    // mov rbx, 0x42  (RBX is callee-saved)
    {
        auto insn = makeInsn(X86_INS_MOV, "mov", 0x1000, 7);
        insn.operands.push_back(regOp(X86_REG_RBX));
        insn.operands.push_back(immOp(0x42));
        insns.push_back(std::move(insn));
    }
    // call 0x2000
    insns.push_back(makeCall(0x2000, 0x1007));

    // Resolving RBX at idx=1 should skip the CALL (RBX is callee-saved)
    // and find the MOV -> Const(0x42)
    auto expr = resolveSymbolic<X86ArchTraits>(
        X86_REG_RBX, 1, insns, 0);

    ASSERT_TRUE(expr.isConstant());
    EXPECT_EQ(*expr.tryEval(), 0x42u);
}

// Arithmetic adjustment with SUB
TEST(SymbolicResolver, ArithmeticAdjustSub) {
    std::vector<Capstone::Instruction> insns;

    // mov rax, 0x2000
    {
        auto insn = makeInsn(X86_INS_MOV, "mov", 0x1000, 7);
        insn.operands.push_back(regOp(X86_REG_RAX));
        insn.operands.push_back(immOp(0x2000));
        insns.push_back(std::move(insn));
    }
    // sub rax, 0x100
    {
        auto insn = makeInsn(X86_INS_SUB, "sub", 0x1007, 6);
        insn.operands.push_back(regOp(X86_REG_RAX));
        insn.operands.push_back(immOp(0x100));
        insns.push_back(std::move(insn));
    }

    auto expr = resolveSymbolic<X86ArchTraits>(
        X86_REG_RAX, 1, insns, 0);

    // SUB gives ArithmeticAdjust{RAX, -0x100}, then resolves RAX to 0x2000.
    // Result: Add(Const(0x2000), Const(-0x100)) = Const(0x1F00)
    ASSERT_TRUE(expr.isConstant());
    EXPECT_EQ(*expr.tryEval(), 0x1F00u);
}

// ---------------------------------------------------------------------------
// Integration tests: Phase 1 extended Traits → SymbolicResolver
// ---------------------------------------------------------------------------

// x86: mov rax, fs:[0x28] → Opaque(SegmentBase, FS)
TEST(SymbolicResolver, X86SegmentAccess) {
    std::vector<Capstone::Instruction> insns;

    {
        auto insn = makeInsn(X86_INS_MOV, "mov", 0x1000, 7);
        insn.operands.push_back(regOp(X86_REG_RAX));
        insn.operands.push_back(memOp(0, 0x28, 8, X86_REG_FS));
        insns.push_back(std::move(insn));
    }

    auto expr = resolveSymbolic<X86ArchTraits>(
        X86_REG_RAX, 0, insns, 0);

    ASSERT_EQ(expr.kind, SymExpr::Kind::Opaque);
    EXPECT_EQ(expr.opaque_kind, SymExpr::OpaqueKind::SegmentBase);
    EXPECT_EQ(expr.reg_id, static_cast<unsigned>(X86_REG_FS));
}

// x86: lea rbx, [rip+TABLE]; mov rax, [rbx+8]
// → Load(Const(TABLE + 8))
TEST(SymbolicResolver, X86ChainThroughLoad) {
    const uint64_t table = 0x402000;  // TABLE address
    std::vector<Capstone::Instruction> insns;

    // lea rbx, [rip+TABLE]  at addr 0x1000, size 7
    // computeRipRelativeVA = 0x1000 + 7 + disp
    // disp = TABLE - (0x1000 + 7) = 0x402000 - 0x1007 = 0x400FF9
    {
        int64_t disp = static_cast<int64_t>(table) - (0x1000 + 7);
        auto insn = makeInsn(X86_INS_LEA, "lea", 0x1000, 7);
        insn.operands.push_back(regOp(X86_REG_RBX));
        insn.operands.push_back(memOp(X86_REG_RIP, disp));
        insns.push_back(std::move(insn));
    }
    // mov rax, [rbx+8]
    {
        auto insn = makeInsn(X86_INS_MOV, "mov", 0x1007, 4);
        insn.operands.push_back(regOp(X86_REG_RAX));
        insn.operands.push_back(memOp(X86_REG_RBX, 8));
        insns.push_back(std::move(insn));
    }

    auto expr = resolveSymbolic<X86ArchTraits>(
        X86_REG_RAX, 1, insns, 0);

    // mov classifies as MemoryLoad{RBX, 8, 8}
    // → Load(Add(resolve(RBX), Const(8)))
    // resolve(RBX) = lea [rip+disp] → Const(TABLE)
    // → Load(Add(Const(TABLE), Const(8))) → Load(Const(TABLE+8))
    ASSERT_TRUE(expr.involvesLoad());
    auto* addr = expr.findLoadAddr();
    ASSERT_NE(addr, nullptr);
    ASSERT_TRUE(addr->isConstant());
    EXPECT_EQ(*addr->tryEval(), table + 8);
}

// ARM64: adrp x1, #page; add x1, x1, #off; ldr x0, [x1, #16]
// → Load(Const(page + off + 16))
TEST(SymbolicResolver, ARM64LdrChain) {
    const uint64_t page = 0x410000;
    const int64_t off = 0x200;
    std::vector<Capstone::Instruction> insns;

    // adrp x1, #page
    {
        auto insn = makeInsn(ARM64_INS_ADRP, "adrp", 0x1000, 4);
        insn.operands.push_back(regOp(ARM64_REG_X1));
        insn.operands.push_back(immOp(static_cast<int64_t>(page)));
        insns.push_back(std::move(insn));
    }
    // add x1, x1, #off
    {
        auto insn = makeInsn(ARM64_INS_ADD, "add", 0x1004, 4);
        insn.operands.push_back(regOp(ARM64_REG_X1));
        insn.operands.push_back(regOp(ARM64_REG_X1));
        insn.operands.push_back(immOp(off));
        insns.push_back(std::move(insn));
    }
    // ldr x0, [x1, #16]
    {
        auto insn = makeInsn(ARM64_INS_LDR, "ldr", 0x1008, 4);
        insn.operands.push_back(regOp(ARM64_REG_X0));
        insn.operands.push_back(memOp(ARM64_REG_X1, 16));
        insns.push_back(std::move(insn));
    }

    auto expr = resolveSymbolic<ARM64ArchTraits>(
        ARM64_REG_X0, 2, insns, 0);

    // ldr → MemoryLoad{X1, 16, 8}
    // → Load(Add(resolve(X1), Const(16)))
    // resolve(X1) at idx=1: ADD → ArithmeticAdjust{X1, off}
    //   → Add(resolve(X1 at idx=0), Const(off))
    //   resolve(X1 at idx=0): ADRP → Const(page)
    //   → Add(Const(page), Const(off)) → Const(page+off)
    // → Load(Add(Const(page+off), Const(16))) → Load(Const(page+off+16))
    ASSERT_TRUE(expr.involvesLoad());
    auto* addr = expr.findLoadAddr();
    ASSERT_NE(addr, nullptr);
    ASSERT_TRUE(addr->isConstant());
    EXPECT_EQ(*addr->tryEval(), page + static_cast<uint64_t>(off) + 16);
}
