#ifndef VMPILOT_RUNTIME_BINDING_UNIT_HPP
#define VMPILOT_RUNTIME_BINDING_UNIT_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <tl/expected.hpp>

#include "binding/package.hpp"
#include "envelope/outer.hpp"
#include "vm/family_policy.hpp"

// Per-unit acceptance: the gate that fires every time a protected unit is
// about to run. Consumes the AcceptedPackage surface from Stage 5 plus the
// raw artifact bytes, resolves the unit's descriptor and binding record,
// cross-checks everything the signed PBR commits to, and hands back a
// fully validated AcceptedUnit.

namespace VMPilot::Runtime::Binding {

// Content-addressed payload identity (doc 06 §3.1).
struct PayloadIdentity {
    std::array<std::uint8_t, 32> sha256_digest;
    std::uint64_t payload_size;
};

// UnitDescriptor (doc 03 §7). Small fixed shape; verified via
// UBR.unit_descriptor_hash, not via a table-level commitment.
struct UnitDescriptor {
    std::string descriptor_version;
    std::string unit_id;
    std::array<std::uint8_t, 32> unit_identity_hash;
    VMPilot::DomainLabels::FamilyId family_id;
    VMPilot::DomainLabels::PolicyId requested_policy_id;
    std::string resolved_family_profile_id;
    PayloadIdentity payload_identity;
    std::string unit_binding_record_id;
};

// UnitBindingAuth (doc 06 §9.3, path A). Not a signature — a package-level
// inclusion proof.
struct UnitBindingAuth {
    std::string kind;                                      // "package_signed_unit_inclusion_v1"
    std::array<std::uint8_t, 32> unit_binding_table_hash;
    std::uint64_t inclusion_index;
    std::array<std::uint8_t, 32> record_hash;
};

// UnitBindingRecord (doc 06 §5.1).
struct UnitBindingRecord {
    std::string unit_binding_record_id;
    std::array<std::uint8_t, 32> unit_identity_hash;
    std::array<std::uint8_t, 32> unit_descriptor_hash;
    VMPilot::DomainLabels::FamilyId family_id;
    VMPilot::DomainLabels::PolicyId requested_policy_id;
    std::string resolved_family_profile_id;
    std::array<std::uint8_t, 32> resolved_family_profile_content_hash;
    PayloadIdentity payload_identity;
    std::uint64_t anti_downgrade_epoch;
    UnitBindingAuth binding_auth;
};

// Result of a successful per-unit acceptance. Retains the parsed
// descriptor + UBR plus the raw profile bytes (for Stage 8 registry
// lookup) and the resolved specialization inputs.
struct AcceptedUnit {
    UnitDescriptor descriptor;
    UnitBindingRecord ubr;
    std::vector<std::uint8_t> resolved_profile_bytes;
    std::size_t payload_offset;
    std::size_t payload_length;
};

enum class UnitAcceptError : std::uint8_t {
    // Structural.
    InnerPartitionMalformed = 1,
    UnitDescriptorTableMalformed,
    UnitDescriptorNotFound,
    UnitDescriptorMalformed,
    UnitBindingTableMalformed,
    UnitBindingRecordNotFound,
    UnitBindingRecordMalformed,
    UnitBindingAuthMalformed,
    ResolvedProfileTableMalformed,
    ResolvedProfileNotFound,

    // Field-level.
    WrongFieldType,
    MissingCoreField,
    WrongHashSize,
    UnknownFamilyId,
    UnknownPolicyId,

    // Whole-unit binding (doc 06 §7.2).
    UnitBindingRecordRecordHashMismatch,  // binding_auth.record_hash !=
                                          // domain_hash("unit-binding-record-v1",
                                          //             canonical_bytes_minus_auth)
    UnitDescriptorHashMismatch,
    ResolvedProfileContentHashMismatch,
    PayloadSha256Mismatch,
    PayloadSizeMismatch,
    DescriptorFamilyIdMismatch,
    DescriptorPolicyIdMismatch,
    DescriptorProfileIdMismatch,
    DescriptorUnitIdentityMismatch,
    UnitDescriptorBindingRecordIdMismatch,

    // Epoch gates.
    AntiDowngradeEpochTooOld,          // UBR vs runtime.minimum_accepted_epoch
    PackageEpochBelowUnitEpoch,        // PBR.anti_downgrade_epoch < UBR's (doc 06 §10)

    // Tier floor (doc 15 §9 #4).
    PolicyBelowRuntimeFloor,           // UBR.policy < config.minimum_policy_floor

    // Profile ↔ UBR cross-field mismatches (doc 08 §9 #2).
    ProfileFamilyIdMismatch,           // profile.family_id       != ubr.family_id
    ProfilePolicyIdMismatch,           // profile.policy_id       != ubr.policy_id
    ProfileIdMismatch,                 // profile.profile_id      != ubr.profile_id

    // Stage 9 — reserved exception / unwind surface.
    ExceptionUnwindContractMalformed,
    ExecutableEhStatusNotReservedDisabled,
    HandlerTableNotReservedEmpty,
    CleanupTableNotReservedEmpty,
    CrossProtectedFrameUnwindPermitted,
    NativeBoundaryUnwindBehaviorNotFailClosed,
    UnknownEhCriticalExtension,
    FamilySpecificUnwindSurfaceMismatch,
};

tl::expected<AcceptedUnit, UnitAcceptError>
accept_unit_entry(const std::uint8_t* artifact_data,
                  std::size_t artifact_size,
                  const Envelope::OuterEnvelope& env,
                  const AcceptedPackage& accepted_pkg,
                  std::string_view unit_id,
                  const AcceptConfig& config) noexcept;

}  // namespace VMPilot::Runtime::Binding

#endif  // VMPILOT_RUNTIME_BINDING_UNIT_HPP
