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
    /// Offset within code where the dispatcher call rel32/BL needs patching.
    size_t call_fixup_offset = 0;
    /// Offset within code where the blob-pointer displacement (LEA disp32 /
    /// ADR imm21) needs patching after final layout is known.
    size_t blob_fixup_offset = 0;
    /// Size of the LEA/ADR instruction that contains the blob displacement.
    /// Used to compute RIP-relative: disp = blob_va - (stub_va + blob_fixup_offset + blob_insn_size)
    size_t blob_insn_size = 0;
};

/// Generates architecture-specific machine-code stubs for binary patching.
///
/// Three kinds of stubs:
///   1. **Region patch**: overwrites the start of a protected region with
///      a JMP/B to the entry stub (+ NOP padding).
///   2. **Entry stub**: saves callee-saved regs, loads blob pointer + region
///      index, calls `_vmpilot_vm_entry`, restores regs, returns.
///   3. **Exit stub**: (placeholder) — returns control to original call site.
class StubGenerator {
public:
    /// Generate a JMP/B instruction to overwrite the start of a protected
    /// region.  On x86-64 this is `E9 rel32` (5 bytes); on ARM64 this is
    /// `B imm26` (4 bytes).  Remaining bytes are filled with NOPs.
    ///
    /// @param arch   Target architecture.
    /// @param mode   Sub-mode (32/64).
    /// @param region_size  Size of the original protected region.
    /// @param from_addr    VA where the JMP will be placed.
    /// @param to_addr      VA of the entry stub to jump to.
    [[nodiscard]] static tl::expected<std::vector<uint8_t>, Common::DiagnosticCode>
    generate_region_patch(Common::FileArch arch, Common::FileMode mode,
                          uint64_t region_size,
                          uint64_t from_addr, uint64_t to_addr) noexcept;

    /// Generate an entry stub that:
    ///   - Saves callee-saved registers
    ///   - Loads blob data pointer (RIP-relative on x86-64, ADR on ARM64)
    ///   - Loads region index
    ///   - Calls _vmpilot_vm_entry (placeholder address — needs relocation)
    ///   - Restores registers and returns
    ///
    /// @param arch        Target architecture.
    /// @param mode        Sub-mode.
    /// @param region_idx  Region index (passed as argument to dispatcher).
    /// @param blob_rel_offset  RIP-relative offset to blob data (from stub).
    /// @return Stub with code and fixup offset for the call target.
    [[nodiscard]] static tl::expected<Stub, Common::DiagnosticCode>
    generate_entry_stub(Common::FileArch arch, Common::FileMode mode,
                        uint32_t region_idx,
                        int64_t blob_rel_offset) noexcept;

    /// Minimum region size required for a JMP/B patch.
    [[nodiscard]] static size_t
    min_region_size(Common::FileArch arch, Common::FileMode mode) noexcept;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_STUB_GENERATOR_HPP__
