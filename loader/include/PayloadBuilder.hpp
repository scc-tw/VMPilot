#ifndef __LOADER_PAYLOAD_BUILDER_HPP__
#define __LOADER_PAYLOAD_BUILDER_HPP__
#pragma once

#include <LoaderTypes.hpp>
#include <StubGenerator.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace VMPilot::Loader {

/// Per-region layout within the assembled payload.
struct RegionLayout {
    std::string name;
    size_t stub_offset = 0;
    size_t stub_size = 0;
    /// Offset within payload where the resume jump displacement must be
    /// patched by FormatPatcher (needs segment VA + region resume address).
    size_t resume_fixup_payload_offset = 0;
    /// Instruction size for resume displacement calculation.
    /// x86: 4 (rel32, target = RIP + disp). ARM64: 0 (B is PC-relative).
    size_t resume_insn_size = 0;
};

/// Fully assembled payload ready for injection.
///
/// Layout: [ blob_data | stored_seed (32B) | stub_0 | stub_1 | ... ]
///
/// Entry stubs reference blob_data and stored_seed via RIP-relative /
/// ADR addressing.  All displacements are fixed up relative to payload
/// offset 0 (blob start).
struct PatchPayload {
    std::vector<uint8_t> data;
    std::vector<RegionLayout> layouts;
    size_t blob_size = 0;
    size_t seed_offset = 0;     // offset of stored_seed within payload
};

/// Build the injection payload from pre-built blob bytes.
///
/// Pure data transformation — no file I/O, no blob interpretation.
///
/// @param regions       Protected regions (for stub generation).
/// @param blob_data     Complete blob bytes from compiler pipeline.
/// @param stored_seed   32-byte root secret to embed alongside blob.
/// @param arch          Target architecture.
/// @param mode          Sub-mode (32/64).
/// @param diag          Diagnostic collector.
[[nodiscard]] tl::expected<PatchPayload, Common::DiagnosticCode>
build_payload(const std::vector<RegionPatchInfo>& regions,
              const std::vector<uint8_t>& blob_data,
              const std::array<uint8_t, SEED_SIZE>& stored_seed,
              Common::FileArch arch, Common::FileMode mode,
              Common::DiagnosticCollector& diag =
                  Common::DiagnosticCollector::noop()) noexcept;

}  // namespace VMPilot::Loader

#endif  // __LOADER_PAYLOAD_BUILDER_HPP__
