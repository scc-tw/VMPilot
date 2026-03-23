#ifndef __LOADER_PAYLOAD_BUILDER_HPP__
#define __LOADER_PAYLOAD_BUILDER_HPP__
#pragma once

#include <CompilationOutput.hpp>
#include <LoaderTypes.hpp>
#include <StubGenerator.hpp>
#include <diagnostic_collector.hpp>
#include <file_type_parser.hpp>

#include <tl/expected.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace VMPilot::Loader {

/// Per-region layout within the assembled payload.
struct RegionLayout {
    std::string name;
    size_t stub_offset = 0;   // offset of entry stub within payload
    size_t stub_size = 0;
};

/// Fully assembled payload ready for injection into a binary.
/// Contains serialized bytecode blob + entry stubs with blob-pointer
/// displacements already fixed up (relative to payload start).
struct PatchPayload {
    std::vector<uint8_t> data;
    std::vector<RegionLayout> layouts;   // one per region
    size_t blob_size = 0;                // for reporting
};

/// Build the payload: serialize bytecodes → blob, generate entry stubs,
/// compute layout, fix up blob-pointer displacements.
///
/// This is a **pure data transformation** — no file I/O. Regions are
/// matched to compiled outputs by name; mismatches are errors.
///
/// @param regions           Protected regions to patch.
/// @param compiled_outputs  Compiled bytecodes (matched by name).
/// @param arch              Target architecture.
/// @param mode              Sub-mode (32/64).
/// @param diag              Diagnostic collector.
[[nodiscard]] tl::expected<PatchPayload, Common::DiagnosticCode>
build_payload(const std::vector<RegionPatchInfo>& regions,
              const std::vector<SDK::BytecodeCompiler::CompilationOutput>& compiled_outputs,
              Common::FileArch arch, Common::FileMode mode,
              Common::DiagnosticCollector& diag =
                  Common::DiagnosticCollector::noop()) noexcept;

}  // namespace VMPilot::Loader

#endif  // __LOADER_PAYLOAD_BUILDER_HPP__
