#ifndef __LOADER_PAYLOAD_BUILDER_HPP__
#define __LOADER_PAYLOAD_BUILDER_HPP__
#pragma once

/// @file PayloadBuilder.hpp
/// @brief Architecture-agnostic payload assembly.
///
/// Builds the injection payload from pre-built blob bytes.
/// All fixups are resolved in one pass — segment_va is known before calling.
/// Architecture-specific encoding is delegated to StubEmitter.

#include <LoaderTypes.hpp>
#include <StubEmitter.hpp>
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
};

/// Fully assembled payload ready for injection.
///
/// Layout: [ blob | seed (32B) | call_slot (8B) | stub_0 | stub_1 | ... ]
///
/// All displacements resolved. Data is final — inject with one add_segment().
struct PatchPayload {
    std::vector<uint8_t> data;
    std::vector<RegionLayout> layouts;
    size_t blob_size = 0;
    size_t seed_offset = 0;
    size_t call_slot_offset = 0;
};

/// Build the injection payload.
///
/// Pure data transformation — no file I/O. Arch-agnostic: all instruction
/// encoding delegated to `emitter`. All displacements resolved because
/// `segment_va` is known (from BinaryEditor::next_segment_va()).
///
/// @param regions      Protected regions (for stub generation + resume addrs).
/// @param blob_data    Complete blob bytes from compiler pipeline.
/// @param stored_seed  32-byte root secret (D15§13.5).
/// @param segment_va   VA where the payload will be placed (pre-computed).
/// @param emitter      Architecture-specific code generator.
/// @param diag         Diagnostic collector.
[[nodiscard]] tl::expected<PatchPayload, Common::DiagnosticCode>
build_payload(const std::vector<RegionPatchInfo>& regions,
              const std::vector<uint8_t>& blob_data,
              const std::array<uint8_t, SEED_SIZE>& stored_seed,
              uint64_t segment_va,
              StubEmitter& emitter,
              Common::DiagnosticCollector& diag =
                  Common::DiagnosticCollector::noop()) noexcept;

}  // namespace VMPilot::Loader

#endif  // __LOADER_PAYLOAD_BUILDER_HPP__
