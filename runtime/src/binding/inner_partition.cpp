#include "binding/inner_partition.hpp"

#include "cbor/strict.hpp"

namespace VMPilot::Runtime::Binding {

namespace {

using VMPilot::Cbor::Value;
using VMPilot::Cbor::parse_strict;

constexpr std::uint64_t kField_UnitBindingTable       = 1;
constexpr std::uint64_t kField_ResolvedProfileTable   = 2;
constexpr std::uint64_t kField_RuntimeSpecRegistry    = 3;

inline tl::unexpected<InnerPartitionError> err(InnerPartitionError e) noexcept {
    return tl::make_unexpected(e);
}

tl::expected<std::vector<std::uint8_t>, InnerPartitionError>
require_bytes(const Value& m, std::uint64_t key) noexcept {
    const Value* v = m.find_by_uint_key(key);
    if (v == nullptr) return err(InnerPartitionError::MissingField);
    if (v->kind() != Value::Kind::Bytes) return err(InnerPartitionError::WrongFieldType);
    return v->as_bytes();
}

}  // namespace

tl::expected<InnerPartition, InnerPartitionError>
parse_inner_partition(const std::uint8_t* data, std::size_t size) noexcept {
    auto tree_or = parse_strict(data, size);
    if (!tree_or) return err(InnerPartitionError::BadCbor);
    const Value& tree = *tree_or;
    if (tree.kind() != Value::Kind::Map) return err(InnerPartitionError::NotAMap);

    auto ubt_or = require_bytes(tree, kField_UnitBindingTable);
    auto rpt_or = require_bytes(tree, kField_ResolvedProfileTable);
    auto reg_or = require_bytes(tree, kField_RuntimeSpecRegistry);
    if (!ubt_or) return err(ubt_or.error());
    if (!rpt_or) return err(rpt_or.error());
    if (!reg_or) return err(reg_or.error());

    InnerPartition out;
    out.unit_binding_table                  = std::move(*ubt_or);
    out.resolved_profile_table              = std::move(*rpt_or);
    out.runtime_specialization_registry     = std::move(*reg_or);
    return out;
}

}  // namespace VMPilot::Runtime::Binding
