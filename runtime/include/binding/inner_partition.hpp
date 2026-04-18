#ifndef VMPILOT_RUNTIME_BINDING_INNER_PARTITION_HPP
#define VMPILOT_RUNTIME_BINDING_INNER_PARTITION_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

#include <tl/expected.hpp>

// Structured inner metadata partition.
//
// The outer envelope carries a single `inner_metadata_partition_ref` locator
// pointing at one contiguous range of bytes. Inside that range we store a
// strict-CBOR map keyed by small unsigned integers — each key names a
// specific sub-table. PBR commits to the hash of each sub-table's raw byte
// payload (not the CBOR envelope around it), so hash commitments stay stable
// across CBOR re-encodes of the outer map.
//
// Schema (doc 06 §6.1 describes the three hashes; doc 15 §5.1 describes the
// inner partition shape):
//
//     {
//       1: bytes  // unit_binding_table   — Stage 7 populates
//       2: bytes  // resolved_profile_table — Stage 7 populates
//       3: bytes  // runtime_specialization_registry — Stage 6 populates
//     }
//
// Stages 6 and 7 layer concrete content inside each byte string without
// changing the outer map shape.

namespace VMPilot::Runtime::Binding {

enum class InnerPartitionError : std::uint8_t {
    BadCbor = 1,
    NotAMap,
    MissingField,
    WrongFieldType,
};

struct InnerPartition {
    // Owning copies of each sub-table's raw byte payload. Keeping owning
    // copies rather than views simplifies lifetime (the original artifact
    // buffer can be released) and every caller today wants to hash or
    // re-parse the bytes anyway, so the copy is paid once.
    std::vector<std::uint8_t> unit_binding_table;
    std::vector<std::uint8_t> resolved_profile_table;
    std::vector<std::uint8_t> runtime_specialization_registry;
};

tl::expected<InnerPartition, InnerPartitionError>
parse_inner_partition(const std::uint8_t* data, std::size_t size) noexcept;

}  // namespace VMPilot::Runtime::Binding

#endif  // VMPILOT_RUNTIME_BINDING_INNER_PARTITION_HPP
