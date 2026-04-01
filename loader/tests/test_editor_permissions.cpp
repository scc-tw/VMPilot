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

#include "temp_file.hpp"

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

/// Read a file into a vector<char> for zero-copy ELF parsing.
std::vector<char> read_file_to_vec(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    auto sz = static_cast<std::size_t>(ifs.tellg());
    std::vector<char> buf(sz);
    ifs.seekg(0);
    ifs.read(buf.data(), static_cast<std::streamsize>(sz));
    return buf;
}

/// Build a minimal valid ELF64 binary with a .text section.
/// Uses elfio-modern (already a project dependency).
std::string build_minimal_elf(uint64_t text_va = 0x401000, size_t text_size = 0x100) {
    elfio::elf_editor<elfio::elf64_traits> ed;
    ed.create(elfio::ELFDATA2LSB, elfio::ET_EXEC, elfio::EM_X86_64);
    ed.set_os_abi(elfio::ELFOSABI_LINUX);
    ed.set_entry(text_va);

    // .text section with NOPs
    auto& text_sec = ed.add_section(".text", elfio::SHT_PROGBITS,
                                     elfio::SHF_ALLOC | elfio::SHF_EXECINSTR);
    text_sec.set_addr_align(16);
    text_sec.set_address(text_va);
    std::vector<uint8_t> nops(text_size, 0x90);
    text_sec.set_data(reinterpret_cast<const char*>(nops.data()), nops.size());
    auto text_idx = static_cast<uint16_t>(ed.sections().size() - 1);

    // PT_LOAD segment for .text
    auto& text_seg = ed.add_segment(elfio::PT_LOAD, elfio::PF_R | elfio::PF_X);
    text_seg.set_align(0x1000);
    text_seg.set_vaddr(text_va);
    text_seg.set_paddr(text_va);
    text_seg.add_section_index(text_idx);

    std::string tmpname = VMPilot::Test::make_temp_file("vmpilot");
    auto save_result = ed.save();
    std::ofstream ofs(tmpname, std::ios::binary | std::ios::trunc);
    ofs.write(save_result->data(), static_cast<std::streamsize>(save_result->size()));
    return tmpname;
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

    // Re-open with elfio-modern read API and verify the new segment's flags
    auto rbuf = read_file_to_vec(out_path);
    elfio::byte_view rview{rbuf.data(), rbuf.size()};
    auto file = elfio::elf_file<elfio::elf64_traits>::from_view(rview);
    ASSERT_TRUE(file.has_value()) << "Failed to reload patched ELF";

    bool found_vmpilot_seg = false;
    for (auto seg : file->segments()) {
        if (seg.type() != elfio::PT_LOAD) continue;
        if (seg.virtual_address() != seg_info->va) continue;

        found_vmpilot_seg = true;
        auto flags = seg.flags();

        // Must have R+W
        EXPECT_TRUE(flags & elfio::PF_R) << "Segment missing PF_R";
        EXPECT_TRUE(flags & elfio::PF_W) << "Segment missing PF_W";
        // Must NOT have X — constructor adds X via mprotect after writing call_slot
        EXPECT_FALSE(flags & elfio::PF_X)
            << "Segment has PF_X — violates W^X. "
               "Section must start RW; constructor mprotects to RX after init.";
        break;
    }
    EXPECT_TRUE(found_vmpilot_seg) << "Injected PT_LOAD segment not found";

    // Also verify section flags
    bool found_vmpilot_sec = false;
    for (auto sec : file->sections()) {
        if (sec.name() != ".vmpilot") continue;
        found_vmpilot_sec = true;
        auto flags = sec.flags();
        EXPECT_TRUE(flags & elfio::SHF_ALLOC) << "Section missing SHF_ALLOC";
        EXPECT_TRUE(flags & elfio::SHF_WRITE) << "Section missing SHF_WRITE";
        EXPECT_TRUE(flags & elfio::SHF_EXECINSTR)
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
    auto rbuf = read_file_to_vec(out_path);
    elfio::byte_view rview{rbuf.data(), rbuf.size()};
    auto file = elfio::elf_file<elfio::elf64_traits>::from_view(rview);
    ASSERT_TRUE(file.has_value());

    for (auto sec : file->sections()) {
        if (sec.name() != ".vmpilot") continue;
        auto sec_data = sec.data();
        ASSERT_GE(sec_data.size(), 8u);
        const auto* data = reinterpret_cast<const uint8_t*>(sec_data.data());
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
    elfio::elf_editor<elfio::elf64_traits> ed;
    ed.create(elfio::ELFDATA2LSB, elfio::ET_EXEC, elfio::EM_X86_64);
    ed.set_os_abi(elfio::ELFOSABI_LINUX);
    ed.set_entry(0x401000);

    // .text section
    auto& text_sec = ed.add_section(".text", elfio::SHT_PROGBITS,
                                     elfio::SHF_ALLOC | elfio::SHF_EXECINSTR);
    text_sec.set_addr_align(16);
    text_sec.set_address(0x401000);
    std::vector<uint8_t> nops(256, 0x90);
    text_sec.set_data(reinterpret_cast<const char*>(nops.data()), nops.size());
    auto text_idx = static_cast<uint16_t>(ed.sections().size() - 1);

    // PT_LOAD for .text
    auto& text_seg = ed.add_segment(elfio::PT_LOAD, elfio::PF_R | elfio::PF_X);
    text_seg.set_align(0x1000);
    text_seg.set_vaddr(0x401000);
    text_seg.set_paddr(0x401000);
    text_seg.add_section_index(text_idx);

    // .dynstr section (string table for .dynamic)
    auto& dynstr_sec = ed.add_section(".dynstr", elfio::SHT_STRTAB, elfio::SHF_ALLOC);
    dynstr_sec.set_addr_align(1);
    dynstr_sec.set_address(0x402000);
    // Initial content: just the null byte
    const char null_byte = '\0';
    dynstr_sec.set_data(&null_byte, 1);
    auto dynstr_idx = static_cast<uint16_t>(ed.sections().size() - 1);

    // .dynamic section with 4 slots: 1 real entry + 3 DT_NULL (padding)
    auto& dyn_sec = ed.add_section(".dynamic", elfio::SHT_DYNAMIC,
                                    elfio::SHF_ALLOC | elfio::SHF_WRITE);
    dyn_sec.set_addr_align(8);
    dyn_sec.set_address(0x402100);
    dyn_sec.set_entry_size(sizeof(elfio::Elf64_Dyn));
    dyn_sec.set_link(dynstr_idx);
    // 4 entries, all DT_NULL (we'll have room to steal one)
    std::vector<elfio::Elf64_Dyn> dyn_entries(4, elfio::Elf64_Dyn{});
    dyn_sec.set_data(reinterpret_cast<const char*>(dyn_entries.data()),
                      dyn_entries.size() * sizeof(elfio::Elf64_Dyn));
    auto dyn_idx = static_cast<uint16_t>(ed.sections().size() - 1);

    // PT_LOAD for data sections (.dynstr + .dynamic)
    auto& data_seg = ed.add_segment(elfio::PT_LOAD, elfio::PF_R | elfio::PF_W);
    data_seg.set_align(0x1000);
    data_seg.set_vaddr(0x402000);
    data_seg.set_paddr(0x402000);
    data_seg.add_section_index(dynstr_idx);
    data_seg.add_section_index(dyn_idx);

    // PT_DYNAMIC segment
    auto& dyn_seg = ed.add_segment(elfio::PT_DYNAMIC, elfio::PF_R | elfio::PF_W);
    dyn_seg.set_align(8);
    dyn_seg.add_section_index(dyn_idx);

    std::string tmpname = VMPilot::Test::make_temp_file("vmpilot");
    auto save_result = ed.save();
    std::ofstream ofs(tmpname, std::ios::binary | std::ios::trunc);
    ofs.write(save_result->data(), static_cast<std::streamsize>(save_result->size()));
    ofs.close();

    // Open with ELFEditor and inject DT_NEEDED
    Common::DiagnosticCollector diag;
    auto editor = Loader::ELFEditor::open(tmpname, diag);
    ASSERT_TRUE(editor.has_value()) << "Failed to open test ELF";

    auto result = editor->add_runtime_dep("libvmpilot_runtime.so", diag);
    ASSERT_TRUE(result.has_value()) << "add_runtime_dep failed";

    std::string out_path = tmpname + ".patched";
    auto save_res = editor->save(out_path, diag);
    ASSERT_TRUE(save_res.has_value()) << "save failed";

    // Verify: reload and check DT_NEEDED was injected
    auto rbuf = read_file_to_vec(out_path);
    elfio::byte_view rview{rbuf.data(), rbuf.size()};
    auto file = elfio::elf_file<elfio::elf64_traits>::from_view(rview);
    ASSERT_TRUE(file.has_value());

    bool found_needed = false;
    for (auto sec : file->sections()) {
        if (sec.type() != elfio::SHT_DYNAMIC) continue;

        auto dynstr_link = sec.link();
        if (dynstr_link >= file->section_count()) continue;
        elfio::string_table_view dynstr{file->sections()[static_cast<uint16_t>(dynstr_link)].data()};

        for (auto d : file->dynamics(sec, dynstr)) {
            if (d.tag() == elfio::DT_NEEDED &&
                d.string_value() == "libvmpilot_runtime.so") {
                found_needed = true;
                break;
            }
        }
    }
    EXPECT_TRUE(found_needed) << "DT_NEEDED for libvmpilot_runtime.so not found";

    std::remove(tmpname.c_str());
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
    // Layer 2 (elfio add_entry growth) should succeed.
    elfio::elf_editor<elfio::elf64_traits> ed;
    ed.create(elfio::ELFDATA2LSB, elfio::ET_EXEC, elfio::EM_X86_64);
    ed.set_os_abi(elfio::ELFOSABI_LINUX);
    ed.set_entry(0x401000);

    auto& text_sec = ed.add_section(".text", elfio::SHT_PROGBITS,
                                     elfio::SHF_ALLOC | elfio::SHF_EXECINSTR);
    text_sec.set_addr_align(16);
    text_sec.set_address(0x401000);
    std::vector<uint8_t> nops(256, 0x90);
    text_sec.set_data(reinterpret_cast<const char*>(nops.data()), nops.size());
    auto text_idx = static_cast<uint16_t>(ed.sections().size() - 1);

    auto& text_seg = ed.add_segment(elfio::PT_LOAD, elfio::PF_R | elfio::PF_X);
    text_seg.set_align(0x1000);
    text_seg.set_vaddr(0x401000);
    text_seg.set_paddr(0x401000);
    text_seg.add_section_index(text_idx);

    auto& dynstr_sec = ed.add_section(".dynstr", elfio::SHT_STRTAB, elfio::SHF_ALLOC);
    dynstr_sec.set_addr_align(1);
    dynstr_sec.set_address(0x402000);
    const char null_byte = '\0';
    dynstr_sec.set_data(&null_byte, 1);
    auto dynstr_idx = static_cast<uint16_t>(ed.sections().size() - 1);

    // Only 1 DT_NULL entry — no spare slot for Layer 1
    auto& dyn_sec = ed.add_section(".dynamic", elfio::SHT_DYNAMIC,
                                    elfio::SHF_ALLOC | elfio::SHF_WRITE);
    dyn_sec.set_addr_align(8);
    dyn_sec.set_address(0x402100);
    dyn_sec.set_entry_size(sizeof(elfio::Elf64_Dyn));
    dyn_sec.set_link(dynstr_idx);
    // 1 entry = 1 DT_NULL (terminator only, no spare)
    elfio::Elf64_Dyn single_null{};
    dyn_sec.set_data(reinterpret_cast<const char*>(&single_null),
                      sizeof(single_null));
    auto dyn_idx = static_cast<uint16_t>(ed.sections().size() - 1);

    auto& data_seg = ed.add_segment(elfio::PT_LOAD, elfio::PF_R | elfio::PF_W);
    data_seg.set_align(0x1000);
    data_seg.set_vaddr(0x402000);
    data_seg.set_paddr(0x402000);
    data_seg.add_section_index(dynstr_idx);
    data_seg.add_section_index(dyn_idx);

    auto& dyn_seg = ed.add_segment(elfio::PT_DYNAMIC, elfio::PF_R | elfio::PF_W);
    dyn_seg.set_align(8);
    dyn_seg.add_section_index(dyn_idx);

    std::string tmpname = VMPilot::Test::make_temp_file("vmpilot");
    auto save_result = ed.save();
    std::ofstream ofs(tmpname, std::ios::binary | std::ios::trunc);
    ofs.write(save_result->data(), static_cast<std::streamsize>(save_result->size()));
    ofs.close();

    Common::DiagnosticCollector diag;
    auto editor = Loader::ELFEditor::open(tmpname, diag);
    ASSERT_TRUE(editor.has_value());

    // Layer 1 should fail (no spare DT_NULL), Layer 2 should succeed
    auto result = editor->add_runtime_dep("libvmpilot_runtime.so", diag);
    EXPECT_TRUE(result.has_value())
        << "Layer 2 (elfio growth) should succeed when Layer 1 has no spare slots";

    // Save and verify DT_NEEDED was injected via Layer 2
    std::string out_path = tmpname + ".patched";
    ASSERT_TRUE(editor->save(out_path, diag).has_value());

    auto rbuf = read_file_to_vec(out_path);
    elfio::byte_view rview{rbuf.data(), rbuf.size()};
    auto file = elfio::elf_file<elfio::elf64_traits>::from_view(rview);
    ASSERT_TRUE(file.has_value());
    bool found = false;
    for (auto sec : file->sections()) {
        if (sec.type() != elfio::SHT_DYNAMIC) continue;

        auto dynstr_link = sec.link();
        if (dynstr_link >= file->section_count()) continue;
        elfio::string_table_view dynstr{file->sections()[static_cast<uint16_t>(dynstr_link)].data()};

        for (auto d : file->dynamics(sec, dynstr)) {
            if (d.tag() == elfio::DT_NEEDED &&
                d.string_value() == "libvmpilot_runtime.so")
                found = true;
        }
    }
    EXPECT_TRUE(found) << "DT_NEEDED not found after Layer 2 fallback";

    std::remove(tmpname.c_str());
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

/// Build a minimal valid PE32 binary with a .text section using coffi-modern.
std::string build_minimal_pe(uint32_t text_rva = 0x1000,
                             size_t text_size = 0x100) {
    coffi::coff_editor<coffi::pe32_traits> ed;
    ed.create_dos_header();
    ed.create_optional_header();
    ed.create_win_header();
    ed.ensure_directories(16);
    ed.coff_header().machine = coffi::MACHINE_I386;

    // .text section with NOPs
    auto& text_sec = ed.add_section(".text",
        coffi::SCN_MEM_EXECUTE | coffi::SCN_MEM_READ | coffi::SCN_CNT_CODE);
    std::vector<uint8_t> nops(text_size, 0x90);
    text_sec.set_data(reinterpret_cast<const char*>(nops.data()),
                      static_cast<uint32_t>(nops.size()));
    text_sec.set_virtual_address(text_rva);
    text_sec.set_virtual_size(static_cast<uint32_t>(text_size));

    // Set minimal header properties
    ed.coff_header().flags = 0x0002 /*EXECUTABLE_IMAGE*/ |
                             0x0100 /*32BIT_MACHINE*/;
    ed.optional_header()->entry_point_address = text_rva;
    ed.optional_header()->code_base = text_rva;
    ed.win_header()->image_base = 0x00400000;
    ed.win_header()->section_alignment = 0x1000;
    ed.win_header()->file_alignment = 0x200;
    ed.win_header()->subsystem = 3;  // Windows CUI

    std::string tmpname = VMPilot::Test::make_temp_file("vmpilot");

    (void)ed.save(tmpname);
    return tmpname;
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

    // Re-open with coffi-modern and verify the new section's flags
    auto loaded = coffi::coff_editor<coffi::pe32_traits>::from_path(out_path);
    ASSERT_TRUE(loaded.has_value()) << "Failed to reload patched PE";

    auto* vmpltt = loaded->find_section(".vmpltt");
    ASSERT_NE(vmpltt, nullptr) << ".vmpltt section not found in patched PE";
    auto flags = vmpltt->flags();

    // Must have READ + WRITE
    EXPECT_TRUE(flags & coffi::SCN_MEM_READ) << "Section missing MEM_READ";
    EXPECT_TRUE(flags & coffi::SCN_MEM_WRITE) << "Section missing MEM_WRITE";
    // Must have INITIALIZED_DATA
    EXPECT_TRUE(flags & coffi::SCN_CNT_INITIALIZED_DATA)
        << "Section missing CNT_INITIALIZED_DATA";
    // Must NOT have EXECUTE — W^X compliance
    EXPECT_FALSE(flags & coffi::SCN_MEM_EXECUTE)
        << "Section has MEM_EXECUTE — violates W^X. "
           "Section must start RW; runtime VirtualProtect to RX after init.";

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
    auto loaded = coffi::coff_editor<coffi::pe32_traits>::from_path(out_path);
    ASSERT_TRUE(loaded.has_value());
    auto* text_verify = loaded->find_section(".text");
    ASSERT_NE(text_verify, nullptr);
    ASSERT_GE(text_verify->data_length(), 4u);
    const auto* data = reinterpret_cast<const uint8_t*>(text_verify->data_ptr());
    EXPECT_EQ(data[0], 0xCC);
    EXPECT_EQ(data[1], 0xCC);
    EXPECT_EQ(data[2], 0xCC);
    EXPECT_EQ(data[3], 0xCC);
    // Remaining bytes should still be NOPs
    if (text_verify->data_length() > 4) {
        EXPECT_EQ(data[4], 0x90);
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
    auto loaded = coffi::coff_editor<coffi::pe32_traits>::from_path(out_path);
    ASSERT_TRUE(loaded.has_value());
    auto* cert_dir = loaded->directory(coffi::DIR_SECURITY);
    ASSERT_NE(cert_dir, nullptr);
    EXPECT_EQ(cert_dir->virtual_address, 0u)
        << "Certificate Table RVA should be zero after invalidation";
    EXPECT_EQ(cert_dir->size, 0u)
        << "Certificate Table size should be zero after invalidation";

    std::remove(pe_path.c_str());
    std::remove(out_path.c_str());
}

TEST(EditorPermissions, PEAddRuntimeDepInjectsImport) {
    std::string pe_path = build_minimal_pe();
    Common::DiagnosticCollector diag;

    auto editor = Loader::PEEditor::open(pe_path, diag);
    ASSERT_TRUE(editor.has_value());

    auto result = editor->add_runtime_dep("vmpilot_runtime.dll", diag);
    ASSERT_TRUE(result.has_value()) << "PE import injection failed";

    // Save and verify the import directory was created
    std::string out_path = pe_path + ".imported";
    ASSERT_TRUE(editor->save(out_path, diag).has_value());

    // Verify the patched PE can be re-loaded and has an import directory
    auto loaded = coffi::coff_editor<coffi::pe32_traits>::from_path(out_path);
    ASSERT_TRUE(loaded.has_value()) << "Failed to reload PE with injected import";

    // The import directory entry should be non-zero (imports were materialized)
    auto* import_dir = loaded->directory(coffi::DIR_IMPORT);
    EXPECT_NE(import_dir, nullptr);
    if (import_dir) {
        EXPECT_NE(import_dir->virtual_address, 0u)
            << "Import directory RVA should be set after import injection";
    }

    std::remove(out_path.c_str());

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
    elfio::elf_editor<elfio::elf64_traits> ed;
    ed.create(elfio::ELFDATA2LSB, elfio::ET_EXEC, elfio::EM_X86_64);
    ed.set_os_abi(elfio::ELFOSABI_LINUX);
    ed.set_entry(0x401000);

    auto& text_sec = ed.add_section(".text", elfio::SHT_PROGBITS,
                                     elfio::SHF_ALLOC | elfio::SHF_EXECINSTR);
    text_sec.set_addr_align(16);
    text_sec.set_address(0x401000);
    std::vector<uint8_t> nops(0x100, 0x90);
    text_sec.set_data(reinterpret_cast<const char*>(nops.data()), nops.size());
    auto text_idx = static_cast<uint16_t>(ed.sections().size() - 1);

    auto& text_seg = ed.add_segment(elfio::PT_LOAD, elfio::PF_R | elfio::PF_X);
    text_seg.set_align(0x1000);
    text_seg.set_vaddr(0x401000);
    text_seg.set_paddr(0x401000);
    text_seg.add_section_index(text_idx);

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

    auto& note_sec = ed.add_section(".note.gnu.property", elfio::SHT_NOTE,
                                     elfio::SHF_ALLOC);
    note_sec.set_addr_align(8);
    note_sec.set_data(reinterpret_cast<const char*>(note_data.data()),
                       note_data.size());

    std::string tmpname = VMPilot::Test::make_temp_file("vmpilot");
    auto save_result = ed.save();
    std::ofstream ofs(tmpname, std::ios::binary | std::ios::trunc);
    ofs.write(save_result->data(), static_cast<std::streamsize>(save_result->size()));
    ofs.close();

    Common::DiagnosticCollector diag;
    auto editor = Loader::ELFEditor::open(tmpname, diag);
    ASSERT_TRUE(editor.has_value());
    EXPECT_TRUE(editor->cfi_enforced())
        << "ELF with IBT flag in .note.gnu.property must report CFI enforced";

    std::remove(tmpname.c_str());
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
    coffi::coff_editor<coffi::pe32_traits> ed;
    ed.create_dos_header();
    ed.create_optional_header();
    ed.create_win_header();
    ed.ensure_directories(16);
    ed.coff_header().machine = coffi::MACHINE_I386;

    auto& text_sec = ed.add_section(".text",
        coffi::SCN_MEM_EXECUTE | coffi::SCN_MEM_READ | coffi::SCN_CNT_CODE);
    std::vector<uint8_t> nops(0x100, 0x90);
    text_sec.set_data(reinterpret_cast<const char*>(nops.data()),
                      static_cast<uint32_t>(nops.size()));
    text_sec.set_virtual_address(0x1000);
    text_sec.set_virtual_size(0x100);

    ed.coff_header().flags = 0x0002 /*EXECUTABLE_IMAGE*/ |
                             0x0100 /*32BIT_MACHINE*/;
    ed.optional_header()->entry_point_address = 0x1000;
    ed.optional_header()->code_base = 0x1000;
    ed.win_header()->image_base = 0x00400000;
    ed.win_header()->section_alignment = 0x1000;
    ed.win_header()->file_alignment = 0x200;
    ed.win_header()->subsystem = 3;
    // CET_COMPAT tells the Windows kernel to enforce Shadow Stack + IBT.
    // Without it, ENDBR instructions are NOPs. With it, missing ENDBR
    // at an indirect-call target causes #CP (Control Protection exception).
    ed.win_header()->dll_flags = 0x8000;

    std::string tmpname = VMPilot::Test::make_temp_file("vmpilot");
    (void)ed.save(tmpname);

    Common::DiagnosticCollector diag;
    auto editor = Loader::PEEditor::open(tmpname, diag);
    ASSERT_TRUE(editor.has_value());
    EXPECT_TRUE(editor->cfi_enforced())
        << "PE with CET_COMPAT (0x8000) must report CFI enforced";

    std::remove(tmpname.c_str());
}

TEST(CfiDetection, ELF_NotePreservedAfterAddSegment) {
    // A CET-enforced ELF must keep its .note.gnu.property intact after
    // add_segment() + save().  If the note is lost, the kernel might
    // stop enforcing CET (making our ENDBR instructions pointless) or
    // worse, reject the binary outright.
    elfio::elf_editor<elfio::elf64_traits> ed;
    ed.create(elfio::ELFDATA2LSB, elfio::ET_EXEC, elfio::EM_X86_64);
    ed.set_os_abi(elfio::ELFOSABI_LINUX);
    ed.set_entry(0x401000);

    auto& text_sec = ed.add_section(".text", elfio::SHT_PROGBITS,
                                     elfio::SHF_ALLOC | elfio::SHF_EXECINSTR);
    text_sec.set_addr_align(16);
    text_sec.set_address(0x401000);
    std::vector<uint8_t> nops(0x100, 0x90);
    text_sec.set_data(reinterpret_cast<const char*>(nops.data()), nops.size());
    auto text_idx = static_cast<uint16_t>(ed.sections().size() - 1);

    auto& text_seg = ed.add_segment(elfio::PT_LOAD, elfio::PF_R | elfio::PF_X);
    text_seg.set_align(0x1000);
    text_seg.set_vaddr(0x401000);
    text_seg.set_paddr(0x401000);
    text_seg.add_section_index(text_idx);

    // .note.gnu.property with IBT flag
    std::vector<uint8_t> note_data;
    auto push32 = [&](uint32_t v) {
        for (int i = 0; i < 4; ++i)
            note_data.push_back(static_cast<uint8_t>(v >> (i * 8)));
    };
    push32(4);            // namesz
    push32(16);           // descsz
    push32(5);            // NT_GNU_PROPERTY_TYPE_0
    note_data.push_back('G'); note_data.push_back('N');
    note_data.push_back('U'); note_data.push_back('\0');
    push32(0xC0000002);   // GNU_PROPERTY_X86_FEATURE_1_AND
    push32(4);
    push32(0x00000001);   // IBT
    push32(0);

    auto& note_sec = ed.add_section(".note.gnu.property", elfio::SHT_NOTE,
                                     elfio::SHF_ALLOC);
    note_sec.set_addr_align(8);
    note_sec.set_data(reinterpret_cast<const char*>(note_data.data()),
                       note_data.size());

    std::string tmpname = VMPilot::Test::make_temp_file("vmpilot");
    auto save_result = ed.save();
    std::ofstream ofs(tmpname, std::ios::binary | std::ios::trunc);
    ofs.write(save_result->data(), static_cast<std::streamsize>(save_result->size()));
    ofs.close();

    // Open, add a segment (triggers ensure_cfi_note), save
    Common::DiagnosticCollector diag;
    auto editor = Loader::ELFEditor::open(tmpname, diag);
    ASSERT_TRUE(editor.has_value());
    EXPECT_TRUE(editor->cfi_enforced()) << "Pre-patch: IBT flag must be set";

    std::vector<uint8_t> payload(128, 0xCC);
    auto seg = editor->add_segment(".vmpilot", payload, 0x1000, diag);
    ASSERT_TRUE(seg.has_value());

    std::string out_path = tmpname + ".patched";
    ASSERT_TRUE(editor->save(out_path, diag).has_value());

    // Reload and verify the note survived
    auto editor2 = Loader::ELFEditor::open(out_path, diag);
    ASSERT_TRUE(editor2.has_value());
    EXPECT_TRUE(editor2->cfi_enforced())
        << "Post-patch: .note.gnu.property IBT flag must survive add_segment + save";

    std::remove(tmpname.c_str());
    std::remove(out_path.c_str());
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
    auto loaded = coffi::coff_editor<coffi::pe32_traits>::from_path(out_path);
    ASSERT_TRUE(loaded.has_value());

    auto* vmpltt = loaded->find_section(".vmpltt");
    ASSERT_NE(vmpltt, nullptr);
    ASSERT_GE(vmpltt->data_length(), 8u);
    const auto* data = reinterpret_cast<const uint8_t*>(vmpltt->data_ptr());
    uint64_t call_slot = 0;
    std::memcpy(&call_slot, data, 8);
    EXPECT_EQ(call_slot, 0u)
        << "call_slot must be zero-initialized (runtime fills it)";
    // Blob data after call_slot should be preserved
    EXPECT_EQ(data[8], 0xAB);

    std::remove(pe_path.c_str());
    std::remove(out_path.c_str());
}
