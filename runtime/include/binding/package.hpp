#ifndef VMPILOT_RUNTIME_BINDING_PACKAGE_HPP
#define VMPILOT_RUNTIME_BINDING_PACKAGE_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <tl/expected.hpp>

#include "envelope/outer.hpp"
#include "trust_root.hpp"
#include "vm/family_policy.hpp"

// PackageBindingRecord acceptance.
//
// The acceptance pass runs exactly once at runtime init: it validates the
// envelope-level binding (Ed25519 signature rooted in VendorTrustRoot plus
// epoch gates and canonical-encoding commitments) and, on success, hands
// back an AcceptedPackage that downstream stages consume without repeating
// the expensive checks.
//
// The acceptance pass does NOT yet unpack the inner metadata partition.
// That is Stage 7's job. For now the four PBR-embedded hashes are matched
// against the inner partition bytes using domain-separated labels; Stages 6
// and 7 will refine per-table byte slicing without changing the contract
// on this function.

namespace VMPilot::Runtime::Binding {

// Runtime-side state the epoch gates compare against. The runtime owns
// these — in Stage 12 they become persistent.
struct RuntimeEpochState {
    std::uint64_t runtime_epoch;
    std::uint64_t minimum_accepted_epoch;
};

// Runtime configuration fed into accept_package. Everything here comes
// from runtime build configuration or platform state, never from the
// artifact itself.
struct AcceptConfig {
    std::vector<std::string> supported_schema_versions;
    std::vector<std::string> supported_canonical_encodings;
    RuntimeEpochState epoch;

    // Per-unit policy floor (doc 15 §9 #4-#5). Enforced in
    // accept_unit_entry: UBR.requested_policy_id must be numerically
    // >= this floor in the {Debug, Standard, HighSec} ordering.
    // Default is Debug — i.e. no floor. Highsec runtimes raise this to
    // HighSec so a standard-tier package silently loaded on a highsec
    // runtime cannot unlock execution.
    VMPilot::DomainLabels::PolicyId minimum_policy_floor{
        VMPilot::DomainLabels::PolicyId::Debug};
};

// What survives acceptance. Retain enough to let Stages 6/7 verify unit
// binding records against the envelope-level commitments without re-
// parsing the PBR partition.
struct AcceptedPackage {
    std::string package_binding_record_id;
    std::string package_schema_version;
    std::string canonical_encoding_id;
    std::array<std::uint8_t, 32> unit_binding_table_hash;
    std::array<std::uint8_t, 32> resolved_profile_table_hash;
    std::array<std::uint8_t, 32> runtime_specialization_registry_hash;
    std::array<std::uint8_t, 32> artifact_layout_hash;
    std::uint64_t anti_downgrade_epoch;
    std::uint64_t minimum_runtime_epoch;
    std::vector<std::uint8_t> pbr_canonical_bytes;  // kept for Stage 7 cross-checks
};

enum class AcceptError : std::uint8_t {
    // Envelope / partition structure.
    PbrLocatorOutOfBounds = 1,
    PbrPartitionMalformed,        // not a CBOR array of [bytes, map]
    PbrCanonicalDecodeFailed,     // inner canonical bytes aren't strict CBOR
    PbrAuthDecodeFailed,          // binding_auth object malformed

    // Field contract.
    MissingCoreField,
    WrongFieldType,
    WrongHashSize,                // expected 32 bytes for a *_hash field
    SignatureWrongSize,

    // Trust root sanity.
    TrustRootMalformed,           // trust_root_is_well_formed() == false
    TrustRootKeyUsageMismatch,    // root.key_usage != "artifact_binding_root"

    // Authenticity.
    SignatureInvalid,
    AuthCoveredDomainMismatch,    // binding_auth.covered_domain != "package-binding-v1"
    AuthKindUnsupported,
    AuthKeyIdMismatch,            // delegate not supported yet; reject unknown key_id
    AuthSignatureAlgMismatch,     // binding_auth.signature_alg_id != root.signature_alg_id

    // Acceptance policy gates.
    UnsupportedPackageSchemaVersion,
    UnsupportedCanonicalEncodingId,
    RuntimeEpochTooOld,           // minimum_runtime_epoch > runtime.epoch
    AntiDowngradeEpochTooOld,     // anti_downgrade_epoch < runtime.minimum_accepted_epoch

    // Whole-package binding.
    UnitBindingTableHashMismatch,
    ResolvedProfileTableHashMismatch,
    RuntimeSpecializationRegistryHashMismatch,
    ArtifactLayoutHashMismatch,
};

// Accept a package at runtime init. Fail-closed: any single rule violation
// returns AcceptError; no partial acceptance.
tl::expected<AcceptedPackage, AcceptError>
accept_package(const std::uint8_t* artifact_data,
               std::size_t artifact_size,
               const VendorTrustRoot& root,
               const Envelope::OuterEnvelope& env,
               const AcceptConfig& config) noexcept;

}  // namespace VMPilot::Runtime::Binding

#endif  // VMPILOT_RUNTIME_BINDING_PACKAGE_HPP
