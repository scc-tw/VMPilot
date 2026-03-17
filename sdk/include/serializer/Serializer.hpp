#ifndef __SDK_SERIALIZER_HPP__
#define __SDK_SERIALIZER_HPP__
#pragma once

#include <CompilationUnit.hpp>
#include <segmentator.hpp>

#include <tl/expected.hpp>

#include <string>
#include <vector>

namespace VMPilot::SDK::Serializer {

/// Build CompilationUnits from a SegmentationResult.
/// This is the single conversion point: SegmentationResult → vector<CompilationUnit>.
[[nodiscard]] std::vector<Core::CompilationUnit> build_units(
    const Segmentator::SegmentationResult& result);

/// Dump CompilationUnits to disk (manifest.toml + context.pb + *.unit.pb).
[[nodiscard]] tl::expected<void, std::string> dump(
    const std::vector<Core::CompilationUnit>& units,
    const std::string& output_dir);

/// Load all CompilationUnits from a previously dumped directory.
[[nodiscard]] tl::expected<std::vector<Core::CompilationUnit>, std::string>
load(const std::string& input_dir);

/// Load a single CompilationUnit from a .unit.pb file + its context.pb.
[[nodiscard]] tl::expected<Core::CompilationUnit, std::string>
load_unit(const std::string& unit_pb_path,
          const std::string& context_pb_path);

}  // namespace VMPilot::SDK::Serializer

#endif  // __SDK_SERIALIZER_HPP__
