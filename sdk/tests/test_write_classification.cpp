#include <RegValueResolver.hpp>
#include <X86Traits.hpp>
#include <ARM64Traits.hpp>

#include <capstone/x86.h>
#include <capstone/arm64.h>

#include <gtest/gtest.h>

using namespace VMPilot::SDK::Segmentator;

// ---------------------------------------------------------------------------
// Helper: build a synthetic Capstone::Instruction
// ---------------------------------------------------------------------------
static Capstone::Instruction makeInsn(
    const std::string& mnemonic, uint64_t addr = 0x1000, uint16_t size = 4) {
    Capstone::Instruction insn;
    insn.id = 0;
    insn.address = addr;
    insn.size = size;
    insn.mnemonic = mnemonic;
    return insn;
}

static Capstone::Operand regOp(unsigned reg, uint8_t sz = 8) {
    Capstone::Operand op;
    op.type = Capstone::OpType::REG;
    op.reg = reg;
    op.imm = 0;
    op.mem = {};
    op.size = sz;
    return op;
}

static Capstone::Operand memOp(unsigned base, int64_t disp,
                                uint8_t sz = 8, unsigned segment = 0,
                                unsigned index = 0, int scale = 0) {
    Capstone::Operand op;
    op.type = Capstone::OpType::MEM;
    op.reg = 0;
    op.imm = 0;
    op.mem.base = base;
    op.mem.index = index;
    op.mem.scale = scale;
    op.mem.disp = disp;
    op.mem.segment = segment;
    op.size = sz;
    return op;
}

// ---------------------------------------------------------------------------
// WriteClassification variant type tests
// ---------------------------------------------------------------------------

TEST(WriteClassification, MemoryLoadHoldsFields) {
    MemoryLoad ml{42, 0x100, 4};
    EXPECT_EQ(ml.base_reg, 42u);
    EXPECT_EQ(ml.offset, 0x100);
    EXPECT_EQ(ml.load_size, 4);
}

TEST(WriteClassification, OpaqueSourceDefaults) {
    OpaqueSource os{OpaqueSource::Kind::SegmentBase, 35, 0};
    EXPECT_EQ(os.kind, OpaqueSource::Kind::SegmentBase);
    EXPECT_EQ(os.id, 35u);
    EXPECT_EQ(os.call_target, 0u);
}

TEST(WriteClassification, VariantHoldsSixTypes) {
    WriteClassification wc;
    wc = ResolvedConstant{42};
    EXPECT_TRUE(std::holds_alternative<ResolvedConstant>(wc));
    wc = RegisterForward{1};
    EXPECT_TRUE(std::holds_alternative<RegisterForward>(wc));
    wc = ArithmeticAdjust{1, -8};
    EXPECT_TRUE(std::holds_alternative<ArithmeticAdjust>(wc));
    wc = MemoryLoad{1, 0, 4};
    EXPECT_TRUE(std::holds_alternative<MemoryLoad>(wc));
    wc = OpaqueSource{OpaqueSource::Kind::SystemReg, 0, 0};
    EXPECT_TRUE(std::holds_alternative<OpaqueSource>(wc));
    wc = Unresolvable{};
    EXPECT_TRUE(std::holds_alternative<Unresolvable>(wc));
}

// ---------------------------------------------------------------------------
// X86ArchTraits::classifyWrite — new MemoryLoad cases
// ---------------------------------------------------------------------------

TEST(X86ClassifyWrite, MovFromMemBaseDisp) {
    // mov rax, [rbx+8]  →  MemoryLoad{RBX, 8, 8}
    auto insn = makeInsn("mov");
    insn.operands.push_back(regOp(X86_REG_RAX, 8));
    insn.operands.push_back(memOp(X86_REG_RBX, 8, 8));

    auto wc = X86ArchTraits::classifyWrite(insn, X86_REG_RAX);
    ASSERT_TRUE(std::holds_alternative<MemoryLoad>(wc));
    auto ml = std::get<MemoryLoad>(wc);
    EXPECT_EQ(ml.base_reg, static_cast<unsigned>(X86_REG_RBX));
    EXPECT_EQ(ml.offset, 8);
    EXPECT_EQ(ml.load_size, 8);
}

TEST(X86ClassifyWrite, MovFromRipRelative) {
    // mov rax, [rip+0x1234] at address 0x1000, size 7
    // effective address = 0x1000 + 7 + 0x1234 = 0x223B
    auto insn = makeInsn("mov", 0x1000, 7);
    insn.operands.push_back(regOp(X86_REG_RAX, 8));
    insn.operands.push_back(memOp(X86_REG_RIP, 0x1234, 8));

    auto wc = X86ArchTraits::classifyWrite(insn, X86_REG_RAX);
    ASSERT_TRUE(std::holds_alternative<MemoryLoad>(wc));
    auto ml = std::get<MemoryLoad>(wc);
    EXPECT_EQ(ml.base_reg, 0u);  // absolute
    EXPECT_EQ(ml.offset, static_cast<int64_t>(0x223B));
    EXPECT_EQ(ml.load_size, 8);
}

TEST(X86ClassifyWrite, MovzxFromMem) {
    // movzx eax, byte ptr [rcx]  →  MemoryLoad{RCX, 0, 1}
    auto insn = makeInsn("movzx");
    insn.operands.push_back(regOp(X86_REG_EAX, 4));
    insn.operands.push_back(memOp(X86_REG_RCX, 0, 1));

    auto wc = X86ArchTraits::classifyWrite(insn, X86_REG_EAX);
    ASSERT_TRUE(std::holds_alternative<MemoryLoad>(wc));
    auto ml = std::get<MemoryLoad>(wc);
    EXPECT_EQ(ml.base_reg, static_cast<unsigned>(X86_REG_RCX));
    EXPECT_EQ(ml.offset, 0);
    EXPECT_EQ(ml.load_size, 1);
}

TEST(X86ClassifyWrite, MovsxdFromMem) {
    // movsxd rax, dword ptr [rcx+rdx*4]  →  Unresolvable (has index)
    auto insn = makeInsn("movsxd");
    insn.operands.push_back(regOp(X86_REG_RAX, 8));
    insn.operands.push_back(memOp(X86_REG_RCX, 0, 4, 0, X86_REG_RDX, 4));

    // Has index register → not a simple load, falls through
    auto wc = X86ArchTraits::classifyWrite(insn, X86_REG_RAX);
    EXPECT_TRUE(std::holds_alternative<Unresolvable>(wc));
}

// ---------------------------------------------------------------------------
// X86ArchTraits::classifyWrite — new OpaqueSource cases
// ---------------------------------------------------------------------------

TEST(X86ClassifyWrite, MovFromFsSegment) {
    // mov rax, fs:[0x28]  →  OpaqueSource{SegmentBase, FS}
    auto insn = makeInsn("mov");
    insn.operands.push_back(regOp(X86_REG_RAX, 8));
    insn.operands.push_back(memOp(0, 0x28, 8, X86_REG_FS));

    auto wc = X86ArchTraits::classifyWrite(insn, X86_REG_RAX);
    ASSERT_TRUE(std::holds_alternative<OpaqueSource>(wc));
    auto os = std::get<OpaqueSource>(wc);
    EXPECT_EQ(os.kind, OpaqueSource::Kind::SegmentBase);
    EXPECT_EQ(os.id, static_cast<unsigned>(X86_REG_FS));
}

TEST(X86ClassifyWrite, MovFromGsSegment) {
    // mov rax, gs:[0x30]  →  OpaqueSource{SegmentBase, GS}
    auto insn = makeInsn("mov");
    insn.operands.push_back(regOp(X86_REG_RAX, 8));
    insn.operands.push_back(memOp(0, 0x30, 8, X86_REG_GS));

    auto wc = X86ArchTraits::classifyWrite(insn, X86_REG_RAX);
    ASSERT_TRUE(std::holds_alternative<OpaqueSource>(wc));
    auto os = std::get<OpaqueSource>(wc);
    EXPECT_EQ(os.kind, OpaqueSource::Kind::SegmentBase);
    EXPECT_EQ(os.id, static_cast<unsigned>(X86_REG_GS));
}

// ---------------------------------------------------------------------------
// X86ArchTraits — existing patterns unchanged
// ---------------------------------------------------------------------------

TEST(X86ClassifyWrite, ExistingPatternsPreserved) {
    namespace CX = Capstone::X86;

    // mov rax, 42  → ResolvedConstant (via isImmediateLoad)
    {
        auto insn = makeInsn("mov");
        insn.id = X86_INS_MOV;
        insn.operands.push_back(regOp(X86_REG_RAX, 8));
        Capstone::Operand imm;
        imm.type = Capstone::OpType::IMM;
        imm.imm = 42;
        imm.reg = 0;
        imm.mem = {};
        imm.size = 8;
        insn.operands.push_back(imm);

        auto wc = X86ArchTraits::classifyWrite(insn, X86_REG_RAX);
        ASSERT_TRUE(std::holds_alternative<ResolvedConstant>(wc));
        EXPECT_EQ(std::get<ResolvedConstant>(wc).value, 42u);
    }
}

// ---------------------------------------------------------------------------
// ARM64ArchTraits::classifyWrite — new MemoryLoad cases
// ---------------------------------------------------------------------------

TEST(ARM64ClassifyWrite, LdrBaseDisp) {
    // ldr x0, [x1, #16]  →  MemoryLoad{X1, 16, 8}
    auto insn = makeInsn("ldr");
    insn.operands.push_back(regOp(ARM64_REG_X0, 8));
    insn.operands.push_back(memOp(ARM64_REG_X1, 16, 8));

    auto wc = ARM64ArchTraits::classifyWrite(insn, ARM64_REG_X0);
    ASSERT_TRUE(std::holds_alternative<MemoryLoad>(wc));
    auto ml = std::get<MemoryLoad>(wc);
    EXPECT_EQ(ml.base_reg, static_cast<unsigned>(ARM64_REG_X1));
    EXPECT_EQ(ml.offset, 16);
    EXPECT_EQ(ml.load_size, 8);
}

TEST(ARM64ClassifyWrite, LdrbByteLoad) {
    // ldrb w0, [x1]  →  MemoryLoad{X1, 0, 1}
    auto insn = makeInsn("ldrb");
    insn.operands.push_back(regOp(ARM64_REG_W0, 4));
    insn.operands.push_back(memOp(ARM64_REG_X1, 0, 1));

    auto wc = ARM64ArchTraits::classifyWrite(insn, ARM64_REG_W0);
    ASSERT_TRUE(std::holds_alternative<MemoryLoad>(wc));
    auto ml = std::get<MemoryLoad>(wc);
    EXPECT_EQ(ml.load_size, 1);
}

TEST(ARM64ClassifyWrite, LdurNegativeOffset) {
    // ldur x0, [x1, #-8]  →  MemoryLoad{X1, -8, 8}
    auto insn = makeInsn("ldur");
    insn.operands.push_back(regOp(ARM64_REG_X0, 8));
    insn.operands.push_back(memOp(ARM64_REG_X1, -8, 8));

    auto wc = ARM64ArchTraits::classifyWrite(insn, ARM64_REG_X0);
    ASSERT_TRUE(std::holds_alternative<MemoryLoad>(wc));
    auto ml = std::get<MemoryLoad>(wc);
    EXPECT_EQ(ml.offset, -8);
}

TEST(ARM64ClassifyWrite, LdrWithIndexIsUnresolvable) {
    // ldr x0, [x1, x2]  →  Unresolvable (has index register)
    auto insn = makeInsn("ldr");
    insn.operands.push_back(regOp(ARM64_REG_X0, 8));
    insn.operands.push_back(memOp(ARM64_REG_X1, 0, 8, 0, ARM64_REG_X2));

    auto wc = ARM64ArchTraits::classifyWrite(insn, ARM64_REG_X0);
    EXPECT_TRUE(std::holds_alternative<Unresolvable>(wc));
}

// ---------------------------------------------------------------------------
// ARM64ArchTraits::classifyWrite — new OpaqueSource cases
// ---------------------------------------------------------------------------

TEST(ARM64ClassifyWrite, MrsSystemReg) {
    // mrs x0, tpidr_el0  →  OpaqueSource{SystemReg}
    auto insn = makeInsn("mrs");
    insn.operands.push_back(regOp(ARM64_REG_X0, 8));

    auto wc = ARM64ArchTraits::classifyWrite(insn, ARM64_REG_X0);
    ASSERT_TRUE(std::holds_alternative<OpaqueSource>(wc));
    auto os = std::get<OpaqueSource>(wc);
    EXPECT_EQ(os.kind, OpaqueSource::Kind::SystemReg);
}

// ---------------------------------------------------------------------------
// ARM64ArchTraits — existing patterns unchanged
// ---------------------------------------------------------------------------

TEST(ARM64ClassifyWrite, AdrpPreserved) {
    auto insn = makeInsn("adrp");
    insn.id = ARM64_INS_ADRP;
    insn.operands.push_back(regOp(ARM64_REG_X0, 8));
    Capstone::Operand imm;
    imm.type = Capstone::OpType::IMM;
    imm.imm = 0x4000;
    imm.reg = 0;
    imm.mem = {};
    imm.size = 8;
    insn.operands.push_back(imm);

    auto wc = ARM64ArchTraits::classifyWrite(insn, ARM64_REG_X0);
    ASSERT_TRUE(std::holds_alternative<ResolvedConstant>(wc));
    EXPECT_EQ(std::get<ResolvedConstant>(wc).value, 0x4000u);
}

// ---------------------------------------------------------------------------
// resolveRegValue — backward compatibility: new cases return 0
// ---------------------------------------------------------------------------

TEST(ResolveRegValue, MemoryLoadReturnsZero) {
    // Build instruction stream: mov rax, [rbx+8]
    std::vector<Capstone::Instruction> insns;
    auto insn = makeInsn("mov");
    insn.operands.push_back(regOp(X86_REG_RAX, 8));
    insn.operands.push_back(memOp(X86_REG_RBX, 8, 8));
    insns.push_back(insn);

    // resolveRegValue cannot read memory → returns 0
    uint64_t val = resolveRegValue<X86ArchTraits>(
        X86_REG_RAX, 0, insns, 0);
    EXPECT_EQ(val, 0u);
}

TEST(ResolveRegValue, OpaqueSourceReturnsZero) {
    // Build instruction stream: mrs x0, tpidr_el0
    std::vector<Capstone::Instruction> insns;
    auto insn = makeInsn("mrs");
    insn.operands.push_back(regOp(ARM64_REG_X0, 8));
    insns.push_back(insn);

    uint64_t val = resolveRegValue<ARM64ArchTraits>(
        ARM64_REG_X0, 0, insns, 0);
    EXPECT_EQ(val, 0u);
}
