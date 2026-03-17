#ifndef __SDK_CORE_COMPILATION_UNIT_HPP__
#define __SDK_CORE_COMPILATION_UNIT_HPP__
#pragma once

#include <CompilationContext.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace VMPilot::SDK::Core {

/// In-memory compilation unit built from segmentation results.
/// NOT a protobuf type — this is the C++ struct used by the compiler backend.
struct CompilationUnit {
    std::string name;                // __FUNCTION__ name
    uint64_t addr = 0;              // original VA
    uint64_t size = 0;
    std::vector<uint8_t> code;      // raw native code bytes
    std::string enclosing_symbol;   // mangled symbol
    bool is_canonical = false;

    /// Non-owning pointer to the shared compilation context.
    /// Must outlive this unit.  Read-only — safe for concurrent access.
    const Segmentator::CompilationContext* context = nullptr;
};

}  // namespace VMPilot::SDK::Core

#endif  // __SDK_CORE_COMPILATION_UNIT_HPP__
