#include <gtest/gtest.h>

#include "InsnLayer.hpp"
#include "SectionLookup.hpp"
#include "AtomicTraits.hpp"

#include <DataReference.hpp>
#include <NativeSymbolTable.hpp>
#include <Section.hpp>
#include <capstone.hpp>

#include <capstone/arm64.h>
#include <capstone/x86.h>

#include <string>
#include <vector>

using namespace VMPilot::SDK;
using namespace VMPilot::SDK::Core;
using namespace VMPilot::SDK::ReferenceAnalyzer;

// ---- Test Helpers ----

namespace {

/// Standard section layout for tests.
std::vector<Section> makeTestSections() {
    return {
        {0x400000, 0x1000, SectionKind::Text, ".text", {}},
        {0x401000, 0x100, SectionKind::Rodata, ".rodata", {}},
        {0x402000, 0x100, SectionKind::Data, ".data", {}},
        {0x403000, 0x100, SectionKind::Bss, ".bss", {}},
        {0x404000, 0x100, SectionKind::Got, ".got", {}},
    };
}

/// Build an empty symbol table.
Segmentator::NativeSymbolTable emptySymbols() { return {}; }

/// Build a symbol table with one entry.
Segmentator::NativeSymbolTable makeSymbols(
    const std::string& name, uint64_t addr) {
    Segmentator::NativeSymbolTable syms;
    Segmentator::NativeSymbolTableEntry entry;
    entry.name = name;
    entry.address = addr;
    entry.size = 8;
    entry.type = Segmentator::SymbolType::OBJECT;
    entry.isGlobal = true;
    syms.push_back(entry);
    return syms;
}

/// Create a synthetic x86 MOV instruction with a RIP-relative memory operand.
/// mov rax, [rip + disp]
/// insn.address = addr, insn.size = size
/// target = addr + size + disp
Capstone::Instruction makeX86RipRelativeMov(uint64_t addr, uint16_t size,
                                             int64_t disp) {
    Capstone::Instruction insn;
    insn.id = X86_INS_MOV;
    insn.address = addr;
    insn.size = size;
    insn.mnemonic = "mov";
    insn.op_str = "rax, [rip + disp]";

    Capstone::Operand dst;
    dst.type = Capstone::OpType::REG;
    dst.reg = X86_REG_RAX;
    dst.size = 8;
    insn.operands.push_back(dst);

    Capstone::Operand src;
    src.type = Capstone::OpType::MEM;
    src.mem.base = X86_REG_RIP;
    src.mem.index = 0;
    src.mem.scale = 0;
    src.mem.disp = disp;
    src.size = 8;
    insn.operands.push_back(src);

    return insn;
}

/// Create a synthetic x86 LEA instruction with a RIP-relative memory operand.
/// lea rax, [rip + disp]
Capstone::Instruction makeX86RipRelativeLea(uint64_t addr, uint16_t size,
                                             int64_t disp) {
    Capstone::Instruction insn;
    insn.id = X86_INS_LEA;
    insn.address = addr;
    insn.size = size;
    insn.mnemonic = "lea";
    insn.op_str = "rax, [rip + disp]";

    Capstone::Operand dst;
    dst.type = Capstone::OpType::REG;
    dst.reg = X86_REG_RAX;
    dst.size = 8;
    insn.operands.push_back(dst);

    Capstone::Operand src;
    src.type = Capstone::OpType::MEM;
    src.mem.base = X86_REG_RIP;
    src.mem.index = 0;
    src.mem.scale = 0;
    src.mem.disp = disp;
    src.size = 8;
    insn.operands.push_back(src);

    return insn;
}

/// Create a synthetic x86 MOV instruction with a stack-relative operand.
/// mov rax, [rsp + offset]
Capstone::Instruction makeX86StackRelativeMov(uint64_t addr, int64_t offset) {
    Capstone::Instruction insn;
    insn.id = X86_INS_MOV;
    insn.address = addr;
    insn.size = 7;
    insn.mnemonic = "mov";
    insn.op_str = "rax, [rsp + offset]";

    Capstone::Operand dst;
    dst.type = Capstone::OpType::REG;
    dst.reg = X86_REG_RAX;
    dst.size = 8;
    insn.operands.push_back(dst);

    Capstone::Operand src;
    src.type = Capstone::OpType::MEM;
    src.mem.base = X86_REG_RSP;
    src.mem.index = 0;
    src.mem.scale = 0;
    src.mem.disp = offset;
    src.size = 8;
    insn.operands.push_back(src);

    return insn;
}

/// Create a synthetic x86 MFENCE instruction.
Capstone::Instruction makeX86Mfence(uint64_t addr) {
    Capstone::Instruction insn;
    insn.id = X86_INS_MFENCE;
    insn.address = addr;
    insn.size = 3;
    insn.mnemonic = "mfence";
    insn.op_str = "";
    // No operands for fence instructions
    return insn;
}

/// Create a synthetic ARM64 DMB instruction.
Capstone::Instruction makeARM64Dmb(uint64_t addr) {
    Capstone::Instruction insn;
    insn.id = ARM64_INS_DMB;
    insn.address = addr;
    insn.size = 4;
    insn.mnemonic = "dmb";
    insn.op_str = "ish";
    // DMB has an IMM operand for barrier type, not MEM
    Capstone::Operand op;
    op.type = Capstone::OpType::IMM;
    op.imm = 0xb;  // ISH
    insn.operands.push_back(op);
    return insn;
}

}  // namespace

// ---- Test Cases ----

/// x86 RIP-relative MOV targeting .rodata -> ReadOnlyData ref
TEST(InsnLayer, X86RipRelativeToRodata) {
    auto sections_vec = makeTestSections();
    SectionLookup sections(sections_vec);
    auto symbols = emptySymbols();

    // insn at 0x400100, size=7, disp such that target = 0x401050 (.rodata)
    // target = 0x400100 + 7 + disp => disp = 0x401050 - 0x400107 = 0xF49
    auto insn = makeX86RipRelativeMov(0x400100, 7, 0xF49);
    std::vector<Capstone::Instruction> insns = {insn};

    auto refs = analyzeInsns<X86InsnTraits>(
        insns, 0x400000, 0x1000, sections, symbols);

    ASSERT_EQ(refs.size(), 1u);
    EXPECT_EQ(refs[0].insn_offset, 0x400100u);
    EXPECT_EQ(refs[0].target_va, 0x401050u);
    EXPECT_EQ(refs[0].kind, DataRefKind::ReadOnlyData);
    EXPECT_EQ(refs[0].source, DataRefSource::InsnAnalysis);
    EXPECT_EQ(refs[0].access_size, 8u);
    EXPECT_TRUE(refs[0].is_pc_relative);
    EXPECT_FALSE(refs[0].is_write);
    EXPECT_FALSE(refs[0].is_mutable_target)
        << ".rodata target must NOT be marked mutable";
}

/// x86 RIP-relative MOV targeting .data -> GlobalVar ref
TEST(InsnLayer, X86RipRelativeToData) {
    auto sections_vec = makeTestSections();
    SectionLookup sections(sections_vec);
    auto symbols = emptySymbols();

    // target = 0x402050 (.data), insn at 0x400200, size=7
    // disp = 0x402050 - 0x400207 = 0x1E49
    auto insn = makeX86RipRelativeMov(0x400200, 7, 0x1E49);
    std::vector<Capstone::Instruction> insns = {insn};

    auto refs = analyzeInsns<X86InsnTraits>(
        insns, 0x400000, 0x1000, sections, symbols);

    ASSERT_EQ(refs.size(), 1u);
    EXPECT_EQ(refs[0].target_va, 0x402050u);
    EXPECT_EQ(refs[0].kind, DataRefKind::GlobalVar);
    EXPECT_TRUE(refs[0].is_pc_relative);
    EXPECT_TRUE(refs[0].is_mutable_target)
        << ".data target must be marked mutable for backend";
}

/// x86 LEA with RIP-relative -> skipped (address computation, not memory access)
TEST(InsnLayer, X86LEASkipped) {
    auto sections_vec = makeTestSections();
    SectionLookup sections(sections_vec);
    auto symbols = emptySymbols();

    // LEA targeting .rodata: should be skipped
    // target = 0x401050 (.rodata), insn at 0x400100, size=7
    auto insn = makeX86RipRelativeLea(0x400100, 7, 0xF49);
    std::vector<Capstone::Instruction> insns = {insn};

    auto refs = analyzeInsns<X86InsnTraits>(
        insns, 0x400000, 0x1000, sections, symbols);

    EXPECT_EQ(refs.size(), 0u);
}

/// x86 stack-relative MOV -> skipped (stack access)
TEST(InsnLayer, X86StackRelativeSkipped) {
    auto sections_vec = makeTestSections();
    SectionLookup sections(sections_vec);
    auto symbols = emptySymbols();

    auto insn = makeX86StackRelativeMov(0x400100, 8);
    std::vector<Capstone::Instruction> insns = {insn};

    auto refs = analyzeInsns<X86InsnTraits>(
        insns, 0x400000, 0x1000, sections, symbols);

    EXPECT_EQ(refs.size(), 0u);
}

/// x86 MFENCE -> fence DataReference with atomic_op=Fence
TEST(InsnLayer, X86FenceDetected) {
    auto sections_vec = makeTestSections();
    SectionLookup sections(sections_vec);
    auto symbols = emptySymbols();

    auto insn = makeX86Mfence(0x400300);
    std::vector<Capstone::Instruction> insns = {insn};

    auto refs = analyzeInsns<X86InsnTraits>(
        insns, 0x400000, 0x1000, sections, symbols);

    ASSERT_EQ(refs.size(), 1u);
    EXPECT_EQ(refs[0].insn_offset, 0x400300u);
    EXPECT_EQ(refs[0].target_va, 0u);
    EXPECT_EQ(refs[0].kind, DataRefKind::Unknown);
    EXPECT_EQ(refs[0].atomic_op, AtomicOp::Fence);
    EXPECT_EQ(refs[0].atomic_ordering, AtomicOrdering::AcqRel);
    EXPECT_EQ(refs[0].atomic_width, AtomicWidth::None);
}

/// ARM64 DMB -> fence DataReference with atomic_op=Fence
TEST(InsnLayer, ARM64FenceDetected) {
    auto sections_vec = makeTestSections();
    SectionLookup sections(sections_vec);
    auto symbols = emptySymbols();

    auto insn = makeARM64Dmb(0x400100);
    std::vector<Capstone::Instruction> insns = {insn};

    auto refs = analyzeInsns<ARM64InsnTraits>(
        insns, 0x400000, 0x1000, sections, symbols);

    ASSERT_EQ(refs.size(), 1u);
    EXPECT_EQ(refs[0].insn_offset, 0x400100u);
    EXPECT_EQ(refs[0].target_va, 0u);
    EXPECT_EQ(refs[0].kind, DataRefKind::Unknown);
    EXPECT_EQ(refs[0].atomic_op, AtomicOp::Fence);
    EXPECT_EQ(refs[0].atomic_ordering, AtomicOrdering::AcqRel);
    EXPECT_EQ(refs[0].atomic_width, AtomicWidth::None);
}

/// Empty instruction list -> no refs
TEST(InsnLayer, EmptyInsns) {
    auto sections_vec = makeTestSections();
    SectionLookup sections(sections_vec);
    auto symbols = emptySymbols();

    std::vector<Capstone::Instruction> insns;

    auto refs = analyzeInsns<X86InsnTraits>(
        insns, 0x400000, 0x1000, sections, symbols);

    EXPECT_EQ(refs.size(), 0u);
}

/// Instruction outside region -> skipped
TEST(InsnLayer, OutOfRegion) {
    auto sections_vec = makeTestSections();
    SectionLookup sections(sections_vec);
    auto symbols = emptySymbols();

    // Instruction at 0x500000, but region is 0x400000..0x401000
    auto insn = makeX86RipRelativeMov(0x500000, 7, 0xF49);
    std::vector<Capstone::Instruction> insns = {insn};

    auto refs = analyzeInsns<X86InsnTraits>(
        insns, 0x400000, 0x1000, sections, symbols);

    EXPECT_EQ(refs.size(), 0u);
}

/// Symbol lookup: target matches symbol -> target_symbol populated
TEST(InsnLayer, SymbolLookup) {
    auto sections_vec = makeTestSections();
    SectionLookup sections(sections_vec);

    // Symbol at 0x402050 in .data
    auto symbols = makeSymbols("my_global", 0x402050);

    // MOV targeting 0x402050
    // target = insn.address + insn.size + disp = 0x400200 + 7 + 0x1E49
    auto insn = makeX86RipRelativeMov(0x400200, 7, 0x1E49);
    std::vector<Capstone::Instruction> insns = {insn};

    auto refs = analyzeInsns<X86InsnTraits>(
        insns, 0x400000, 0x1000, sections, symbols);

    ASSERT_EQ(refs.size(), 1u);
    EXPECT_EQ(refs[0].target_symbol, "my_global");
    EXPECT_EQ(refs[0].target_va, 0x402050u);
    EXPECT_EQ(refs[0].kind, DataRefKind::GlobalVar);
}

/// x86 RIP-relative targeting .text -> skipped (code, not data)
TEST(InsnLayer, X86TextSectionSkipped) {
    auto sections_vec = makeTestSections();
    SectionLookup sections(sections_vec);
    auto symbols = emptySymbols();

    // target = 0x400500 (in .text 0x400000..0x401000)
    // disp = 0x400500 - 0x400107 = 0x3F9
    auto insn = makeX86RipRelativeMov(0x400100, 7, 0x3F9);
    std::vector<Capstone::Instruction> insns = {insn};

    auto refs = analyzeInsns<X86InsnTraits>(
        insns, 0x400000, 0x1000, sections, symbols);

    EXPECT_EQ(refs.size(), 0u);
}

/// classifyBySection helper covers all section kinds
TEST(InsnLayer, ClassifyBySectionMapping) {
    EXPECT_EQ(classifyBySection(SectionKind::Data), DataRefKind::GlobalVar);
    EXPECT_EQ(classifyBySection(SectionKind::Bss), DataRefKind::GlobalVar);
    EXPECT_EQ(classifyBySection(SectionKind::Rodata),
              DataRefKind::ReadOnlyData);
    EXPECT_EQ(classifyBySection(SectionKind::Got), DataRefKind::GotLoad);
    EXPECT_EQ(classifyBySection(SectionKind::Tls), DataRefKind::TlsVar);
    EXPECT_EQ(classifyBySection(SectionKind::Text), DataRefKind::Unknown);
    EXPECT_EQ(classifyBySection(SectionKind::Plt), DataRefKind::Unknown);
    EXPECT_EQ(classifyBySection(SectionKind::Unknown), DataRefKind::Unknown);
}
