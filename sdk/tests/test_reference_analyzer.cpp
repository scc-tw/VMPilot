#include <gtest/gtest.h>

#include <DataReference.hpp>
#include <ReferenceAnalyzer.hpp>
#include <Section.hpp>
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

std::vector<Section> makeTestSections() {
    return {
        {0x401000, 0x1000, SectionKind::Text, ".text", {}},
        {0x402000, 0x200, SectionKind::Rodata, ".rodata", {}},
        {0x403000, 0x200, SectionKind::Data, ".data", {}},
        {0x404000, 0x100, SectionKind::Bss, ".bss", {}},
        {0x405000, 0x100, SectionKind::Tls, ".tdata", {}},
        {0x406000, 0x100, SectionKind::Got, ".got", {}},
        {0x407000, 0x100, SectionKind::Plt, ".plt", {}},
    };
}

}  // namespace

// ---- SectionInfo Tests ----

TEST(SectionInfo, ContainsBasic) {
    Section sec{0x1000, 0x100, SectionKind::Data, ".data", {}};
    EXPECT_TRUE(sec.contains(0x1000));
    EXPECT_TRUE(sec.contains(0x1050));
    EXPECT_TRUE(sec.contains(0x10FF));
    EXPECT_FALSE(sec.contains(0x1100));
    EXPECT_FALSE(sec.contains(0x0FFF));
}

TEST(SectionInfo, ContainsEmpty) {
    Section sec{0x1000, 0, SectionKind::Data, ".data", {}};
    EXPECT_FALSE(sec.contains(0x1000));
}

// ---- DataReference Enum Tests ----

TEST(DataReference, EnumDefaults) {
    DataReference ref;
    EXPECT_EQ(ref.kind, DataRefKind::Unknown);
    EXPECT_EQ(ref.tls_model, TlsModel::None);
    EXPECT_EQ(ref.source, DataRefSource::InsnAnalysis);
    EXPECT_EQ(ref.atomic_width, AtomicWidth::None);
    EXPECT_EQ(ref.atomic_ordering, AtomicOrdering::None);
    EXPECT_EQ(ref.atomic_op, AtomicOp::None);
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
    auto refs = analyze({}, 0x401400, 0x200, sections, relocs, {},
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

    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
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

    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
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

    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
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
    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
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

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
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

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
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

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
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
    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
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

    auto refs = analyze(insns, 0x401000, 0x1000, sections, relocs, {},
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

    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
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
    auto refs = analyze({}, 0, 0, {}, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    EXPECT_TRUE(refs.empty());
}

TEST(ReferenceAnalyzer, NoSectionsProducesNoRefs) {
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);
    // nop
    std::vector<uint8_t> code = {0x90};
    auto insns = cs.disasm(code, 0x401000);

    auto refs = analyze(insns, 0x401000, 1, {}, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    EXPECT_TRUE(refs.empty());
}

// ---- Jump Table Tests ----

namespace {

/// Helper: build a rodata section with 32-bit relative entries.
/// Each entry is a signed 32-bit offset from table_base to a target in .text.
Section makeRodataWithRelativeJumpTable(
    uint64_t table_base,
    const std::vector<uint64_t>& targets) {
    Section sec;
    sec.base_addr = table_base;
    sec.kind = SectionKind::Rodata;
    sec.name = ".rodata";
    sec.data.resize(targets.size() * 4);
    for (size_t i = 0; i < targets.size(); ++i) {
        int32_t offset =
            static_cast<int32_t>(static_cast<int64_t>(targets[i]) -
                                  static_cast<int64_t>(table_base));
        std::memcpy(sec.data.data() + i * 4, &offset, 4);
    }
    sec.size = sec.data.size();
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
    std::vector<Section> rodata_sections = {rodata};

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

    // Merge rodata data into sections for unified API
    for (auto& rs : rodata_sections)
        sections.push_back(rs);
    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
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
    Section rodata_sec;
    rodata_sec.base_addr = 0x402000;
    rodata_sec.kind = SectionKind::Rodata;
    rodata_sec.name = ".rodata";
    rodata_sec.data.resize(targets.size() * 4);
    for (size_t i = 0; i < targets.size(); ++i) {
        uint32_t val = static_cast<uint32_t>(targets[i]);
        std::memcpy(rodata_sec.data.data() + i * 4, &val, 4);
    }
    rodata_sec.size = rodata_sec.data.size();
    std::vector<Section> rodata_sections = {rodata_sec};

    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_32);

    std::vector<uint8_t> code = {
        0x83, 0xf8, 0x02,                                // cmp eax, 2
        0x77, 0x07,                                        // ja +7
        0xff, 0x24, 0x85, 0x00, 0x20, 0x40, 0x00,        // jmp [eax*4+0x402000]
    };

    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 3u);

    // Merge rodata data into sections for unified API
    for (auto& rs : rodata_sections)
        sections.push_back(rs);
    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
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
    std::vector<Section> rodata_sections = {rodata};

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

    // Merge rodata data into sections for unified API
    for (auto& rs : rodata_sections)
        sections.push_back(rs);
    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
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
    std::vector<Section> rodata_sections = {rodata};

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

    // Merge rodata data into sections for unified API
    for (auto& rs : rodata_sections)
        sections.push_back(rs);
    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
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
    std::vector<Section> rodata_sections = {rodata};

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

    // Merge rodata data into sections for unified API
    for (auto& rs : rodata_sections)
        sections.push_back(rs);
    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
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

// ====================================================================
// Real-World Scenario Tests
//
// Each test simulates a pattern from actual compiler output (GCC/Clang
// -O2 x86-64 or ARM64).  The byte sequences were obtained from real
// toolchain output and verified with capstone.
// ====================================================================

// ---- Layer 1: Relocation Type Coverage ----
//
// Real ELF binaries contain many relocation types.  Each test below
// exercises a specific type the way the linker would emit it.

TEST(RelocationAnalyzer, TlsInitialExec) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401050, 22 /*R_X86_64_GOTTPOFF*/, 1, -4, "tls_ie_var"},
    };
    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    bool found = false;
    for (const auto& ref : refs) {
        if (ref.insn_offset == 0x401050) {
            found = true;
            EXPECT_EQ(ref.kind, DataRefKind::TlsVar);
            EXPECT_EQ(ref.tls_model, TlsModel::InitialExec);
            EXPECT_TRUE(ref.is_pc_relative);
        }
    }
    EXPECT_TRUE(found);
}

TEST(RelocationAnalyzer, TlsGeneralDynamic) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401060, 19 /*R_X86_64_TLSGD*/, 1, 0, "tls_gd_var"},
    };
    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    bool found = false;
    for (const auto& ref : refs) {
        if (ref.insn_offset == 0x401060) {
            found = true;
            EXPECT_EQ(ref.kind, DataRefKind::TlsVar);
            EXPECT_EQ(ref.tls_model, TlsModel::GeneralDynamic);
        }
    }
    EXPECT_TRUE(found);
}

TEST(RelocationAnalyzer, TlsLocalDynamic) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401070, 20 /*R_X86_64_TLSLD*/, 1, 0, "tls_ld_var"},
    };
    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    bool found = false;
    for (const auto& ref : refs) {
        if (ref.insn_offset == 0x401070) {
            found = true;
            EXPECT_EQ(ref.kind, DataRefKind::TlsVar);
            EXPECT_EQ(ref.tls_model, TlsModel::LocalDynamic);
        }
    }
    EXPECT_TRUE(found);
}

TEST(RelocationAnalyzer, Tpoff64) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401080, 18 /*R_X86_64_TPOFF64*/, 1, 0, "tls64"},
    };
    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    bool found = false;
    for (const auto& ref : refs) {
        if (ref.insn_offset == 0x401080) {
            found = true;
            EXPECT_EQ(ref.kind, DataRefKind::TlsVar);
            EXPECT_EQ(ref.tls_model, TlsModel::LocalExec);
        }
    }
    EXPECT_TRUE(found);
}

TEST(RelocationAnalyzer, GotpcrelxVariants) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401090, 41 /*GOTPCRELX*/, 1, -4, "sym_a"},
        {0x4010A0, 42 /*REX_GOTPCRELX*/, 2, -4, "sym_b"},
    };
    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    int got_count = 0;
    for (const auto& ref : refs) {
        if (ref.kind == DataRefKind::GotLoad &&
            ref.source == DataRefSource::Relocation) {
            ++got_count;
            EXPECT_TRUE(ref.is_pc_relative);
        }
    }
    EXPECT_EQ(got_count, 2);
}

TEST(RelocationAnalyzer, DirectRefPC32) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401030, 2 /*R_X86_64_PC32*/, 1, -4, "my_global"},
    };
    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    bool found = false;
    for (const auto& ref : refs) {
        if (ref.insn_offset == 0x401030) {
            found = true;
            EXPECT_EQ(ref.kind, DataRefKind::Unknown);
            EXPECT_TRUE(ref.is_pc_relative);
        }
    }
    EXPECT_TRUE(found);
}

TEST(RelocationAnalyzer, DirectRef64) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401040, 1 /*R_X86_64_64*/, 1, 0, "abs_sym"},
    };
    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    bool found = false;
    for (const auto& ref : refs) {
        if (ref.insn_offset == 0x401040) {
            found = true;
            EXPECT_EQ(ref.kind, DataRefKind::Unknown);
            EXPECT_FALSE(ref.is_pc_relative);
        }
    }
    EXPECT_TRUE(found);
}

TEST(RelocationAnalyzer, NoneSkipped) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401050, 0 /*R_X86_64_NONE*/, 0, 0, ""},
    };
    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    for (const auto& ref : refs) {
        EXPECT_NE(ref.insn_offset, 0x401050u) << "R_X86_64_NONE should be skipped";
    }
}

TEST(RelocationAnalyzer, UnknownTypeProducesUnknownKind) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401060, 9999, 1, 0, "weird"},
    };
    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    bool found = false;
    for (const auto& ref : refs) {
        if (ref.insn_offset == 0x401060) {
            found = true;
            EXPECT_EQ(ref.kind, DataRefKind::Unknown);
        }
    }
    EXPECT_TRUE(found);
}

TEST(RelocationAnalyzer, SymbolNamePreserved) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401050, 2, 1, -4, "my_special_symbol"},
    };
    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    bool found = false;
    for (const auto& ref : refs) {
        if (ref.insn_offset == 0x401050) {
            found = true;
            EXPECT_EQ(ref.target_symbol, "my_special_symbol");
        }
    }
    EXPECT_TRUE(found);
}

TEST(RelocationAnalyzer, RegionBoundaryExact) {
    auto sections = makeTestSections();
    // Region [0x401100, 0x401200)
    std::vector<RelocationEntry> relocs = {
        {0x401100, 2, 1, -4, "at_start"},    // exactly at start — included
        {0x4011FF, 2, 2, -4, "at_last"},     // last byte — included
        {0x401200, 2, 3, -4, "at_end"},      // at end — excluded
        {0x4010FF, 2, 4, -4, "before"},       // before start — excluded
    };
    auto refs = analyze({}, 0x401100, 0x100, sections, relocs, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    bool found_start = false, found_last = false;
    for (const auto& ref : refs) {
        if (ref.insn_offset == 0x401100) found_start = true;
        if (ref.insn_offset == 0x4011FF) found_last = true;
        EXPECT_NE(ref.insn_offset, 0x401200u) << "At region_end should be excluded";
        EXPECT_NE(ref.insn_offset, 0x4010FFu) << "Before region should be excluded";
    }
    EXPECT_TRUE(found_start);
    EXPECT_TRUE(found_last);
}

TEST(RelocationAnalyzer, MultipleRelocsAllPreserved) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401010, 23, 1, 0, "tls_a"},
        {0x401020, 9, 2, -4, "got_b"},
        {0x401030, 2, 3, -4, "pc32_c"},
        {0x401040, 22, 4, -4, "ie_d"},
    };
    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    EXPECT_GE(refs.size(), 4u);
    // Verify all 4 present
    int count = 0;
    for (const auto& ref : refs) {
        if (ref.source == DataRefSource::Relocation) ++count;
    }
    EXPECT_EQ(count, 4);
}

TEST(RelocationAnalyzer, AArch64GotReloc) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401010, 311 /*R_AARCH64_ADR_GOT_PAGE*/, 1, 0, "got_sym"},
        {0x401014, 312 /*R_AARCH64_LD64_GOT_LO12_NC*/, 1, 0, "got_sym"},
    };
    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::ARM64,
                        VMPilot::Common::FileMode::MODE_64);
    int got_count = 0;
    for (const auto& ref : refs) {
        if (ref.kind == DataRefKind::GotLoad) ++got_count;
    }
    EXPECT_EQ(got_count, 2);
}

TEST(RelocationAnalyzer, AArch64TlsLocalExec) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401010, 549 /*TLSLE_ADD_TPREL_HI12*/, 1, 0, "tls_le"},
        {0x401014, 551 /*TLSLE_ADD_TPREL_LO12_NC*/, 1, 0, "tls_le"},
    };
    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::ARM64,
                        VMPilot::Common::FileMode::MODE_64);
    int le_count = 0;
    for (const auto& ref : refs) {
        if (ref.kind == DataRefKind::TlsVar &&
            ref.tls_model == TlsModel::LocalExec) ++le_count;
    }
    EXPECT_EQ(le_count, 2);
}

TEST(RelocationAnalyzer, AArch64TlsInitialExec) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401010, 539 /*TLSIE_ADR_GOTTPREL_PAGE21*/, 1, 0, "tls_ie"},
    };
    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::ARM64,
                        VMPilot::Common::FileMode::MODE_64);
    bool found = false;
    for (const auto& ref : refs) {
        if (ref.insn_offset == 0x401010) {
            found = true;
            EXPECT_EQ(ref.tls_model, TlsModel::InitialExec);
        }
    }
    EXPECT_TRUE(found);
}

TEST(RelocationAnalyzer, AArch64TlsGeneralDynamic) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401010, 513 /*TLSGD_ADR_PAGE21*/, 1, 0, "tls_gd"},
        {0x401014, 514 /*TLSGD_ADD_LO12_NC*/, 1, 0, "tls_gd"},
    };
    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::ARM64,
                        VMPilot::Common::FileMode::MODE_64);
    int gd_count = 0;
    for (const auto& ref : refs) {
        if (ref.tls_model == TlsModel::GeneralDynamic) ++gd_count;
    }
    EXPECT_EQ(gd_count, 2);
}

TEST(RelocationAnalyzer, AArch64TlsLocalDynamic) {
    auto sections = makeTestSections();
    std::vector<RelocationEntry> relocs = {
        {0x401010, 517 /*TLSLD_ADR_PAGE21*/, 1, 0, "tls_ld"},
    };
    auto refs = analyze({}, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::ARM64,
                        VMPilot::Common::FileMode::MODE_64);
    bool found = false;
    for (const auto& ref : refs) {
        if (ref.insn_offset == 0x401010) {
            found = true;
            EXPECT_EQ(ref.tls_model, TlsModel::LocalDynamic);
        }
    }
    EXPECT_TRUE(found);
}

// ---- Realistic Scenario Tests ----
//
// Each test simulates a pattern from actual compiler output.
// Byte sequences are from GCC/Clang -O2 targeting x86-64 or AArch64.

// -- Scenario: GCC std::atomic<int> global_counter++ --
// Source: void inc() { global_counter.fetch_add(1, std::memory_order_relaxed); }
// GCC -O2 emits: lock add dword ptr [rip+counter], 1 ; ret
TEST(Scenario, GccAtomicCounterIncrement) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // lock add dword ptr [rip+0x1FF8], 1  (size=8, f0 83 05 disp 01)
    //   target = 0x401000 + 8 + 0x1FF8 = 0x403000 (.data)
    // ret
    std::vector<uint8_t> code = {
        0xf0, 0x83, 0x05, 0xf8, 0x1f, 0x00, 0x00, 0x01,  // lock add [rip], 1
        0xc3,                                                // ret
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 2u);

    Segmentator::NativeSymbolTable syms;
    Segmentator::NativeSymbolTableEntry sym;
    sym.name = "global_counter";
    sym.address = 0x403000;
    sym.size = 4;
    sym.isGlobal = true;
    syms.push_back(sym);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, syms,
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    ASSERT_EQ(refs.size(), 1u);
    const auto& ref = refs[0];
    EXPECT_EQ(ref.insn_offset, 0x401000u);
    EXPECT_EQ(ref.target_va, 0x403000u);
    EXPECT_EQ(ref.target_symbol, "global_counter");
    EXPECT_EQ(ref.kind, DataRefKind::GlobalVar);
    EXPECT_EQ(ref.atomic_op, AtomicOp::RMW);
    EXPECT_EQ(ref.atomic_ordering, AtomicOrdering::AcqRel);
    EXPECT_EQ(ref.atomic_width, AtomicWidth::Atomic32);
    EXPECT_EQ(ref.access_size, 4u);
    EXPECT_TRUE(ref.is_write);
    EXPECT_TRUE(ref.is_pc_relative);
}

// -- Scenario: Clang fetch_add with return value --
// Source: int old = counter.fetch_add(1);
// Clang -O2: mov eax, 1 ; lock xadd [rip+counter], eax ; ret
TEST(Scenario, ClangFetchAddReturnValue) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // 0x401000: mov eax, 1                  (5 bytes: b8 01 00 00 00)
    // 0x401005: lock xadd [rip+0x1FF3], eax (8 bytes: f0 0f c1 05 f3 1f 00 00)
    //           target = 0x401005 + 8 + 0x1FF3 = 0x403000 (.data)
    // 0x40100D: ret
    std::vector<uint8_t> code = {
        0xb8, 0x01, 0x00, 0x00, 0x00,                      // mov eax, 1
        0xf0, 0x0f, 0xc1, 0x05, 0xf3, 0x1f, 0x00, 0x00,  // lock xadd
        0xc3,                                                // ret
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 3u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    bool found_xadd = false;
    for (const auto& ref : refs) {
        if (ref.atomic_op == AtomicOp::FetchAdd) {
            found_xadd = true;
            EXPECT_EQ(ref.target_va, 0x403000u);
            EXPECT_EQ(ref.atomic_ordering, AtomicOrdering::AcqRel);
            EXPECT_EQ(ref.atomic_width, AtomicWidth::Atomic32);
        }
    }
    EXPECT_TRUE(found_xadd);
}

// -- Scenario: compare_exchange_strong on a global --
// Source: expected = old; flag.compare_exchange_strong(expected, 1);
// GCC -O2: mov eax, ecx ; lock cmpxchg [rip+flag], edx ; ret
TEST(Scenario, CompareExchangeOnGlobal) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // 0x401000: mov eax, ecx     (2 bytes: 89 c8)
    // 0x401002: lock cmpxchg [rip+0x1FF6], edx
    //           (8 bytes: f0 0f b1 15 f6 1f 00 00)
    //           target = 0x401002 + 8 + 0x1FF6 = 0x403000 (.data)
    // 0x40100A: ret
    std::vector<uint8_t> code = {
        0x89, 0xc8,                                          // mov eax, ecx
        0xf0, 0x0f, 0xb1, 0x15, 0xf6, 0x1f, 0x00, 0x00,  // lock cmpxchg
        0xc3,                                                // ret
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 3u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    bool found_cas = false;
    for (const auto& ref : refs) {
        if (ref.atomic_op == AtomicOp::CompareSwap) {
            found_cas = true;
            EXPECT_EQ(ref.target_va, 0x403000u);
            EXPECT_EQ(ref.atomic_width, AtomicWidth::Atomic32);
        }
    }
    EXPECT_TRUE(found_cas);
}

// -- Scenario: spinlock acquire via xchg --
// Source: while (lock.exchange(1, acquire)) {}
// GCC -O2: .spin: mov eax, 1 ; xchg [rip+lock], eax ; test eax,eax ; jne .spin ; ret
TEST(Scenario, SpinlockAcquireViaXchg) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // 0x401000: mov eax, 1                    (5 bytes: b8 01 00 00 00)
    // 0x401005: xchg [rip+0x1FF5], eax        (6 bytes: 87 05 f5 1f 00 00)
    //           target = 0x401005 + 6 + 0x1FF5 = 0x403000 (.data)
    // 0x40100B: test eax, eax                  (2 bytes: 85 c0)
    // 0x40100D: jne 0x401000                   (2 bytes: 75 f1)
    // 0x40100F: ret                            (1 byte: c3)
    std::vector<uint8_t> code = {
        0xb8, 0x01, 0x00, 0x00, 0x00,              // mov eax, 1
        0x87, 0x05, 0xf5, 0x1f, 0x00, 0x00,        // xchg [rip+disp], eax
        0x85, 0xc0,                                  // test eax, eax
        0x75, 0xf1,                                  // jne -15
        0xc3,                                        // ret
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 5u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    bool found_swap = false;
    for (const auto& ref : refs) {
        if (ref.atomic_op == AtomicOp::Swap) {
            found_swap = true;
            EXPECT_EQ(ref.target_va, 0x403000u);
            EXPECT_EQ(ref.atomic_ordering, AtomicOrdering::AcqRel);
        }
    }
    EXPECT_TRUE(found_swap);
}

// -- Scenario: seq_cst store via mov + mfence --
// Source: flag.store(1, std::memory_order_seq_cst);
// Some compilers emit: mov [rip+flag], 1 ; mfence ; ret
TEST(Scenario, SeqCstStoreWithMfence) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // 0x401000: mov dword ptr [rip+0x1FF6], 1
    //           (10 bytes: c7 05 f6 1f 00 00 01 00 00 00)
    //           target = 0x401000 + 10 + 0x1FF6 = 0x403000 (.data)
    // 0x40100A: mfence (3 bytes: 0f ae f0)
    // 0x40100D: ret
    std::vector<uint8_t> code = {
        0xc7, 0x05, 0xf6, 0x1f, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00,                     // mov dword [rip+disp], 1
        0x0f, 0xae, 0xf0,                            // mfence
        0xc3,                                         // ret
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 3u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    bool found_store = false, found_fence = false;
    for (const auto& ref : refs) {
        if (ref.source == DataRefSource::InsnAnalysis &&
            ref.atomic_op == AtomicOp::None && ref.target_va == 0x403000) {
            found_store = true;
            EXPECT_TRUE(ref.is_write);
            EXPECT_EQ(ref.kind, DataRefKind::GlobalVar);
        }
        if (ref.atomic_op == AtomicOp::Fence) {
            found_fence = true;
        }
    }
    EXPECT_TRUE(found_store) << "Non-atomic store to .data expected";
    EXPECT_TRUE(found_fence) << "mfence expected";
}

// -- Scenario: function that reads config, atomically bumps counter,
//    then writes result --
// Source:
//   static const int multiplier = ...;  // .rodata
//   std::atomic<int> counter;           // .data @ 0x403000
//   int last_result;                    // .data @ 0x403004
//   void process() {
//       int m = multiplier;
//       counter.fetch_add(m);
//       last_result = m * 2;
//   }
TEST(Scenario, MixedAtomicAndNonAtomicFunction) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // 0x401000: mov eax, [rip+0x0FFA]          (6 bytes) → 0x402000 .rodata
    // 0x401006: lock add [rip+0x1FF3], eax     (7 bytes) → 0x403000 .data
    // 0x40100D: shl eax, 1                      (2 bytes)
    // 0x40100F: mov [rip+0x1FEF], eax          (6 bytes) → 0x403004 .data
    // 0x401015: ret
    std::vector<uint8_t> code = {
        0x8b, 0x05, 0xfa, 0x0f, 0x00, 0x00,              // mov eax,[rip+0xFFA]
        0xf0, 0x01, 0x05, 0xf3, 0x1f, 0x00, 0x00,        // lock add [rip],eax
        0xd1, 0xe0,                                        // shl eax, 1
        0x89, 0x05, 0xef, 0x1f, 0x00, 0x00,              // mov [rip+disp],eax
        0xc3,                                              // ret
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 5u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    // Expect 3 data refs: .rodata read, atomic .data write, plain .data write
    bool found_rodata = false, found_atomic = false, found_plain_store = false;
    for (const auto& ref : refs) {
        if (ref.source != DataRefSource::InsnAnalysis) continue;
        if (ref.target_va == 0x402000 && ref.kind == DataRefKind::ReadOnlyData) {
            found_rodata = true;
            EXPECT_FALSE(ref.is_write);
            EXPECT_EQ(ref.atomic_op, AtomicOp::None);
        }
        if (ref.target_va == 0x403000 && ref.atomic_op == AtomicOp::RMW) {
            found_atomic = true;
            EXPECT_EQ(ref.atomic_ordering, AtomicOrdering::AcqRel);
            EXPECT_EQ(ref.atomic_width, AtomicWidth::Atomic32);
        }
        if (ref.target_va == 0x403004 && ref.atomic_op == AtomicOp::None) {
            found_plain_store = true;
            EXPECT_TRUE(ref.is_write);
        }
    }
    EXPECT_TRUE(found_rodata) << "rodata read missing";
    EXPECT_TRUE(found_atomic) << "atomic add missing";
    EXPECT_TRUE(found_plain_store) << "plain store missing";
}

// -- Scenario: GCC stack canary prologue/epilogue --
// Source: void foo() { char buf[32]; ... }
// GCC -fstack-protector: reads fs:[0x28] canary at entry
TEST(Scenario, GccStackCanaryPrologue) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // 0x401000: push rbp                       (1 byte: 55)
    // 0x401001: mov rbp, rsp                   (3 bytes: 48 89 e5)
    // 0x401004: sub rsp, 0x30                  (4 bytes: 48 83 ec 30)
    // 0x401008: mov rax, fs:[0x28]             (9 bytes: 64 48 8b 04 25 28 00 00 00)
    // 0x401011: mov [rbp-8], rax               (4 bytes: 48 89 45 f8)
    // 0x401015: xor eax, eax                   (2 bytes: 31 c0)
    // 0x401017: nop                            (1 byte: 90)
    std::vector<uint8_t> code = {
        0x55,                                              // push rbp
        0x48, 0x89, 0xe5,                                  // mov rbp, rsp
        0x48, 0x83, 0xec, 0x30,                            // sub rsp, 0x30
        0x64, 0x48, 0x8b, 0x04, 0x25,
        0x28, 0x00, 0x00, 0x00,                            // mov rax,fs:0x28
        0x48, 0x89, 0x45, 0xf8,                            // mov [rbp-8],rax
        0x31, 0xc0,                                        // xor eax,eax
        0x90,                                              // nop
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 7u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    // Expect L3 TLS ref from fs:[0x28]
    bool found_tls = false;
    for (const auto& ref : refs) {
        if (ref.kind == DataRefKind::TlsVar &&
            ref.source == DataRefSource::PatternMatch) {
            found_tls = true;
            EXPECT_EQ(ref.tls_model, TlsModel::LocalExec);
            EXPECT_EQ(ref.insn_offset, 0x401008u);
        }
    }
    EXPECT_TRUE(found_tls);
}

// -- Scenario: GOT-indirect external symbol load --
// Source: extern int ext_var; int get() { return ext_var; }
// GCC -O2 -fPIC: mov rax,[rip+GOT_entry] ; mov eax,[rax] ; ret
TEST(Scenario, GotIndirectExternalLoad) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // 0x401000: mov rax, [rip+0x4FF9]  (7 bytes: 48 8b 05 f9 4f 00 00)
    //           target = 0x401000 + 7 + 0x4FF9 = 0x406000 (.got)
    // 0x401007: mov eax, [rax]          (2 bytes: 8b 00) — indirect, won't resolve
    // 0x401009: ret
    std::vector<uint8_t> code = {
        0x48, 0x8b, 0x05, 0xf9, 0x4f, 0x00, 0x00,  // mov rax,[rip+disp]
        0x8b, 0x00,                                    // mov eax,[rax]
        0xc3,                                          // ret
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 3u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    // Only the GOT load should produce a ref; [rax] is unresolvable
    ASSERT_EQ(refs.size(), 1u);
    EXPECT_EQ(refs[0].target_va, 0x406000u);
    EXPECT_EQ(refs[0].kind, DataRefKind::GotLoad);
    EXPECT_TRUE(refs[0].is_pc_relative);
    EXPECT_EQ(refs[0].access_size, 8u);
}

// -- Scenario: function loading .bss global (zero-initialized) --
// Source: static int bss_var; int get() { return bss_var; }
TEST(Scenario, BssGlobalAccess) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // 0x401000: mov eax, [rip+0x2FFA]  (6 bytes)
    //           target = 0x401000 + 6 + 0x2FFA = 0x404000 (.bss)
    // 0x401006: ret
    std::vector<uint8_t> code = {
        0x8b, 0x05, 0xfa, 0x2f, 0x00, 0x00,  // mov eax,[rip+disp]
        0xc3,                                    // ret
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 2u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    ASSERT_EQ(refs.size(), 1u);
    EXPECT_EQ(refs[0].target_va, 0x404000u);
    EXPECT_EQ(refs[0].kind, DataRefKind::GlobalVar);
    EXPECT_FALSE(refs[0].is_write);
}

// -- Scenario: L1 relocation + L2 instruction at same offset → L1 wins --
// Real case: linker provides TPOFF32 reloc on a mov that L2 sees as .rodata
TEST(Scenario, RelocationOverridesInsnAnalysis) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // mov eax, [rip+0x0FFA] → L2 classifies as .rodata (0x402000)
    std::vector<uint8_t> code = {0x8b, 0x05, 0xfa, 0x0f, 0x00, 0x00};
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 1u);

    // But L1 says this offset is a TLS access (linker knows better)
    std::vector<RelocationEntry> relocs = {
        {0x401000, 23 /*TPOFF32*/, 1, 0, "tls_var"},
    };

    auto refs = analyze(insns, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    // Should be exactly 1 ref and L1 wins
    ASSERT_EQ(refs.size(), 1u);
    EXPECT_EQ(refs[0].source, DataRefSource::Relocation);
    EXPECT_EQ(refs[0].kind, DataRefKind::TlsVar);
    EXPECT_EQ(refs[0].tls_model, TlsModel::LocalExec);
}

// -- Scenario: atomic byte flag (std::atomic<bool>/std::atomic<char>) --
// Source: std::atomic<bool> ready; ready.store(true, relaxed);
// x86: lock or byte ptr [rip+ready], 1  (on some compilers)
// Or:  lock add byte ptr [rip+ready], cl (from fetch_or pattern)
TEST(Scenario, AtomicByteFlag) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // lock add byte ptr [rip+0x1FF9], cl
    // f0 00 0d f9 1f 00 00 (size=7)
    // target = 0x401000 + 7 + 0x1FF9 = 0x403000 (.data)
    // ret
    std::vector<uint8_t> code = {
        0xf0, 0x00, 0x0d, 0xf9, 0x1f, 0x00, 0x00,  // lock add byte
        0xc3,                                          // ret
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 2u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    bool found = false;
    for (const auto& ref : refs) {
        if (ref.atomic_op != AtomicOp::None) {
            found = true;
            EXPECT_EQ(ref.atomic_width, AtomicWidth::Atomic8);
            EXPECT_EQ(ref.access_size, 1u);
        }
    }
    EXPECT_TRUE(found);
}

// -- Scenario: 64-bit atomic on a pointer-sized global --
// Source: std::atomic<void*> head; head.fetch_add(8);
// GCC: lock add qword ptr [rip+head], rcx ; ret
TEST(Scenario, AtomicQwordPointerSized) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // lock add qword ptr [rip+0x1FF8], rcx
    // f0 48 01 0d f8 1f 00 00 (size=8)
    // target = 0x401000 + 8 + 0x1FF8 = 0x403000 (.data)
    // ret
    std::vector<uint8_t> code = {
        0xf0, 0x48, 0x01, 0x0d, 0xf8, 0x1f, 0x00, 0x00,
        0xc3,
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 2u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    bool found = false;
    for (const auto& ref : refs) {
        if (ref.atomic_op != AtomicOp::None) {
            found = true;
            EXPECT_EQ(ref.atomic_width, AtomicWidth::Atomic64);
            EXPECT_EQ(ref.access_size, 8u);
        }
    }
    EXPECT_TRUE(found);
}

// -- Scenario: non-atomic add to global is NOT flagged atomic --
// Source: global_var += val;  (without std::atomic)
// GCC: add [rip+var], ecx ; ret   — no lock prefix
TEST(Scenario, NonAtomicGlobalAddNotFlaggedAtomic) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // add dword ptr [rip+0x1FFA], ecx
    // 01 0d fa 1f 00 00 (size=6)
    // target = 0x403000 (.data)
    // ret
    std::vector<uint8_t> code = {
        0x01, 0x0d, 0xfa, 0x1f, 0x00, 0x00,
        0xc3,
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 2u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    ASSERT_EQ(refs.size(), 1u);
    EXPECT_EQ(refs[0].atomic_op, AtomicOp::None);
    EXPECT_EQ(refs[0].atomic_ordering, AtomicOrdering::None);
    EXPECT_EQ(refs[0].atomic_width, AtomicWidth::None);
    EXPECT_TRUE(refs[0].is_write);
}

// -- Scenario: ARM64 data memory barrier between loads --
// Real pattern: load-acquire via ldr + dmb
TEST(Scenario, ARM64DmbBetweenLoads) {
    Capstone::Capstone cs(Capstone::Arch::ARM64, Capstone::Mode::MODE_ARM);

    // nop ; dmb ish ; nop ; ret
    std::vector<uint8_t> code = {
        0x1f, 0x20, 0x03, 0xd5,  // nop
        0xbf, 0x3b, 0x03, 0xd5,  // dmb ish
        0x1f, 0x20, 0x03, 0xd5,  // nop
        0xc0, 0x03, 0x5f, 0xd6,  // ret
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 4u);

    auto sections = makeTestSections();
    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        VMPilot::Common::FileArch::ARM64,
                        VMPilot::Common::FileMode::MODE_64);

    // Only the dmb should produce a ref
    ASSERT_EQ(refs.size(), 1u);
    EXPECT_EQ(refs[0].atomic_op, AtomicOp::Fence);
    EXPECT_EQ(refs[0].atomic_ordering, AtomicOrdering::AcqRel);
    EXPECT_EQ(refs[0].target_va, 0u);
    EXPECT_EQ(refs[0].insn_offset, 0x401004u);
}

// -- Scenario: all three layers producing refs at different offsets --
TEST(Scenario, AllThreeLayersRealMerge) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // 0x401000: mov eax, [rip+0x0FFA]    → L2 .rodata (6 bytes)
    // 0x401006: mov rax, fs:[0x28]        → L3 TLS (9 bytes)
    std::vector<uint8_t> code = {
        0x8b, 0x05, 0xfa, 0x0f, 0x00, 0x00,
        0x64, 0x48, 0x8b, 0x04, 0x25, 0x28, 0x00, 0x00, 0x00,
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 2u);

    // L1 relocation at a separate instruction offset
    std::vector<RelocationEntry> relocs = {
        {0x401020, 9 /*GOTPCREL*/, 1, -4, "extern_sym"},
    };

    auto refs = analyze(insns, 0x401000, 0x1000, sections, relocs, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    bool l1 = false, l2 = false, l3 = false;
    for (const auto& ref : refs) {
        if (ref.source == DataRefSource::Relocation) l1 = true;
        if (ref.source == DataRefSource::InsnAnalysis &&
            ref.kind == DataRefKind::ReadOnlyData) l2 = true;
        if (ref.source == DataRefSource::PatternMatch &&
            ref.kind == DataRefKind::TlsVar) l3 = true;
    }
    EXPECT_TRUE(l1) << "L1 (relocation) expected";
    EXPECT_TRUE(l2) << "L2 (insn analysis) expected";
    EXPECT_TRUE(l3) << "L3 (pattern match) expected";

    // Results must be sorted by insn_offset
    for (size_t i = 1; i < refs.size(); ++i) {
        EXPECT_LE(refs[i - 1].insn_offset, refs[i].insn_offset);
    }
}

// -- Scenario: MSVC /GS stack cookie on Windows x64 --
// Source: any function with local buffers compiled with /GS
// MSVC emits: mov rax, gs:[0x28]  (Windows TEB security cookie)
// This is the GS-segment equivalent of GCC's fs:[0x28]
TEST(Scenario, MsvcStackCookieGsSegment) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // MSVC prologue: push rbp ; mov rbp, rsp ; sub rsp, 0x20 ;
    //               mov rax, gs:[0x28] ; mov [rbp-8], rax ; xor eax, eax
    // gs:[0x28] encoding: 65 48 8b 04 25 28 00 00 00
    //   (0x65 = GS segment override, vs 0x64 = FS)
    std::vector<uint8_t> code = {
        0x55,                                              // push rbp
        0x48, 0x89, 0xe5,                                  // mov rbp, rsp
        0x48, 0x83, 0xec, 0x20,                            // sub rsp, 0x20
        0x65, 0x48, 0x8b, 0x04, 0x25,
        0x28, 0x00, 0x00, 0x00,                            // mov rax, gs:0x28
        0x48, 0x89, 0x45, 0xf8,                            // mov [rbp-8], rax
        0x31, 0xc0,                                        // xor eax, eax
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 6u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    bool found_tls = false;
    for (const auto& ref : refs) {
        if (ref.kind == DataRefKind::TlsVar &&
            ref.source == DataRefSource::PatternMatch) {
            found_tls = true;
            EXPECT_EQ(ref.tls_model, TlsModel::LocalExec);
            EXPECT_EQ(ref.insn_offset, 0x401008u);
        }
    }
    EXPECT_TRUE(found_tls)
        << "MSVC gs:[0x28] security cookie should be detected as TLS";
}

// -- Scenario: MSVC _InterlockedIncrement / _InterlockedDecrement --
// Source: _InterlockedIncrement(&refcount); ... _InterlockedDecrement(&refcount);
// MSVC emits: lock inc dword ptr [...] ; ... ; lock dec dword ptr [...]
// Same as GCC __sync_fetch_and_add(&x, 1) at machine level.
// Tests the COM AddRef/Release reference counting pattern.
TEST(Scenario, MsvcInterlockedIncDec) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // Realistic COM AddRef/Release on a global singleton refcount:
    //   lock inc dword ptr [rip+refcount]   ; AddRef
    //   ...
    //   lock dec dword ptr [rip+refcount]   ; Release
    //   jnz .not_zero
    //   ret
    //
    // 0x401000: lock inc [rip+0x1FF9] (7 bytes: f0 ff 05 f9 1f 00 00)
    //           target = 0x401007 + 0x1FF9 = 0x403000 (.data)
    // 0x401007: lock dec [rip+0x1FF2] (7 bytes: f0 ff 0d f2 1f 00 00)
    //           target = 0x40100E + 0x1FF2 = 0x403000 (.data)
    // 0x40100E: jnz +0 (skip)         (2 bytes: 75 00)
    // 0x401010: ret                    (1 byte: c3)
    std::vector<uint8_t> code = {
        0xf0, 0xff, 0x05, 0xf9, 0x1f, 0x00, 0x00,  // lock inc
        0xf0, 0xff, 0x0d, 0xf2, 0x1f, 0x00, 0x00,  // lock dec
        0x75, 0x00,                                    // jnz .+0
        0xc3,                                          // ret
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 4u);

    Segmentator::NativeSymbolTable syms;
    Segmentator::NativeSymbolTableEntry sym;
    sym.name = "g_refcount";
    sym.address = 0x403000;
    sym.size = 4;
    sym.isGlobal = true;
    syms.push_back(sym);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, syms,
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    // Both lock inc and lock dec should produce atomic RMW refs
    int rmw_count = 0;
    for (const auto& ref : refs) {
        if (ref.atomic_op == AtomicOp::RMW) {
            ++rmw_count;
            EXPECT_EQ(ref.target_va, 0x403000u);
            EXPECT_EQ(ref.target_symbol, "g_refcount");
            EXPECT_EQ(ref.atomic_ordering, AtomicOrdering::AcqRel);
            EXPECT_EQ(ref.atomic_width, AtomicWidth::Atomic32);
        }
    }
    EXPECT_EQ(rmw_count, 2) << "Expected 2 atomic RMW refs (inc + dec)";
}

// -- Scenario: MSVC _InterlockedCompareExchange on global --
// Source: _InterlockedCompareExchange((long*)&g_flag, desired, expected);
// MSVC emits: mov eax, expected ; lock cmpxchg [rip+g_flag], edx
TEST(Scenario, MsvcInterlockedCompareExchange) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);

    // 0x401000: xor eax, eax                    (2 bytes: 31 c0)  — expected=0
    // 0x401002: mov edx, 1                       (5 bytes: ba 01 00 00 00)
    // 0x401007: lock cmpxchg [rip+0x1FF1], edx  (8 bytes: f0 0f b1 15 f1 1f 00 00)
    //           target = 0x401007 + 8 + 0x1FF1 = 0x403000 (.data)
    // 0x40100F: ret
    std::vector<uint8_t> code = {
        0x31, 0xc0,                                          // xor eax, eax
        0xba, 0x01, 0x00, 0x00, 0x00,                      // mov edx, 1
        0xf0, 0x0f, 0xb1, 0x15, 0xf1, 0x1f, 0x00, 0x00,  // lock cmpxchg
        0xc3,                                                // ret
    };
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 4u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);

    bool found_cas = false;
    for (const auto& ref : refs) {
        if (ref.atomic_op == AtomicOp::CompareSwap) {
            found_cas = true;
            EXPECT_EQ(ref.target_va, 0x403000u);
            EXPECT_EQ(ref.atomic_ordering, AtomicOrdering::AcqRel);
            EXPECT_EQ(ref.atomic_width, AtomicWidth::Atomic32);
        }
    }
    EXPECT_TRUE(found_cas);
}

// ---- Edge Cases ----

TEST(ReferenceAnalyzer, ZeroSizeRegion) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);
    std::vector<uint8_t> code = {0x8b, 0x05, 0xfa, 0x0f, 0x00, 0x00};
    auto insns = cs.disasm(code, 0x401000);

    auto refs = analyze(insns, 0x401000, 0, sections, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    EXPECT_TRUE(refs.empty()) << "Zero-size region should produce no refs";
}

TEST(ReferenceAnalyzer, NopOnlyProducesNoRefs) {
    auto sections = makeTestSections();
    Capstone::Capstone cs(Capstone::Arch::X86, Capstone::Mode::MODE_64);
    std::vector<uint8_t> code = {0x90, 0x90, 0x90, 0x90};
    auto insns = cs.disasm(code, 0x401000);
    ASSERT_GE(insns.size(), 4u);

    auto refs = analyze(insns, 0x401000, 0x1000, sections, {}, {},
                        VMPilot::Common::FileArch::X86,
                        VMPilot::Common::FileMode::MODE_64);
    EXPECT_TRUE(refs.empty());
}

TEST(ReferenceAnalyzer, ARM64EmptyStream) {
    auto sections = makeTestSections();
    auto refs = analyze({}, 0x401000, 0x1000, sections, {}, {},
                        VMPilot::Common::FileArch::ARM64,
                        VMPilot::Common::FileMode::MODE_64);
    EXPECT_TRUE(refs.empty());
}
