#ifndef VMPILOT_RUNTIME_BINDING_INNER_PARTITION_HPP
#define VMPILOT_RUNTIME_BINDING_INNER_PARTITION_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

#include <tl/expected.hpp>

// Structured inner metadata partition.
//
// Schema (doc 06 §6.1 + doc 15 §5.1):
//
//     {
//       1: bytes  unit_binding_table               — Stage 7
//       2: bytes  resolved_profile_table           — Stage 7
//       3: bytes  runtime_specialization_registry  — Stage 6
//       4: bytes  unit_descriptor_table            — Stage 7 (no PBR hash;
//                                                   authenticity flows
//                                                   through UBR per
//                                                   doc 06 §6.3)
//     }
//
// Keys 1..3 are committed by PackageBindingRecord hashes. Key 4 is NOT
// separately hashed — each UnitBindingRecord carries a
// `unit_descriptor_hash` that covers its corresponding descriptor.

namespace VMPilot::Runtime::Binding {

enum class InnerPartitionError : std::uint8_t {
    BadCbor = 1,
    NotAMap,
    MissingField,
    WrongFieldType,
    UnknownCoreField,
};

struct InnerPartition {
    // Owning copies of each sub-table's raw byte payload. Keeping owning
    // copies rather than views simplifies lifetime (the original artifact
    // buffer can be released) and every caller today wants to hash or
    // re-parse the bytes anyway, so the copy is paid once.
    std::vector<std::uint8_t> unit_binding_table;
    std::vector<std::uint8_t> resolved_profile_table;
    std::vector<std::uint8_t> runtime_specialization_registry;
    std::vector<std::uint8_t> unit_descriptor_table;
};

[[nodiscard]] tl::expected<InnerPartition, InnerPartitionError>
parse_inner_partition(const std::uint8_t* data, std::size_t size) noexcept;

}  // namespace VMPilot::Runtime::Binding

#endif  // VMPILOT_RUNTIME_BINDING_INNER_PARTITION_HPP
