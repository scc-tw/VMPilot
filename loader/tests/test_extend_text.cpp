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

#include <cstdio>
#include <cstring>
#include <vector>

using namespace VMPilot;
using namespace VMPilot::Loader;
using DC = Common::DiagnosticCode;

// ============================================================================
// Helpers
// ============================================================================

namespace {

constexpr uint64_t TEXT_VA   = 0x401000;
constexpr uint64_t TEXT_SIZE = 0x1000;

/// Build a minimal valid ELF64 binary with a .text section.
std::string build_minimal_elf() {
    ELFIO::elfio writer;
    writer.create(ELFIO::ELFCLASS64, ELFIO::ELFDATA2LSB);
    writer.set_os_abi(ELFIO::ELFOSABI_LINUX);
    writer.set_type(ELFIO::ET_EXEC);
    writer.set_machine(ELFIO::EM_X86_64);
    writer.set_entry(TEXT_VA);

    // .text filled with NOPs
    auto* text_sec = writer.sections.add(".text");
    text_sec->set_type(ELFIO::SHT_PROGBITS);
    text_sec->set_flags(ELFIO::SHF_ALLOC | ELFIO::SHF_EXECINSTR);
    text_sec->set_addr_align(16);
    text_sec->set_address(TEXT_VA);
    std::vector<uint8_t> nops(TEXT_SIZE, 0x90);
    text_sec->set_data(reinterpret_cast<const char*>(nops.data()), nops.size());

    // PT_LOAD for .text (RX)
    auto* text_seg = writer.segments.add();
    text_seg->set_type(ELFIO::PT_LOAD);
    text_seg->set_flags(ELFIO::PF_R | ELFIO::PF_X);
    text_seg->set_align(0x1000);
    text_seg->set_virtual_address(TEXT_VA);
    text_seg->set_physical_address(TEXT_VA);
    text_seg->add_section_index(text_sec->get_index(), text_sec->get_addr_align());

    char tmpname[] = "/tmp/vmpilot_ext_XXXXXX";
    int fd = mkstemp(tmpname);
    EXPECT_GE(fd, 0);
    close(fd);
    writer.save(tmpname);
    return std::string(tmpname);
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
    ELFIO::elfio reader;
    ASSERT_TRUE(reader.load(out_path)) << "Failed to reload extended ELF";

    // Find .text section
    const ELFIO::section* text_sec = nullptr;
    for (const auto& sec : reader.sections) {
        if (sec->get_name() == ".text") {
            text_sec = sec.get();
            break;
        }
    }
    ASSERT_NE(text_sec, nullptr) << ".text section not found after extension";

    // .text size should be larger
    EXPECT_GT(text_sec->get_size(), TEXT_SIZE);

    // Original NOP content should be preserved at the start
    const auto* data = reinterpret_cast<const uint8_t*>(text_sec->get_data());
    for (size_t i = 0; i < TEXT_SIZE; ++i) {
        EXPECT_EQ(data[i], 0x90) << "Original .text content corrupted at offset " << i;
    }

    // Payload should appear at the end (after padding)
    const size_t padding = static_cast<size_t>(expected_va - (TEXT_VA + TEXT_SIZE));
    const size_t payload_start = TEXT_SIZE + padding;
    ASSERT_LE(payload_start + payload.size(), text_sec->get_size());
    EXPECT_EQ(std::memcmp(data + payload_start, payload.data(), payload.size()), 0)
        << "Payload data not found at expected offset in extended .text";

    // Verify PT_LOAD segment was updated
    bool found_text_seg = false;
    for (const auto& seg : reader.segments) {
        if (seg->get_type() != ELFIO::PT_LOAD) continue;
        if (seg->get_virtual_address() == TEXT_VA) {
            found_text_seg = true;
            EXPECT_GE(seg->get_memory_size(), TEXT_SIZE + padding + payload.size())
                << "PT_LOAD memsz not updated";
            EXPECT_GE(seg->get_file_size(), TEXT_SIZE + padding + payload.size())
                << "PT_LOAD filesz not updated";
            // Permissions should be preserved (original .text is RX)
            EXPECT_TRUE(seg->get_flags() & ELFIO::PF_R) << "Missing PF_R";
            EXPECT_TRUE(seg->get_flags() & ELFIO::PF_X) << "Missing PF_X";
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

    ELFIO::elfio reader;
    ASSERT_TRUE(reader.load(out_path));

    const ELFIO::section* text_sec = nullptr;
    for (const auto& sec : reader.sections) {
        if (sec->get_name() == ".text") {
            text_sec = sec.get();
            break;
        }
    }
    ASSERT_NE(text_sec, nullptr);

    const auto* data = reinterpret_cast<const uint8_t*>(text_sec->get_data());
    const size_t sec_size = text_sec->get_size();

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
    ELFIO::elfio writer;
    writer.create(ELFIO::ELFCLASS64, ELFIO::ELFDATA2LSB);
    writer.set_os_abi(ELFIO::ELFOSABI_LINUX);
    writer.set_type(ELFIO::ET_EXEC);
    writer.set_machine(ELFIO::EM_X86_64);
    writer.set_entry(TEXT_VA);

    // .text layout (256 bytes total):
    //   [0..31]    real code (0x55 = push rbp)
    //   [32..63]   NOP sled (32 × 0x90)
    //   [64..79]   real code (0x48)
    //   [80..95]   INT3 padding (16 × 0xCC)
    //   [96..111]  real code (0xC3)
    //   [112..119] zero pad (8 × 0x00)
    //   [120..127] real code (0x48)
    //   [128..135] NOP sled (8 × 0x90)
    //   [136..255] real code (0x48)
    std::vector<uint8_t> text(256, 0x48);  // fill with 0x48

    std::memset(text.data() + 32, 0x90, 32);   // NOP sled
    std::memset(text.data() + 80, 0xCC, 16);   // INT3 pad
    std::memset(text.data() + 112, 0x00, 8);   // zero pad
    std::memset(text.data() + 128, 0x90, 8);   // small NOP sled

    auto* text_sec = writer.sections.add(".text");
    text_sec->set_type(ELFIO::SHT_PROGBITS);
    text_sec->set_flags(ELFIO::SHF_ALLOC | ELFIO::SHF_EXECINSTR);
    text_sec->set_addr_align(16);
    text_sec->set_address(TEXT_VA);
    text_sec->set_data(reinterpret_cast<const char*>(text.data()), text.size());

    auto* seg = writer.segments.add();
    seg->set_type(ELFIO::PT_LOAD);
    seg->set_flags(ELFIO::PF_R | ELFIO::PF_X);
    seg->set_align(0x1000);
    seg->set_virtual_address(TEXT_VA);
    seg->set_physical_address(TEXT_VA);
    seg->add_section_index(text_sec->get_index(), text_sec->get_addr_align());

    char tmpname[] = "/tmp/vmpilot_gaps_XXXXXX";
    int fd = mkstemp(tmpname);
    EXPECT_GE(fd, 0);
    close(fd);
    writer.save(tmpname);
    return std::string(tmpname);
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
    COFFI::coffi writer;
    writer.create(COFFI::COFFI_ARCHITECTURE_PE);
    writer.create_optional_header();

    auto* text_sec = writer.add_section(".text");
    std::vector<uint8_t> nops(PE_TEXT_SIZE, 0x90);
    text_sec->set_data(reinterpret_cast<const char*>(nops.data()),
                       static_cast<uint32_t>(nops.size()));
    text_sec->set_virtual_address(PE_TEXT_RVA);
    text_sec->set_virtual_size(static_cast<uint32_t>(PE_TEXT_SIZE));
    text_sec->set_flags(IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ |
                        IMAGE_SCN_CNT_CODE);

    writer.get_header()->set_flags(IMAGE_FILE_EXECUTABLE_IMAGE |
                                   IMAGE_FILE_32BIT_MACHINE);
    writer.get_optional_header()->set_entry_point_address(PE_TEXT_RVA);
    writer.get_optional_header()->set_code_base(PE_TEXT_RVA);
    writer.get_win_header()->set_image_base(PE_IMAGE_BASE);
    writer.get_win_header()->set_section_alignment(0x1000);
    writer.get_win_header()->set_file_alignment(0x200);
    writer.get_win_header()->set_subsystem(3);
    for (int i = 0; i < 16; ++i)
        writer.add_directory(COFFI::image_data_directory{0, 0});
    writer.layout();

    char tmpname[] = "/tmp/vmpilot_pe_ext_XXXXXX";
    int fd = mkstemp(tmpname);
    EXPECT_GE(fd, 0);
    close(fd);
    writer.save(tmpname);
    return std::string(tmpname);
}

std::string build_pe_with_gaps() {
    COFFI::coffi writer;
    writer.create(COFFI::COFFI_ARCHITECTURE_PE);
    writer.create_optional_header();

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

    auto* text_sec = writer.add_section(".text");
    text_sec->set_data(reinterpret_cast<const char*>(text.data()),
                       static_cast<uint32_t>(text.size()));
    text_sec->set_virtual_address(PE_TEXT_RVA);
    text_sec->set_virtual_size(static_cast<uint32_t>(text.size()));
    text_sec->set_flags(IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ |
                        IMAGE_SCN_CNT_CODE);

    writer.get_header()->set_flags(IMAGE_FILE_EXECUTABLE_IMAGE |
                                   IMAGE_FILE_32BIT_MACHINE);
    writer.get_optional_header()->set_entry_point_address(PE_TEXT_RVA);
    writer.get_optional_header()->set_code_base(PE_TEXT_RVA);
    writer.get_win_header()->set_image_base(PE_IMAGE_BASE);
    writer.get_win_header()->set_section_alignment(0x1000);
    writer.get_win_header()->set_file_alignment(0x200);
    writer.get_win_header()->set_subsystem(3);
    for (int i = 0; i < 16; ++i)
        writer.add_directory(COFFI::image_data_directory{0, 0});
    writer.layout();

    char tmpname[] = "/tmp/vmpilot_pe_gaps_XXXXXX";
    int fd = mkstemp(tmpname);
    EXPECT_GE(fd, 0);
    close(fd);
    writer.save(tmpname);
    return std::string(tmpname);
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

    COFFI::coffi reader;
    ASSERT_TRUE(reader.load(out_path));
    for (const auto& sec : reader.get_sections()) {
        if (sec.get_name() != ".text") continue;
        EXPECT_GT(sec.get_data_size(), PE_TEXT_SIZE);
        // Original NOPs preserved at start
        const auto* d = reinterpret_cast<const uint8_t*>(sec.get_data());
        EXPECT_EQ(d[0], 0x90);
        break;
    }

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
