#include <gtest/gtest.h>

#include "SymExpr.hpp"
#include "SymbolicResolver.hpp"
#include "SectionLookup.hpp"
#include "layers/PatternLayer.hpp"

#include <ARM64Traits.hpp>
#include <DataReference.hpp>
#include <NativeSymbolTable.hpp>
#include <ReadOnlySection.hpp>
#include <X86Traits.hpp>
#include <capstone.hpp>

#include <capstone/capstone.h>
#include <capstone/arm64.h>
#include <capstone/x86.h>

#include <cstring>
#include <string>
#include <vector>

using namespace VMPilot::SDK;
using namespace VMPilot::SDK::Core;
using namespace VMPilot::SDK::ReferenceAnalyzer;

// ============================================================================
// Test Helpers
// ============================================================================

namespace {

std::vector<SectionInfo> makeTestSections() {
    return {
        {0x401000, 0x1000, SectionKind::Text, ".text"},
        {0x402000, 0x200, SectionKind::Rodata, ".rodata"},
        {0x403000, 0x200, SectionKind::Data, ".data"},
        {0x404000, 0x100, SectionKind::Bss, ".bss"},
        {0x405000, 0x100, SectionKind::Tls, ".tdata"},
        {0x406000, 0x100, SectionKind::Got, ".got"},
        {0x407000, 0x100, SectionKind::Plt, ".plt"},
    };
}

/// Build a synthetic instruction with specified fields.
Capstone::Instruction makeSyntheticInsn(
    uint64_t address, uint16_t size,
    const std::string& mnemonic,
    const std::string& op_str = {},
    const std::vector<Capstone::Operand>& operands = {},
    const std::vector<uint8_t>& groups = {},
    uint32_t id = 0) {
    Capstone::Instruction insn;
    insn.id = id;
    insn.address = address;
    insn.size = size;
    insn.mnemonic = mnemonic;
    insn.op_str = op_str;
    insn.operands = operands;
    insn.groups = groups;
    std::memset(insn.x86_prefix, 0, sizeof(insn.x86_prefix));
    return insn;
}

/// Build a REG operand.
Capstone::Operand makeRegOp(unsigned reg, uint8_t size = 8) {
    Capstone::Operand op;
    op.type = Capstone::OpType::REG;
    op.reg = reg;
    op.size = size;
    return op;
}

/// Build an IMM operand.
Capstone::Operand makeImmOp(int64_t imm) {
    Capstone::Operand op;
    op.type = Capstone::OpType::IMM;
    op.imm = imm;
    return op;
}

/// Build a MEM operand.
Capstone::Operand makeMemOp(unsigned base, int64_t disp,
                             unsigned segment = 0,
                             unsigned index = 0, int scale = 0,
                             uint8_t size = 4) {
    Capstone::Operand op;
    op.type = Capstone::OpType::MEM;
    op.mem.base = base;
    op.mem.disp = disp;
    op.mem.segment = segment;
    op.mem.index = index;
    op.mem.scale = scale;
    op.size = size;
    return op;
}

}  // namespace

// ============================================================================
// PatternLayer TLS Detection Tests
// ============================================================================

TEST(PatternLayer_X86FsSegmentTls, DetectsStackCanary) {
    // Use real capstone disassembly of: mov eax, dword ptr fs:[0x28]
    // Encoding: 64 8b 04 25 28 00 00 00
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);
    std::vector<uint8_t> code = {0x64, 0x8b, 0x04, 0x25,
                                  0x28, 0x00, 0x00, 0x00};
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 1u);

    auto sections = makeTestSections();
    SectionLookup lookup(sections);
    Segmentator::NativeSymbolTable symbols;

    auto refs = analyzePatterns<Segmentator::X86ArchTraits>(
        insns, 0x401000, 0x1000, symbols, lookup, {});

    bool found_tls = false;
    for (const auto& ref : refs) {
        if (ref.kind == DataRefKind::TlsVar &&
            ref.source == DataRefSource::PatternMatch) {
            found_tls = true;
            EXPECT_EQ(ref.tls_model, TlsModel::LocalExec);
            EXPECT_EQ(ref.insn_offset, 0x401000u);
        }
    }
    EXPECT_TRUE(found_tls) << "FS segment override should be detected as TLS";
}

TEST(PatternLayer_X86GsSegmentTls, DetectsGsAccess) {
    // mov rax, gs:[0x28]
    // Encoding: 65 48 8b 04 25 28 00 00 00
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);
    std::vector<uint8_t> code = {0x65, 0x48, 0x8b, 0x04, 0x25,
                                  0x28, 0x00, 0x00, 0x00};
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 1u);

    auto sections = makeTestSections();
    SectionLookup lookup(sections);
    Segmentator::NativeSymbolTable symbols;

    auto refs = analyzePatterns<Segmentator::X86ArchTraits>(
        insns, 0x401000, 0x1000, symbols, lookup, {});

    bool found_tls = false;
    for (const auto& ref : refs) {
        if (ref.kind == DataRefKind::TlsVar &&
            ref.source == DataRefSource::PatternMatch) {
            found_tls = true;
            EXPECT_EQ(ref.tls_model, TlsModel::LocalExec);
        }
    }
    EXPECT_TRUE(found_tls) << "GS segment override should be detected as TLS";
}

TEST(PatternLayer_ARM64MrsTls, DetectsTpidrEl0) {
    // Synthetic ARM64 instruction sequence:
    //   [0] mrs x0, tpidr_el0       -> reads system register
    //   [1] ldr w1, [x0, #0x10]     -> loads from TLS base + offset
    //
    // The ldr instruction has a MEM operand with base=X0.
    // resolveSymbolic should trace X0 back to the mrs and produce
    // OpaqueKind::SystemReg.

    std::vector<Capstone::Instruction> insns;

    // Instruction 0: mrs x0, tpidr_el0
    insns.push_back(makeSyntheticInsn(
        0x401000, 4, "mrs", "x0, tpidr_el0",
        {makeRegOp(ARM64_REG_X0)}));

    // Instruction 1: ldr w1, [x0, #0x10]
    insns.push_back(makeSyntheticInsn(
        0x401004, 4, "ldr", "w1, [x0, #0x10]",
        {makeRegOp(ARM64_REG_W1, 4),
         makeMemOp(ARM64_REG_X0, 0x10, 0, 0, 0, 4)}));

    auto sections = makeTestSections();
    SectionLookup lookup(sections);
    Segmentator::NativeSymbolTable symbols;

    auto refs = analyzePatterns<Segmentator::ARM64ArchTraits>(
        insns, 0x401000, 0x1000, symbols, lookup, {});

    bool found_tls = false;
    for (const auto& ref : refs) {
        if (ref.kind == DataRefKind::TlsVar &&
            ref.source == DataRefSource::PatternMatch &&
            ref.tls_model == TlsModel::LocalExec) {
            found_tls = true;
            EXPECT_EQ(ref.insn_offset, 0x401004u);
        }
    }
    EXPECT_TRUE(found_tls)
        << "mrs tpidr_el0 + ldr should be detected as TLS LocalExec";
}

TEST(PatternLayer_X86TlsGetAddrCall, DetectsGeneralDynamic) {
    // Synthetic instruction sequence:
    //   [0] lea rdi, [rip+0x2000]     -> load TLSGD symbol address
    //   [1] call __tls_get_addr       -> TLS General Dynamic

    std::vector<Capstone::Instruction> insns;

    // Instruction 0: lea rdi, [rip+0x2000]
    // At address 0x401000, size 7 -> target VA = 0x401000 + 7 + 0x2000 = 0x403007
    {
        auto mem = makeMemOp(X86_REG_RIP, 0x2000);
        insns.push_back(makeSyntheticInsn(
            0x401000, 7, "lea", "rdi, [rip+0x2000]",
            {makeRegOp(X86_REG_RDI), mem}));
    }

    // Instruction 1: call __tls_get_addr (direct call to 0x407010)
    {
        // Groups: CS_GRP_CALL = 7 (standard capstone group)
        insns.push_back(makeSyntheticInsn(
            0x401007, 5, "call", "__tls_get_addr",
            {makeImmOp(0x407010)},
            {CS_GRP_CALL}));
    }

    auto sections = makeTestSections();
    SectionLookup lookup(sections);

    // Set up symbol table with __tls_get_addr at 0x407010
    Segmentator::NativeSymbolTable symbols;
    {
        Segmentator::NativeSymbolTableEntry sym;
        sym.name = "__tls_get_addr";
        sym.address = 0x407010;
        sym.size = 0;
        sym.type = Segmentator::SymbolType::FUNC;
        sym.isGlobal = true;
        symbols.push_back(std::move(sym));
    }

    auto refs = analyzePatterns<Segmentator::X86ArchTraits>(
        insns, 0x401000, 0x1000, symbols, lookup, {});

    bool found_tls = false;
    for (const auto& ref : refs) {
        if (ref.kind == DataRefKind::TlsVar &&
            ref.source == DataRefSource::PatternMatch &&
            ref.tls_model == TlsModel::GeneralDynamic) {
            found_tls = true;
            EXPECT_EQ(ref.insn_offset, 0x401007u);
            // The preceding LEA should provide the target VA
            EXPECT_EQ(ref.target_va, 0x401000u + 7u + 0x2000u);
        }
    }
    EXPECT_TRUE(found_tls)
        << "call __tls_get_addr should be detected as TLS GeneralDynamic";
}

TEST(PatternLayer_EmptyInsns, NoRefs) {
    std::vector<Capstone::Instruction> insns;
    auto sections = makeTestSections();
    SectionLookup lookup(sections);
    Segmentator::NativeSymbolTable symbols;

    auto refs = analyzePatterns<Segmentator::X86ArchTraits>(
        insns, 0x401000, 0x1000, symbols, lookup, {});

    EXPECT_TRUE(refs.empty());
}

TEST(PatternLayer_NonTlsInstruction, NoTlsRef) {
    // Regular mov eax, [rip+0x1000] should NOT produce TLS ref
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);
    // mov eax, dword ptr [rip+0x0FFA]
    std::vector<uint8_t> code = {0x8b, 0x05, 0xfa, 0x0f, 0x00, 0x00};
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 1u);

    auto sections = makeTestSections();
    SectionLookup lookup(sections);
    Segmentator::NativeSymbolTable symbols;

    auto refs = analyzePatterns<Segmentator::X86ArchTraits>(
        insns, 0x401000, 0x1000, symbols, lookup, {});

    for (const auto& ref : refs) {
        EXPECT_NE(ref.kind, DataRefKind::TlsVar)
            << "Regular rip-relative mov should not produce TLS ref";
    }
}

TEST(PatternLayer_MacOsTlvResolver, DetectsLocalDynamic) {
    // Synthetic ARM64 instruction sequence for macOS TLV resolver:
    //   [0] adrp  x0, 0x405000        -> page of .tdata section
    //   [1] add   x0, x0, #0x10       -> x0 = 0x405010 (in .tdata)
    //   [2] ldr   x11, [x0]           -> load TLV resolver thunk
    //   [3] blr   x11                 -> call resolver

    std::vector<Capstone::Instruction> insns;

    // Instruction 0: adrp x0, 0x405000
    insns.push_back(makeSyntheticInsn(
        0x401000, 4, "adrp", "x0, #0x405000",
        {makeRegOp(ARM64_REG_X0), makeImmOp(0x405000)},
        {}, ARM64_INS_ADRP));

    // Instruction 1: add x0, x0, #0x10
    insns.push_back(makeSyntheticInsn(
        0x401004, 4, "add", "x0, x0, #0x10",
        {makeRegOp(ARM64_REG_X0), makeRegOp(ARM64_REG_X0),
         makeImmOp(0x10)},
        {}, ARM64_INS_ADD));

    // Instruction 2: ldr x11, [x0]
    // LDR is not classified by ARM64ArchTraits, so id=0 (Unresolvable)
    // triggers the load-from-MEM fallback in resolveSymbolic.
    insns.push_back(makeSyntheticInsn(
        0x401008, 4, "ldr", "x11, [x0]",
        {makeRegOp(ARM64_REG_X11, 8),
         makeMemOp(ARM64_REG_X0, 0, 0, 0, 0, 8)},
        {}, ARM64_INS_LDR));

    // Instruction 3: blr x11
    insns.push_back(makeSyntheticInsn(
        0x40100C, 4, "blr", "x11",
        {makeRegOp(ARM64_REG_X11)},
        {7},  // CS_GRP_CALL
        ARM64_INS_BLR));

    auto sections = makeTestSections();
    SectionLookup lookup(sections);
    Segmentator::NativeSymbolTable symbols;

    auto refs = analyzePatterns<Segmentator::ARM64ArchTraits>(
        insns, 0x401000, 0x1000, symbols, lookup, {});

    bool found_tls = false;
    for (const auto& ref : refs) {
        if (ref.kind == DataRefKind::TlsVar &&
            ref.source == DataRefSource::PatternMatch &&
            ref.tls_model == TlsModel::LocalDynamic) {
            found_tls = true;
            EXPECT_EQ(ref.insn_offset, 0x40100Cu);
            EXPECT_EQ(ref.target_va, 0x405010u);
        }
    }
    EXPECT_TRUE(found_tls)
        << "blr xN with load from .tls section should be TLS LocalDynamic";
}

// ============================================================================
// SymbolicResolver Tests
// ============================================================================

TEST(SymbolicResolver, ResolveConstant) {
    // adrp x0, 0x1000
    std::vector<Capstone::Instruction> insns;
    insns.push_back(makeSyntheticInsn(
        0x401000, 4, "adrp", "x0, #0x1000",
        {makeRegOp(ARM64_REG_X0), makeImmOp(0x1000)},
        {}, ARM64_INS_ADRP));

    auto expr = resolveSymbolic<Segmentator::ARM64ArchTraits>(
        ARM64_REG_X0, 0, insns, 0);

    auto val = expr.tryEval();
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 0x1000u);
}

TEST(SymbolicResolver, ResolveArithmeticChain) {
    // adrp x0, 0x405000
    // add  x0, x0, #0x10
    std::vector<Capstone::Instruction> insns;
    insns.push_back(makeSyntheticInsn(
        0x401000, 4, "adrp", "x0, #0x405000",
        {makeRegOp(ARM64_REG_X0), makeImmOp(0x405000)},
        {}, ARM64_INS_ADRP));
    insns.push_back(makeSyntheticInsn(
        0x401004, 4, "add", "x0, x0, #0x10",
        {makeRegOp(ARM64_REG_X0), makeRegOp(ARM64_REG_X0),
         makeImmOp(0x10)},
        {}, ARM64_INS_ADD));

    auto expr = resolveSymbolic<Segmentator::ARM64ArchTraits>(
        ARM64_REG_X0, 1, insns, 0);

    auto val = expr.tryEval();
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 0x405010u);
}

TEST(SymbolicResolver, ResolveMrsSystemReg) {
    // mrs x0, tpidr_el0
    std::vector<Capstone::Instruction> insns;
    insns.push_back(makeSyntheticInsn(
        0x401000, 4, "mrs", "x0, tpidr_el0",
        {makeRegOp(ARM64_REG_X0)}));

    auto expr = resolveSymbolic<Segmentator::ARM64ArchTraits>(
        ARM64_REG_X0, 0, insns, 0);

    EXPECT_TRUE(expr.involvesOpaque(SymExpr::OpaqueKind::SystemReg));
}

TEST(SymbolicResolver, ResolveSegmentBase) {
    // Synthetic: mov rax, fs:[0x28]
    // The instruction has segment=FS on the MEM operand and writes to RAX
    std::vector<Capstone::Instruction> insns;
    insns.push_back(makeSyntheticInsn(
        0x401000, 8, "mov", "rax, fs:[0x28]",
        {makeRegOp(X86_REG_RAX, 8),
         makeMemOp(0, 0x28, X86_REG_FS, 0, 0, 8)}));

    auto expr = resolveSymbolic<Segmentator::X86ArchTraits>(
        X86_REG_RAX, 0, insns, 0);

    EXPECT_TRUE(expr.involvesOpaque(SymExpr::OpaqueKind::SegmentBase));
}

TEST(SymbolicResolver, ResolveLoadChain) {
    // adrp x0, 0x405000
    // add  x0, x0, #0x10
    // ldr  x11, [x0]
    std::vector<Capstone::Instruction> insns;
    insns.push_back(makeSyntheticInsn(
        0x401000, 4, "adrp", "x0, #0x405000",
        {makeRegOp(ARM64_REG_X0), makeImmOp(0x405000)},
        {}, ARM64_INS_ADRP));
    insns.push_back(makeSyntheticInsn(
        0x401004, 4, "add", "x0, x0, #0x10",
        {makeRegOp(ARM64_REG_X0), makeRegOp(ARM64_REG_X0),
         makeImmOp(0x10)},
        {}, ARM64_INS_ADD));
    insns.push_back(makeSyntheticInsn(
        0x401008, 4, "ldr", "x11, [x0]",
        {makeRegOp(ARM64_REG_X11, 8),
         makeMemOp(ARM64_REG_X0, 0, 0, 0, 0, 8)},
        {}, ARM64_INS_LDR));

    auto expr = resolveSymbolic<Segmentator::ARM64ArchTraits>(
        ARM64_REG_X11, 2, insns, 0);

    EXPECT_TRUE(expr.involvesLoad());
    const auto* load_addr = expr.findLoadAddr();
    ASSERT_NE(load_addr, nullptr);
    auto val = load_addr->tryEval();
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 0x405010u);
}

// ============================================================================
// Jump Table Helper Tests
// ============================================================================

TEST(PatternLayer_JumpTableHelpers, ReadRodataBytes) {
    Segmentator::ReadOnlySection sec;
    sec.base_addr = 0x402000;
    sec.data = {0x01, 0x02, 0x03, 0x04, 0x05};

    uint8_t buf[4] = {};
    EXPECT_TRUE(readRodataBytes({sec}, 0x402000, buf, 4));
    EXPECT_EQ(buf[0], 0x01);
    EXPECT_EQ(buf[3], 0x04);

    EXPECT_FALSE(readRodataBytes({sec}, 0x402003, buf, 4));
    EXPECT_FALSE(readRodataBytes({sec}, 0x403000, buf, 1));
}

TEST(PatternLayer_JumpTableHelpers, FindBoundsCheckX86) {
    std::vector<Capstone::Instruction> insns;

    // cmp eax, 5
    insns.push_back(makeSyntheticInsn(
        0x401000, 3, "cmp", "eax, 5",
        {makeRegOp(X86_REG_EAX, 4), makeImmOp(5)}));

    // ja 0x401100
    insns.push_back(makeSyntheticInsn(
        0x401003, 2, "ja", "0x401100",
        {makeImmOp(0x401100)},
        {1}));  // CS_GRP_JUMP

    // nop (index 2, the insn_idx we search backward from)
    insns.push_back(makeSyntheticInsn(0x401005, 1, "nop", ""));

    uint32_t count = findBoundsCheck(insns, 2, false);
    EXPECT_EQ(count, 6u);  // 5 + 1
}

TEST(PatternLayer_JumpTableHelpers, FindBoundsCheckARM64) {
    std::vector<Capstone::Instruction> insns;

    // cmp w0, #7
    insns.push_back(makeSyntheticInsn(
        0x401000, 4, "cmp", "w0, #7",
        {makeRegOp(ARM64_REG_W0, 4), makeImmOp(7)}));

    // b.hi 0x401100
    insns.push_back(makeSyntheticInsn(
        0x401004, 4, "b.hi", "0x401100",
        {makeImmOp(0x401100)},
        {1}));  // CS_GRP_JUMP

    // nop
    insns.push_back(makeSyntheticInsn(0x401008, 4, "nop", ""));

    uint32_t count = findBoundsCheck(insns, 2, true);
    EXPECT_EQ(count, 8u);  // 7 + 1
}

// ============================================================================
// Integration: analyzePatterns template dispatch
// ============================================================================

TEST(PatternLayer_Integration, X86EmptyRodataNoJumpTables) {
    // A simple non-jump instruction should produce no jump table refs
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);
    std::vector<uint8_t> code = {0x90};  // nop
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 1u);

    auto sections = makeTestSections();
    SectionLookup lookup(sections);
    Segmentator::NativeSymbolTable symbols;

    auto refs = analyzePatterns<Segmentator::X86ArchTraits>(
        insns, 0x401000, 0x1000, symbols, lookup, {});

    EXPECT_TRUE(refs.empty());
}

TEST(PatternLayer_Integration, ARM64EmptyRodataNoJumpTables) {
    std::vector<Capstone::Instruction> insns;
    insns.push_back(makeSyntheticInsn(0x401000, 4, "nop", ""));

    auto sections = makeTestSections();
    SectionLookup lookup(sections);
    Segmentator::NativeSymbolTable symbols;

    auto refs = analyzePatterns<Segmentator::ARM64ArchTraits>(
        insns, 0x401000, 0x1000, symbols, lookup, {});

    EXPECT_TRUE(refs.empty());
}

TEST(PatternLayer_OutOfRegion, SkipsInstructions) {
    // Instruction outside the region should be ignored
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);
    std::vector<uint8_t> code = {0x64, 0x8b, 0x04, 0x25,
                                  0x28, 0x00, 0x00, 0x00};
    auto insns = cs.disasm(code, 0x501000);  // Outside 0x401000-0x402000
    ASSERT_GE(insns.size(), 1u);

    auto sections = makeTestSections();
    SectionLookup lookup(sections);
    Segmentator::NativeSymbolTable symbols;

    auto refs = analyzePatterns<Segmentator::X86ArchTraits>(
        insns, 0x401000, 0x1000, symbols, lookup, {});

    EXPECT_TRUE(refs.empty())
        << "Instructions outside region should not produce refs";
}
