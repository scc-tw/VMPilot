/// @file test_loader.cpp
/// @brief Unit tests for the Loader v2 redesign.

#include <ArchTraits.hpp>
#include <BinaryEditor.hpp>
#include <Loader.hpp>
#include <LoaderTypes.hpp>
#include <PayloadBuilder.hpp>
#include <PlatformTraits.hpp>
#include <StubEmitter.hpp>
#include <diagnostic_collector.hpp>

#include <gtest/gtest.h>

#include <array>
#include <cstring>

using namespace VMPilot;
using DC = Common::DiagnosticCode;

// ============================================================================
// Helpers
// ============================================================================

static std::vector<uint8_t> make_fake_blob(size_t size = 256) {
    std::vector<uint8_t> b(size);
    for (size_t i = 0; i < size; ++i)
        b[i] = static_cast<uint8_t>((i * 0x37 + 0x13) & 0xFF);
    return b;
}

static constexpr std::array<uint8_t, 32> FAKE_SEED = {
    0xDE,0xAD,0xBE,0xEF, 0xCA,0xFE,0xBA,0xBE,
    0x01,0x23,0x45,0x67, 0x89,0xAB,0xCD,0xEF,
    0xFE,0xDC,0xBA,0x98, 0x76,0x54,0x32,0x10,
    0xAA,0xBB,0xCC,0xDD, 0xEE,0xFF,0x00,0x11,
};

// ============================================================================
// ArchTraits
// ============================================================================

TEST(ArchTraits, X86_64Constants) {
    using T = Loader::ArchTraits<Common::FileArch::X86, Common::FileMode::MODE_64>;
    EXPECT_EQ(T::min_region_size, 5u);
    EXPECT_EQ(T::ptr_size, 8u);
    EXPECT_EQ(T::callee_saved.size(), 6u);
    EXPECT_EQ(T::arg_regs.size(), 6u);
    EXPECT_EQ(T::return_reg, 0);
    EXPECT_EQ(T::gpr_count, 16);
}

TEST(ArchTraits, ARM64Constants) {
    using T = Loader::ArchTraits<Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN>;
    EXPECT_EQ(T::min_region_size, 4u);
    EXPECT_EQ(T::ptr_size, 8u);
    EXPECT_EQ(T::callee_saved.size(), 12u);
    EXPECT_EQ(T::arg_regs.size(), 8u);
    EXPECT_EQ(T::return_reg, 0);
    EXPECT_EQ(T::gpr_count, 31);
}

TEST(ArchTraits, X86_32Constants) {
    using T = Loader::ArchTraits<Common::FileArch::X86, Common::FileMode::MODE_32>;
    EXPECT_EQ(T::min_region_size, 5u);
    EXPECT_EQ(T::ptr_size, 4u);
    EXPECT_EQ(T::callee_saved.size(), 4u);
    EXPECT_EQ(T::arg_regs.size(), 0u); // cdecl: stack args
}

// ============================================================================
// PlatformTraits + FormatConfig
// ============================================================================

TEST(PlatformTraits, MachOConfig) {
    auto cfg = Loader::FormatConfig::for_format(Common::FileFormat::MachO);
    EXPECT_EQ(cfg.runtime_lib, "@rpath/libvmpilot_runtime.dylib");
    EXPECT_EQ(cfg.section_name, ".vmpilot");
    EXPECT_EQ(cfg.page_align, 0x4000u);
}

TEST(PlatformTraits, ELFConfig) {
    auto cfg = Loader::FormatConfig::for_format(Common::FileFormat::ELF);
    EXPECT_EQ(cfg.runtime_lib, "libvmpilot_runtime.so");
    EXPECT_EQ(cfg.section_name, ".vmpilot");
    EXPECT_EQ(cfg.page_align, 0x1000u);
}

TEST(PlatformTraits, PEConfig) {
    auto cfg = Loader::FormatConfig::for_format(Common::FileFormat::PE);
    EXPECT_EQ(cfg.runtime_lib, "vmpilot_runtime.dll");
    EXPECT_EQ(cfg.section_name, ".vmpltt");
}

// ============================================================================
// StubEmitter — create_emitter factory
// ============================================================================

TEST(StubEmitter, CreateX64) {
    auto e = Loader::create_emitter(Common::FileArch::X86, Common::FileMode::MODE_64);
    ASSERT_NE(e, nullptr);
    EXPECT_EQ(e->min_region_size(), 5u);
    EXPECT_EQ(e->max_branch_distance(), INT32_MAX);
}

TEST(StubEmitter, CreateARM64) {
    auto e = Loader::create_emitter(Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN);
    ASSERT_NE(e, nullptr);
    EXPECT_EQ(e->min_region_size(), 4u);
    EXPECT_EQ(e->max_branch_distance(), 128LL * 1024 * 1024);
}

TEST(StubEmitter, CreateUnsupported) {
    auto e = Loader::create_emitter(Common::FileArch::MIPS, Common::FileMode::MODE_32);
    EXPECT_EQ(e, nullptr);
}

// ============================================================================
// StubEmitter — emit_entry_stub
// ============================================================================

TEST(StubEmitter, X64EntryStubStructure) {
    auto e = Loader::create_emitter(Common::FileArch::X86, Common::FileMode::MODE_64);
    auto r = e->emit_entry_stub();
    ASSERT_TRUE(r);
    auto& s = *r;
    // Ends with JMP rel32 (E9 xx xx xx xx)
    ASSERT_GE(s.code.size(), 5u);
    EXPECT_EQ(s.code[s.code.size() - 5], 0xE9);
    // All fixup offsets within bounds
    EXPECT_LT(s.blob_fixup_offset, s.code.size());
    EXPECT_LT(s.seed_fixup_offset, s.code.size());
    EXPECT_LT(s.size_fixup_offset, s.code.size());
    EXPECT_LT(s.call_slot_fixup_offset, s.code.size());
    EXPECT_LT(s.resume_fixup_offset, s.code.size());
    EXPECT_LT(s.delta_fixup_offset, s.code.size());
    EXPECT_EQ(s.delta_fixup_size, 8u);
}

TEST(StubEmitter, ARM64EntryStubStructure) {
    auto e = Loader::create_emitter(Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN);
    auto r = e->emit_entry_stub();
    ASSERT_TRUE(r);
    auto& s = *r;
    EXPECT_EQ(s.code.size() % 4, 0u);
    // Ends with B (bits 31:26 = 000101)
    uint32_t last;
    std::memcpy(&last, s.code.data() + s.code.size() - 4, 4);
    EXPECT_EQ(last >> 26, 0x05u);
    // All fixup offsets within bounds
    EXPECT_LT(s.blob_fixup_offset, s.code.size());
    EXPECT_LT(s.seed_fixup_offset, s.code.size());
    EXPECT_LT(s.size_fixup_offset, s.code.size());
    EXPECT_LT(s.call_slot_fixup_offset, s.code.size());
    EXPECT_LT(s.resume_fixup_offset, s.code.size());
    EXPECT_LT(s.delta_fixup_offset, s.code.size());
    EXPECT_EQ(s.delta_fixup_size, 16u);
}

// ============================================================================
// StubEmitter — CET/BTI landing pads
// ============================================================================

TEST(StubEmitter, X64EntryStubStartsWithENDBR64) {
    auto e = Loader::create_emitter(Common::FileArch::X86, Common::FileMode::MODE_64);
    auto r = e->emit_entry_stub();
    ASSERT_TRUE(r);
    auto& c = r->code;
    ASSERT_GE(c.size(), 4u);
    // ENDBR64 = F3 0F 1E FA
    EXPECT_EQ(c[0], 0xF3);
    EXPECT_EQ(c[1], 0x0F);
    EXPECT_EQ(c[2], 0x1E);
    EXPECT_EQ(c[3], 0xFA);
}

TEST(StubEmitter, X32EntryStubStartsWithENDBR32) {
    auto e = Loader::create_emitter(Common::FileArch::X86, Common::FileMode::MODE_32);
    ASSERT_NE(e, nullptr);
    auto r = e->emit_entry_stub();
    ASSERT_TRUE(r);
    auto& c = r->code;
    ASSERT_GE(c.size(), 4u);
    // ENDBR32 = F3 0F 1E FB
    EXPECT_EQ(c[0], 0xF3);
    EXPECT_EQ(c[1], 0x0F);
    EXPECT_EQ(c[2], 0x1E);
    EXPECT_EQ(c[3], 0xFB);
}

TEST(StubEmitter, ARM64EntryStubStartsWithBTI) {
    auto e = Loader::create_emitter(Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN);
    auto r = e->emit_entry_stub();
    ASSERT_TRUE(r);
    auto& c = r->code;
    ASSERT_GE(c.size(), 4u);
    // BTI c = D503245F (little-endian: 5F 24 03 D5)
    uint32_t first_insn;
    std::memcpy(&first_insn, c.data(), 4);
    EXPECT_EQ(first_insn, 0xD503245Fu) << "First instruction must be BTI c";
}

// ============================================================================
// StubEmitter — emit_region_patch
// ============================================================================

TEST(StubEmitter, X64RegionPatch) {
    auto e = Loader::create_emitter(Common::FileArch::X86, Common::FileMode::MODE_64);
    auto r = e->emit_region_patch(32, 0x401000, 0x500000);
    ASSERT_TRUE(r);
    EXPECT_EQ(r->size(), 32u);
    EXPECT_EQ((*r)[0], 0xE9);
    for (size_t i = 5; i < 32; ++i) EXPECT_EQ((*r)[i], 0x90);
    int32_t rel;
    std::memcpy(&rel, r->data() + 1, 4);
    EXPECT_EQ(0x401000u + 5 + rel, 0x500000u);
}

TEST(StubEmitter, ARM64RegionPatch) {
    auto e = Loader::create_emitter(Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN);
    auto r = e->emit_region_patch(16, 0x1000, 0x1100);
    ASSERT_TRUE(r);
    EXPECT_EQ(r->size(), 16u);
    uint32_t insn;
    std::memcpy(&insn, r->data(), 4);
    EXPECT_EQ(insn >> 26, 0x05u);
}

TEST(StubEmitter, RegionPatchTooSmall) {
    auto e = Loader::create_emitter(Common::FileArch::X86, Common::FileMode::MODE_64);
    auto r = e->emit_region_patch(4, 0x1000, 0x2000);
    EXPECT_FALSE(r);
}

// ============================================================================
// StubEmitter — fixup methods
// ============================================================================

TEST(StubEmitter, X64FixupPtr) {
    auto e = Loader::create_emitter(Common::FileArch::X86, Common::FileMode::MODE_64);
    auto r = e->emit_entry_stub();
    ASSERT_TRUE(r);
    // fixup_ptr takes (fixup_va, target_va); emitter computes disp internally.
    // x86_64: disp = target_va - (fixup_va + 4) = 1004 - (2000 + 4) = -1000
    auto fx = e->fixup_ptr(r->code, r->blob_fixup_offset, 2000, 1004);
    EXPECT_TRUE(fx);
    int32_t patched;
    std::memcpy(&patched, r->code.data() + r->blob_fixup_offset, 4);
    EXPECT_EQ(patched, -1000);
}

TEST(StubEmitter, X64FixupImmediate) {
    auto e = Loader::create_emitter(Common::FileArch::X86, Common::FileMode::MODE_64);
    auto r = e->emit_entry_stub();
    ASSERT_TRUE(r);
    e->fixup_immediate(r->code, r->size_fixup_offset, 0xDEAD);
    uint32_t patched;
    std::memcpy(&patched, r->code.data() + r->size_fixup_offset, 4);
    EXPECT_EQ(patched, 0xDEADu);
}

TEST(StubEmitter, X64FixupStaticVa) {
    auto e = Loader::create_emitter(Common::FileArch::X86, Common::FileMode::MODE_64);
    auto r = e->emit_entry_stub();
    ASSERT_TRUE(r);
    constexpr uint64_t va = 0xDEADBEEF12345678ULL;
    e->fixup_static_va(r->code, r->delta_fixup_offset, r->delta_fixup_size, va);
    uint64_t patched;
    std::memcpy(&patched, r->code.data() + r->delta_fixup_offset, 8);
    EXPECT_EQ(patched, va);
}

TEST(StubEmitter, ARM64FixupStaticVa) {
    auto e = Loader::create_emitter(Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN);
    auto r = e->emit_entry_stub();
    ASSERT_TRUE(r);
    constexpr uint64_t va = 0x0000000100004000ULL;
    e->fixup_static_va(r->code, r->delta_fixup_offset, r->delta_fixup_size, va);
    // Check MOVZ/MOVK 16-bit chunks
    auto off = r->delta_fixup_offset;
    uint32_t i0, i1, i2, i3;
    std::memcpy(&i0, r->code.data() + off,      4);
    std::memcpy(&i1, r->code.data() + off + 4,  4);
    std::memcpy(&i2, r->code.data() + off + 8,  4);
    std::memcpy(&i3, r->code.data() + off + 12, 4);
    EXPECT_EQ((i0 >> 5) & 0xFFFF, 0x4000u);
    EXPECT_EQ((i1 >> 5) & 0xFFFF, 0x0000u);
    EXPECT_EQ((i2 >> 5) & 0xFFFF, 0x0001u);
    EXPECT_EQ((i3 >> 5) & 0xFFFF, 0x0000u);
}

// ============================================================================
// PayloadBuilder
// ============================================================================

TEST(PayloadBuilder, EmptyRegionsFails) {
    auto e = Loader::create_emitter(Common::FileArch::X86, Common::FileMode::MODE_64);
    auto r = Loader::build_payload({}, make_fake_blob(), FAKE_SEED, 0x10000, *e);
    EXPECT_FALSE(r);
}

TEST(PayloadBuilder, EmptyBlobFails) {
    auto e = Loader::create_emitter(Common::FileArch::X86, Common::FileMode::MODE_64);
    std::vector<Loader::RegionPatchInfo> regions = {{"f", 0x1000, 32}};
    auto r = Loader::build_payload(regions, {}, FAKE_SEED, 0x10000, *e);
    EXPECT_FALSE(r);
}

TEST(PayloadBuilder, SingleRegionLayout) {
    auto e = Loader::create_emitter(Common::FileArch::X86, Common::FileMode::MODE_64);
    auto blob = make_fake_blob(128);
    std::vector<Loader::RegionPatchInfo> regions = {{"f", 0x1000, 32}};
    auto r = Loader::build_payload(regions, blob, FAKE_SEED, 0x10000, *e);
    ASSERT_TRUE(r);
    // New layout: [call_slot(8)] [blob(128)] [seed(32)] [stubs...]
    EXPECT_EQ(r->blob_size, 128u);
    EXPECT_EQ(r->call_slot_offset, 0u);
    EXPECT_EQ(r->seed_offset, 8u + 128u);
    EXPECT_EQ(r->layouts.size(), 1u);
    EXPECT_EQ(r->layouts[0].stub_offset, 8u + 128u + 32u); // slot+blob+seed
    // Verify call slot at offset 0 is zero
    uint64_t slot;
    std::memcpy(&slot, r->data.data(), 8);
    EXPECT_EQ(slot, 0u);
    // Verify blob preserved at offset 8
    EXPECT_EQ(std::memcmp(r->data.data() + 8, blob.data(), 128), 0);
    // Verify seed preserved
    EXPECT_EQ(std::memcmp(r->data.data() + 8 + 128, FAKE_SEED.data(), 32), 0);
}

TEST(PayloadBuilder, MultipleRegionsLayout) {
    auto e = Loader::create_emitter(Common::FileArch::X86, Common::FileMode::MODE_64);
    std::vector<Loader::RegionPatchInfo> regions = {
        {"a", 0x1000, 32}, {"b", 0x1100, 32}, {"c", 0x1200, 32}
    };
    auto r = Loader::build_payload(regions, make_fake_blob(), FAKE_SEED, 0x10000, *e);
    ASSERT_TRUE(r);
    EXPECT_EQ(r->layouts.size(), 3u);
    for (size_t i = 1; i < r->layouts.size(); ++i) {
        EXPECT_GE(r->layouts[i].stub_offset,
                   r->layouts[i-1].stub_offset + r->layouts[i-1].stub_size);
    }
}

// ============================================================================
// DiagnosticCode helpers
// ============================================================================

TEST(DiagnosticCode, ModuleOf) {
    EXPECT_EQ(Common::module_of(DC::None), 0x0000);
    EXPECT_EQ(Common::module_of(DC::FileNotFound), 0x0001);
    EXPECT_EQ(Common::module_of(DC::PatchInputInvalid), 0x0007);
}

TEST(DiagnosticCode, ModuleName) {
    EXPECT_STREQ(Common::module_name(0x0001), "segmentator");
    EXPECT_STREQ(Common::module_name(0x0006), "runtime");
    EXPECT_STREQ(Common::module_name(0x0007), "loader");
}
