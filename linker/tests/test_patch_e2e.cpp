/// @file test_patch_e2e.cpp
/// @brief End-to-end integration tests: full Loader::patch() pipeline.
///
/// Verifies the complete chain:
///   build_test_blob → PatchRequest → patch() → patched binary on disk
///
/// Structural verification on the patched binary:
///   - .vmpilot section exists with correct payload layout
///   - call_slot at offset 0 is zero (runtime fills it)
///   - Blob data intact at offset 8
///   - Seed data intact after blob
///   - Protected regions overwritten with JMP to stub VA
///   - DT_NEEDED for libvmpilot_runtime.so injected

#include <Loader.hpp>
#include <LoaderTypes.hpp>
#include <MachOStructs.hpp>
#include <PlatformTraits.hpp>

#include <coffi/coffi.hpp>
#include <elfio/elfio.hpp>

#include <fstream>

#include <gtest/gtest.h>

#include "temp_file.hpp"

#include <array>
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

/// Read a file into a vector<char> for zero-copy ELF parsing.
std::vector<char> read_file_to_vec(const std::string& path) {
    std::ifstream ifs(path, std::ios::binary | std::ios::ate);
    auto sz = static_cast<std::size_t>(ifs.tellg());
    std::vector<char> buf(sz);
    ifs.seekg(0);
    ifs.read(buf.data(), static_cast<std::streamsize>(sz));
    return buf;
}

}  // namespace

namespace {

constexpr uint64_t TEXT_VA   = 0x401000;
constexpr uint64_t TEXT_SIZE = 0x1000;

constexpr std::array<uint8_t, SEED_SIZE> TEST_SEED = {
    0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11, 0x22,
    0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0x00,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
};

/// Build a fake blob (not a real encrypted blob — just recognizable bytes).
/// For structural tests, we only need to verify the blob bytes survive
/// the patch pipeline intact.  The blob content doesn't matter.
std::vector<uint8_t> make_fake_blob(size_t size = 256) {
    std::vector<uint8_t> b(size);
    for (size_t i = 0; i < size; ++i)
        b[i] = static_cast<uint8_t>((i * 0x37 + 0x13) & 0xFF);
    return b;
}

/// Build a minimal valid ELF64 binary with a .text section and a
/// .dynamic section (needed for DT_NEEDED injection).
std::string build_test_elf() {
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

    // PT_LOAD for .text
    auto& text_seg = ed.add_segment(elfio::PT_LOAD, elfio::PF_R | elfio::PF_X);
    text_seg.set_align(0x1000);
    text_seg.set_vaddr(TEXT_VA);
    text_seg.set_paddr(TEXT_VA);
    auto text_idx = static_cast<uint16_t>(ed.sections().size() - 1);
    text_seg.add_section_index(text_idx);

    // .dynstr
    auto& dynstr_sec = ed.add_section(".dynstr", elfio::SHT_STRTAB, elfio::SHF_ALLOC);
    dynstr_sec.set_addr_align(1);
    dynstr_sec.set_address(0x402000);
    const char null_byte = '\0';
    dynstr_sec.set_data(&null_byte, 1);
    auto dynstr_idx = static_cast<uint16_t>(ed.sections().size() - 1);

    // .dynamic with 4 DT_NULL padding slots
    auto& dyn_sec = ed.add_section(".dynamic", elfio::SHT_DYNAMIC,
                                    elfio::SHF_ALLOC | elfio::SHF_WRITE);
    dyn_sec.set_addr_align(8);
    dyn_sec.set_address(0x402100);
    dyn_sec.set_entry_size(sizeof(elfio::Elf64_Dyn));
    dyn_sec.set_link(dynstr_idx);
    std::vector<elfio::Elf64_Dyn> dyn_entries(4, elfio::Elf64_Dyn{});
    dyn_sec.set_data(reinterpret_cast<const char*>(dyn_entries.data()),
                      dyn_entries.size() * sizeof(elfio::Elf64_Dyn));
    auto dyn_idx = static_cast<uint16_t>(ed.sections().size() - 1);

    // PT_LOAD for data sections
    auto& data_seg = ed.add_segment(elfio::PT_LOAD, elfio::PF_R | elfio::PF_W);
    data_seg.set_align(0x1000);
    data_seg.set_vaddr(0x402000);
    data_seg.set_paddr(0x402000);
    data_seg.add_section_index(dynstr_idx);
    data_seg.add_section_index(dyn_idx);

    // PT_DYNAMIC
    auto& dyn_seg = ed.add_segment(elfio::PT_DYNAMIC, elfio::PF_R | elfio::PF_W);
    dyn_seg.set_align(8);
    dyn_seg.add_section_index(dyn_idx);

    std::string tmpname = VMPilot::Test::make_temp_file("vmpilot");
    auto save_result = ed.save();
    std::ofstream ofs(tmpname, std::ios::binary | std::ios::trunc);
    ofs.write(save_result->data(), static_cast<std::streamsize>(save_result->size()));
    return tmpname;
}

uint64_t read64_le(const uint8_t* p) {
    uint64_t v;
    std::memcpy(&v, p, 8);
    return v;
}

int32_t read_i32_le(const uint8_t* p) {
    int32_t v;
    std::memcpy(&v, p, 4);
    return v;
}

}  // namespace

// ============================================================================
// Full patch() E2E Tests
// ============================================================================

TEST(PatchE2E, ELF_X86_64_SingleRegion) {
    auto elf_path = build_test_elf();
    std::string out_path = elf_path + ".patched";
    auto blob = make_fake_blob(256);
    Common::DiagnosticCollector diag;

    // The region must be within .text and large enough for a JMP rel32 (5 bytes)
    PatchRequest req;
    req.input_path  = elf_path;
    req.output_path = out_path;
    req.regions     = {{"test_fn", TEXT_VA, 32}};
    req.blob_data   = blob;
    req.stored_seed = TEST_SEED;
    req.arch        = Common::FileArch::X86;
    req.mode        = Common::FileMode::MODE_64;
    req.format      = Common::FileFormat::ELF;

    // --- PATCH ---
    auto result = patch(req, diag);
    ASSERT_TRUE(result.has_value())
        << "patch() failed; diagnostics may contain details";
    EXPECT_EQ(result->regions_patched, 1u);
    EXPECT_EQ(result->blob_bytes_injected, 256u);

    // --- STRUCTURAL VERIFICATION ---
    auto rbuf = read_file_to_vec(out_path);
    elfio::byte_view rview{rbuf.data(), rbuf.size()};
    auto file = elfio::elf_file<elfio::elf64_traits>::from_view(rview);
    ASSERT_TRUE(file.has_value()) << "Failed to reload patched ELF";

    // Find .vmpilot section
    auto vmpilot_opt = file->find_section(".vmpilot");
    ASSERT_TRUE(vmpilot_opt.has_value()) << ".vmpilot section not found";
    auto vmpilot_sec = *vmpilot_opt;
    auto vmpilot_data = vmpilot_sec.data();
    const auto* payload = reinterpret_cast<const uint8_t*>(vmpilot_data.data());
    const size_t payload_size = vmpilot_data.size();

    // call_slot at offset 0 must be zero (runtime constructor fills it)
    ASSERT_GE(payload_size, 8u + blob.size() + SEED_SIZE);
    uint64_t call_slot = read64_le(payload);
    EXPECT_EQ(call_slot, 0u) << "call_slot must be zero-initialized";

    // Blob at offset 8
    EXPECT_EQ(std::memcmp(payload + 8, blob.data(), blob.size()), 0)
        << "Blob data not preserved at offset 8";

    // Seed after blob
    size_t seed_offset = 8 + blob.size();
    EXPECT_EQ(std::memcmp(payload + seed_offset, TEST_SEED.data(), SEED_SIZE), 0)
        << "Seed data not preserved after blob";

    // Verify .text region was overwritten (first byte should be JMP = 0xE9)
    auto text_opt = file->find_section(".text");
    ASSERT_TRUE(text_opt.has_value());
    auto text_sec_data = text_opt->data();
    const auto* text_data = reinterpret_cast<const uint8_t*>(text_sec_data.data());
    EXPECT_EQ(text_data[0], 0xE9) << "Region not overwritten with JMP rel32";

    // Verify JMP target lands inside .vmpilot section
    int32_t jmp_disp = read_i32_le(text_data + 1);
    uint64_t jmp_target = TEXT_VA + 5 + static_cast<uint64_t>(static_cast<int64_t>(jmp_disp));
    uint64_t vmpilot_va = vmpilot_sec.address();
    EXPECT_GE(jmp_target, vmpilot_va)
        << "JMP target before .vmpilot section";
    EXPECT_LT(jmp_target, vmpilot_va + payload_size)
        << "JMP target past .vmpilot section";

    // Verify remaining bytes in region are NOPs (0x90)
    for (size_t i = 5; i < 32; ++i) {
        EXPECT_EQ(text_data[i], 0x90)
            << "Byte " << i << " in region should be NOP";
    }

    // --- W^X CHECK ---
    for (auto seg : file->segments()) {
        if (seg.type() != elfio::PT_LOAD) continue;
        if (seg.virtual_address() != vmpilot_va) continue;
        EXPECT_TRUE(seg.flags() & elfio::PF_R);
        EXPECT_TRUE(seg.flags() & elfio::PF_W);
        EXPECT_FALSE(seg.flags() & elfio::PF_X)
            << ".vmpilot segment must be RW, not RWX (W^X)";
        break;
    }

    // --- DT_NEEDED CHECK ---
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
    EXPECT_TRUE(found_needed) << "DT_NEEDED for libvmpilot_runtime.so missing";

    std::remove(elf_path.c_str());
    std::remove(out_path.c_str());
}

TEST(PatchE2E, ELF_X86_64_MultipleRegions) {
    auto elf_path = build_test_elf();
    std::string out_path = elf_path + ".patched";
    auto blob = make_fake_blob(128);
    Common::DiagnosticCollector diag;

    // Two regions, both within .text
    PatchRequest req;
    req.input_path  = elf_path;
    req.output_path = out_path;
    req.regions     = {
        {"fn_alpha", TEXT_VA,        64},
        {"fn_beta",  TEXT_VA + 0x200, 32},
    };
    req.blob_data   = blob;
    req.stored_seed = TEST_SEED;
    req.arch        = Common::FileArch::X86;
    req.mode        = Common::FileMode::MODE_64;
    req.format      = Common::FileFormat::ELF;

    auto result = patch(req, diag);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->regions_patched, 2u);

    // Both regions should start with JMP (0xE9)
    auto rbuf2 = read_file_to_vec(out_path);
    elfio::byte_view rview2{rbuf2.data(), rbuf2.size()};
    auto file2 = elfio::elf_file<elfio::elf64_traits>::from_view(rview2);
    ASSERT_TRUE(file2.has_value());
    auto text_opt2 = file2->find_section(".text");
    ASSERT_TRUE(text_opt2.has_value());
    auto text_data2 = text_opt2->data();
    const auto* d = reinterpret_cast<const uint8_t*>(text_data2.data());
    EXPECT_EQ(d[0], 0xE9) << "fn_alpha not patched";
    EXPECT_EQ(d[0x200], 0xE9) << "fn_beta not patched";

    std::remove(elf_path.c_str());
    std::remove(out_path.c_str());
}

TEST(PatchE2E, ELF_X86_64_RegionOutsideText) {
    auto elf_path = build_test_elf();
    std::string out_path = elf_path + ".patched";
    Common::DiagnosticCollector diag;

    PatchRequest req;
    req.input_path  = elf_path;
    req.output_path = out_path;
    req.regions     = {{"bad_fn", 0x900000, 32}};  // outside .text
    req.blob_data   = make_fake_blob(64);
    req.stored_seed = TEST_SEED;
    req.arch        = Common::FileArch::X86;
    req.mode        = Common::FileMode::MODE_64;
    req.format      = Common::FileFormat::ELF;

    auto result = patch(req, diag);
    EXPECT_FALSE(result.has_value()) << "patch should fail for region outside .text";

    std::remove(elf_path.c_str());
}

TEST(PatchE2E, ELF_X86_64_RegionTooSmall) {
    auto elf_path = build_test_elf();
    std::string out_path = elf_path + ".patched";
    Common::DiagnosticCollector diag;

    PatchRequest req;
    req.input_path  = elf_path;
    req.output_path = out_path;
    req.regions     = {{"tiny_fn", TEXT_VA, 3}};  // < 5 bytes (min for JMP rel32)
    req.blob_data   = make_fake_blob(64);
    req.stored_seed = TEST_SEED;
    req.arch        = Common::FileArch::X86;
    req.mode        = Common::FileMode::MODE_64;
    req.format      = Common::FileFormat::ELF;

    auto result = patch(req, diag);
    EXPECT_FALSE(result.has_value()) << "patch should fail for region < 5 bytes";

    std::remove(elf_path.c_str());
}

// ============================================================================
// PE E2E Tests
// ============================================================================

namespace {

constexpr uint32_t PE_IMAGE_BASE = 0x00400000;
constexpr uint32_t PE_TEXT_RVA   = 0x1000;
constexpr uint64_t PE_TEXT_VA    = PE_IMAGE_BASE + PE_TEXT_RVA;
constexpr size_t   PE_TEXT_SIZE  = 0x1000;

std::string build_test_pe() {
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

}  // namespace

TEST(PatchE2E, PE_X86_32_SingleRegion) {
    auto pe_path = build_test_pe();
    std::string out_path = pe_path + ".patched";
    auto blob = make_fake_blob(256);
    Common::DiagnosticCollector diag;

    PatchRequest req;
    req.input_path  = pe_path;
    req.output_path = out_path;
    req.regions     = {{"test_fn", PE_TEXT_VA, 32}};
    req.blob_data   = blob;
    req.stored_seed = TEST_SEED;
    req.arch        = Common::FileArch::X86;
    req.mode        = Common::FileMode::MODE_32;
    req.format      = Common::FileFormat::PE;

    auto result = patch(req, diag);
    ASSERT_TRUE(result.has_value()) << "PE patch() failed";
    EXPECT_EQ(result->regions_patched, 1u);
    EXPECT_EQ(result->blob_bytes_injected, 256u);

    // Verify import was injected
    auto loaded = coffi::coff_editor<coffi::pe32_traits>::from_path(out_path);
    ASSERT_TRUE(loaded.has_value()) << "Failed to reload patched PE";
    auto& reader = *loaded;

    auto* vmpltt_sec = reader.find_section(".vmpltt");

    // Verify .vmpltt section exists with payload
    bool found_vmpltt = (vmpltt_sec != nullptr);
    if (found_vmpltt) {
        const auto* data = reinterpret_cast<const uint8_t*>(vmpltt_sec->data_ptr());
        ASSERT_GE(vmpltt_sec->data_length(), 8u + blob.size() + SEED_SIZE);
        uint64_t call_slot = read64_le(data);
        EXPECT_EQ(call_slot, 0u);
        EXPECT_EQ(std::memcmp(data + 8, blob.data(), blob.size()), 0);
    }
    EXPECT_TRUE(found_vmpltt);

    // Verify .text region overwritten with JMP
    auto* text_sec_verify = reader.find_section(".text");
    if (text_sec_verify) {
        const auto* data = reinterpret_cast<const uint8_t*>(text_sec_verify->data_ptr());
        EXPECT_EQ(data[0], 0xE9);
    }

    std::remove(pe_path.c_str());
    std::remove(out_path.c_str());
}

TEST(PatchE2E, PE_X86_32_RegionOutsideText) {
    auto pe_path = build_test_pe();
    std::string out_path = pe_path + ".patched";
    Common::DiagnosticCollector diag;

    PatchRequest req;
    req.input_path  = pe_path;
    req.output_path = out_path;
    req.regions     = {{"bad_fn", 0x900000, 32}};
    req.blob_data   = make_fake_blob(64);
    req.stored_seed = TEST_SEED;
    req.arch        = Common::FileArch::X86;
    req.mode        = Common::FileMode::MODE_32;
    req.format      = Common::FileFormat::PE;

    auto result = patch(req, diag);
    EXPECT_FALSE(result.has_value()) << "patch should fail for region outside .text";

    std::remove(pe_path.c_str());
}

// ============================================================================
// Mach-O E2E Tests
// ============================================================================

namespace {

namespace MO = VMPilot::Loader::MachO;

constexpr uint64_t MACHO_TEXT_VA   = 0x100001000;
constexpr size_t   MACHO_TEXT_SIZE = 0x1000;

/// Build a minimal valid Mach-O ARM64 binary from raw bytes.
/// Layout: mach_header_64 + LC_SEGMENT_64(__TEXT with __text) + header padding + .text data
std::string build_test_macho() {
    // ARM64 NOP = 0xD503201F (4 bytes, little-endian)
    std::vector<uint8_t> nops(MACHO_TEXT_SIZE);
    for (size_t i = 0; i + 3 < nops.size(); i += 4) {
        nops[i]     = 0x1F;
        nops[i + 1] = 0x20;
        nops[i + 2] = 0x03;
        nops[i + 3] = 0xD5;
    }

    // Build __TEXT segment with one __text section
    MO::segment_command_64 text_seg{};
    text_seg.cmd      = MO::LC_SEGMENT_64;
    text_seg.cmdsize  = sizeof(MO::segment_command_64) + sizeof(MO::section_64);
    std::memcpy(text_seg.segname, "__TEXT\0\0\0\0\0\0\0\0\0\0", 16);
    text_seg.vmaddr   = 0x100000000;
    text_seg.vmsize   = 0x2000;  // header page + text page
    text_seg.fileoff  = 0;
    text_seg.filesize = 0;  // set below
    text_seg.maxprot  = MO::VM_PROT_READ | MO::VM_PROT_EXECUTE;
    text_seg.initprot = MO::VM_PROT_READ | MO::VM_PROT_EXECUTE;
    text_seg.nsects   = 1;

    MO::section_64 text_sec{};
    std::memcpy(text_sec.sectname, "__text\0\0\0\0\0\0\0\0\0", 16);
    std::memcpy(text_sec.segname,  "__TEXT\0\0\0\0\0\0\0\0\0\0", 16);
    text_sec.addr   = MACHO_TEXT_VA;
    text_sec.size   = MACHO_TEXT_SIZE;
    text_sec.align  = 4;  // 2^4 = 16

    // Compute layout
    const size_t lc_size = sizeof(MO::segment_command_64) + sizeof(MO::section_64);
    // Pad headers to page boundary (0x1000) so .text data starts at 0x1000
    const size_t text_file_off = 0x1000;
    const size_t total_size = text_file_off + MACHO_TEXT_SIZE;

    text_sec.offset = static_cast<uint32_t>(text_file_off);
    text_seg.filesize = total_size;

    // Build header
    MO::mach_header_64 hdr{};
    hdr.magic      = MO::MH_MAGIC_64;
    hdr.cputype    = 0x0100000C;  // CPU_TYPE_ARM64
    hdr.cpusubtype = 0;
    hdr.filetype   = 2;           // MH_EXECUTE
    hdr.ncmds      = 1;
    hdr.sizeofcmds = static_cast<uint32_t>(lc_size);
    hdr.flags      = 0;

    // Assemble the binary
    std::vector<uint8_t> buf(total_size, 0);
    std::memcpy(buf.data(), &hdr, sizeof(hdr));
    std::memcpy(buf.data() + sizeof(hdr), &text_seg, sizeof(text_seg));
    std::memcpy(buf.data() + sizeof(hdr) + sizeof(text_seg), &text_sec, sizeof(text_sec));
    std::memcpy(buf.data() + text_file_off, nops.data(), nops.size());

    std::string tmpname = VMPilot::Test::make_temp_file("vmpilot");

    std::ofstream ofs(tmpname, std::ios::binary | std::ios::trunc);
    ofs.write(reinterpret_cast<const char*>(buf.data()),
              static_cast<std::streamsize>(buf.size()));
    ofs.close();
    return tmpname;
}

}  // namespace

TEST(PatchE2E, MachO_ARM64_SingleRegion) {
    auto macho_path = build_test_macho();
    std::string out_path = macho_path + ".patched";
    auto blob = make_fake_blob(256);
    Common::DiagnosticCollector diag;

    PatchRequest req;
    req.input_path  = macho_path;
    req.output_path = out_path;
    req.regions     = {{"test_fn", MACHO_TEXT_VA, 32}};
    req.blob_data   = blob;
    req.stored_seed = TEST_SEED;
    req.arch        = Common::FileArch::ARM64;
    req.mode        = Common::FileMode::MODE_LITTLE_ENDIAN;
    req.format      = Common::FileFormat::MachO;

    auto result = patch(req, diag);
    ASSERT_TRUE(result.has_value()) << "MachO patch() failed";
    EXPECT_EQ(result->regions_patched, 1u);
    EXPECT_EQ(result->blob_bytes_injected, 256u);

    // Reload and verify __VMPILOT segment exists
    std::ifstream ifs(out_path, std::ios::binary | std::ios::ate);
    ASSERT_TRUE(ifs.good());
    std::vector<uint8_t> buf(static_cast<size_t>(ifs.tellg()));
    ifs.seekg(0);
    ifs.read(reinterpret_cast<char*>(buf.data()),
             static_cast<std::streamsize>(buf.size()));

    auto hdr = *reinterpret_cast<const MO::mach_header_64*>(buf.data());
    ASSERT_EQ(hdr.magic, MO::MH_MAGIC_64);

    // Find __VMPILOT segment
    bool found_vmpilot = false;
    size_t off = sizeof(MO::mach_header_64);
    for (uint32_t i = 0; i < hdr.ncmds; ++i) {
        if (off + sizeof(MO::load_command) > buf.size()) break;
        MO::load_command lc{};
        std::memcpy(&lc, buf.data() + off, sizeof(lc));
        if (lc.cmd == MO::LC_SEGMENT_64) {
            MO::segment_command_64 seg{};
            std::memcpy(&seg, buf.data() + off, sizeof(seg));
            if (std::string_view{seg.segname, strnlen(seg.segname, 16)} == "__VMPILOT") {
                found_vmpilot = true;

                // Verify payload at fileoff
                ASSERT_LE(seg.fileoff + 8 + blob.size() + SEED_SIZE, buf.size());
                const uint8_t* payload = buf.data() + seg.fileoff;

                // call_slot zero
                uint64_t call_slot = read64_le(payload);
                EXPECT_EQ(call_slot, 0u);

                // Blob at offset 8
                EXPECT_EQ(std::memcmp(payload + 8, blob.data(), blob.size()), 0);

                // Seed after blob
                EXPECT_EQ(std::memcmp(payload + 8 + blob.size(),
                                      TEST_SEED.data(), SEED_SIZE), 0);

                // W^X: initprot should be RW (not RX)
                EXPECT_EQ(seg.initprot, MO::VM_PROT_READ | MO::VM_PROT_WRITE);
                EXPECT_FALSE(seg.initprot & MO::VM_PROT_EXECUTE)
                    << "__VMPILOT initprot must not include EXECUTE (W^X)";
                break;
            }
        }
        off += lc.cmdsize;
    }
    EXPECT_TRUE(found_vmpilot) << "__VMPILOT segment not found";

    // Verify __text region overwritten (first insn should be B, not NOP)
    // ARM64 B encoding: bits [31:26] = 000101
    const uint8_t* text_data = buf.data() + 0x1000;
    uint32_t first_insn;
    std::memcpy(&first_insn, text_data, 4);
    EXPECT_EQ(first_insn >> 26, 0x05u)
        << "Region not overwritten with ARM64 B instruction";

    std::remove(macho_path.c_str());
    std::remove(out_path.c_str());
}
