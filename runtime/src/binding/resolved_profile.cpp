#include "binding/resolved_profile.hpp"

#include "cbor/schema.hpp"
#include "cbor/strict.hpp"
#include "vm/family_policy.hpp"

namespace VMPilot::Cbor {
template <>
struct CborConsumerTraits<VMPilot::Runtime::Binding::ResolvedFamilyProfileParseError> {
    using E = VMPilot::Runtime::Binding::ResolvedFamilyProfileParseError;
    static constexpr E missing_field    = E::MissingField;
    static constexpr E wrong_field_type = E::WrongFieldType;
    static constexpr E bad_cbor         = E::BadCbor;
    static constexpr E not_a_map        = E::NotAMap;
};
}  // namespace VMPilot::Cbor

namespace VMPilot::Runtime::Binding {

namespace {

constexpr std::uint64_t kField_ProfileId                  = 1;
constexpr std::uint64_t kField_FamilyId                   = 2;
constexpr std::uint64_t kField_RequestedPolicyId          = 3;
constexpr std::uint64_t kField_ProfileRevision            = 4;
constexpr std::uint64_t kField_RuntimeSpecializationId    = 5;

}  // namespace

tl::expected<ResolvedFamilyProfileHeader, ResolvedFamilyProfileParseError>
parse_resolved_family_profile_header(const std::uint8_t* data, std::size_t size) noexcept {
    using namespace VMPilot::Cbor::Schema;
    using E = ResolvedFamilyProfileParseError;
    auto tree_or = VMPilot::Cbor::parse_strict(data, size);
    if (!tree_or) return tl::make_unexpected(E::BadCbor);

    const auto schema = std::make_tuple(
        TextField<ResolvedFamilyProfileHeader>{
            kField_ProfileId, &ResolvedFamilyProfileHeader::profile_id},
        EnumTextField<ResolvedFamilyProfileHeader,
                      VMPilot::DomainLabels::FamilyId, E>{
            kField_FamilyId, &ResolvedFamilyProfileHeader::family_id,
            E::UnknownFamilyId},
        EnumTextField<ResolvedFamilyProfileHeader,
                      VMPilot::DomainLabels::PolicyId, E>{
            kField_RequestedPolicyId,
            &ResolvedFamilyProfileHeader::requested_policy_id,
            E::UnknownPolicyId},
        TextField<ResolvedFamilyProfileHeader>{
            kField_ProfileRevision,
            &ResolvedFamilyProfileHeader::profile_revision},
        TextField<ResolvedFamilyProfileHeader>{
            kField_RuntimeSpecializationId,
            &ResolvedFamilyProfileHeader::runtime_specialization_id}
    );
    return parse_schema<ResolvedFamilyProfileHeader, E>(*tree_or, schema);
}

}  // namespace VMPilot::Runtime::Binding
