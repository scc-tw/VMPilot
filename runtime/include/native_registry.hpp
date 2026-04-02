#pragma once
#ifndef __RUNTIME_NATIVE_REGISTRY_HPP__
#define __RUNTIME_NATIVE_REGISTRY_HPP__

/// @file native_registry.hpp
/// @brief NativeRegistry — name-to-index mapping for native function calls.
///
/// Owns the mapping from human-readable names (e.g. "puts", "draw") to
/// the TransitionEntry fields needed by the blob builder.  Auto-assigns
/// indices in registration order.
///
/// Used by VmRunner to:
///   1. Collect native function registrations via `.native("name", fn, argc)`
///   2. Inject a reference into ProgramBuilder so `native_call("name")`
///      resolves the transition entry index at build time
///   3. Provide the TestNativeCall vector for blob construction
///
/// Implementation: native_registry.cpp.

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace VMPilot::Runtime {

/// Descriptor for a registered native function.
struct NativeDescriptor {
    std::string name;           ///< human-readable name
    uint64_t    fn_addr;        ///< absolute function pointer (cast to uint64_t)
    uint8_t     arg_count;      ///< number of arguments (0-8)
    uint8_t     fp_mask;        ///< bit i set => arg i is float/double
    bool        is_variadic;    ///< true for variadic functions (sets AL on x86-64)
    uint32_t    index;          ///< auto-assigned transition entry index
};

/// Registry of native functions, mapping names to transition entry metadata.
///
/// Thread-safety: not thread-safe. Intended for single-threaded build phase.
class NativeRegistry {
public:
    NativeRegistry() = default;

    /// Register a native function by name.
    uint32_t add(const std::string& name, uint64_t fn_addr,
                 uint8_t arg_count, uint8_t fp_mask = 0,
                 bool is_variadic = false);

    /// Look up a native by name. Returns the index, or throws if not found.
    [[nodiscard]] uint32_t index_of(const std::string& name) const;

    /// Check if a name is registered.
    [[nodiscard]] bool has(const std::string& name) const noexcept;

    /// Get descriptor by index.
    [[nodiscard]] const NativeDescriptor& at(uint32_t idx) const;

    /// Number of registered natives.
    [[nodiscard]] uint32_t size() const noexcept;

    /// All registered descriptors (in index order).
    [[nodiscard]] const std::vector<NativeDescriptor>& entries() const noexcept;

    /// Build the packed arg_count field for TransitionEntry.
    ///
    /// Layout (from vm_blob.hpp):
    ///   [3:0]   = actual_arg_count
    ///   [11:4]  = fp_arg_mask
    ///   [15:12] = flags (bit 12: is_variadic)
    ///   [31:16] = 0
    ///
    /// WHY inline: trivial bit-packing, pure computation, no platform dependency.
    [[nodiscard]] static uint32_t pack_arg_count(
        uint8_t argc, uint8_t fp_mask = 0, bool variadic = false) noexcept
    {
        uint32_t packed = static_cast<uint32_t>(argc & 0x0F);
        packed |= static_cast<uint32_t>(fp_mask) << 4;
        if (variadic) packed |= (1u << 12);
        return packed;
    }

private:
    std::vector<NativeDescriptor> entries_;
    std::unordered_map<std::string, uint32_t> name_to_index_;
};

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_NATIVE_REGISTRY_HPP__
