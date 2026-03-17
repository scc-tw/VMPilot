#ifndef __SDK_SERIALIZER_HPP__
#define __SDK_SERIALIZER_HPP__
#pragma once

#include <segmentator.hpp>

#include <tl/expected.hpp>

#include <string>

namespace VMPilot::SDK::Serializer {

[[nodiscard]] tl::expected<void, std::string> dump(
    const Segmentator::SegmentationResult& result,
    const std::string& output_dir);

}  // namespace VMPilot::SDK::Serializer

#endif  // __SDK_SERIALIZER_HPP__
