#include "binding/resolved_profile.hpp"

#include "cbor/strict.hpp"
#include "vm/family_policy.hpp"

namespace VMPilot::Runtime::Binding {

namespace {

using VMPilot::Cbor::Value;
using VMPilot::Cbor::parse_strict;

// Field IDs match the fixture builder in
// runtime/test/fixtures/fixture_generator.cpp
// (ResolvedFamilyProfileBuilder::build). Keep in sync.
constexpr std::uint64_t kField_ProfileId                  = 1;
constexpr std::uint64_t kField_FamilyId                   = 2;
constexpr std::uint64_t kField_RequestedPolicyId          = 3;
constexpr std::uint64_t kField_ProfileRevision            = 4;
constexpr std::uint64_t kField_RuntimeSpecializationId    = 5;

inline tl::unexpected<ResolvedFamilyProfileParseError>
err(ResolvedFamilyProfileParseError e) noexcept {
    return tl::make_unexpected(e);
}

tl::expected<std::string, ResolvedFamilyProfileParseError>
require_text(const Value& m, std::uint64_t key) noexcept {
    const Value* v = m.find_by_uint_key(key);
    if (v == nullptr) return err(ResolvedFamilyProfileParseError::MissingField);
    if (v->kind() != Value::Kind::Text) return err(ResolvedFamilyProfileParseError::WrongFieldType);
    return v->as_text();
}

}  // namespace

tl::expected<ResolvedFamilyProfileHeader, ResolvedFamilyProfileParseError>
parse_resolved_family_profile_header(const std::uint8_t* data, std::size_t size) noexcept {
    auto tree_or = parse_strict(data, size);
    if (!tree_or) return err(ResolvedFamilyProfileParseError::BadCbor);
    const Value& tree = *tree_or;
    if (tree.kind() != Value::Kind::Map) return err(ResolvedFamilyProfileParseError::NotAMap);

    auto pid = require_text(tree, kField_ProfileId);
    auto fid = require_text(tree, kField_FamilyId);
    auto rpid = require_text(tree, kField_RequestedPolicyId);
    auto rev = require_text(tree, kField_ProfileRevision);
    auto spec = require_text(tree, kField_RuntimeSpecializationId);
    if (!pid) return err(pid.error());
    if (!fid) return err(fid.error());
    if (!rpid) return err(rpid.error());
    if (!rev) return err(rev.error());
    if (!spec) return err(spec.error());

    auto fam_enum = VMPilot::DomainLabels::parse_family_id(*fid);
    if (!fam_enum) return err(ResolvedFamilyProfileParseError::UnknownFamilyId);
    auto pol_enum = VMPilot::DomainLabels::parse_policy_id(*rpid);
    if (!pol_enum) return err(ResolvedFamilyProfileParseError::UnknownPolicyId);

    ResolvedFamilyProfileHeader out;
    out.profile_id                      = std::move(*pid);
    out.family_id                       = *fam_enum;
    out.requested_policy_id             = *pol_enum;
    out.profile_revision                = std::move(*rev);
    out.runtime_specialization_id       = std::move(*spec);
    return out;
}

}  // namespace VMPilot::Runtime::Binding
