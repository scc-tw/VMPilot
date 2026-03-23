#ifndef __LOADER_STUB_GENERATOR_HPP__
#define __LOADER_STUB_GENERATOR_HPP__
#pragma once

#include <diagnostic.hpp>
#include <file_type_parser.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <vector>

namespace VMPilot::Loader {

/// Machine-code stub with fixup metadata.
struct Stub {
    std::vector<uint8_t> code;
    /// Offset within code where the dispatcher call needs patching (rel32 / BL).
    size_t call_fixup_offset = 0;
    /// Offset within code where the blob-pointer displacement needs patching
    /// after final payload layout is known (LEA disp32 / ADR imm21).
    size_t blob_fixup_offset = 0;
    /// Remaining bytes of the instruction at blob_fixup_offset.
    /// For x86-64 LEA: 4 (disp32).  For ARM64 ADR: 4 (full instruction).
    /// Used for RIP-relative: disp = target - (fixup_addr + blob_insn_size).
    size_t blob_insn_size = 0;
};

/// Generates architecture-specific machine-code stubs for binary patching.
class StubGenerator {
public:
    /// Generate a JMP/B to overwrite the start of a protected region.
    /// x86: E9 rel32 (5 bytes) + NOP fill.  ARM64: B imm26 (4 bytes) + NOP fill.
    [[nodiscard]] static tl::expected<std::vector<uint8_t>, Common::DiagnosticCode>
    generate_region_patch(Common::FileArch arch, Common::FileMode mode,
                          uint64_t region_size,
                          uint64_t from_addr, uint64_t to_addr) noexcept;

    /// Generate an entry stub (save regs → load blob ptr → call dispatcher → restore → ret).
    /// The blob_rel_offset is a placeholder; call fixup_blob_displacement() after layout.
    [[nodiscard]] static tl::expected<Stub, Common::DiagnosticCode>
    generate_entry_stub(Common::FileArch arch, Common::FileMode mode,
                        uint32_t region_idx,
                        int64_t blob_rel_offset = 0) noexcept;

    /// Patch the blob-pointer displacement in a stub after final layout is known.
    /// Encapsulates arch-specific encoding (x86 disp32, ARM64 ADR imm21).
    /// Returns error if displacement exceeds the architecture's range.
    [[nodiscard]] static tl::expected<void, Common::DiagnosticCode>
    fixup_blob_displacement(Stub& stub, int64_t displacement,
                            Common::FileArch arch) noexcept;

    /// Minimum region size required for a JMP/B patch.
    [[nodiscard]] static size_t
    min_region_size(Common::FileArch arch, Common::FileMode mode) noexcept;

    /// Maximum branch distance for region patch → entry stub.
    /// x86: ±2GB (rel32).  ARM64: ±128MB (B imm26).
    [[nodiscard]] static int64_t
    max_branch_distance(Common::FileArch arch) noexcept;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_STUB_GENERATOR_HPP__
