#ifndef __LOADER_STUB_GENERATOR_HPP__
#define __LOADER_STUB_GENERATOR_HPP__
#pragma once

#include <diagnostic.hpp>
#include <file_type_parser.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <vector>

namespace VMPilot::Loader {

/// Machine-code entry stub with fixup metadata.
///
/// Entry stubs perform mid-function handover: native → VM → native.
///
///   1. Save ALL GPRs (native code may have live values in any register)
///   2. Compute load_base_delta (ASLR support)
///   3. Call vm_execute_with_args(blob, size, seed, saved_regs, 16, nullptr)
///   4. Write VM return value back to saved register slots
///   5. Restore ALL GPRs
///   6. Jump to resume_addr (= region.addr + region.size, NOT ret)
///
/// Fixup offsets mark positions in `code` that PayloadBuilder or
/// FormatPatcher patches after layout is known.
struct Stub {
    std::vector<uint8_t> code;

    // --- arg0: blob_data pointer (RIP-relative LEA / ADR) ---
    size_t blob_fixup_offset = 0;
    size_t blob_insn_size = 0;     // 4 for x86 disp32, 0 for ARM64 ADR

    // --- arg1: blob_size (immediate) ---
    size_t size_fixup_offset = 0;

    // --- arg2: stored_seed pointer (RIP-relative LEA / ADR) ---
    size_t seed_fixup_offset = 0;
    size_t seed_insn_size = 0;

    // --- call target (vm_execute_with_args) ---
    size_t call_fixup_offset = 0;

    // --- resume jump to region.addr + region.size ---
    // Patched by FormatPatcher (needs final segment VA + region address)
    size_t resume_fixup_offset = 0;
    size_t resume_insn_size = 0;   // 4 for x86 rel32, 0 for ARM64 B

    // --- load_base_delta: static VA of stub (for ASLR delta computation) ---
    size_t delta_static_va_fixup_offset = 0;
    /// Offset within the stub of the "delta reference point" instruction
    /// (ADR on ARM64, LEA on x86-64). The patched static VA corresponds to
    /// the runtime value this instruction computes (which may differ from
    /// the instruction's own address — e.g. x86 LEA [rip+0] yields IP of
    /// the *next* instruction).
    size_t delta_ref_offset = 0;
    /// Size of the fixup region: 8 for x86-64 (imm64), 16 for ARM64
    /// (4 × 4-byte MOVZ/MOVK instructions).
    size_t delta_fixup_size = 0;
};

class StubGenerator {
public:
    /// Generate a JMP/B to overwrite a protected region's first bytes.
    [[nodiscard]] static tl::expected<std::vector<uint8_t>, Common::DiagnosticCode>
    generate_region_patch(Common::FileArch arch, Common::FileMode mode,
                          uint64_t region_size,
                          uint64_t from_addr, uint64_t to_addr) noexcept;

    /// Generate a full mid-function entry stub.
    [[nodiscard]] static tl::expected<Stub, Common::DiagnosticCode>
    generate_entry_stub(Common::FileArch arch, Common::FileMode mode,
                        uint32_t region_idx = 0) noexcept;

    /// Patch blob-pointer displacement (arg0).
    [[nodiscard]] static tl::expected<void, Common::DiagnosticCode>
    fixup_blob_displacement(Stub& stub, int64_t displacement,
                            Common::FileArch arch) noexcept;

    /// Patch seed-pointer displacement (arg2).
    [[nodiscard]] static tl::expected<void, Common::DiagnosticCode>
    fixup_seed_displacement(Stub& stub, int64_t displacement,
                            Common::FileArch arch) noexcept;

    /// Patch blob_size immediate (arg1).
    static void fixup_blob_size(Stub& stub, uint64_t blob_size) noexcept;

    /// Patch resume jump displacement (jmp/b to region.addr + region.size).
    [[nodiscard]] static tl::expected<void, Common::DiagnosticCode>
    fixup_resume_displacement(Stub& stub, int64_t displacement,
                              Common::FileArch arch) noexcept;

    /// Patch the static VA immediate used for load_base_delta computation.
    static void fixup_delta_static_va(Stub& stub, uint64_t static_va) noexcept;

    [[nodiscard]] static size_t
    min_region_size(Common::FileArch arch, Common::FileMode mode) noexcept;

    [[nodiscard]] static int64_t
    max_branch_distance(Common::FileArch arch) noexcept;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_STUB_GENERATOR_HPP__
