/// @file test_extend_text.cpp
/// @brief Tests for BinaryEditor::extend_text() — .text extension.
///
/// Verifies:
///   - ELF: extend_text() grows .text section and PT_LOAD segment
///   - Payload data appears at the expected VA at the end of .text
///   - .text section size increases by padding + data size
///   - PT_LOAD segment sizes updated correctly
///   - Original .text content is preserved
///   - Extended region inherits .text permissions (RX)

#include <ELFEditor.hpp>
#include <PEEditor.hpp>
#include <LoaderTypes.hpp>

#include <coffi/coffi.hpp>
#include <elfio/elfio.hpp>

#include <gtest/gtest.h>

#include "temp_file.hpp"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <vector>

using namespace VMPilot;
using namespace VMPilot::Loader;
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

constexpr uint64_t TEXT_VA   = 0x401000;
constexpr uint64_t TEXT_SIZE = 0x1000;

/// Build a minimal valid ELF64 binary with a .text section.
std::string build_minimal_elf() {
    elfio::elf_editor<elfio::elf64_traits> ed;
    ed.create(elfio::ELFDATA2LSB, elfio::ET_EXEC, elfio::EM_X86_64);
    ed.set_os_abi(elfio::ELFOSABI_LINUX);
    ed.set_entry(TEXT_VA);

    // .text filled with NOPs
    auto& text_sec = ed.add_section(".text", elfio::SHT_PROGBITS,
                                     elfio::SHF_ALLOC | elfio::SHF_EXECINSTR);
    text_sec.set_addr_align(16);
    text_sec.set_address(TEXT_VA);
    std::vector<uint8_t> nops(TEXT_SIZE, 0x90);
    text_sec.set_data(reinterpret_cast<const char*>(nops.data()), nops.size());
    auto text_idx = static_cast<uint16_t>(ed.sections().size() - 1);

    // PT_LOAD for .text (RX)
    auto& text_seg = ed.add_segment(elfio::PT_LOAD, elfio::PF_R | elfio::PF_X);
    text_seg.set_align(0x1000);
    text_seg.set_vaddr(TEXT_VA);
    text_seg.set_paddr(TEXT_VA);
    text_seg.add_section_index(text_idx);

    std::string tmpname = VMPilot::Test::make_temp_file("vmpilot");
    auto save_result = ed.save();
    std::ofstream ofs(tmpname, std::ios::binary | std::ios::trunc);
    ofs.write(save_result->data(), static_cast<std::streamsize>(save_result->size()));
    return tmpname;
}

/// Build recognizable test payload data.
std::vector<uint8_t> make_test_payload(size_t size = 256) {
    std::vector<uint8_t> p(size);
    for (size_t i = 0; i < size; ++i)
        p[i] = static_cast<uint8_t>((i * 0x37 + 0x13) & 0xFF);
    return p;
}

}  // namespace

// ============================================================================
// ELF extend_text() Tests
// ============================================================================

TEST(ExtendText, ELF_BasicExtension) {
    auto elf_path = build_minimal_elf();
    Common::DiagnosticCollector diag;

    auto editor = ELFEditor::open(elf_path, diag);
    ASSERT_TRUE(editor.has_value()) << "Failed to open test ELF";

    auto orig_text = editor->text_section();
    EXPECT_EQ(orig_text.base_addr, TEXT_VA);
    EXPECT_EQ(orig_text.size, TEXT_SIZE);

    // Extend .text with 256 bytes, 16-byte aligned
    auto payload = make_test_payload(256);
    auto result = editor->extend_text(payload, 16, diag);
    ASSERT_TRUE(result.has_value()) << "extend_text() failed";

    // The returned VA should be at .text end, aligned to 16
    const uint64_t expected_va = (TEXT_VA + TEXT_SIZE + 15) & ~uint64_t{15};
    EXPECT_EQ(result->va, expected_va);
    EXPECT_EQ(result->size, 256u);

    // .text section should have grown
    auto new_text = editor->text_section();
    EXPECT_EQ(new_text.base_addr, TEXT_VA);
    EXPECT_GT(new_text.size, TEXT_SIZE);

    // Save and verify
    std::string out_path = elf_path + ".extended";
    auto sv = editor->save(out_path, diag);
    ASSERT_TRUE(sv.has_value()) << "save() failed";

    // Reload and verify structure
    auto rbuf = read_file_to_vec(out_path);
    elfio::byte_view rview{rbuf.data(), rbuf.size()};
    auto file = elfio::elf_file<elfio::elf64_traits>::from_view(rview);
    ASSERT_TRUE(file.has_value()) << "Failed to reload extended ELF";

    // Find .text section
    auto text_opt = file->find_section(".text");
    ASSERT_TRUE(text_opt.has_value()) << ".text section not found after extension";
    auto text_sec_data = text_opt->data();

    // .text size should be larger
    EXPECT_GT(text_sec_data.size(), TEXT_SIZE);

    // Original NOP content should be preserved at the start
    const auto* data = reinterpret_cast<const uint8_t*>(text_sec_data.data());
    for (size_t i = 0; i < TEXT_SIZE; ++i) {
        EXPECT_EQ(data[i], 0x90) << "Original .text content corrupted at offset " << i;
    }

    // Payload should appear at the end (after padding)
    const size_t padding = static_cast<size_t>(expected_va - (TEXT_VA + TEXT_SIZE));
    const size_t payload_start = TEXT_SIZE + padding;
    ASSERT_LE(payload_start + payload.size(), text_sec_data.size());
    EXPECT_EQ(std::memcmp(data + payload_start, payload.data(), payload.size()), 0)
        << "Payload data not found at expected offset in extended .text";

    // Verify PT_LOAD segment was updated
    bool found_text_seg = false;
    for (auto seg : file->segments()) {
        if (seg.type() != elfio::PT_LOAD) continue;
        if (seg.virtual_address() == TEXT_VA) {
            found_text_seg = true;
            EXPECT_GE(seg.memory_size(), TEXT_SIZE + padding + payload.size())
                << "PT_LOAD memsz not updated";
            EXPECT_GE(seg.file_size(), TEXT_SIZE + padding + payload.size())
                << "PT_LOAD filesz not updated";
            // Permissions should be preserved (original .text is RX)
            EXPECT_TRUE(seg.flags() & elfio::PF_R) << "Missing PF_R";
            EXPECT_TRUE(seg.flags() & elfio::PF_X) << "Missing PF_X";
            break;
        }
    }
    EXPECT_TRUE(found_text_seg) << "PT_LOAD containing .text not found";

    std::remove(elf_path.c_str());
    std::remove(out_path.c_str());
}

TEST(ExtendText, ELF_PayloadAccessibleAtExpectedVA) {
    auto elf_path = build_minimal_elf();
    Common::DiagnosticCollector diag;

    auto editor = ELFEditor::open(elf_path, diag);
    ASSERT_TRUE(editor.has_value());

    auto payload = make_test_payload(128);
    auto result = editor->extend_text(payload, 16, diag);
    ASSERT_TRUE(result.has_value());

    // The VA returned should be usable for overwrite_text (within new .text range)
    const uint64_t payload_va = result->va;

    // Overwrite a few bytes at the payload VA — should succeed since .text grew
    std::vector<uint8_t> patch = {0xCC, 0xCC, 0xCC, 0xCC};
    auto ow = editor->overwrite_text(payload_va, patch.data(), patch.size(), diag);
    EXPECT_TRUE(ow.has_value())
        << "overwrite_text() at extend_text VA should succeed";

    std::remove(elf_path.c_str());
}

TEST(ExtendText, ELF_PageAlignment) {
    auto elf_path = build_minimal_elf();
    Common::DiagnosticCollector diag;

    auto editor = ELFEditor::open(elf_path, diag);
    ASSERT_TRUE(editor.has_value());

    // Use page-level alignment (4096)
    auto payload = make_test_payload(512);
    auto result = editor->extend_text(payload, 0x1000, diag);
    ASSERT_TRUE(result.has_value());

    // VA should be page-aligned
    EXPECT_EQ(result->va % 0x1000, 0u) << "VA not page-aligned";

    std::remove(elf_path.c_str());
}

TEST(ExtendText, ELF_MultipleExtensions) {
    auto elf_path = build_minimal_elf();
    Common::DiagnosticCollector diag;

    auto editor = ELFEditor::open(elf_path, diag);
    ASSERT_TRUE(editor.has_value());

    // First extension
    auto payload1 = make_test_payload(64);
    auto r1 = editor->extend_text(payload1, 16, diag);
    ASSERT_TRUE(r1.has_value());

    // Second extension — should follow first
    std::vector<uint8_t> payload2(128, 0xAB);
    auto r2 = editor->extend_text(payload2, 16, diag);
    ASSERT_TRUE(r2.has_value());

    // Second VA must be after first
    EXPECT_GE(r2->va, r1->va + r1->size)
        << "Second extension VA overlaps first";

    // Save and verify both payloads are intact
    std::string out_path = elf_path + ".extended";
    auto sv = editor->save(out_path, diag);
    ASSERT_TRUE(sv.has_value());

    auto rbuf2 = read_file_to_vec(out_path);
    elfio::byte_view rview2{rbuf2.data(), rbuf2.size()};
    auto file2 = elfio::elf_file<elfio::elf64_traits>::from_view(rview2);
    ASSERT_TRUE(file2.has_value());

    auto text_opt2 = file2->find_section(".text");
    ASSERT_TRUE(text_opt2.has_value());
    auto text_sec_data2 = text_opt2->data();

    const auto* data = reinterpret_cast<const uint8_t*>(text_sec_data2.data());
    const size_t sec_size = text_sec_data2.size();

    // First payload
    const size_t off1 = static_cast<size_t>(r1->va - TEXT_VA);
    ASSERT_LE(off1 + payload1.size(), sec_size);
    EXPECT_EQ(std::memcmp(data + off1, payload1.data(), payload1.size()), 0)
        << "First payload corrupted after second extension";

    // Second payload
    const size_t off2 = static_cast<size_t>(r2->va - TEXT_VA);
    ASSERT_LE(off2 + payload2.size(), sec_size);
    EXPECT_EQ(std::memcmp(data + off2, payload2.data(), payload2.size()), 0)
        << "Second payload not found at expected offset";

    std::remove(elf_path.c_str());
    std::remove(out_path.c_str());
}

TEST(ExtendText, ELF_SmallPayload) {
    auto elf_path = build_minimal_elf();
    Common::DiagnosticCollector diag;

    auto editor = ELFEditor::open(elf_path, diag);
    ASSERT_TRUE(editor.has_value());

    // Extend with just 1 byte
    std::vector<uint8_t> payload = {0xFF};
    auto result = editor->extend_text(payload, 1, diag);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->size, 1u);

    std::remove(elf_path.c_str());
}

// ============================================================================
// ELF find_text_gaps() Tests
// ============================================================================

namespace {

/// Build an ELF with a .text section containing known gap patterns.
std::string build_elf_with_gaps() {
    elfio::elf_editor<elfio::elf64_traits> ed;
    ed.create(elfio::ELFDATA2LSB, elfio::ET_EXEC, elfio::EM_X86_64);
    ed.set_os_abi(elfio::ELFOSABI_LINUX);
    ed.set_entry(TEXT_VA);

    // .text layout (256 bytes total):
    //   [0..31]    real code (0x55 = push rbp)
    //   [32..63]   NOP sled (32 x 0x90)
    //   [64..79]   real code (0x48)
    //   [80..95]   INT3 padding (16 x 0xCC)
    //   [96..111]  real code (0xC3)
    //   [112..119] zero pad (8 x 0x00)
    //   [120..127] real code (0x48)
    //   [128..135] NOP sled (8 x 0x90)
    //   [136..255] real code (0x48)
    std::vector<uint8_t> text(256, 0x48);  // fill with 0x48

    std::memset(text.data() + 32, 0x90, 32);   // NOP sled
    std::memset(text.data() + 80, 0xCC, 16);   // INT3 pad
    std::memset(text.data() + 112, 0x00, 8);   // zero pad
    std::memset(text.data() + 128, 0x90, 8);   // small NOP sled

    auto& text_sec = ed.add_section(".text", elfio::SHT_PROGBITS,
                                     elfio::SHF_ALLOC | elfio::SHF_EXECINSTR);
    text_sec.set_addr_align(16);
    text_sec.set_address(TEXT_VA);
    text_sec.set_data(reinterpret_cast<const char*>(text.data()), text.size());
    auto text_idx = static_cast<uint16_t>(ed.sections().size() - 1);

    auto& seg = ed.add_segment(elfio::PT_LOAD, elfio::PF_R | elfio::PF_X);
    seg.set_align(0x1000);
    seg.set_vaddr(TEXT_VA);
    seg.set_paddr(TEXT_VA);
    seg.add_section_index(text_idx);

    std::string tmpname = VMPilot::Test::make_temp_file("vmpilot");
    auto save_result = ed.save();
    std::ofstream ofs(tmpname, std::ios::binary | std::ios::trunc);
    ofs.write(save_result->data(), static_cast<std::streamsize>(save_result->size()));
    return tmpname;
}

}  // namespace

TEST(FindTextGaps, ELF_DetectsNopAndInt3Gaps) {
    auto elf_path = build_elf_with_gaps();
    Common::DiagnosticCollector diag;

    auto editor = ELFEditor::open(elf_path, diag);
    ASSERT_TRUE(editor.has_value());

    // min_size=8: should find all 4 gaps (NOP32, INT3_16, ZERO8, NOP8)
    auto gaps = editor->find_text_gaps(8);
    ASSERT_GE(gaps.size(), 4u);

    // Sorted by size descending
    EXPECT_GE(gaps[0].size, gaps[1].size);

    // Verify the 32-byte NOP sled
    bool found_nop32 = false;
    for (const auto& g : gaps) {
        if (g.va == TEXT_VA + 32 && g.size == 32) found_nop32 = true;
    }
    EXPECT_TRUE(found_nop32) << "32-byte NOP sled not found";

    // Verify the 16-byte INT3 pad
    bool found_int3 = false;
    for (const auto& g : gaps) {
        if (g.va == TEXT_VA + 80 && g.size == 16) found_int3 = true;
    }
    EXPECT_TRUE(found_int3) << "16-byte INT3 pad not found";

    std::remove(elf_path.c_str());
}

TEST(FindTextGaps, ELF_MinSizeFilters) {
    auto elf_path = build_elf_with_gaps();
    Common::DiagnosticCollector diag;

    auto editor = ELFEditor::open(elf_path, diag);
    ASSERT_TRUE(editor.has_value());

    auto gaps16 = editor->find_text_gaps(16);
    for (const auto& g : gaps16)
        EXPECT_GE(g.size, 16u);

    auto gaps32 = editor->find_text_gaps(32);
    ASSERT_EQ(gaps32.size(), 1u);
    EXPECT_EQ(gaps32[0].size, 32u);

    EXPECT_TRUE(editor->find_text_gaps(64).empty());

    std::remove(elf_path.c_str());
}

// ============================================================================
// PE extend_text() + find_text_gaps() Tests
// ============================================================================

namespace {

constexpr uint32_t PE_IMAGE_BASE = 0x00400000;
constexpr uint32_t PE_TEXT_RVA   = 0x1000;
constexpr uint64_t PE_TEXT_VA    = PE_IMAGE_BASE + PE_TEXT_RVA;
constexpr size_t   PE_TEXT_SIZE  = 0x200;

std::string build_pe_for_extend() {
    coffi::coff_editor<coffi::pe32_traits> ed;
    ed.create_dos_header();
    ed.create_optional_header();
    ed.create_win_header();
    ed.ensure_directories(16);
    ed.coff_header().machine = coffi::MACHINE_I386;

    auto& text_sec = ed.add_section(".text",
        coffi::SCN_MEM_EXECUTE | coffi::SCN_MEM_READ | coffi::SCN_CNT_CODE);
    std::vector<uint8_t> nops(PE_TEXT_SIZE, 0x90);
    text_sec.set_data(reinterpret_cast<const char*>(nops.data()),
                      static_cast<uint32_t>(nops.size()));
    text_sec.set_virtual_address(PE_TEXT_RVA);
    text_sec.set_virtual_size(static_cast<uint32_t>(PE_TEXT_SIZE));

    ed.coff_header().flags = 0x0002 /*EXECUTABLE_IMAGE*/ |
                             0x0100 /*32BIT_MACHINE*/;
    ed.optional_header()->entry_point_address = PE_TEXT_RVA;
    ed.optional_header()->code_base = PE_TEXT_RVA;
    ed.win_header()->image_base = PE_IMAGE_BASE;
    ed.win_header()->section_alignment = 0x1000;
    ed.win_header()->file_alignment = 0x200;
    ed.win_header()->subsystem = 3;

    std::string tmpname = VMPilot::Test::make_temp_file("vmpilot");
    (void)ed.save(tmpname);
    return tmpname;
}

std::string build_pe_with_gaps() {
    coffi::coff_editor<coffi::pe32_traits> ed;
    ed.create_dos_header();
    ed.create_optional_header();
    ed.create_win_header();
    ed.ensure_directories(16);
    ed.coff_header().machine = coffi::MACHINE_I386;

    // .text with known gap pattern:
    //   [0..31]    code (0x55)
    //   [32..63]   NOP sled (0x90)
    //   [64..79]   code (0x48)
    //   [80..95]   INT3 pad (0xCC)
    //   [96..255]  code (0xC3)
    std::vector<uint8_t> text(256, 0xC3);
    std::memset(text.data(), 0x55, 32);
    std::memset(text.data() + 32, 0x90, 32);
    std::memset(text.data() + 64, 0x48, 16);
    std::memset(text.data() + 80, 0xCC, 16);

    auto& text_sec = ed.add_section(".text",
        coffi::SCN_MEM_EXECUTE | coffi::SCN_MEM_READ | coffi::SCN_CNT_CODE);
    text_sec.set_data(reinterpret_cast<const char*>(text.data()),
                      static_cast<uint32_t>(text.size()));
    text_sec.set_virtual_address(PE_TEXT_RVA);
    text_sec.set_virtual_size(static_cast<uint32_t>(text.size()));

    ed.coff_header().flags = 0x0002 /*EXECUTABLE_IMAGE*/ |
                             0x0100 /*32BIT_MACHINE*/;
    ed.optional_header()->entry_point_address = PE_TEXT_RVA;
    ed.optional_header()->code_base = PE_TEXT_RVA;
    ed.win_header()->image_base = PE_IMAGE_BASE;
    ed.win_header()->section_alignment = 0x1000;
    ed.win_header()->file_alignment = 0x200;
    ed.win_header()->subsystem = 3;

    std::string tmpname = VMPilot::Test::make_temp_file("vmpilot");
    (void)ed.save(tmpname);
    return tmpname;
}

}  // namespace

TEST(ExtendText, PE_BasicExtension) {
    auto pe_path = build_pe_for_extend();
    Common::DiagnosticCollector diag;

    auto editor = PEEditor::open(pe_path, diag);
    ASSERT_TRUE(editor.has_value());

    auto orig_text = editor->text_section();
    EXPECT_EQ(orig_text.base_addr, PE_TEXT_VA);

    auto payload = make_test_payload(128);
    auto result = editor->extend_text(payload, 16, diag);
    ASSERT_TRUE(result.has_value());

    // VA should be at .text end, aligned
    EXPECT_GE(result->va, PE_TEXT_VA + PE_TEXT_SIZE);
    EXPECT_EQ(result->va % 16, 0u);
    EXPECT_EQ(result->size, 128u);

    // .text should have grown
    auto new_text = editor->text_section();
    EXPECT_GT(new_text.size, PE_TEXT_SIZE);

    // overwrite_text at the new VA should succeed
    std::vector<uint8_t> patch = {0xCC, 0xCC};
    auto ow = editor->overwrite_text(result->va, patch.data(), patch.size(), diag);
    EXPECT_TRUE(ow.has_value());

    // Save and reload
    std::string out_path = pe_path + ".extended";
    ASSERT_TRUE(editor->save(out_path, diag).has_value());

    auto reloaded = coffi::coff_editor<coffi::pe32_traits>::from_path(out_path);
    ASSERT_TRUE(reloaded.has_value());
    auto* text_reload = reloaded->find_section(".text");
    ASSERT_NE(text_reload, nullptr);
    EXPECT_GT(text_reload->data_length(), PE_TEXT_SIZE);
    // Original NOPs preserved at start
    const auto* d = reinterpret_cast<const uint8_t*>(text_reload->data_ptr());
    EXPECT_EQ(d[0], 0x90);

    std::remove(pe_path.c_str());
    std::remove(out_path.c_str());
}

TEST(FindTextGaps, PE_DetectsNopAndInt3) {
    auto pe_path = build_pe_with_gaps();
    Common::DiagnosticCollector diag;

    auto editor = PEEditor::open(pe_path, diag);
    ASSERT_TRUE(editor.has_value());

    auto gaps = editor->find_text_gaps(8);
    ASSERT_GE(gaps.size(), 2u) << "Expected NOP sled + INT3 pad";

    // Sorted descending
    EXPECT_GE(gaps[0].size, gaps[1].size);

    // 32-byte NOP sled
    bool found_nop = false;
    for (const auto& g : gaps) {
        if (g.va == PE_TEXT_VA + 32 && g.size == 32) found_nop = true;
    }
    EXPECT_TRUE(found_nop) << "32-byte NOP sled not found";

    // 16-byte INT3 pad
    bool found_int3 = false;
    for (const auto& g : gaps) {
        if (g.va == PE_TEXT_VA + 80 && g.size == 16) found_int3 = true;
    }
    EXPECT_TRUE(found_int3) << "16-byte INT3 pad not found";

    std::remove(pe_path.c_str());
}

TEST(FindTextGaps, PE_MinSizeFilters) {
    auto pe_path = build_pe_with_gaps();
    Common::DiagnosticCollector diag;

    auto editor = PEEditor::open(pe_path, diag);
    ASSERT_TRUE(editor.has_value());

    auto gaps32 = editor->find_text_gaps(32);
    ASSERT_EQ(gaps32.size(), 1u);
    EXPECT_EQ(gaps32[0].size, 32u);

    EXPECT_TRUE(editor->find_text_gaps(64).empty());

    std::remove(pe_path.c_str());
}
