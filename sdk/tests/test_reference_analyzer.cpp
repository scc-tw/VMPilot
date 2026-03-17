#include <gtest/gtest.h>

#include <DataReference.hpp>
#include <ReadOnlySection.hpp>
#include <ReferenceAnalyzer.hpp>
#include <SectionInfo.hpp>
#include <capstone.hpp>

// Internal headers for unit-testing individual layers
// We test through the public API primarily, with synthetic data
#include <NativeSymbolTable.hpp>

#include <cstring>

using namespace VMPilot::SDK;
using namespace VMPilot::SDK::Core;
using namespace VMPilot::SDK::ReferenceAnalyzer;

// ---- Test Helpers ----

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

}  // namespace

// ---- SectionInfo Tests ----

TEST(SectionInfo, ContainsBasic) {
    SectionInfo sec{0x1000, 0x100, SectionKind::Data, ".data"};
    EXPECT_TRUE(sec.contains(0x1000));
    EXPECT_TRUE(sec.contains(0x1050));
    EXPECT_TRUE(sec.contains(0x10FF));
    EXPECT_FALSE(sec.contains(0x1100));
    EXPECT_FALSE(sec.contains(0x0FFF));
}

TEST(SectionInfo, ContainsEmpty) {
    SectionInfo sec{0x1000, 0, SectionKind::Data, ".data"};
    EXPECT_FALSE(sec.contains(0x1000));
}

// ---- DataReference Enum Tests ----

TEST(DataReference, EnumDefaults) {
    DataReference ref;
    EXPECT_EQ(ref.kind, DataRefKind::Unknown);
    EXPECT_EQ(ref.tls_model, TlsModel::None);
    EXPECT_EQ(ref.source, DataRefSource::InsnAnalysis);
    EXPECT_EQ(ref.atomic_width, AtomicWidth::None);
    EXPECT_FALSE(ref.is_write);
    EXPECT_FALSE(ref.is_pc_relative);
}

// ---- RelocationEntry Tests ----

TEST(RelocationEntry, Defaults) {
    RelocationEntry entry;
    EXPECT_EQ(entry.offset, 0u);
    EXPECT_EQ(entry.type, 0u);
    EXPECT_EQ(entry.symbol_index, 0u);
    EXPECT_EQ(entry.addend, 0);
    EXPECT_TRUE(entry.symbol_name.empty());
}

// ---- Layer 1: Relocation Analyzer Tests ----

TEST(RelocationAnalyzer, FilterToRegion) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401010, 2 /*R_X86_64_PC32*/, 1, -4, "some_global"},
        {0x401500, 2, 2, -4, "other_global"},  // in region
        {0x402000, 2, 3, 0, "outside"},         // outside .text region
    };

    // Region is 0x401400..0x401600
    auto refs = analyze({}, 0x401400, 0x200, sections, relocs, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    // Only the reloc at 0x401500 should be in the region
    bool found_in_region = false;
    for (const auto& ref : refs) {
        if (ref.source == DataRefSource::Relocation &&
            ref.insn_offset == 0x401500) {
            found_in_region = true;
        }
        // Should NOT find 0x401010 or 0x402000
        EXPECT_NE(ref.insn_offset, 0x401010u);
    }
    EXPECT_TRUE(found_in_region);
}

TEST(RelocationAnalyzer, TlsRelocationTypes) {
    auto sections = makeTestSections();

    // R_X86_64_TPOFF32 = 23
    std::vector<RelocationEntry> relocs = {
        {0x401050, 23 /*TPOFF32*/, 1, 0, "tls_var"},
    };

    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    bool found_tls = false;
    for (const auto& ref : refs) {
        if (ref.source == DataRefSource::Relocation &&
            ref.insn_offset == 0x401050) {
            found_tls = true;
            EXPECT_EQ(ref.kind, DataRefKind::TlsVar);
            EXPECT_EQ(ref.tls_model, TlsModel::LocalExec);
        }
    }
    EXPECT_TRUE(found_tls);
}

TEST(RelocationAnalyzer, GotRelocation) {
    auto sections = makeTestSections();

    // R_X86_64_GOTPCREL = 9
    std::vector<RelocationEntry> relocs = {
        {0x401100, 9, 1, -4, "extern_sym"},
    };

    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    bool found_got = false;
    for (const auto& ref : refs) {
        if (ref.source == DataRefSource::Relocation &&
            ref.insn_offset == 0x401100) {
            found_got = true;
            EXPECT_EQ(ref.kind, DataRefKind::GotLoad);
            EXPECT_TRUE(ref.is_pc_relative);
        }
    }
    EXPECT_TRUE(found_got);
}

TEST(RelocationAnalyzer, Plt32Skipped) {
    auto sections = makeTestSections();

    // R_X86_64_PLT32 = 4 — should be skipped (code call)
    std::vector<RelocationEntry> relocs = {
        {0x401200, 4, 1, -4, "some_func"},
    };

    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    for (const auto& ref : refs) {
        if (ref.source == DataRefSource::Relocation) {
            EXPECT_NE(ref.insn_offset, 0x401200u)
                << "PLT32 relocation should be skipped";
        }
    }
}

// ---- Layer 2: Instruction Analyzer Tests ----

TEST(InsnAnalyzer, RipRelativeToRodata) {
    auto sections = makeTestSections();

    // Build a synthetic x86-64 instruction: mov eax, [rip+0xFFA]
    // At address 0x401000, size 6 → target = 0x401000 + 6 + 0xFFA = 0x402000
    // (which is in .rodata)
    // We'll use capstone to disassemble real bytes for this.
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);
    // mov eax, dword ptr [rip+0x0FFA]
    // 8b 05 fa 0f 00 00
    std::vector<uint8_t> code = {0x8b, 0x05, 0xfa, 0x0f, 0x00, 0x00};
    auto insns = cs.disasm(code, 0x401000);

    ASSERT_GE(insns.size(), 1u);
    EXPECT_EQ(insns[0].address, 0x401000u);

    // Target: 0x401000 + 6 + 0x0FFA = 0x402000 (in .rodata)
    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    bool found_rodata = false;
    for (const auto& ref : refs) {
        if (ref.insn_offset == 0x401000 &&
            ref.source == DataRefSource::InsnAnalysis) {
            found_rodata = true;
            EXPECT_EQ(ref.target_va, 0x402000u);
            EXPECT_EQ(ref.kind, DataRefKind::ReadOnlyData);
            EXPECT_TRUE(ref.is_pc_relative);
        }
    }
    EXPECT_TRUE(found_rodata);
}

TEST(InsnAnalyzer, RipRelativeToData) {
    auto sections = makeTestSections();

    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);
    // mov eax, dword ptr [rip+0x1FFA]
    // 8b 05 fa 1f 00 00
    // Target: 0x401000 + 6 + 0x1FFA = 0x403000 (.data)
    std::vector<uint8_t> code = {0x8b, 0x05, 0xfa, 0x1f, 0x00, 0x00};
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 1u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    bool found_data = false;
    for (const auto& ref : refs) {
        if (ref.insn_offset == 0x401000 &&
            ref.source == DataRefSource::InsnAnalysis) {
            found_data = true;
            EXPECT_EQ(ref.target_va, 0x403000u);
            EXPECT_EQ(ref.kind, DataRefKind::GlobalVar);
        }
    }
    EXPECT_TRUE(found_data);
}

TEST(InsnAnalyzer, LEASkipped) {
    auto sections = makeTestSections();

    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);
    // lea rax, [rip+0x0FFA]
    // 48 8d 05 fa 0f 00 00
    // Target would be 0x402001 (.rodata) but LEA should be skipped
    std::vector<uint8_t> code = {0x48, 0x8d, 0x05, 0xfa, 0x0f, 0x00, 0x00};
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 1u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    // Should NOT find any InsnAnalysis ref for the LEA
    for (const auto& ref : refs) {
        if (ref.source == DataRefSource::InsnAnalysis) {
            FAIL() << "LEA should not produce InsnAnalysis data reference";
        }
    }
}

TEST(InsnAnalyzer, StackRelativeSkipped) {
    auto sections = makeTestSections();

    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);
    // mov dword ptr [rsp+8], eax
    // 89 44 24 08
    std::vector<uint8_t> code = {0x89, 0x44, 0x24, 0x08};
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 1u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    for (const auto& ref : refs) {
        if (ref.source == DataRefSource::InsnAnalysis) {
            FAIL() << "Stack-relative access should be skipped";
        }
    }
}

// ---- Layer 3: Pattern Analyzer Tests ----

TEST(PatternAnalyzer, FsSegmentTls) {
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);
    // mov eax, dword ptr fs:[0x28]  — typical TLS stack canary access
    // 64 8b 04 25 28 00 00 00
    std::vector<uint8_t> code = {0x64, 0x8b, 0x04, 0x25,
                                  0x28, 0x00, 0x00, 0x00};
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 1u);

    auto sections = makeTestSections();
    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    bool found_tls = false;
    for (const auto& ref : refs) {
        if (ref.kind == DataRefKind::TlsVar &&
            ref.source == DataRefSource::PatternMatch) {
            found_tls = true;
            EXPECT_EQ(ref.tls_model, TlsModel::LocalExec);
        }
    }
    EXPECT_TRUE(found_tls);
}

// ---- Merge Tests ----

TEST(Merge, L1OverridesL2) {
    auto sections = makeTestSections();

    // Create a relocation at the same offset as an instruction analysis
    // result. L1 should win.
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);
    // mov eax, dword ptr [rip+0x0FFA] → target = 0x402000 (.rodata)
    std::vector<uint8_t> code = {0x8b, 0x05, 0xfa, 0x0f, 0x00, 0x00};
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 1u);

    // R_X86_64_TPOFF32 at same offset — should override L2's ReadOnlyData
    std::vector<RelocationEntry> relocs = {
        {0x401000, 23 /*TPOFF32*/, 1, 0, "tls_var"},
    };

    auto refs = analyze(insns, 0x401000, 0x1000, sections, relocs, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    bool found = false;
    for (const auto& ref : refs) {
        if (ref.insn_offset == 0x401000) {
            found = true;
            // L1 (relocation) should win — TlsVar from TPOFF32
            EXPECT_EQ(ref.source, DataRefSource::Relocation);
            EXPECT_EQ(ref.kind, DataRefKind::TlsVar);
        }
    }
    EXPECT_TRUE(found);
}

TEST(Merge, ResultsSorted) {
    auto sections = makeTestSections();

    std::vector<RelocationEntry> relocs = {
        {0x401200, 2, 1, -4, "sym_b"},
        {0x401100, 2, 2, -4, "sym_a"},
    };

    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    // Results should be sorted by insn_offset
    for (size_t i = 1; i < refs.size(); ++i) {
        EXPECT_LE(refs[i - 1].insn_offset, refs[i].insn_offset)
            << "Results should be sorted by insn_offset";
    }
}

// ---- Empty Input Tests ----

TEST(ReferenceAnalyzer, EmptyInputs) {
    auto refs = analyze({}, 0, 0, {}, {}, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    EXPECT_TRUE(refs.empty());
}

TEST(ReferenceAnalyzer, NoSectionsProducesNoRefs) {
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);
    // nop
    std::vector<uint8_t> code = {0x90};
    auto insns = cs.disasm(code, 0x401000);

    auto refs = analyze(insns, 0x401000, 1, {}, {}, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    EXPECT_TRUE(refs.empty());
}

// ---- Jump Table Tests ----

namespace {

/// Helper: build a rodata section with 32-bit relative entries.
/// Each entry is a signed 32-bit offset from table_base to a target in .text.
Segmentator::ReadOnlySection makeRodataWithRelativeJumpTable(
    uint64_t table_base,
    const std::vector<uint64_t>& targets) {
    Segmentator::ReadOnlySection sec;
    sec.base_addr = table_base;
    sec.data.resize(targets.size() * 4);
    for (size_t i = 0; i < targets.size(); ++i) {
        int32_t offset =
            static_cast<int32_t>(static_cast<int64_t>(targets[i]) -
                                  static_cast<int64_t>(table_base));
        std::memcpy(sec.data.data() + i * 4, &offset, 4);
    }
    return sec;
}

}  // namespace

TEST(JumpTable, X86_64_JmpRegWithLea) {
    // Simulate x86-64 jump table pattern:
    //   0x401000: cmp eax, 3          ; 83 f8 03
    //   0x401003: ja  0x401020        ; 77 1b
    //   0x401005: lea rcx, [rip+0xFF4] ; 48 8d 0d f4 0f 00 00
    //                                  → table_base = 0x401005 + 7 + 0xFF4 = 0x402000
    //   0x40100C: movsxd rax, [rcx+rax*4] ; 48 63 04 81
    //   0x401010: add rax, rcx         ; 48 01 c8
    //   0x401012: jmp rax              ; ff e0

    auto sections = makeTestSections();

    // Jump table targets in .text
    std::vector<uint64_t> targets = {0x401100, 0x401200, 0x401300, 0x401400};
    auto rodata = makeRodataWithRelativeJumpTable(0x402000, targets);
    std::vector<Segmentator::ReadOnlySection> rodata_sections = {rodata};

    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // Assemble the instruction sequence
    std::vector<uint8_t> code = {
        0x83, 0xf8, 0x03,                          // cmp eax, 3
        0x77, 0x1b,                                  // ja +0x1b (relative)
        0x48, 0x8d, 0x0d, 0xf4, 0x0f, 0x00, 0x00,  // lea rcx, [rip+0xFF4]
        0x48, 0x63, 0x04, 0x81,                      // movsxd rax, [rcx+rax*4]
        0x48, 0x01, 0xc8,                            // add rax, rcx
        0xff, 0xe0,                                  // jmp rax
    };

    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 6u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        rodata_sections,
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    // Find the JumpTable ref
    const DataReference* jt_ref = nullptr;
    for (const auto& ref : refs) {
        if (ref.kind == DataRefKind::JumpTable) {
            jt_ref = &ref;
            break;
        }
    }
    ASSERT_NE(jt_ref, nullptr) << "Expected a JumpTable reference";
    ASSERT_TRUE(jt_ref->jump_table.has_value());

    const auto& jt = *jt_ref->jump_table;
    EXPECT_EQ(jt.table_base, 0x402000u);
    EXPECT_EQ(jt.entry_size, 4u);
    EXPECT_TRUE(jt.relative_entries);
    EXPECT_EQ(jt.entry_count, 4u);
    ASSERT_EQ(jt.targets.size(), 4u);
    EXPECT_EQ(jt.targets[0], 0x401100u);
    EXPECT_EQ(jt.targets[1], 0x401200u);
    EXPECT_EQ(jt.targets[2], 0x401300u);
    EXPECT_EQ(jt.targets[3], 0x401400u);
    EXPECT_EQ(jt_ref->source, DataRefSource::PatternMatch);
    EXPECT_TRUE(jt_ref->is_pc_relative);
}

TEST(JumpTable, X86_64_IndexedMemoryJmp) {
    // Simulate: jmp qword ptr [rip+TABLE+rax*8]
    // At 0x401000, size=7: ff 24 c5 <disp32>
    // Actually this is jmp [rax*8 + disp32] (SIB form)
    // For RIP-relative indexed, use: ff 24 05 <disp32> won't work easily.
    //
    // Use x86-32 pattern instead for indexed memory jump:
    //   0x401000: cmp eax, 2      ; 83 f8 02
    //   0x401003: ja 0x401020     ; 77 1b
    //   0x401005: jmp [eax*4 + 0x402000] ; ff 24 85 00 20 40 00
    auto sections = makeTestSections();

    std::vector<uint64_t> targets = {0x401100, 0x401200, 0x401300};

    // Build rodata with absolute 32-bit entries
    Segmentator::ReadOnlySection rodata_sec;
    rodata_sec.base_addr = 0x402000;
    rodata_sec.data.resize(targets.size() * 4);
    for (size_t i = 0; i < targets.size(); ++i) {
        uint32_t val = static_cast<uint32_t>(targets[i]);
        std::memcpy(rodata_sec.data.data() + i * 4, &val, 4);
    }
    std::vector<Segmentator::ReadOnlySection> rodata_sections = {rodata_sec};

    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_32);

    std::vector<uint8_t> code = {
        0x83, 0xf8, 0x02,                                // cmp eax, 2
        0x77, 0x07,                                        // ja +7
        0xff, 0x24, 0x85, 0x00, 0x20, 0x40, 0x00,        // jmp [eax*4+0x402000]
    };

    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 3u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        rodata_sections,
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_32);

    const DataReference* jt_ref = nullptr;
    for (const auto& ref : refs) {
        if (ref.kind == DataRefKind::JumpTable) {
            jt_ref = &ref;
            break;
        }
    }
    ASSERT_NE(jt_ref, nullptr) << "Expected a JumpTable reference for x86-32";
    ASSERT_TRUE(jt_ref->jump_table.has_value());

    const auto& jt = *jt_ref->jump_table;
    EXPECT_EQ(jt.table_base, 0x402000u);
    EXPECT_EQ(jt.entry_size, 4u);
    EXPECT_FALSE(jt.relative_entries);
    EXPECT_EQ(jt.entry_count, 3u);
    ASSERT_EQ(jt.targets.size(), 3u);
    EXPECT_EQ(jt.targets[0], 0x401100u);
    EXPECT_EQ(jt.targets[1], 0x401200u);
    EXPECT_EQ(jt.targets[2], 0x401300u);
}

TEST(JumpTable, InvalidEntryTruncatesTable) {
    // Build a rodata with 4 entries but 3rd entry points outside .text
    auto sections = makeTestSections();

    std::vector<uint64_t> targets_raw = {0x401100, 0x401200, 0x500000,
                                          0x401400};
    auto rodata =
        makeRodataWithRelativeJumpTable(0x402000, targets_raw);
    std::vector<Segmentator::ReadOnlySection> rodata_sections = {rodata};

    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // jmp-reg pattern with LEA
    // lea at 0x401000, size=7 → next_addr = 0x401007
    // disp = 0x402000 - 0x401007 = 0xFF9
    std::vector<uint8_t> code = {
        0x48, 0x8d, 0x0d, 0xf9, 0x0f, 0x00, 0x00,  // lea rcx, [rip+0xFF9]
        0xff, 0xe0,                                  // jmp rax
    };

    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 2u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        rodata_sections,
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    const DataReference* jt_ref = nullptr;
    for (const auto& ref : refs) {
        if (ref.kind == DataRefKind::JumpTable) {
            jt_ref = &ref;
            break;
        }
    }
    ASSERT_NE(jt_ref, nullptr);
    ASSERT_TRUE(jt_ref->jump_table.has_value());

    // Should have 2 entries (3rd was invalid, truncated there)
    const auto& jt = *jt_ref->jump_table;
    EXPECT_EQ(jt.entry_count, 2u);
    ASSERT_EQ(jt.targets.size(), 2u);
    EXPECT_EQ(jt.targets[0], 0x401100u);
    EXPECT_EQ(jt.targets[1], 0x401200u);
}

TEST(JumpTable, BoundsCheckDetection) {
    // Verify that bounds check (cmp+ja) limits entry count
    auto sections = makeTestSections();

    // Create rodata with 10 valid entries but bounds check says 3
    std::vector<uint64_t> many_targets;
    for (int i = 0; i < 10; ++i)
        many_targets.push_back(0x401100 + i * 0x10);
    auto rodata = makeRodataWithRelativeJumpTable(0x402000, many_targets);
    std::vector<Segmentator::ReadOnlySection> rodata_sections = {rodata};

    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    std::vector<uint8_t> code = {
        0x83, 0xf8, 0x02,                              // cmp eax, 2
        0x77, 0x09,                                      // ja +9
        0x48, 0x8d, 0x0d, 0xf0, 0x0f, 0x00, 0x00,      // lea rcx, [rip+0xFF0]
        0xff, 0xe0,                                      // jmp rax
    };
    // lea at 0x401005, size=7, disp=0xFF0 → table = 0x401005+7+0xFF0 = 0x401FFC
    // Hmm, that doesn't give 0x402000. Let me recalculate.
    // lea at addr 0x401005, size=7 → next_addr = 0x40100C
    // target = 0x40100C + 0xFF0 = 0x401FFC — not 0x402000
    // Need disp = 0x402000 - 0x40100C = 0xFF4
    // Fix the code bytes

    code = {
        0x83, 0xf8, 0x02,                              // cmp eax, 2
        0x77, 0x09,                                      // ja +9
        0x48, 0x8d, 0x0d, 0xf4, 0x0f, 0x00, 0x00,      // lea rcx, [rip+0xFF4]
        0xff, 0xe0,                                      // jmp rax
    };

    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 4u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        rodata_sections,
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    const DataReference* jt_ref = nullptr;
    for (const auto& ref : refs) {
        if (ref.kind == DataRefKind::JumpTable) {
            jt_ref = &ref;
            break;
        }
    }
    ASSERT_NE(jt_ref, nullptr);
    ASSERT_TRUE(jt_ref->jump_table.has_value());

    // Bounds check: cmp eax, 2 + ja → entry_count = 3
    EXPECT_EQ(jt_ref->jump_table->entry_count, 3u);
    EXPECT_EQ(jt_ref->jump_table->targets.size(), 3u);
}

TEST(JumpTable, EntryResolutionWithRelativeOffsets) {
    // Directly test that relative offset resolution works correctly
    auto sections = makeTestSections();

    // targets are in .text at various locations
    std::vector<uint64_t> targets = {0x401050, 0x4010A0, 0x401800};
    auto rodata = makeRodataWithRelativeJumpTable(0x402000, targets);
    std::vector<Segmentator::ReadOnlySection> rodata_sections = {rodata};

    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // Simple jmp-reg + lea pattern (no bounds check → scan-based)
    std::vector<uint8_t> code = {
        0x48, 0x8d, 0x0d, 0xf9, 0x0f, 0x00, 0x00,  // lea rcx, [rip+0xFF9]
        0xff, 0xe0,                                  // jmp rax
    };
    // lea at 0x401000, size=7 → next_addr = 0x401007
    // target = 0x401007 + 0xFF9 = 0x402000 ✓

    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 2u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        rodata_sections,
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    const DataReference* jt_ref = nullptr;
    for (const auto& ref : refs) {
        if (ref.kind == DataRefKind::JumpTable) {
            jt_ref = &ref;
            break;
        }
    }
    ASSERT_NE(jt_ref, nullptr);
    ASSERT_TRUE(jt_ref->jump_table.has_value());

    const auto& jt = *jt_ref->jump_table;
    EXPECT_EQ(jt.entry_count, 3u);
    ASSERT_EQ(jt.targets.size(), 3u);
    EXPECT_EQ(jt.targets[0], 0x401050u);
    EXPECT_EQ(jt.targets[1], 0x4010A0u);
    EXPECT_EQ(jt.targets[2], 0x401800u);
}

TEST(JumpTable, JumpTableRefDefaults) {
    Core::JumpTableRef jt;
    EXPECT_EQ(jt.table_base, 0u);
    EXPECT_EQ(jt.entry_size, 0u);
    EXPECT_EQ(jt.entry_count, 0u);
    EXPECT_FALSE(jt.relative_entries);
    EXPECT_TRUE(jt.targets.empty());
}

TEST(JumpTable, DataReferenceJumpTableOptional) {
    DataReference ref;
    EXPECT_FALSE(ref.jump_table.has_value());

    ref.jump_table = Core::JumpTableRef{};
    ref.jump_table->table_base = 0x402000;
    EXPECT_TRUE(ref.jump_table.has_value());
    EXPECT_EQ(ref.jump_table->table_base, 0x402000u);
}
