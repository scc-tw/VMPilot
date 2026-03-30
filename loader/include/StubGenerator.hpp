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
/// Entry stubs call the Runtime API:
///   vm_execute(blob_data*, blob_size, stored_seed*, config*)
///
/// Fixup offsets mark positions in `code` where PayloadBuilder patches
/// addresses/immediates after the final layout is known.
struct Stub {
    std::vector<uint8_t> code;

    // --- arg0: blob_data pointer (RIP-relative LEA / ADR) ---
    size_t blob_fixup_offset = 0;
    size_t blob_insn_size = 0;    // remaining bytes of the fixup instruction

    // --- arg1: blob_size (immediate) ---
    size_t size_fixup_offset = 0;

    // --- arg2: stored_seed pointer (RIP-relative LEA / ADR) ---
    size_t seed_fixup_offset = 0;
    size_t seed_insn_size = 0;

    // --- call target (vm_execute, needs relocation) ---
    size_t call_fixup_offset = 0;
};

/// Generates architecture-specific machine-code stubs.
class StubGenerator {
public:
    /// Generate a JMP/B to overwrite a protected region.
    [[nodiscard]] static tl::expected<std::vector<uint8_t>, Common::DiagnosticCode>
    generate_region_patch(Common::FileArch arch, Common::FileMode mode,
                          uint64_t region_size,
                          uint64_t from_addr, uint64_t to_addr) noexcept;

    /// Generate an entry stub.  All pointer/size fixups are placeholders
    /// until PayloadBuilder calls the fixup_*() methods.
    [[nodiscard]] static tl::expected<Stub, Common::DiagnosticCode>
    generate_entry_stub(Common::FileArch arch, Common::FileMode mode,
                        uint32_t region_idx = 0) noexcept;

    /// Patch blob-pointer displacement (arg0).
    [[nodiscard]] static tl::expected<void, Common::DiagnosticCode>
    fixup_blob_displacement(Stub& stub, int64_t displacement,
                            Common::FileArch arch) noexcept;

    /// Patch seed-pointer displacement (arg2).  Same encoding as blob fixup.
    [[nodiscard]] static tl::expected<void, Common::DiagnosticCode>
    fixup_seed_displacement(Stub& stub, int64_t displacement,
                            Common::FileArch arch) noexcept;

    /// Patch blob_size immediate (arg1).
    static void fixup_blob_size(Stub& stub, uint64_t blob_size) noexcept;

    [[nodiscard]] static size_t
    min_region_size(Common::FileArch arch, Common::FileMode mode) noexcept;

    [[nodiscard]] static int64_t
    max_branch_distance(Common::FileArch arch) noexcept;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_STUB_GENERATOR_HPP__
