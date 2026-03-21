#include <gtest/gtest.h>

#include "AtomicTraits.hpp"

#include <capstone/arm64.h>
#include <capstone/x86.h>

#include <string>

using namespace VMPilot::SDK::Core;
using namespace VMPilot::SDK::ReferenceAnalyzer;

// ---- Test Helpers ----

namespace {

/// Build a synthetic x86 instruction.
Capstone::Instruction makeX86Insn(uint32_t id, const std::string& mnemonic,
                                   bool lock_prefix = false) {
    Capstone::Instruction insn;
    insn.id = id;
    insn.mnemonic = mnemonic;
    insn.address = 0x1000;
    insn.size = 4;
    if (lock_prefix)
        insn.x86_prefix[0] = 0xF0;  // X86_PREFIX_LOCK
    return insn;
}

/// Build a synthetic ARM64 instruction with optional register operand.
Capstone::Instruction makeARM64Insn(const std::string& mnemonic,
                                     unsigned reg = 0) {
    Capstone::Instruction insn;
    insn.id = 0;  // ARM64 classification uses mnemonic, not id
    insn.mnemonic = mnemonic;
    insn.address = 0x1000;
    insn.size = 4;
    if (reg != 0) {
        Capstone::Operand op;
        op.type = Capstone::OpType::REG;
        op.reg = reg;
        op.size = 8;
        insn.operands.push_back(op);
    }
    return insn;
}

/// Add a memory operand to an instruction.
void addMemOp(Capstone::Instruction& insn, uint8_t size = 4) {
    Capstone::Operand op;
    op.type = Capstone::OpType::MEM;
    op.size = size;
    op.mem.base = X86_REG_RAX;
    insn.operands.push_back(op);
}

/// Add a register operand to an instruction.
void addRegOp(Capstone::Instruction& insn, unsigned reg, uint8_t size = 4) {
    Capstone::Operand op;
    op.type = Capstone::OpType::REG;
    op.reg = reg;
    op.size = size;
    insn.operands.push_back(op);
}

}  // namespace

// ---- atomicWidthFromSize tests ----

TEST(AtomicWidthFromSize, AllSizes) {
    EXPECT_EQ(atomicWidthFromSize(1), AtomicWidth::Atomic8);
    EXPECT_EQ(atomicWidthFromSize(2), AtomicWidth::Atomic16);
    EXPECT_EQ(atomicWidthFromSize(4), AtomicWidth::Atomic32);
    EXPECT_EQ(atomicWidthFromSize(8), AtomicWidth::Atomic64);
    EXPECT_EQ(atomicWidthFromSize(16), AtomicWidth::Atomic128);
    EXPECT_EQ(atomicWidthFromSize(0), AtomicWidth::None);
    EXPECT_EQ(atomicWidthFromSize(3), AtomicWidth::None);
}

// ---- X86AtomicTraits tests ----

TEST(X86AtomicTraits, LockAdd) {
    auto insn = makeX86Insn(X86_INS_ADD, "add", /*lock_prefix=*/true);
    addMemOp(insn, 4);
    addRegOp(insn, X86_REG_EAX, 4);

    auto info = X86AtomicTraits::classify(insn, true);
    EXPECT_EQ(info.op, AtomicOp::RMW);
    EXPECT_EQ(info.ordering, AtomicOrdering::AcqRel);
}

TEST(X86AtomicTraits, LockCmpxchg) {
    auto insn = makeX86Insn(X86_INS_CMPXCHG, "cmpxchg", /*lock_prefix=*/true);
    addMemOp(insn, 4);
    addRegOp(insn, X86_REG_EAX, 4);

    auto info = X86AtomicTraits::classify(insn, true);
    EXPECT_EQ(info.op, AtomicOp::CompareSwap);
    EXPECT_EQ(info.ordering, AtomicOrdering::AcqRel);
}

TEST(X86AtomicTraits, Xchg) {
    // XCHG with memory operand is implicitly atomic (no LOCK needed)
    auto insn = makeX86Insn(X86_INS_XCHG, "xchg", /*lock_prefix=*/false);
    addMemOp(insn, 4);
    addRegOp(insn, X86_REG_EAX, 4);

    auto info = X86AtomicTraits::classify(insn, true);
    EXPECT_EQ(info.op, AtomicOp::Swap);
    EXPECT_EQ(info.ordering, AtomicOrdering::AcqRel);
}

TEST(X86AtomicTraits, NoLockNotAtomic) {
    auto insn = makeX86Insn(X86_INS_ADD, "add", /*lock_prefix=*/false);
    addMemOp(insn, 4);
    addRegOp(insn, X86_REG_EAX, 4);

    auto info = X86AtomicTraits::classify(insn, true);
    EXPECT_EQ(info.op, AtomicOp::None);
    EXPECT_EQ(info.ordering, AtomicOrdering::None);
    EXPECT_EQ(info.width, AtomicWidth::None);
}

TEST(X86AtomicTraits, NoMemNotAtomic) {
    auto insn = makeX86Insn(X86_INS_ADD, "add", /*lock_prefix=*/true);
    addRegOp(insn, X86_REG_EAX, 4);
    addRegOp(insn, X86_REG_EBX, 4);

    auto info = X86AtomicTraits::classify(insn, false);
    EXPECT_EQ(info.op, AtomicOp::None);
    EXPECT_EQ(info.ordering, AtomicOrdering::None);
    EXPECT_EQ(info.width, AtomicWidth::None);
}

TEST(X86AtomicTraits, Cmpxchg16b) {
    auto w = atomicWidthForCmpxchg(X86_INS_CMPXCHG16B);
    EXPECT_EQ(w, AtomicWidth::Atomic128);

    auto w8 = atomicWidthForCmpxchg(X86_INS_CMPXCHG8B);
    EXPECT_EQ(w8, AtomicWidth::Atomic64);

    auto wn = atomicWidthForCmpxchg(X86_INS_CMPXCHG);
    EXPECT_EQ(wn, AtomicWidth::None);
}

TEST(X86AtomicTraits, IsMemWrite_Mov) {
    auto insn = makeX86Insn(X86_INS_MOV, "mov");
    addMemOp(insn, 4);
    addRegOp(insn, X86_REG_EAX, 4);

    EXPECT_TRUE(X86AtomicTraits::isMemWrite(insn, 0));
    EXPECT_FALSE(X86AtomicTraits::isMemWrite(insn, 1));
}

TEST(X86AtomicTraits, IsMemWrite_Cmp) {
    auto insn = makeX86Insn(X86_INS_CMP, "cmp");
    addMemOp(insn, 4);
    addRegOp(insn, X86_REG_EAX, 4);

    EXPECT_FALSE(X86AtomicTraits::isMemWrite(insn, 0));
}

TEST(X86AtomicTraits, IsMemWrite_Test) {
    auto insn = makeX86Insn(X86_INS_TEST, "test");
    addMemOp(insn, 4);
    addRegOp(insn, X86_REG_EAX, 4);

    EXPECT_FALSE(X86AtomicTraits::isMemWrite(insn, 0));
}

// ---- ARM64AtomicTraits tests ----

TEST(ARM64AtomicTraits, Ldar) {
    auto insn = makeARM64Insn("ldar", ARM64_REG_W0);
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::LoadAcquire);
    EXPECT_EQ(info.ordering, AtomicOrdering::Acquire);
    EXPECT_EQ(info.width, AtomicWidth::Atomic32);
}

TEST(ARM64AtomicTraits, Stlr) {
    auto insn = makeARM64Insn("stlr", ARM64_REG_W0);
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::StoreRelease);
    EXPECT_EQ(info.ordering, AtomicOrdering::Release);
    EXPECT_EQ(info.width, AtomicWidth::Atomic32);
}

TEST(ARM64AtomicTraits, Cas) {
    auto insn = makeARM64Insn("cas", ARM64_REG_W0);
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::CompareSwap);
    EXPECT_EQ(info.ordering, AtomicOrdering::Relaxed);
    EXPECT_EQ(info.width, AtomicWidth::Atomic32);
}

TEST(ARM64AtomicTraits, Casal) {
    auto insn = makeARM64Insn("casal", ARM64_REG_W0);
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::CompareSwap);
    EXPECT_EQ(info.ordering, AtomicOrdering::AcqRel);
    EXPECT_EQ(info.width, AtomicWidth::Atomic32);
}

TEST(ARM64AtomicTraits, Swp) {
    auto insn = makeARM64Insn("swp", ARM64_REG_W0);
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::Swap);
    EXPECT_EQ(info.ordering, AtomicOrdering::Relaxed);
    EXPECT_EQ(info.width, AtomicWidth::Atomic32);
}

TEST(ARM64AtomicTraits, Ldadd) {
    auto insn = makeARM64Insn("ldadd", ARM64_REG_W0);
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::FetchAdd);
    EXPECT_EQ(info.ordering, AtomicOrdering::Relaxed);
    EXPECT_EQ(info.width, AtomicWidth::Atomic32);
}

TEST(ARM64AtomicTraits, Dmb) {
    auto insn = makeARM64Insn("dmb");
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::Fence);
    EXPECT_EQ(info.ordering, AtomicOrdering::AcqRel);
    EXPECT_EQ(info.width, AtomicWidth::None);
}

TEST(ARM64AtomicTraits, LdarWidth_Byte) {
    auto insn = makeARM64Insn("ldarb");
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::LoadAcquire);
    EXPECT_EQ(info.width, AtomicWidth::Atomic8);
}

TEST(ARM64AtomicTraits, LdarWidth_Half) {
    auto insn = makeARM64Insn("ldarh");
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::LoadAcquire);
    EXPECT_EQ(info.width, AtomicWidth::Atomic16);
}

TEST(ARM64AtomicTraits, LdarWidth_32bit) {
    auto insn = makeARM64Insn("ldar", ARM64_REG_W0);
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::LoadAcquire);
    EXPECT_EQ(info.width, AtomicWidth::Atomic32);
}

TEST(ARM64AtomicTraits, LdarWidth_64bit) {
    auto insn = makeARM64Insn("ldar", ARM64_REG_X0);
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::LoadAcquire);
    EXPECT_EQ(info.width, AtomicWidth::Atomic64);
}

TEST(ARM64AtomicTraits, IsMemWrite_Str) {
    auto insn = makeARM64Insn("str", ARM64_REG_W0);
    AtomicInfo atomic;  // None
    EXPECT_TRUE(ARM64AtomicTraits::isMemWrite(insn, atomic));
}

TEST(ARM64AtomicTraits, IsMemWrite_Ldr) {
    auto insn = makeARM64Insn("ldr", ARM64_REG_W0);
    AtomicInfo atomic;  // None
    EXPECT_FALSE(ARM64AtomicTraits::isMemWrite(insn, atomic));
}

TEST(ARM64AtomicTraits, IsMemWrite_Stp) {
    auto insn = makeARM64Insn("stp", ARM64_REG_W0);
    AtomicInfo atomic;  // None
    EXPECT_TRUE(ARM64AtomicTraits::isMemWrite(insn, atomic));
}

TEST(ARM64AtomicTraits, IsMemWrite_Stlr) {
    auto insn = makeARM64Insn("stlr", ARM64_REG_W0);
    auto atomic = ARM64AtomicTraits::classify(insn);
    EXPECT_TRUE(ARM64AtomicTraits::isMemWrite(insn, atomic));
}

TEST(ARM64AtomicTraits, IsMemWrite_Cas_RMW) {
    auto insn = makeARM64Insn("cas", ARM64_REG_W0);
    auto atomic = ARM64AtomicTraits::classify(insn);
    // CAS is a read-modify-write, so it is a write
    EXPECT_TRUE(ARM64AtomicTraits::isMemWrite(insn, atomic));
}

TEST(ARM64AtomicTraits, IsMemWrite_Swp_RMW) {
    auto insn = makeARM64Insn("swp", ARM64_REG_W0);
    auto atomic = ARM64AtomicTraits::classify(insn);
    EXPECT_TRUE(ARM64AtomicTraits::isMemWrite(insn, atomic));
}

TEST(ARM64AtomicTraits, IsMemWrite_Ldadd_RMW) {
    auto insn = makeARM64Insn("ldadd", ARM64_REG_W0);
    auto atomic = ARM64AtomicTraits::classify(insn);
    EXPECT_TRUE(ARM64AtomicTraits::isMemWrite(insn, atomic));
}

// ---- Additional ARM64 coverage ----

TEST(ARM64AtomicTraits, Dsb) {
    auto insn = makeARM64Insn("dsb");
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::Fence);
    EXPECT_EQ(info.ordering, AtomicOrdering::AcqRel);
}

TEST(ARM64AtomicTraits, Isb) {
    auto insn = makeARM64Insn("isb");
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::Fence);
    EXPECT_EQ(info.ordering, AtomicOrdering::AcqRel);
}

TEST(ARM64AtomicTraits, Ldxr) {
    auto insn = makeARM64Insn("ldxr", ARM64_REG_W0);
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::LoadExclusive);
    EXPECT_EQ(info.ordering, AtomicOrdering::Relaxed);
}

TEST(ARM64AtomicTraits, Ldaxr) {
    auto insn = makeARM64Insn("ldaxr", ARM64_REG_W0);
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::LoadExclusive);
    EXPECT_EQ(info.ordering, AtomicOrdering::Acquire);
}

TEST(ARM64AtomicTraits, Stxr) {
    auto insn = makeARM64Insn("stxr", ARM64_REG_W0);
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::StoreExclusive);
    EXPECT_EQ(info.ordering, AtomicOrdering::Relaxed);
}

TEST(ARM64AtomicTraits, Stlxr) {
    auto insn = makeARM64Insn("stlxr", ARM64_REG_W0);
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::StoreExclusive);
    EXPECT_EQ(info.ordering, AtomicOrdering::Release);
}

TEST(ARM64AtomicTraits, Ldclr) {
    auto insn = makeARM64Insn("ldclr", ARM64_REG_W0);
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::RMW);
    EXPECT_EQ(info.ordering, AtomicOrdering::Relaxed);
}

TEST(ARM64AtomicTraits, Ldset) {
    auto insn = makeARM64Insn("ldset", ARM64_REG_W0);
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::RMW);
    EXPECT_EQ(info.ordering, AtomicOrdering::Relaxed);
}

TEST(ARM64AtomicTraits, NonAtomic) {
    auto insn = makeARM64Insn("add", ARM64_REG_W0);
    auto info = ARM64AtomicTraits::classify(insn);
    EXPECT_EQ(info.op, AtomicOp::None);
    EXPECT_EQ(info.ordering, AtomicOrdering::None);
    EXPECT_EQ(info.width, AtomicWidth::None);
}

// ---- X86 additional coverage ----

TEST(X86AtomicTraits, LockXadd) {
    auto insn = makeX86Insn(X86_INS_XADD, "xadd", /*lock_prefix=*/true);
    addMemOp(insn, 4);
    addRegOp(insn, X86_REG_EAX, 4);

    auto info = X86AtomicTraits::classify(insn, true);
    EXPECT_EQ(info.op, AtomicOp::FetchAdd);
    EXPECT_EQ(info.ordering, AtomicOrdering::AcqRel);
}

TEST(X86AtomicTraits, LockSub) {
    auto insn = makeX86Insn(X86_INS_SUB, "sub", /*lock_prefix=*/true);
    addMemOp(insn, 4);
    addRegOp(insn, X86_REG_EAX, 4);

    auto info = X86AtomicTraits::classify(insn, true);
    EXPECT_EQ(info.op, AtomicOp::RMW);
    EXPECT_EQ(info.ordering, AtomicOrdering::AcqRel);
}

TEST(X86AtomicTraits, LockInc) {
    auto insn = makeX86Insn(X86_INS_INC, "inc", /*lock_prefix=*/true);
    addMemOp(insn, 4);

    auto info = X86AtomicTraits::classify(insn, true);
    EXPECT_EQ(info.op, AtomicOp::RMW);
    EXPECT_EQ(info.ordering, AtomicOrdering::AcqRel);
}

TEST(X86AtomicTraits, IsMemWrite_Push) {
    auto insn = makeX86Insn(X86_INS_PUSH, "push");
    addRegOp(insn, X86_REG_EAX, 4);

    EXPECT_FALSE(X86AtomicTraits::isMemWrite(insn, 0));
}
