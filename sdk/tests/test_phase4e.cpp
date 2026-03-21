#include <gtest/gtest.h>

#include "RefMerger.hpp"
#include "JumpTableResolver.hpp"
#include <AnalysisContext.hpp>

#include <DataReference.hpp>
#include <ReadOnlySection.hpp>
#include <SectionInfo.hpp>

#include <cstdint>
#include <vector>

using namespace VMPilot::SDK::ReferenceAnalyzer;
using namespace VMPilot::SDK::ReferenceAnalyzer::JumpTable;
using namespace VMPilot::SDK::Core;
using namespace VMPilot::SDK::Segmentator;

// ============================================================================
// RefMerger Tests
// ============================================================================

TEST(RefMerger, L1Wins) {
    DataReference l1_ref;
    l1_ref.insn_offset = 0x1000;
    l1_ref.kind = DataRefKind::GotLoad;
    l1_ref.source = DataRefSource::Relocation;

    DataReference l2_ref;
    l2_ref.insn_offset = 0x1000;
    l2_ref.kind = DataRefKind::ReadOnlyData;
    l2_ref.source = DataRefSource::InsnAnalysis;

    auto result = RefMerger::merge({l1_ref}, {l2_ref}, {});
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].kind, DataRefKind::GotLoad);
    EXPECT_EQ(result[0].source, DataRefSource::Relocation);
}

TEST(RefMerger, L2FillsGaps) {
    DataReference l1_ref;
    l1_ref.insn_offset = 0x1000;
    l1_ref.kind = DataRefKind::GotLoad;

    DataReference l2_ref;
    l2_ref.insn_offset = 0x2000;
    l2_ref.kind = DataRefKind::GlobalVar;

    auto result = RefMerger::merge({l1_ref}, {l2_ref}, {});
    ASSERT_EQ(result.size(), 2u);
    EXPECT_EQ(result[0].insn_offset, 0x1000u);
    EXPECT_EQ(result[1].insn_offset, 0x2000u);
    EXPECT_EQ(result[1].kind, DataRefKind::GlobalVar);
}

TEST(RefMerger, L3UpgradesTlsKind) {
    DataReference l2_ref;
    l2_ref.insn_offset = 0x1000;
    l2_ref.kind = DataRefKind::ReadOnlyData;

    DataReference l3_ref;
    l3_ref.insn_offset = 0x1000;
    l3_ref.kind = DataRefKind::TlsVar;
    l3_ref.tls_model = TlsModel::LocalExec;

    auto result = RefMerger::merge({}, {l2_ref}, {l3_ref});
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].kind, DataRefKind::TlsVar);
    EXPECT_EQ(result[0].tls_model, TlsModel::LocalExec);
}

TEST(RefMerger, L3UpgradesTlsModel) {
    DataReference l2_ref;
    l2_ref.insn_offset = 0x1000;
    l2_ref.kind = DataRefKind::TlsVar;
    l2_ref.tls_model = TlsModel::None;

    DataReference l3_ref;
    l3_ref.insn_offset = 0x1000;
    l3_ref.kind = DataRefKind::TlsVar;
    l3_ref.tls_model = TlsModel::GeneralDynamic;

    auto result = RefMerger::merge({}, {l2_ref}, {l3_ref});
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].tls_model, TlsModel::GeneralDynamic);
}

TEST(RefMerger, L3UpgradesJumpTable) {
    DataReference l2_ref;
    l2_ref.insn_offset = 0x1000;
    l2_ref.kind = DataRefKind::ReadOnlyData;

    DataReference l3_ref;
    l3_ref.insn_offset = 0x1000;
    l3_ref.kind = DataRefKind::JumpTable;
    l3_ref.jump_table = JumpTableRef{0x402000, 4, 8, true, {0x1000, 0x1010}};

    auto result = RefMerger::merge({}, {l2_ref}, {l3_ref});
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].kind, DataRefKind::JumpTable);
    ASSERT_TRUE(result[0].jump_table.has_value());
    EXPECT_EQ(result[0].jump_table->entry_count, 8u);
}

TEST(RefMerger, L3InsertsNew) {
    DataReference l3_ref;
    l3_ref.insn_offset = 0x3000;
    l3_ref.kind = DataRefKind::TlsVar;
    l3_ref.tls_model = TlsModel::LocalExec;

    auto result = RefMerger::merge({}, {}, {l3_ref});
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].insn_offset, 0x3000u);
}

TEST(RefMerger, ResultsSortedByInsnOffset) {
    DataReference r1, r2, r3;
    r1.insn_offset = 0x3000;
    r2.insn_offset = 0x1000;
    r3.insn_offset = 0x2000;

    auto result = RefMerger::merge({r1}, {r2}, {r3});
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0].insn_offset, 0x1000u);
    EXPECT_EQ(result[1].insn_offset, 0x2000u);
    EXPECT_EQ(result[2].insn_offset, 0x3000u);
}

TEST(RefMerger, EmptyInputs) {
    auto result = RefMerger::merge({}, {}, {});
    EXPECT_TRUE(result.empty());
}

// ============================================================================
// JumpTableResolver Tests
// ============================================================================

TEST(JumpTableResolver, Absolute8ByteEntries) {
    // 3 absolute 8-byte entries pointing into .text
    uint8_t rodata[] = {
        0x00, 0x10, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,  // 0x401000
        0x10, 0x10, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,  // 0x401010
        0x20, 0x10, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,  // 0x401020
    };
    ReadOnlySection sec;
    sec.base_addr = 0x402000;
    sec.data.assign(rodata, rodata + sizeof(rodata));

    std::vector<SectionInfo> sections = {
        {0x401000, 0x1000, SectionKind::Text, ".text"},
        {0x402000, 0x100, SectionKind::Rodata, ".rodata"},
    };
    SectionLookup lookup(sections);

    JumpTableLayout layout;
    layout.table_base = 0x402000;
    layout.entry_size = 8;
    layout.relative = false;
    layout.max_entries = 1024;

    auto jt = resolveEntries(layout, {sec}, lookup);
    ASSERT_EQ(jt.targets.size(), 3u);
    EXPECT_EQ(jt.targets[0], 0x401000u);
    EXPECT_EQ(jt.targets[1], 0x401010u);
    EXPECT_EQ(jt.targets[2], 0x401020u);
    EXPECT_EQ(jt.entry_count, 3u);
    EXPECT_EQ(jt.entry_size, 8u);
    EXPECT_FALSE(jt.relative_entries);
}

TEST(JumpTableResolver, Relative4ByteEntries) {
    // Relative entries: target = table_base + sext(entry)
    // Entry 0: offset 0x100 → target = 0x402000 + 0x100 = 0x402100 (wrong, not .text)
    // Let's use offsets that point back to .text:
    // table_base = 0x402000, .text = 0x401000
    // offset = 0x401000 - 0x402000 = -0x1000 = 0xFFFFF000 as int32_t
    int32_t offset = -0x1000;
    uint32_t raw = static_cast<uint32_t>(offset);

    uint8_t rodata[4];
    std::memcpy(rodata, &raw, 4);

    ReadOnlySection sec;
    sec.base_addr = 0x402000;
    sec.data.assign(rodata, rodata + 4);

    std::vector<SectionInfo> sections = {
        {0x401000, 0x1000, SectionKind::Text, ".text"},
        {0x402000, 0x100, SectionKind::Rodata, ".rodata"},
    };
    SectionLookup lookup(sections);

    JumpTableLayout layout;
    layout.table_base = 0x402000;
    layout.entry_size = 4;
    layout.relative = true;
    layout.resolve_base = 0x402000;
    layout.max_entries = 1024;

    auto jt = resolveEntries(layout, {sec}, lookup);
    ASSERT_EQ(jt.targets.size(), 1u);
    EXPECT_EQ(jt.targets[0], 0x401000u);
    EXPECT_TRUE(jt.relative_entries);
}

TEST(JumpTableResolver, ByteOffsetWithShift) {
    // Apple Clang byte-offset: target = case_base + (entry << shift)
    // case_base = 0x401000, shift = 2
    // entry[0] = 0 → target = 0x401000
    // entry[1] = 4 → target = 0x401000 + (4 << 2) = 0x401010
    uint8_t rodata[] = {0x00, 0x04};

    ReadOnlySection sec;
    sec.base_addr = 0x402000;
    sec.data.assign(rodata, rodata + 2);

    std::vector<SectionInfo> sections = {
        {0x401000, 0x1000, SectionKind::Text, ".text"},
        {0x402000, 0x100, SectionKind::Rodata, ".rodata"},
    };
    SectionLookup lookup(sections);

    JumpTableLayout layout;
    layout.table_base = 0x402000;
    layout.entry_size = 1;
    layout.relative = true;  // byte-offset is relative
    layout.resolve_base = 0x401000;  // case_base
    layout.shift = 2;
    layout.max_entries = 1024;

    auto jt = resolveEntries(layout, {sec}, lookup);
    ASSERT_EQ(jt.targets.size(), 2u);
    EXPECT_EQ(jt.targets[0], 0x401000u);
    EXPECT_EQ(jt.targets[1], 0x401010u);
}

TEST(JumpTableResolver, InvalidEntryTruncates) {
    // Entry pointing outside .text → stops scanning
    uint8_t rodata[] = {
        0x00, 0x10, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,  // 0x401000 valid
        0x00, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00,  // 0x500000 invalid
    };
    ReadOnlySection sec;
    sec.base_addr = 0x402000;
    sec.data.assign(rodata, rodata + sizeof(rodata));

    std::vector<SectionInfo> sections = {
        {0x401000, 0x1000, SectionKind::Text, ".text"},
        {0x402000, 0x100, SectionKind::Rodata, ".rodata"},
    };
    SectionLookup lookup(sections);

    JumpTableLayout layout;
    layout.table_base = 0x402000;
    layout.entry_size = 8;
    layout.relative = false;
    layout.max_entries = 1024;

    auto jt = resolveEntries(layout, {sec}, lookup);
    ASSERT_EQ(jt.targets.size(), 1u);  // Only first entry valid
}

TEST(JumpTableResolver, MaxEntriesLimit) {
    // Max entries = 2, even though more valid entries exist
    uint8_t rodata[] = {
        0x00, 0x10, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0x10, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x20, 0x10, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    ReadOnlySection sec;
    sec.base_addr = 0x402000;
    sec.data.assign(rodata, rodata + sizeof(rodata));

    std::vector<SectionInfo> sections = {
        {0x401000, 0x1000, SectionKind::Text, ".text"},
        {0x402000, 0x100, SectionKind::Rodata, ".rodata"},
    };
    SectionLookup lookup(sections);

    JumpTableLayout layout;
    layout.table_base = 0x402000;
    layout.entry_size = 8;
    layout.relative = false;
    layout.max_entries = 2;

    auto jt = resolveEntries(layout, {sec}, lookup);
    ASSERT_EQ(jt.targets.size(), 2u);
}

// ============================================================================
// AnalysisContext Tests
// ============================================================================

TEST(AnalysisContext, BuildPopulatesSymbolLookup) {
    NativeSymbolTable symbols;
    NativeSymbolTableEntry sym;
    sym.name = "my_func";
    sym.address = 0x401000;
    sym.type = SymbolType::FUNC;
    symbols.push_back(sym);

    auto ctx = AnalysisContext::build(
        {{0x401000, 0x1000, SectionKind::Text, ".text"}},
        {},
        std::move(symbols),
        {},
        Arch::X86,
        Mode::MODE_64);

    EXPECT_EQ(ctx.arch, Arch::X86);
    EXPECT_EQ(ctx.mode, Mode::MODE_64);
    EXPECT_EQ(ctx.sections.size(), 1u);
    EXPECT_EQ(ctx.symbols.size(), 1u);

    auto it = ctx.symbol_lookup.find(0x401000);
    ASSERT_NE(it, ctx.symbol_lookup.end());
    EXPECT_EQ(it->second, "my_func");
}

TEST(AnalysisContext, EmptySymbolsSkipped) {
    NativeSymbolTable symbols;
    NativeSymbolTableEntry empty_sym;
    empty_sym.name = "";
    empty_sym.address = 0x401000;
    symbols.push_back(empty_sym);

    NativeSymbolTableEntry zero_addr;
    zero_addr.name = "zero";
    zero_addr.address = 0;
    symbols.push_back(zero_addr);

    auto ctx = AnalysisContext::build({}, {}, std::move(symbols), {},
                                      Arch::ARM64, Mode::MODE_64);

    EXPECT_TRUE(ctx.symbol_lookup.empty());
}
