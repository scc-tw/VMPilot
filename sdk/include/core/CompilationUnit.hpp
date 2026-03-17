#ifndef __SDK_CORE_COMPILATION_UNIT_HPP__
#define __SDK_CORE_COMPILATION_UNIT_HPP__
#pragma once

#include <CompilationContext.hpp>
#include <DataReference.hpp>

#include <cstdint>
#include <memory>
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

    /// Shared compilation context.  Read-only, safe for concurrent access.
    /// Shared across all units from the same binary — lifetime-safe via shared_ptr.
    std::shared_ptr<const Segmentator::CompilationContext> context;

    /// Data references discovered by ReferenceAnalyzer (globals, TLS, GOT loads).
    std::vector<Core::DataReference> data_references;
};

}  // namespace VMPilot::SDK::Core

#endif  // __SDK_CORE_COMPILATION_UNIT_HPP__
