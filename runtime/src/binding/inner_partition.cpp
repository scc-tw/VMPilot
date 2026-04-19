#include "binding/inner_partition.hpp"

#include "cbor/schema.hpp"
#include "cbor/strict.hpp"

namespace VMPilot::Cbor {
template <>
struct CborConsumerTraits<VMPilot::Runtime::Binding::InnerPartitionError> {
    using E = VMPilot::Runtime::Binding::InnerPartitionError;
    static constexpr E missing_field       = E::MissingField;
    static constexpr E wrong_field_type    = E::WrongFieldType;
    static constexpr E bad_cbor            = E::BadCbor;
    static constexpr E not_a_map           = E::NotAMap;
    static constexpr E unknown_core_field  = E::UnknownCoreField;
};
}  // namespace VMPilot::Cbor

namespace VMPilot::Runtime::Binding {

namespace {

constexpr std::uint64_t kField_UnitBindingTable       = 1;
constexpr std::uint64_t kField_ResolvedProfileTable   = 2;
constexpr std::uint64_t kField_RuntimeSpecRegistry    = 3;
constexpr std::uint64_t kField_UnitDescriptorTable    = 4;

}  // namespace

tl::expected<InnerPartition, InnerPartitionError>
parse_inner_partition(const std::uint8_t* data, std::size_t size) noexcept {
    using namespace VMPilot::Cbor::Schema;
    auto tree_or = VMPilot::Cbor::parse_strict(data, size);
    if (!tree_or) return tl::make_unexpected(InnerPartitionError::BadCbor);

    {
        auto unknown_or = reject_unknown_keys<InnerPartitionError>(
            *tree_or,
            {kField_UnitBindingTable, kField_ResolvedProfileTable,
             kField_RuntimeSpecRegistry, kField_UnitDescriptorTable});
        if (!unknown_or) return tl::make_unexpected(unknown_or.error());
    }

    const auto schema = std::make_tuple(
        BytesField<InnerPartition>{kField_UnitBindingTable,
                                   &InnerPartition::unit_binding_table},
        BytesField<InnerPartition>{kField_ResolvedProfileTable,
                                   &InnerPartition::resolved_profile_table},
        BytesField<InnerPartition>{kField_RuntimeSpecRegistry,
                                   &InnerPartition::runtime_specialization_registry},
        BytesField<InnerPartition>{kField_UnitDescriptorTable,
                                   &InnerPartition::unit_descriptor_table}
    );
    return parse_schema<InnerPartition, InnerPartitionError>(*tree_or, schema);
}

}  // namespace VMPilot::Runtime::Binding
