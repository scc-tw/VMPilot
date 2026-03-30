/// @file test_handover.cpp
/// @brief Verify native → VM handover correctness in patched binaries.
///
/// The Loader's purpose is to embed the VM blob+seed and rewrite protected
/// regions so native execution correctly transfers control to the VM runtime.
///
/// These tests patch a real Mach-O binary and then verify, purely by reading
/// the patched bytes, that the handover chain is intact:
///
///   1. Protected region's first instruction is B <offset> to the entry stub
///   2. Entry stub's LEA rdi/ADR x0 resolves to the blob's VA
///   3. Entry stub's MOV rsi/MOVZ x1 contains the correct blob size
///   4. Entry stub's LEA rdx/ADR x2 resolves to the stored_seed's VA
///   5. Blob bytes in __VMPILOT segment match the input verbatim
///   6. Seed bytes in __VMPILOT segment match the input verbatim
///
/// No binary execution required.  No compiler dependency.

#include <BinaryPatcher.hpp>
#include <LoaderTypes.hpp>
#include <MachOEditor.hpp>
#include <MachOStructs.hpp>
#include <diagnostic_collector.hpp>

#include <gtest/gtest.h>

#include <array>
#include <cstring>
#include <fstream>

using namespace VMPilot;
using DC = Common::DiagnosticCode;

// ============================================================================
// Helpers
// ============================================================================

static const char* MACHO_BINARY = "data/basic/bin/basic_binary.Darwin.arm64";

static std::vector<uint8_t> make_blob(size_t size = 512) {
    std::vector<uint8_t> b(size);
    for (size_t i = 0; i < size; ++i)
        b[i] = static_cast<uint8_t>((i * 0x37 + 0x13) & 0xFF);
    return b;
}

static constexpr std::array<uint8_t, 32> TEST_SEED = {
    0xDE,0xAD,0xBE,0xEF, 0xCA,0xFE,0xBA,0xBE,
    0x01,0x23,0x45,0x67, 0x89,0xAB,0xCD,0xEF,
    0xFE,0xDC,0xBA,0x98, 0x76,0x54,0x32,0x10,
    0xAA,0xBB,0xCC,0xDD, 0xEE,0xFF,0x00,0x11,
};

/// Read patched binary into memory.
static std::vector<uint8_t> read_file(const std::string& path) {
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f) return {};
    auto sz = static_cast<size_t>(f.tellg());
    f.seekg(0);
    std::vector<uint8_t> data(sz);
    f.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(sz));
    return data;
}

/// Find __VMPILOT segment in a Mach-O binary.
struct SegmentLocation {
    uint64_t va = 0;
    uint64_t fileoff = 0;
    uint64_t filesize = 0;
};

static SegmentLocation find_vmpilot_segment(const std::vector<uint8_t>& data) {
    namespace MO = Loader::MachO;
    if (data.size() < sizeof(MO::mach_header_64)) return {};

    MO::mach_header_64 hdr{};
    std::memcpy(&hdr, data.data(), sizeof(hdr));

    size_t off = sizeof(MO::mach_header_64);
    for (uint32_t i = 0; i < hdr.ncmds; ++i) {
        MO::load_command lc{};
        std::memcpy(&lc, data.data() + off, sizeof(lc));
        if (lc.cmd == MO::LC_SEGMENT_64) {
            MO::segment_command_64 seg{};
            std::memcpy(&seg, data.data() + off, sizeof(seg));
            if (std::strncmp(seg.segname, "__VMPILOT", 10) == 0) {
                return {seg.vmaddr, seg.fileoff, seg.filesize};
            }
        }
        off += lc.cmdsize;
    }
    return {};
}

/// Find __text section file offset and VA.
struct TextLocation {
    uint64_t va = 0;
    uint32_t fileoff = 0;
};

static TextLocation find_text_section(const std::vector<uint8_t>& data) {
    namespace MO = Loader::MachO;
    MO::mach_header_64 hdr{};
    std::memcpy(&hdr, data.data(), sizeof(hdr));

    size_t off = sizeof(MO::mach_header_64);
    for (uint32_t i = 0; i < hdr.ncmds; ++i) {
        MO::load_command lc{};
        std::memcpy(&lc, data.data() + off, sizeof(lc));
        if (lc.cmd == MO::LC_SEGMENT_64) {
            MO::segment_command_64 seg{};
            std::memcpy(&seg, data.data() + off, sizeof(seg));
            size_t sec_off = off + sizeof(MO::segment_command_64);
            for (uint32_t s = 0; s < seg.nsects; ++s) {
                MO::section_64 sec{};
                std::memcpy(&sec, data.data() + sec_off, sizeof(sec));
                if (std::strncmp(sec.sectname, "__text", 7) == 0 &&
                    std::strncmp(sec.segname, "__TEXT", 6) == 0)
                    return {sec.addr, sec.offset};
                sec_off += sizeof(MO::section_64);
            }
        }
        off += lc.cmdsize;
    }
    return {};
}

/// Decode ARM64 B instruction's target VA.
static uint64_t decode_arm64_b_target(uint32_t insn, uint64_t insn_va) {
    const int32_t imm26 = static_cast<int32_t>(insn << 6) >> 6;  // sign-extend
    return insn_va + static_cast<int64_t>(imm26) * 4;
}

// ============================================================================
// Test fixture
// ============================================================================

class HandoverTest : public ::testing::Test {
protected:
    void SetUp() override {
        if (!std::ifstream(MACHO_BINARY).good()) {
            GTEST_SKIP() << "Mach-O test binary not found";
        }

        auto ed = Loader::MachOEditor::open(MACHO_BINARY, diag_);
        ASSERT_TRUE(ed) << diag_.summary();
        text_va_ = ed->text_section().base_addr;
        text_size_ = ed->text_section().size;

        // Patch with known blob + seed
        blob_ = make_blob(512);

        Loader::PatchRequest req;
        req.input_path = MACHO_BINARY;
        req.output_path = output_path_;
        req.arch = Common::FileArch::ARM64;
        req.mode = Common::FileMode::MODE_LITTLE_ENDIAN;
        req.format = Common::FileFormat::MachO;
        req.blob_data = blob_;
        req.stored_seed = TEST_SEED;
        // Patch at start of __text, 16 bytes (4 ARM64 instructions)
        req.regions.push_back({"test_region", text_va_, 16});

        auto patcher = Loader::create_patcher(Common::FileFormat::MachO);
        auto result = patcher->patch(req, diag_);
        ASSERT_TRUE(result) << diag_.summary();

        patched_ = read_file(output_path_);
        ASSERT_FALSE(patched_.empty());

        vmpilot_ = find_vmpilot_segment(patched_);
        ASSERT_NE(vmpilot_.va, 0u) << "no __VMPILOT segment found";

        text_ = find_text_section(patched_);
        ASSERT_NE(text_.va, 0u);
    }

    void TearDown() override {
        std::remove(output_path_.c_str());
    }

    Common::DiagnosticCollector diag_;
    std::string output_path_ = "/tmp/test_handover";
    uint64_t text_va_ = 0;
    uint64_t text_size_ = 0;
    std::vector<uint8_t> blob_;
    std::vector<uint8_t> patched_;
    SegmentLocation vmpilot_{};
    TextLocation text_{};
};

// ============================================================================
// Handover chain verification
// ============================================================================

TEST_F(HandoverTest, RegionBranchTargetsEntryStub) {
    // The first 4 bytes at the patched region should be ARM64 B
    const size_t region_foff = text_.fileoff;
    uint32_t insn;
    std::memcpy(&insn, patched_.data() + region_foff, 4);

    // Verify it's a B instruction (bits 31:26 = 000101)
    ASSERT_EQ(insn >> 26, 0x05u) << "expected B instruction at patched region";

    // Decode the branch target
    const uint64_t target_va = decode_arm64_b_target(insn, text_va_);

    // Target should be inside __VMPILOT segment (where stubs live)
    // Stubs are after blob + seed in the payload
    const uint64_t stubs_start_va = vmpilot_.va + blob_.size() + Loader::SEED_SIZE;
    EXPECT_GE(target_va, stubs_start_va)
        << "B target 0x" << std::hex << target_va
        << " is before the stubs area (0x" << stubs_start_va << ")";
    EXPECT_LT(target_va, vmpilot_.va + vmpilot_.filesize)
        << "B target is beyond __VMPILOT segment";
}

TEST_F(HandoverTest, BlobDataIntact) {
    // First blob_.size() bytes of __VMPILOT should match input blob verbatim
    ASSERT_GE(patched_.size(), vmpilot_.fileoff + blob_.size());
    EXPECT_EQ(std::memcmp(patched_.data() + vmpilot_.fileoff,
                           blob_.data(), blob_.size()), 0)
        << "blob data corrupted in patched binary";
}

TEST_F(HandoverTest, SeedDataIntact) {
    // Seed is at vmpilot_.fileoff + blob_.size()
    const size_t seed_foff = vmpilot_.fileoff + blob_.size();
    ASSERT_GE(patched_.size(), seed_foff + Loader::SEED_SIZE);
    EXPECT_EQ(std::memcmp(patched_.data() + seed_foff,
                           TEST_SEED.data(), Loader::SEED_SIZE), 0)
        << "stored_seed corrupted in patched binary";
}

TEST_F(HandoverTest, EntryStubBlobPointerResolvesToBlob) {
    // Find the entry stub's ADR x0 instruction and verify it points to blob VA
    const size_t region_foff = text_.fileoff;
    uint32_t b_insn;
    std::memcpy(&b_insn, patched_.data() + region_foff, 4);
    const uint64_t stub_va = decode_arm64_b_target(b_insn, text_va_);
    const size_t stub_foff = vmpilot_.fileoff +
        static_cast<size_t>(stub_va - vmpilot_.va);

    // Walk stub instructions looking for ADR x0 (opcode bits: xx10000x, Rd=0)
    // ADR encoding: [31]=immlo[1], [30]=immlo[0], [29:24]=010000, [23:5]=immhi, [4:0]=Rd
    bool found_adr_x0 = false;
    for (size_t i = 0; i < 256; i += 4) {  // scan first 64 instructions
        if (stub_foff + i + 4 > patched_.size()) break;
        uint32_t insn;
        std::memcpy(&insn, patched_.data() + stub_foff + i, 4);

        const uint8_t rd = insn & 0x1F;
        const uint32_t op = (insn >> 24) & 0x1F;
        if (op == 0x10 && rd == 0) {  // ADR x0
            // Decode ADR: immhi = insn[23:5], immlo = insn[30:29]
            const int32_t immhi = static_cast<int32_t>((insn >> 5) & 0x7FFFF);
            const int32_t immlo = static_cast<int32_t>((insn >> 29) & 0x3);
            int32_t offset = (immhi << 2) | immlo;
            // Sign-extend from 21 bits
            if (offset & (1 << 20)) offset |= static_cast<int32_t>(0xFFE00000);

            const uint64_t adr_va = stub_va + i;
            const uint64_t computed_target = adr_va + offset;

            EXPECT_EQ(computed_target, vmpilot_.va)
                << "ADR x0 resolves to 0x" << std::hex << computed_target
                << " but blob is at 0x" << vmpilot_.va;
            found_adr_x0 = true;
            break;
        }
    }
    EXPECT_TRUE(found_adr_x0) << "no ADR x0 found in entry stub";
}

TEST_F(HandoverTest, EntryStubSeedPointerResolvesToSeed) {
    // Find ADR x2 in the entry stub
    const size_t region_foff = text_.fileoff;
    uint32_t b_insn;
    std::memcpy(&b_insn, patched_.data() + region_foff, 4);
    const uint64_t stub_va = decode_arm64_b_target(b_insn, text_va_);
    const size_t stub_foff = vmpilot_.fileoff +
        static_cast<size_t>(stub_va - vmpilot_.va);

    const uint64_t expected_seed_va = vmpilot_.va + blob_.size();

    bool found_adr_x2 = false;
    for (size_t i = 0; i < 64; i += 4) {
        if (stub_foff + i + 4 > patched_.size()) break;
        uint32_t insn;
        std::memcpy(&insn, patched_.data() + stub_foff + i, 4);

        const uint8_t rd = insn & 0x1F;
        const uint32_t op = (insn >> 24) & 0x1F;
        if (op == 0x10 && rd == 2) {  // ADR x2
            const int32_t immhi = static_cast<int32_t>((insn >> 5) & 0x7FFFF);
            const int32_t immlo = static_cast<int32_t>((insn >> 29) & 0x3);
            int32_t offset = (immhi << 2) | immlo;
            if (offset & (1 << 20)) offset |= static_cast<int32_t>(0xFFE00000);

            const uint64_t adr_va = stub_va + i;
            const uint64_t computed = adr_va + offset;

            EXPECT_EQ(computed, expected_seed_va)
                << "ADR x2 resolves to 0x" << std::hex << computed
                << " but seed is at 0x" << expected_seed_va;
            found_adr_x2 = true;
            break;
        }
    }
    EXPECT_TRUE(found_adr_x2) << "no ADR x2 found in entry stub";
}

TEST_F(HandoverTest, EntryStubBlobSizeCorrect) {
    // Find MOVZ x1 in the entry stub
    const size_t region_foff = text_.fileoff;
    uint32_t b_insn;
    std::memcpy(&b_insn, patched_.data() + region_foff, 4);
    const uint64_t stub_va = decode_arm64_b_target(b_insn, text_va_);
    const size_t stub_foff = vmpilot_.fileoff +
        static_cast<size_t>(stub_va - vmpilot_.va);

    bool found_movz_x1 = false;
    for (size_t i = 0; i < 64; i += 4) {
        if (stub_foff + i + 4 > patched_.size()) break;
        uint32_t insn;
        std::memcpy(&insn, patched_.data() + stub_foff + i, 4);

        const uint8_t rd = insn & 0x1F;
        // MOVZ Xd: [31]=1 [30:23]=10100101 [22:21]=hw [20:5]=imm16 [4:0]=Rd
        // Actually: MOVZ X is 1_10_100101_xx_...
        // Bits [31]=sf, [30:29]=opc(10), [28:23]=100101, ...
        // For 64-bit MOVZ: 0xD2800000 base
        if ((insn & 0xFF800000) == 0xD2800000 && rd == 1) {
            const uint16_t imm16 = static_cast<uint16_t>((insn >> 5) & 0xFFFF);
            EXPECT_EQ(imm16, static_cast<uint16_t>(blob_.size()))
                << "MOVZ x1 has imm16=" << imm16
                << " but blob size is " << blob_.size();
            found_movz_x1 = true;
            break;
        }
    }
    EXPECT_TRUE(found_movz_x1) << "no MOVZ x1 found in entry stub";
}

TEST_F(HandoverTest, PatchedBinaryStillParseable) {
    auto ed = Loader::MachOEditor::open(output_path_, diag_);
    EXPECT_TRUE(ed) << "patched binary is not a valid Mach-O: " << diag_.summary();
}
