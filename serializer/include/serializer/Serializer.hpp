#ifndef __SDK_SERIALIZER_HPP__
#define __SDK_SERIALIZER_HPP__
#pragma once

#include <CompilationUnit.hpp>
#include <diagnostic_collector.hpp>
#include <segmentator.hpp>

#include <tl/expected.hpp>

#include <string>
#include <vector>

namespace VMPilot::SDK::Serializer {

/// Build CompilationUnits from a SegmentationResult.
/// Single conversion point. Reports orphan sites as Error diagnostics.
[[nodiscard]] std::vector<Core::CompilationUnit> build_units(
    const Segmentator::SegmentationResult& result,
    Common::DiagnosticCollector& diag =
        Common::DiagnosticCollector::noop());

/// Dump CompilationUnits to disk (manifest.toml + context.pb + *.unit.pb).
/// Cleans up output_dir on failure.
[[nodiscard]] tl::expected<void, Common::DiagnosticCode> dump(
    const std::vector<Core::CompilationUnit>& units,
    const std::string& output_dir,
    Common::DiagnosticCollector& diag =
        Common::DiagnosticCollector::noop());

/// Load all CompilationUnits from a previously dumped directory.
/// Strict mode: fails if any unit cannot be loaded.
[[nodiscard]] tl::expected<std::vector<Core::CompilationUnit>,
                           Common::DiagnosticCode>
load(const std::string& input_dir,
     Common::DiagnosticCollector& diag =
         Common::DiagnosticCollector::noop());

/// Load a single CompilationUnit from a .unit.pb file + its context.pb.
[[nodiscard]] tl::expected<Core::CompilationUnit, Common::DiagnosticCode>
load_unit(const std::string& unit_pb_path,
          const std::string& context_pb_path,
          Common::DiagnosticCollector& diag =
              Common::DiagnosticCollector::noop());

}  // namespace VMPilot::SDK::Serializer

#endif  // __SDK_SERIALIZER_HPP__
