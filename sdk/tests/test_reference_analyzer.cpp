#include <gtest/gtest.h>

#include <DataReference.hpp>
#include <ReferenceAnalyzer.hpp>
#include <SectionInfo.hpp>
#include <capstone.hpp>

// Internal headers for unit-testing individual layers
// We test through the public API primarily, with synthetic data
#include <NativeSymbolTable.hpp>

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
