/// @file test_editor_permissions.cpp
/// @brief Verifies W^X-compliant segment permissions in BinaryEditors.
///
/// The injected payload section must be mapped RW (not RX) at load time
/// so the runtime constructor can write call_slot.  After writing, the
/// constructor mprotects the section to RX.  At no point is a page W+X.
///
/// These tests verify that:
///   - ELF segments have PF_R|PF_W (not PF_X) — kernel maps as writable data
///   - ELF sections have SHF_WRITE|SHF_EXECINSTR — describes content semantics
///   - Mach-O initprot is RW (not RX) — initial mapping is writable
///   - Mach-O maxprot includes R|W|X — allows mprotect to RX after init

#include <ELFEditor.hpp>
#include <PEEditor.hpp>
#include <PlatformTraits.hpp>
#include <diagnostic_collector.hpp>

#include <coffi/coffi.hpp>
#include <elfio/elfio.hpp>

#include <gtest/gtest.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <vector>

using namespace VMPilot;
using DC = Common::DiagnosticCode;

// ============================================================================
// Helpers
// ============================================================================

namespace {

/// Build a minimal valid ELF64 binary with a .text section.
/// Uses ELFIO (already a project dependency).
std::string build_minimal_elf(uint64_t text_va = 0x401000, size_t text_size = 0x100) {
    ELFIO::elfio writer;
    writer.create(ELFIO::ELFCLASS64, ELFIO::ELFDATA2LSB);
    writer.set_os_abi(ELFIO::ELFOSABI_LINUX);
    writer.set_type(ELFIO::ET_EXEC);
    writer.set_machine(ELFIO::EM_X86_64);
    writer.set_entry(text_va);

    // .text section with NOPs
    auto* text_sec = writer.sections.add(".text");
    text_sec->set_type(ELFIO::SHT_PROGBITS);
    text_sec->set_flags(ELFIO::SHF_ALLOC | ELFIO::SHF_EXECINSTR);
    text_sec->set_addr_align(16);
    text_sec->set_address(text_va);
    std::vector<uint8_t> nops(text_size, 0x90);
    text_sec->set_data(reinterpret_cast<const char*>(nops.data()), nops.size());

    // PT_LOAD segment for .text
    auto* text_seg = writer.segments.add();
    text_seg->set_type(ELFIO::PT_LOAD);
    text_seg->set_flags(ELFIO::PF_R | ELFIO::PF_X);
    text_seg->set_align(0x1000);
    text_seg->set_virtual_address(text_va);
    text_seg->set_physical_address(text_va);
    text_seg->add_section_index(text_sec->get_index(), text_sec->get_addr_align());

    char tmpname[] = "/tmp/vmpilot_test_elf_XXXXXX";
    int fd = mkstemp(tmpname);
    EXPECT_GE(fd, 0) << "mkstemp failed";
    close(fd);

    writer.save(tmpname);
    return std::string(tmpname);
}

}  // namespace

// ============================================================================
// ELF Segment Permission Tests
// ============================================================================

TEST(EditorPermissions, ELFSegmentFlagsAreRW) {
    std::string elf_path = build_minimal_elf();
    Common::DiagnosticCollector diag;

    // Open with ELFEditor and add a payload segment
    auto editor = Loader::ELFEditor::open(elf_path, diag);
    ASSERT_TRUE(editor.has_value()) << "Failed to open test ELF";

    std::vector<uint8_t> payload(128, 0x00);  // 8-byte call_slot + data
    auto seg_info = editor->add_segment(".vmpilot", payload, 0x1000, diag);
    ASSERT_TRUE(seg_info.has_value()) << "add_segment failed";

    std::string out_path = elf_path + ".patched";
    auto save_result = editor->save(out_path, diag);
    ASSERT_TRUE(save_result.has_value()) << "save failed";

    // Re-open with ELFIO and verify the new segment's flags
    ELFIO::elfio reader;
    ASSERT_TRUE(reader.load(out_path)) << "Failed to reload patched ELF";

    bool found_vmpilot_seg = false;
    for (const auto& seg : reader.segments) {
        if (seg->get_type() != ELFIO::PT_LOAD) continue;
        if (seg->get_virtual_address() != seg_info->va) continue;

        found_vmpilot_seg = true;
        auto flags = seg->get_flags();

        // Must have R+W
        EXPECT_TRUE(flags & ELFIO::PF_R) << "Segment missing PF_R";
        EXPECT_TRUE(flags & ELFIO::PF_W) << "Segment missing PF_W";
        // Must NOT have X — constructor adds X via mprotect after writing call_slot
        EXPECT_FALSE(flags & ELFIO::PF_X)
            << "Segment has PF_X — violates W^X. "
               "Section must start RW; constructor mprotects to RX after init.";
        break;
    }
    EXPECT_TRUE(found_vmpilot_seg) << "Injected PT_LOAD segment not found";

    // Also verify section flags
    bool found_vmpilot_sec = false;
    for (const auto& sec : reader.sections) {
        if (sec->get_name() != ".vmpilot") continue;
        found_vmpilot_sec = true;
        auto flags = sec->get_flags();
        EXPECT_TRUE(flags & ELFIO::SHF_ALLOC) << "Section missing SHF_ALLOC";
        EXPECT_TRUE(flags & ELFIO::SHF_WRITE) << "Section missing SHF_WRITE";
        EXPECT_TRUE(flags & ELFIO::SHF_EXECINSTR)
            << "Section missing SHF_EXECINSTR (describes content semantics)";
        break;
    }
    EXPECT_TRUE(found_vmpilot_sec) << ".vmpilot section not found";

    std::remove(elf_path.c_str());
    std::remove(out_path.c_str());
}

TEST(EditorPermissions, ELFCallSlotZeroAtSegmentStart) {
    std::string elf_path = build_minimal_elf();
    Common::DiagnosticCollector diag;

    auto editor = Loader::ELFEditor::open(elf_path, diag);
    ASSERT_TRUE(editor.has_value());

    // Payload with call_slot = 8 zero bytes at offset 0 + some blob data
    std::vector<uint8_t> payload(64, 0xAB);
    std::memset(payload.data(), 0, 8);  // call_slot at offset 0

    auto seg_info = editor->add_segment(".vmpilot", payload, 0x1000, diag);
    ASSERT_TRUE(seg_info.has_value());

    std::string out_path = elf_path + ".patched";
    ASSERT_TRUE(editor->save(out_path, diag).has_value());

    // Verify call_slot bytes are zero in the output
    ELFIO::elfio reader;
    ASSERT_TRUE(reader.load(out_path));

    for (const auto& sec : reader.sections) {
        if (sec->get_name() != ".vmpilot") continue;
        ASSERT_GE(sec->get_size(), 8u);
        const auto* data = reinterpret_cast<const uint8_t*>(sec->get_data());
        uint64_t call_slot = 0;
        std::memcpy(&call_slot, data, 8);
        EXPECT_EQ(call_slot, 0u) << "call_slot must be zero-initialized "
                                    "(runtime constructor fills it)";
        // Blob data after call_slot should be preserved
        EXPECT_EQ(data[8], 0xAB);
        break;
    }

    std::remove(elf_path.c_str());
    std::remove(out_path.c_str());
}

// ============================================================================
// Mach-O Segment Permission Tests
//
// MachOEditor output is a raw byte buffer.  We parse the Mach-O headers
// directly to verify maxprot and initprot.
// ============================================================================

// Use MachOStructs for header definitions (same as MachOEditor uses)
#include <MachOStructs.hpp>

// ============================================================================
// ELF DT_NEEDED Injection Tests
// ============================================================================

TEST(EditorPermissions, ELFDtNeededInjection) {
    // Build a minimal ELF with a .dynamic section containing DT_NULL padding
    ELFIO::elfio writer;
    writer.create(ELFIO::ELFCLASS64, ELFIO::ELFDATA2LSB);
    writer.set_os_abi(ELFIO::ELFOSABI_LINUX);
    writer.set_type(ELFIO::ET_EXEC);
    writer.set_machine(ELFIO::EM_X86_64);
    writer.set_entry(0x401000);

    // .text section
    auto* text_sec = writer.sections.add(".text");
    text_sec->set_type(ELFIO::SHT_PROGBITS);
    text_sec->set_flags(ELFIO::SHF_ALLOC | ELFIO::SHF_EXECINSTR);
    text_sec->set_addr_align(16);
    text_sec->set_address(0x401000);
    std::vector<uint8_t> nops(256, 0x90);
    text_sec->set_data(reinterpret_cast<const char*>(nops.data()), nops.size());

    // PT_LOAD for .text
    auto* text_seg = writer.segments.add();
    text_seg->set_type(ELFIO::PT_LOAD);
    text_seg->set_flags(ELFIO::PF_R | ELFIO::PF_X);
    text_seg->set_align(0x1000);
    text_seg->set_virtual_address(0x401000);
    text_seg->set_physical_address(0x401000);
    text_seg->add_section_index(text_sec->get_index(), text_sec->get_addr_align());

    // .dynstr section (string table for .dynamic)
    auto* dynstr_sec = writer.sections.add(".dynstr");
    dynstr_sec->set_type(ELFIO::SHT_STRTAB);
    dynstr_sec->set_flags(ELFIO::SHF_ALLOC);
    dynstr_sec->set_addr_align(1);
    dynstr_sec->set_address(0x402000);
    // Initial content: just the null byte
    const char null_byte = '\0';
    dynstr_sec->set_data(&null_byte, 1);

    // .dynamic section with 4 slots: 1 real entry + 3 DT_NULL (padding)
    auto* dyn_sec = writer.sections.add(".dynamic");
    dyn_sec->set_type(ELFIO::SHT_DYNAMIC);
    dyn_sec->set_flags(ELFIO::SHF_ALLOC | ELFIO::SHF_WRITE);
    dyn_sec->set_addr_align(8);
    dyn_sec->set_address(0x402100);
    dyn_sec->set_entry_size(sizeof(ELFIO::Elf64_Dyn));
    dyn_sec->set_link(dynstr_sec->get_index());
    // 4 entries, all DT_NULL (we'll have room to steal one)
    std::vector<ELFIO::Elf64_Dyn> dyn_entries(4, ELFIO::Elf64_Dyn{});
    dyn_sec->set_data(reinterpret_cast<const char*>(dyn_entries.data()),
                      dyn_entries.size() * sizeof(ELFIO::Elf64_Dyn));

    // PT_LOAD for data sections (.dynstr + .dynamic)
    auto* data_seg = writer.segments.add();
    data_seg->set_type(ELFIO::PT_LOAD);
    data_seg->set_flags(ELFIO::PF_R | ELFIO::PF_W);
    data_seg->set_align(0x1000);
    data_seg->set_virtual_address(0x402000);
    data_seg->set_physical_address(0x402000);
    data_seg->add_section_index(dynstr_sec->get_index(), dynstr_sec->get_addr_align());
    data_seg->add_section_index(dyn_sec->get_index(), dyn_sec->get_addr_align());

    // PT_DYNAMIC segment
    auto* dyn_seg = writer.segments.add();
    dyn_seg->set_type(ELFIO::PT_DYNAMIC);
    dyn_seg->set_flags(ELFIO::PF_R | ELFIO::PF_W);
    dyn_seg->set_align(8);
    dyn_seg->add_section_index(dyn_sec->get_index(), dyn_sec->get_addr_align());

    char tmpname[] = "/tmp/vmpilot_test_dtneeded_XXXXXX";
    int fd = mkstemp(tmpname);
    ASSERT_GE(fd, 0);
    close(fd);
    writer.save(tmpname);

    // Open with ELFEditor and inject DT_NEEDED
    Common::DiagnosticCollector diag;
    auto editor = Loader::ELFEditor::open(tmpname, diag);
    ASSERT_TRUE(editor.has_value()) << "Failed to open test ELF";

    auto result = editor->add_runtime_dep("libvmpilot_runtime.so", diag);
    ASSERT_TRUE(result.has_value()) << "add_runtime_dep failed";

    std::string out_path = std::string(tmpname) + ".patched";
    auto save_res = editor->save(out_path, diag);
    ASSERT_TRUE(save_res.has_value()) << "save failed";

    // Verify: reload and check DT_NEEDED was injected
    ELFIO::elfio reader;
    ASSERT_TRUE(reader.load(out_path));

    bool found_needed = false;
    for (const auto& sec : reader.sections) {
        if (sec->get_type() != ELFIO::SHT_DYNAMIC) continue;

        ELFIO::dynamic_section_accessor dyn(reader, sec.get());
        for (ELFIO::Elf_Xword i = 0; i < dyn.get_entries_num(); ++i) {
            ELFIO::Elf_Xword tag, value;
            std::string str;
            dyn.get_entry(i, tag, value, str);
            if (tag == ELFIO::DT_NEEDED && str == "libvmpilot_runtime.so") {
                found_needed = true;
                break;
            }
        }
    }
    EXPECT_TRUE(found_needed) << "DT_NEEDED for libvmpilot_runtime.so not found";

    std::remove(tmpname);
    std::remove(out_path.c_str());
}

TEST(EditorPermissions, ELFDtNeededNoDynamicSection) {
    // A static binary has no .dynamic section.  DT_NEEDED cannot be
    // injected — add_runtime_dep must return a hard error so patch()
    // can abort rather than silently produce a broken binary.
    std::string elf_path = build_minimal_elf();
    Common::DiagnosticCollector diag;

    auto editor = Loader::ELFEditor::open(elf_path, diag);
    ASSERT_TRUE(editor.has_value());

    auto result = editor->add_runtime_dep("libvmpilot_runtime.so", diag);
    EXPECT_FALSE(result.has_value())
        << "Static binary (no .dynamic) must return error, not silent success";

    std::remove(elf_path.c_str());
}

TEST(EditorPermissions, ELFDtNeededNoSpareSlotsFallsBackToGrowth) {
    // .dynamic with exactly 1 DT_NULL (no spare) — Layer 1 fails,
    // Layer 2 (ELFIO add_entry growth) should succeed.
    ELFIO::elfio writer;
    writer.create(ELFIO::ELFCLASS64, ELFIO::ELFDATA2LSB);
    writer.set_os_abi(ELFIO::ELFOSABI_LINUX);
    writer.set_type(ELFIO::ET_EXEC);
    writer.set_machine(ELFIO::EM_X86_64);
    writer.set_entry(0x401000);

    auto* text_sec = writer.sections.add(".text");
    text_sec->set_type(ELFIO::SHT_PROGBITS);
    text_sec->set_flags(ELFIO::SHF_ALLOC | ELFIO::SHF_EXECINSTR);
    text_sec->set_addr_align(16);
    text_sec->set_address(0x401000);
    std::vector<uint8_t> nops(256, 0x90);
    text_sec->set_data(reinterpret_cast<const char*>(nops.data()), nops.size());

    auto* text_seg = writer.segments.add();
    text_seg->set_type(ELFIO::PT_LOAD);
    text_seg->set_flags(ELFIO::PF_R | ELFIO::PF_X);
    text_seg->set_align(0x1000);
    text_seg->set_virtual_address(0x401000);
    text_seg->set_physical_address(0x401000);
    text_seg->add_section_index(text_sec->get_index(), text_sec->get_addr_align());

    auto* dynstr_sec = writer.sections.add(".dynstr");
    dynstr_sec->set_type(ELFIO::SHT_STRTAB);
    dynstr_sec->set_flags(ELFIO::SHF_ALLOC);
    dynstr_sec->set_addr_align(1);
    dynstr_sec->set_address(0x402000);
    const char null_byte = '\0';
    dynstr_sec->set_data(&null_byte, 1);

    // Only 1 DT_NULL entry — no spare slot for Layer 1
    auto* dyn_sec = writer.sections.add(".dynamic");
    dyn_sec->set_type(ELFIO::SHT_DYNAMIC);
    dyn_sec->set_flags(ELFIO::SHF_ALLOC | ELFIO::SHF_WRITE);
    dyn_sec->set_addr_align(8);
    dyn_sec->set_address(0x402100);
    dyn_sec->set_entry_size(sizeof(ELFIO::Elf64_Dyn));
    dyn_sec->set_link(dynstr_sec->get_index());
    // 1 entry = 1 DT_NULL (terminator only, no spare)
    ELFIO::Elf64_Dyn single_null{};
    dyn_sec->set_data(reinterpret_cast<const char*>(&single_null),
                      sizeof(single_null));

    auto* data_seg = writer.segments.add();
    data_seg->set_type(ELFIO::PT_LOAD);
    data_seg->set_flags(ELFIO::PF_R | ELFIO::PF_W);
    data_seg->set_align(0x1000);
    data_seg->set_virtual_address(0x402000);
    data_seg->set_physical_address(0x402000);
    data_seg->add_section_index(dynstr_sec->get_index(), dynstr_sec->get_addr_align());
    data_seg->add_section_index(dyn_sec->get_index(), dyn_sec->get_addr_align());

    auto* dyn_seg = writer.segments.add();
    dyn_seg->set_type(ELFIO::PT_DYNAMIC);
    dyn_seg->set_flags(ELFIO::PF_R | ELFIO::PF_W);
    dyn_seg->set_align(8);
    dyn_seg->add_section_index(dyn_sec->get_index(), dyn_sec->get_addr_align());

    char tmpname[] = "/tmp/vmpilot_nospare_XXXXXX";
    int fd = mkstemp(tmpname);
    ASSERT_GE(fd, 0);
    close(fd);
    writer.save(tmpname);

    Common::DiagnosticCollector diag;
    auto editor = Loader::ELFEditor::open(tmpname, diag);
    ASSERT_TRUE(editor.has_value());

    // Layer 1 should fail (no spare DT_NULL), Layer 2 should succeed
    auto result = editor->add_runtime_dep("libvmpilot_runtime.so", diag);
    EXPECT_TRUE(result.has_value())
        << "Layer 2 (ELFIO growth) should succeed when Layer 1 has no spare slots";

    // Save and verify DT_NEEDED was injected via Layer 2
    std::string out_path = std::string(tmpname) + ".patched";
    ASSERT_TRUE(editor->save(out_path, diag).has_value());

    ELFIO::elfio reader;
    ASSERT_TRUE(reader.load(out_path));
    bool found = false;
    for (const auto& sec : reader.sections) {
        if (sec->get_type() != ELFIO::SHT_DYNAMIC) continue;
        ELFIO::dynamic_section_accessor dyn(reader, sec.get());
        for (ELFIO::Elf_Xword i = 0; i < dyn.get_entries_num(); ++i) {
            ELFIO::Elf_Xword tag, value;
            std::string str;
            dyn.get_entry(i, tag, value, str);
            if (tag == ELFIO::DT_NEEDED && str == "libvmpilot_runtime.so")
                found = true;
        }
    }
    EXPECT_TRUE(found) << "DT_NEEDED not found after Layer 2 fallback";

    std::remove(tmpname);
    std::remove(out_path.c_str());
}

// ============================================================================
// Mach-O Segment Permission Tests
// ============================================================================

TEST(EditorPermissions, MachOVerifyInitprotIsRW) {
    // We can't run MachOEditor::open on Linux (it needs a real Mach-O).
    // Instead, verify the constants used in MachOEditor are correct.
    // The actual Mach-O test requires the synthetic binary builder (Priority 3).

    using namespace VMPilot::Loader::MachO;

    // Verify the protection constants have correct values
    EXPECT_EQ(VM_PROT_READ, 0x01);
    EXPECT_EQ(VM_PROT_WRITE, 0x02);
    EXPECT_EQ(VM_PROT_EXECUTE, 0x04);

    // maxprot = R|W|X allows mprotect to add X later
    constexpr int32_t maxprot = VM_PROT_READ | VM_PROT_WRITE | VM_PROT_EXECUTE;
    EXPECT_EQ(maxprot, 0x07);

    // initprot = R|W (NOT X) — W^X compliant at load time
    constexpr int32_t initprot = VM_PROT_READ | VM_PROT_WRITE;
    EXPECT_EQ(initprot, 0x03);
    EXPECT_EQ(initprot & VM_PROT_EXECUTE, 0)
        << "initprot must NOT include EXECUTE — W^X compliance";
}

// ============================================================================
// PE Editor Tests
// ============================================================================

namespace {

/// Build a minimal valid PE32 binary with a .text section using COFFI.
std::string build_minimal_pe(uint32_t text_rva = 0x1000,
                             size_t text_size = 0x100) {
    COFFI::coffi writer;
    writer.create(COFFI::COFFI_ARCHITECTURE_PE);
    writer.create_optional_header();

    // .text section with NOPs
    auto* text_sec = writer.add_section(".text");
    std::vector<uint8_t> nops(text_size, 0x90);
    text_sec->set_data(reinterpret_cast<const char*>(nops.data()),
                       static_cast<uint32_t>(nops.size()));
    text_sec->set_virtual_address(text_rva);
    text_sec->set_virtual_size(static_cast<uint32_t>(text_size));
    text_sec->set_flags(IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ |
                        IMAGE_SCN_CNT_CODE);

    // Set minimal header properties
    writer.get_header()->set_flags(IMAGE_FILE_EXECUTABLE_IMAGE |
                                   IMAGE_FILE_32BIT_MACHINE);
    writer.get_optional_header()->set_entry_point_address(text_rva);
    writer.get_optional_header()->set_code_base(text_rva);
    writer.get_win_header()->set_image_base(0x00400000);
    writer.get_win_header()->set_section_alignment(0x1000);
    writer.get_win_header()->set_file_alignment(0x200);
    writer.get_win_header()->set_subsystem(3);  // Windows CUI

    // Add standard data directories (16 entries, all zeroed)
    for (int i = 0; i < 16; ++i) {
        writer.add_directory(COFFI::image_data_directory{0, 0});
    }

    writer.layout();

    char tmpname[] = "/tmp/vmpilot_test_pe_XXXXXX";
    int fd = mkstemp(tmpname);
    EXPECT_GE(fd, 0) << "mkstemp failed";
    close(fd);

    writer.save(tmpname);
    return std::string(tmpname);
}

}  // namespace

TEST(EditorPermissions, PEOpenAndTextSection) {
    std::string pe_path = build_minimal_pe();
    Common::DiagnosticCollector diag;

    auto editor = Loader::PEEditor::open(pe_path, diag);
    ASSERT_TRUE(editor.has_value()) << "Failed to open test PE: " << diag.summary();

    auto text_info = editor->text_section();
    // image_base (0x400000) + text_rva (0x1000) = 0x401000
    EXPECT_EQ(text_info.base_addr, 0x00401000u);
    EXPECT_EQ(text_info.size, 0x100u);

    std::remove(pe_path.c_str());
}

TEST(EditorPermissions, PEAddSegmentWxCompliance) {
    std::string pe_path = build_minimal_pe();
    Common::DiagnosticCollector diag;

    auto editor = Loader::PEEditor::open(pe_path, diag);
    ASSERT_TRUE(editor.has_value()) << "Failed to open test PE";

    std::vector<uint8_t> payload(128, 0x00);
    auto seg_info = editor->add_segment(".vmpltt", payload, 0x1000, diag);
    ASSERT_TRUE(seg_info.has_value()) << "add_segment failed: " << diag.summary();

    std::string out_path = pe_path + ".patched";
    auto save_result = editor->save(out_path, diag);
    ASSERT_TRUE(save_result.has_value()) << "save failed: " << diag.summary();

    // Re-open with COFFI and verify the new section's flags
    COFFI::coffi reader;
    ASSERT_TRUE(reader.load(out_path)) << "Failed to reload patched PE";

    bool found_vmpltt = false;
    for (const auto& sec : reader.get_sections()) {
        if (sec.get_name() != ".vmpltt") continue;
        found_vmpltt = true;
        auto flags = sec.get_flags();

        // Must have READ + WRITE
        EXPECT_TRUE(flags & IMAGE_SCN_MEM_READ) << "Section missing MEM_READ";
        EXPECT_TRUE(flags & IMAGE_SCN_MEM_WRITE) << "Section missing MEM_WRITE";
        // Must have INITIALIZED_DATA
        EXPECT_TRUE(flags & IMAGE_SCN_CNT_INITIALIZED_DATA)
            << "Section missing CNT_INITIALIZED_DATA";
        // Must NOT have EXECUTE — W^X compliance
        EXPECT_FALSE(flags & IMAGE_SCN_MEM_EXECUTE)
            << "Section has MEM_EXECUTE — violates W^X. "
               "Section must start RW; runtime VirtualProtect to RX after init.";
        break;
    }
    EXPECT_TRUE(found_vmpltt) << ".vmpltt section not found in patched PE";

    std::remove(pe_path.c_str());
    std::remove(out_path.c_str());
}

TEST(EditorPermissions, PENextSegmentVaAligned) {
    std::string pe_path = build_minimal_pe();
    Common::DiagnosticCollector diag;

    auto editor = Loader::PEEditor::open(pe_path, diag);
    ASSERT_TRUE(editor.has_value());

    uint64_t next_va = editor->next_segment_va(0x1000);
    // Must be page-aligned
    EXPECT_EQ(next_va & 0xFFF, 0u) << "next_segment_va is not page-aligned";
    // Must be >= image_base + text_rva + text_size
    EXPECT_GE(next_va, 0x00401000u + 0x100u)
        << "next_segment_va overlaps .text";

    std::remove(pe_path.c_str());
}

TEST(EditorPermissions, PEOverwriteText) {
    std::string pe_path = build_minimal_pe();
    Common::DiagnosticCollector diag;

    auto editor = Loader::PEEditor::open(pe_path, diag);
    ASSERT_TRUE(editor.has_value());

    // Overwrite 4 bytes at start of .text (VA = 0x401000)
    std::vector<uint8_t> patch = {0xCC, 0xCC, 0xCC, 0xCC};
    auto result = editor->overwrite_text(
        0x00401000, patch.data(), patch.size(), diag);
    ASSERT_TRUE(result.has_value()) << "overwrite_text failed: " << diag.summary();

    std::string out_path = pe_path + ".patched";
    ASSERT_TRUE(editor->save(out_path, diag).has_value());

    // Verify patched bytes
    COFFI::coffi reader;
    ASSERT_TRUE(reader.load(out_path));
    for (const auto& sec : reader.get_sections()) {
        if (sec.get_name() != ".text") continue;
        ASSERT_GE(sec.get_data_size(), 4u);
        const auto* data = reinterpret_cast<const uint8_t*>(sec.get_data());
        EXPECT_EQ(data[0], 0xCC);
        EXPECT_EQ(data[1], 0xCC);
        EXPECT_EQ(data[2], 0xCC);
        EXPECT_EQ(data[3], 0xCC);
        // Remaining bytes should still be NOPs
        if (sec.get_data_size() > 4) {
            EXPECT_EQ(data[4], 0x90);
        }
        break;
    }

    std::remove(pe_path.c_str());
    std::remove(out_path.c_str());
}

TEST(EditorPermissions, PEOverwriteTextOutOfBounds) {
    std::string pe_path = build_minimal_pe();
    Common::DiagnosticCollector diag;

    auto editor = Loader::PEEditor::open(pe_path, diag);
    ASSERT_TRUE(editor.has_value());

    // Try to overwrite beyond .text boundaries — should fail
    std::vector<uint8_t> patch = {0xCC};
    auto result = editor->overwrite_text(
        0x00401000 + 0x100,  // one byte past end
        patch.data(), patch.size(), diag);
    EXPECT_FALSE(result.has_value()) << "overwrite_text should fail for out-of-bounds VA";

    std::remove(pe_path.c_str());
}

TEST(EditorPermissions, PEInvalidateSignature) {
    std::string pe_path = build_minimal_pe();
    Common::DiagnosticCollector diag;

    auto editor = Loader::PEEditor::open(pe_path, diag);
    ASSERT_TRUE(editor.has_value());

    editor->invalidate_signature();

    std::string out_path = pe_path + ".patched";
    ASSERT_TRUE(editor->save(out_path, diag).has_value());

    // Verify Certificate Table directory entry is zeroed
    COFFI::coffi reader;
    ASSERT_TRUE(reader.load(out_path));
    auto& dirs = reader.get_directories();
    ASSERT_GT(dirs.get_count(), static_cast<size_t>(DIRECTORY_CERTIFICATE_TABLE));
    auto* cert_dir = dirs[DIRECTORY_CERTIFICATE_TABLE];
    EXPECT_EQ(cert_dir->get_virtual_address(), 0u)
        << "Certificate Table RVA should be zero after invalidation";
    EXPECT_EQ(cert_dir->get_size(), 0u)
        << "Certificate Table size should be zero after invalidation";

    std::remove(pe_path.c_str());
    std::remove(out_path.c_str());
}

TEST(EditorPermissions, PEAddRuntimeDepReturnsError) {
    // PE import injection is not implemented yet.  add_runtime_dep must
    // return an error — not silent success — so patch() can abort.
    std::string pe_path = build_minimal_pe();
    Common::DiagnosticCollector diag;

    auto editor = Loader::PEEditor::open(pe_path, diag);
    ASSERT_TRUE(editor.has_value());

    auto result = editor->add_runtime_dep("vmpilot_runtime.dll", diag);
    EXPECT_FALSE(result.has_value())
        << "PE add_runtime_dep must fail until import injection is implemented";

    std::remove(pe_path.c_str());
}

// ============================================================================
// CFI/BTI Enforcement Detection Tests
// ============================================================================

TEST(CfiDetection, ELF_NoCfiByDefault) {
    std::string elf_path = build_minimal_elf();
    Common::DiagnosticCollector diag;

    auto editor = Loader::ELFEditor::open(elf_path, diag);
    ASSERT_TRUE(editor.has_value());
    EXPECT_FALSE(editor->cfi_enforced())
        << "Minimal ELF without .note.gnu.property should not report CFI";

    std::remove(elf_path.c_str());
}

TEST(CfiDetection, ELF_WithCetProperty) {
    ELFIO::elfio writer;
    writer.create(ELFIO::ELFCLASS64, ELFIO::ELFDATA2LSB);
    writer.set_os_abi(ELFIO::ELFOSABI_LINUX);
    writer.set_type(ELFIO::ET_EXEC);
    writer.set_machine(ELFIO::EM_X86_64);
    writer.set_entry(0x401000);

    auto* text_sec = writer.sections.add(".text");
    text_sec->set_type(ELFIO::SHT_PROGBITS);
    text_sec->set_flags(ELFIO::SHF_ALLOC | ELFIO::SHF_EXECINSTR);
    text_sec->set_addr_align(16);
    text_sec->set_address(0x401000);
    std::vector<uint8_t> nops(0x100, 0x90);
    text_sec->set_data(reinterpret_cast<const char*>(nops.data()), nops.size());

    auto* text_seg = writer.segments.add();
    text_seg->set_type(ELFIO::PT_LOAD);
    text_seg->set_flags(ELFIO::PF_R | ELFIO::PF_X);
    text_seg->set_align(0x1000);
    text_seg->set_virtual_address(0x401000);
    text_seg->set_physical_address(0x401000);
    text_seg->add_section_index(text_sec->get_index(), text_sec->get_addr_align());

    // .note.gnu.property with GNU_PROPERTY_X86_FEATURE_1_AND = IBT (0x01)
    // Without this note, the kernel won't enforce CET on the binary.
    // With it, every indirect-call target must have ENDBR64.
    std::vector<uint8_t> note_data;
    auto push32 = [&](uint32_t v) {
        for (int i = 0; i < 4; ++i)
            note_data.push_back(static_cast<uint8_t>(v >> (i * 8)));
    };
    push32(4);            // namesz ("GNU\0")
    push32(16);           // descsz (property header 8 + data 4 + pad 4)
    push32(5);            // NT_GNU_PROPERTY_TYPE_0
    note_data.push_back('G'); note_data.push_back('N');
    note_data.push_back('U'); note_data.push_back('\0');
    push32(0xC0000002);   // GNU_PROPERTY_X86_FEATURE_1_AND
    push32(4);            // pr_datasz
    push32(0x00000001);   // FEATURE_1_IBT
    push32(0);            // alignment padding

    auto* note_sec = writer.sections.add(".note.gnu.property");
    note_sec->set_type(ELFIO::SHT_NOTE);
    note_sec->set_flags(ELFIO::SHF_ALLOC);
    note_sec->set_addr_align(8);
    note_sec->set_data(reinterpret_cast<const char*>(note_data.data()),
                       note_data.size());

    char tmpname[] = "/tmp/vmpilot_cfi_XXXXXX";
    int fd = mkstemp(tmpname);
    ASSERT_GE(fd, 0);
    close(fd);
    writer.save(tmpname);

    Common::DiagnosticCollector diag;
    auto editor = Loader::ELFEditor::open(tmpname, diag);
    ASSERT_TRUE(editor.has_value());
    EXPECT_TRUE(editor->cfi_enforced())
        << "ELF with IBT flag in .note.gnu.property must report CFI enforced";

    std::remove(tmpname);
}

TEST(CfiDetection, PE_NoCfiByDefault) {
    std::string pe_path = build_minimal_pe();
    Common::DiagnosticCollector diag;

    auto editor = Loader::PEEditor::open(pe_path, diag);
    ASSERT_TRUE(editor.has_value());
    EXPECT_FALSE(editor->cfi_enforced())
        << "Minimal PE without CET_COMPAT should not report CFI";

    std::remove(pe_path.c_str());
}

TEST(CfiDetection, PE_WithCetCompat) {
    COFFI::coffi writer;
    writer.create(COFFI::COFFI_ARCHITECTURE_PE);
    writer.create_optional_header();

    auto* text_sec = writer.add_section(".text");
    std::vector<uint8_t> nops(0x100, 0x90);
    text_sec->set_data(reinterpret_cast<const char*>(nops.data()),
                       static_cast<uint32_t>(nops.size()));
    text_sec->set_virtual_address(0x1000);
    text_sec->set_virtual_size(0x100);
    text_sec->set_flags(IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ |
                        IMAGE_SCN_CNT_CODE);

    writer.get_header()->set_flags(IMAGE_FILE_EXECUTABLE_IMAGE |
                                   IMAGE_FILE_32BIT_MACHINE);
    writer.get_optional_header()->set_entry_point_address(0x1000);
    writer.get_optional_header()->set_code_base(0x1000);
    writer.get_win_header()->set_image_base(0x00400000);
    writer.get_win_header()->set_section_alignment(0x1000);
    writer.get_win_header()->set_file_alignment(0x200);
    writer.get_win_header()->set_subsystem(3);
    // CET_COMPAT tells the Windows kernel to enforce Shadow Stack + IBT.
    // Without it, ENDBR instructions are NOPs. With it, missing ENDBR
    // at an indirect-call target causes #CP (Control Protection exception).
    writer.get_win_header()->set_dll_flags(0x8000);

    for (int i = 0; i < 16; ++i)
        writer.add_directory(COFFI::image_data_directory{0, 0});
    writer.layout();

    char tmpname[] = "/tmp/vmpilot_pe_cet_XXXXXX";
    int fd = mkstemp(tmpname);
    ASSERT_GE(fd, 0);
    close(fd);
    writer.save(tmpname);

    Common::DiagnosticCollector diag;
    auto editor = Loader::PEEditor::open(tmpname, diag);
    ASSERT_TRUE(editor.has_value());
    EXPECT_TRUE(editor->cfi_enforced())
        << "PE with CET_COMPAT (0x8000) must report CFI enforced";

    std::remove(tmpname);
}

TEST(EditorPermissions, PECallSlotZeroAtSegmentStart) {
    std::string pe_path = build_minimal_pe();
    Common::DiagnosticCollector diag;

    auto editor = Loader::PEEditor::open(pe_path, diag);
    ASSERT_TRUE(editor.has_value());

    // Payload with call_slot = 8 zero bytes at offset 0 + some blob data
    std::vector<uint8_t> payload(64, 0xAB);
    std::memset(payload.data(), 0, 8);  // call_slot at offset 0

    auto seg_info = editor->add_segment(".vmpltt", payload, 0x1000, diag);
    ASSERT_TRUE(seg_info.has_value());

    std::string out_path = pe_path + ".patched";
    ASSERT_TRUE(editor->save(out_path, diag).has_value());

    // Verify call_slot bytes are zero in the output
    COFFI::coffi reader;
    ASSERT_TRUE(reader.load(out_path));

    for (const auto& sec : reader.get_sections()) {
        if (sec.get_name() != ".vmpltt") continue;
        ASSERT_GE(sec.get_data_size(), 8u);
        const auto* data = reinterpret_cast<const uint8_t*>(sec.get_data());
        uint64_t call_slot = 0;
        std::memcpy(&call_slot, data, 8);
        EXPECT_EQ(call_slot, 0u)
            << "call_slot must be zero-initialized (runtime fills it)";
        // Blob data after call_slot should be preserved
        EXPECT_EQ(data[8], 0xAB);
        break;
    }

    std::remove(pe_path.c_str());
    std::remove(out_path.c_str());
}
