#ifndef __LOADER_HPP__
#define __LOADER_HPP__
#pragma once

/// @file Loader.hpp
/// @brief Public API for the Loader component.
///
/// The Loader patches a binary so that protected regions (VMPilot_Begin/End)
/// transfer control to the VM runtime at execution time.

#include <LoaderTypes.hpp>
#include <diagnostic_collector.hpp>

#include <tl/expected.hpp>

namespace VMPilot::Loader {

/// Patch a binary: inject blob + stubs, overwrite protected regions.
///
/// This is the top-level entry point. It:
///   1. Opens the binary (format-specific BinaryEditor)
///   2. Creates an architecture-specific StubEmitter
///   3. Queries the next segment VA (no mutation)
///   4. Builds the payload with all fixups resolved
///   5. Injects the segment, overwrites regions, adds runtime dep
///   6. Invalidates old code signature and saves
///
/// @param request  Input parameters (paths, regions, blob, seed, arch/format)
/// @param diag     Diagnostic collector
[[nodiscard]] tl::expected<PatchResult, Common::DiagnosticCode>
patch(const PatchRequest& request,
      Common::DiagnosticCollector& diag =
          Common::DiagnosticCollector::noop()) noexcept;

}  // namespace VMPilot::Loader

#endif  // __LOADER_HPP__
