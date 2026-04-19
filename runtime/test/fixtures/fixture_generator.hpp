#ifndef VMPILOT_RUNTIME_TEST_FIXTURES_FIXTURE_GENERATOR_HPP
#define VMPILOT_RUNTIME_TEST_FIXTURES_FIXTURE_GENERATOR_HPP

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "vm/family_policy.hpp"

// Test fixture builder for the redesigned binding artifacts.
//
// Each builder exposes defaults matching a "well-formed happy path" fixture,
// plus setter methods so negative tests can mutate individual fields to
// exercise reject-on-mismatch acceptance rules. The builders produce paired
// (canonical_bytes, signature) so consumer tests can feed bytes straight to
// the runtime verifier without rerouting through the builder.
//
// Canonical encoding comes from cbor_test_encoder.hpp; signing from
// sign_ed25519_testonly.hpp. Domain labels from common/include/vm/domain_labels.hpp.

namespace VMPilot::Fixtures {

// Result of a single builder `.build()` call.
struct SignedArtifact {
    std::vector<std::uint8_t> canonical_bytes;        // pre-signature content
    std::array<std::uint8_t, 64> signature{};         // over length-prefixed domain || canonical_bytes
    std::string covered_domain;                       // "package-binding-v1" / etc.
    std::array<std::uint8_t, 32> signer_public_key{}; // so the test can wire it through trust root
};

// ─── PackageBindingRecord ────────────────────────────────────────────────
//
class PackageBindingRecordBuilder {
public:
    PackageBindingRecordBuilder();  // installs happy-path defaults

    PackageBindingRecordBuilder& package_binding_record_id(std::string v);
    PackageBindingRecordBuilder& package_schema_version(std::string v);
    PackageBindingRecordBuilder& canonical_encoding_id(std::string v);
    PackageBindingRecordBuilder& unit_binding_table_hash(std::array<std::uint8_t, 32> v);
    PackageBindingRecordBuilder& resolved_profile_table_hash(std::array<std::uint8_t, 32> v);
    PackageBindingRecordBuilder& runtime_specialization_registry_hash(std::array<std::uint8_t, 32> v);
    PackageBindingRecordBuilder& artifact_layout_hash(std::array<std::uint8_t, 32> v);
    PackageBindingRecordBuilder& anti_downgrade_epoch(std::uint64_t v);
    PackageBindingRecordBuilder& minimum_runtime_epoch(std::uint64_t v);

    // Sign with the hard-coded RFC 8032 test key (kPrivateSeed / kPublicKey)
    // by default. Tests that want to exercise signature mismatch can swap in
    // an arbitrary seed.
    PackageBindingRecordBuilder& signing_seed(std::array<std::uint8_t, 32> v);
    PackageBindingRecordBuilder& signing_public_key(std::array<std::uint8_t, 32> v);

    // Optional key_id to embed inside the PackageBindingAuth object. Defaults
    // to "vmpilot-dev-rfc8032-test1" — matches the runtime's trust root.
    PackageBindingRecordBuilder& auth_key_id(std::string v);

    SignedArtifact build() const;

    // Wrap canonical PBR bytes + PackageBindingAuth map into the on-disk
    // PBR partition format: a strict-CBOR array of exactly two elements
    // ([bytes, map]). The shape is what Stage 5 acceptance expects to
    // pull out of the artifact.
    std::vector<std::uint8_t> build_partition_bytes() const;

private:
    std::string id_;
    std::string schema_version_;
    std::string encoding_id_;
    std::array<std::uint8_t, 32> unit_binding_table_hash_{};
    std::array<std::uint8_t, 32> resolved_profile_table_hash_{};
    std::array<std::uint8_t, 32> runtime_specialization_registry_hash_{};
    std::array<std::uint8_t, 32> artifact_layout_hash_{};
    std::uint64_t anti_downgrade_epoch_{0};
    std::uint64_t minimum_runtime_epoch_{0};
    std::array<std::uint8_t, 32> signing_seed_{};
    std::array<std::uint8_t, 32> signing_public_key_{};
    std::string auth_key_id_;
};

// ─── UnitDescriptor builder ─────────────────────────────────────────────

class UnitDescriptorBuilder {
public:
    UnitDescriptorBuilder();

    UnitDescriptorBuilder& descriptor_version(std::string v);
    UnitDescriptorBuilder& unit_id(std::string v);
    UnitDescriptorBuilder& unit_identity_hash(std::array<std::uint8_t, 32> v);
    UnitDescriptorBuilder& family_id(std::string v);
    UnitDescriptorBuilder& family_id(VMPilot::DomainLabels::FamilyId v) {
        return family_id(std::string(VMPilot::DomainLabels::to_text(v)));
    }
    UnitDescriptorBuilder& requested_policy_id(std::string v);
    UnitDescriptorBuilder& requested_policy_id(VMPilot::DomainLabels::PolicyId v) {
        return requested_policy_id(std::string(VMPilot::DomainLabels::to_text(v)));
    }
    UnitDescriptorBuilder& resolved_family_profile_id(std::string v);
    UnitDescriptorBuilder& payload_sha256(std::array<std::uint8_t, 32> v);
    UnitDescriptorBuilder& payload_size(std::uint64_t v);
    UnitDescriptorBuilder& unit_binding_record_id(std::string v);

    std::vector<std::uint8_t> build() const;

private:
    std::string descriptor_version_;
    std::string unit_id_;
    std::array<std::uint8_t, 32> unit_identity_hash_{};
    std::string family_id_;
    std::string requested_policy_id_;
    std::string resolved_family_profile_id_;
    std::array<std::uint8_t, 32> payload_sha256_{};
    std::uint64_t payload_size_{0};
    std::string unit_binding_record_id_;
};

// ─── ResolvedFamilyProfile (minimal) builder ────────────────────────────
//
// The shared header fields (profile_id / family_id / policy_id /
// profile_revision / runtime_specialization_id) are consumed by Stage 8.
// Stage 9 additionally requires the Layer-1 exception_unwind_contract
// reserved surface to be present in canonical profile bytes.

struct ExceptionUnwindContractSpec {
    std::string eh_contract_version{"eh-contract-v1"};
    std::string executable_eh_status{"reserved_disabled_v1"};
    std::string planned_executable_eh_epoch{"v1_1"};
    std::string cross_protected_frame_unwind{"forbidden"};
    std::string native_boundary_unwind_behavior{
        "translate_to_trap_or_fail_closed"};
    std::string handler_table_status{"reserved_empty"};
    std::string cleanup_table_status{"reserved_empty"};
    std::string frame_contract_ref{"frame-contract-v1"};
    std::string stackmap_contract_ref{"stackmap-contract-v1"};
    std::string resume_contract_ref{"resume-contract-v1"};
    std::string verifier_rules_ref{"exception-unwind-verifier-rules-v1"};
    std::string family_specific_unwind_surface_ref;
    std::vector<std::string> critical_extensions;
};

class ResolvedFamilyProfileBuilder {
public:
    ResolvedFamilyProfileBuilder();

    ResolvedFamilyProfileBuilder& profile_id(std::string v);
    ResolvedFamilyProfileBuilder& family_id(std::string v);
    ResolvedFamilyProfileBuilder& family_id(VMPilot::DomainLabels::FamilyId v) {
        return family_id(std::string(VMPilot::DomainLabels::to_text(v)));
    }
    ResolvedFamilyProfileBuilder& requested_policy_id(std::string v);
    ResolvedFamilyProfileBuilder& requested_policy_id(VMPilot::DomainLabels::PolicyId v) {
        return requested_policy_id(std::string(VMPilot::DomainLabels::to_text(v)));
    }
    ResolvedFamilyProfileBuilder& profile_revision(std::string v);
    ResolvedFamilyProfileBuilder& runtime_specialization_id(std::string v);
    ResolvedFamilyProfileBuilder& semantic_contract_version(std::string v);
    ResolvedFamilyProfileBuilder& exception_unwind_contract(ExceptionUnwindContractSpec v);

    std::vector<std::uint8_t> build() const;

private:
    std::string profile_id_;
    std::string family_id_;
    std::string requested_policy_id_;
    std::string profile_revision_;
    std::string runtime_specialization_id_;
    std::string semantic_contract_version_;
    ExceptionUnwindContractSpec exception_unwind_contract_;
};

// ─── UnitBindingRecord builder ──────────────────────────────────────────

struct UnitBindingAuthSpec {
    std::string kind{"package_signed_unit_inclusion_v1"};
    std::array<std::uint8_t, 32> unit_binding_table_hash{};
    std::uint64_t inclusion_index{0};
    std::array<std::uint8_t, 32> record_hash{};
};

class UnitBindingRecordBuilder {
public:
    UnitBindingRecordBuilder();

    UnitBindingRecordBuilder& unit_binding_record_id(std::string v);
    UnitBindingRecordBuilder& unit_identity_hash(std::array<std::uint8_t, 32> v);
    UnitBindingRecordBuilder& unit_descriptor_hash(std::array<std::uint8_t, 32> v);
    UnitBindingRecordBuilder& family_id(std::string v);
    UnitBindingRecordBuilder& family_id(VMPilot::DomainLabels::FamilyId v) {
        return family_id(std::string(VMPilot::DomainLabels::to_text(v)));
    }
    UnitBindingRecordBuilder& requested_policy_id(std::string v);
    UnitBindingRecordBuilder& requested_policy_id(VMPilot::DomainLabels::PolicyId v) {
        return requested_policy_id(std::string(VMPilot::DomainLabels::to_text(v)));
    }
    UnitBindingRecordBuilder& resolved_family_profile_id(std::string v);
    UnitBindingRecordBuilder& resolved_family_profile_content_hash(std::array<std::uint8_t, 32> v);
    UnitBindingRecordBuilder& payload_sha256(std::array<std::uint8_t, 32> v);
    UnitBindingRecordBuilder& payload_size(std::uint64_t v);
    UnitBindingRecordBuilder& anti_downgrade_epoch(std::uint64_t v);
    UnitBindingRecordBuilder& binding_auth(UnitBindingAuthSpec v);

    std::vector<std::uint8_t> build() const;

private:
    std::string unit_binding_record_id_;
    std::array<std::uint8_t, 32> unit_identity_hash_{};
    std::array<std::uint8_t, 32> unit_descriptor_hash_{};
    std::string family_id_;
    std::string requested_policy_id_;
    std::string resolved_family_profile_id_;
    std::array<std::uint8_t, 32> resolved_family_profile_content_hash_{};
    std::array<std::uint8_t, 32> payload_sha256_{};
    std::uint64_t payload_size_{0};
    std::uint64_t anti_downgrade_epoch_{1};
    UnitBindingAuthSpec binding_auth_;
};

// SHA-256 of the given bytes. Wraps VMPilot::Crypto::SHA256 so tests
// can reach the digest without including VMPilot_crypto.hpp (which
// opens an ambiguous `Crypto` namespace against runtime internals).
std::array<std::uint8_t, 32>
sha256_of(const std::vector<std::uint8_t>& bytes);

// Wrap a list of UBR canonical byte strings into a strict-CBOR array —
// the on-disk shape of `unit_binding_table`. Each element in `ubr_bytes`
// is itself the full UBR map encoding.
std::vector<std::uint8_t>
build_unit_binding_table_bytes(const std::vector<std::vector<std::uint8_t>>& ubr_bytes);

// Wrap a list of (unit_id, descriptor_bytes) into the
// UnitDescriptorTable CBOR map (text-keyed by unit_id).
std::vector<std::uint8_t>
build_unit_descriptor_table_bytes(
    const std::vector<std::pair<std::string, std::vector<std::uint8_t>>>& entries);

// Wrap a list of (profile_id, profile_bytes) into the
// ResolvedFamilyProfileTable CBOR map (text-keyed by profile_id).
std::vector<std::uint8_t>
build_resolved_profile_table_bytes(
    const std::vector<std::pair<std::string, std::vector<std::uint8_t>>>& entries);

// ─── RuntimeSpecializationRegistry builder ──────────────────────────────
//
// Assembles the canonical CBOR bytes that make up the registry sub-table
// inside the inner partition. Entries follow doc 08 §3.2.

// PolicyRequirement spec for fixture building. Producer canonical
// CBOR bytes are generated by encode_policy_requirement() below;
// RegistryEntrySpec picks up both the bytes and the hash automatically
// when policy_requirement is populated. Negative tests can set
// provider_requirement_canonical_bytes / provider_requirement_hash
// directly to inject inconsistencies.
struct PolicyRequirementSpec {
    std::string requirement_version{"policy-requirement-v1"};
    std::string required_policy_floor{"standard"};
    std::vector<std::string> required_family_set{"f1"};
    bool require_hardware_bound{false};
    bool require_non_exportable_key{false};
    bool require_online_freshness{false};
    bool require_remote_attestation{false};
    std::string require_recovery_model{"self_service"};
    std::vector<std::string> allowed_provider_classes{"local_embedded"};
    std::uint64_t minimum_provider_epoch{0};
};

// Encode a PolicyRequirementSpec into the strict-CBOR canonical form
// that the runtime parses (keys 1..10 per the schema in
// runtime/src/provider/local_embedded.cpp). The hand-rolled
// stringly-typed encoder lives in fixtures only; production runtime
// never serializes PolicyRequirement.
std::vector<std::uint8_t>
encode_policy_requirement(const PolicyRequirementSpec& spec);

struct RegistryEntrySpec {
    std::string runtime_specialization_id;
    std::string family_id;
    std::string requested_policy_id;
    std::string profile_revision;
    std::string semantic_contract_version{"semantic-contract-v1"};
    std::string execution_contract_ref{"execution-contract-ref-v1"};
    std::array<std::uint8_t, 32> required_runtime_primitives_hash{};
    std::array<std::uint8_t, 32> required_runtime_helpers_hash{};
    std::array<std::uint8_t, 32> provider_requirement_hash{};
    std::array<std::uint8_t, 32> accepted_profile_content_hash{};
    std::uint64_t diagnostic_visibility_class{0};
    bool enabled_in_this_runtime{true};

    // Optional PolicyRequirement spec; when present,
    // add_entry() auto-fills the canonical bytes and hash.
    std::optional<PolicyRequirementSpec> policy_requirement;

    // Explicit override surface for negative tests. When the bytes
    // are non-empty AND policy_requirement is unset, add_entry()
    // ships them as-is without recomputing the hash, so tests can
    // inject mismatched bytes ↔ hash pairs.
    std::vector<std::uint8_t> provider_requirement_canonical_bytes;
};

class RuntimeSpecializationRegistryBuilder {
public:
    RuntimeSpecializationRegistryBuilder();

    RuntimeSpecializationRegistryBuilder& registry_version(std::string v);
    RuntimeSpecializationRegistryBuilder& runtime_build_id(std::string v);
    RuntimeSpecializationRegistryBuilder& package_schema_version(std::string v);
    RuntimeSpecializationRegistryBuilder& registry_epoch(std::uint64_t v);
    RuntimeSpecializationRegistryBuilder& add_entry(RegistryEntrySpec e);
    RuntimeSpecializationRegistryBuilder& clear_entries();

    RuntimeSpecializationRegistryBuilder& signing_seed(std::array<std::uint8_t, 32> v);
    RuntimeSpecializationRegistryBuilder& auth_key_id(std::string v);

    // The inner canonical CBOR map of the registry (unsigned). Tests
    // that want to verify raw canonical round-trips use this entry
    // point; runtime consumers should target `build()` instead.
    std::vector<std::uint8_t> build_canonical_bytes() const;

    // The signed on-disk partition: strict-CBOR array of
    // [canonical_bytes, RegistryBindingAuth]. This is what goes into
    // the inner partition's `runtime_specialization_registry` slot and
    // what `Registry::parse_partition` consumes.
    std::vector<std::uint8_t> build() const;

private:
    std::string registry_version_;
    std::string runtime_build_id_;
    std::string package_schema_version_;
    std::uint64_t registry_epoch_;
    std::vector<RegistryEntrySpec> entries_;
    std::array<std::uint8_t, 32> signing_seed_;
    std::string auth_key_id_;
};

// Build the inner partition CBOR map that wraps the four sub-tables.
// Keys 1..3 are committed by PBR hashes; key 4 (unit_descriptor_table)
// flows through UBR.unit_descriptor_hash instead.
std::vector<std::uint8_t>
build_inner_partition_bytes(const std::vector<std::uint8_t>& unit_binding_table_bytes,
                            const std::vector<std::uint8_t>& resolved_profile_table_bytes,
                            const std::vector<std::uint8_t>& registry_bytes,
                            const std::vector<std::uint8_t>& unit_descriptor_table_bytes);

// ─── PackageArtifact: full end-to-end artifact assembly ──────────────────
//
// Composes outer envelope, PBR partition (with correct hashes over inner
// partition + envelope body + registry, signed under the fixture's test
// key), inner partition placeholder, and payload placeholder into one
// byte stream that `accept_package` can consume directly.

struct PackageArtifactAssembly {
    std::vector<std::uint8_t> bytes;             // full on-disk artifact
    std::size_t metadata_offset;                 // outer envelope body start
    std::size_t metadata_length;
    std::size_t envelope_body_end;               // metadata_offset + metadata_length
    std::size_t pbr_offset;
    std::size_t pbr_length;
    std::size_t inner_offset;
    std::size_t inner_length;
    std::size_t payload_offset;
    std::size_t payload_length;

    // Audit trail — the hashes that went into the PBR. Tests can mutate
    // these and reassemble to build reject fixtures.
    std::array<std::uint8_t, 32> unit_binding_table_hash;
    std::array<std::uint8_t, 32> resolved_profile_table_hash;
    std::array<std::uint8_t, 32> runtime_specialization_registry_hash;
    std::array<std::uint8_t, 32> artifact_layout_hash;

    std::array<std::uint8_t, 32> signer_public_key;
};

class PackageArtifactBuilder {
public:
    PackageArtifactBuilder();

    PackageArtifactBuilder& package_binding_record_id(std::string v);
    PackageArtifactBuilder& package_schema_version(std::string v);
    PackageArtifactBuilder& canonical_encoding_id(std::string v);
    PackageArtifactBuilder& anti_downgrade_epoch(std::uint64_t v);
    PackageArtifactBuilder& minimum_runtime_epoch(std::uint64_t v);
    // Replace the default inner partition bytes (the pre-assembled CBOR
    // map wrapping UBT + profile table + registry). Callers that just
    // want to override a single sub-table should prefer the targeted
    // setters below.
    PackageArtifactBuilder& inner_partition_bytes(std::vector<std::uint8_t> v);

    // Targeted sub-table overrides — only used when the caller has not
    // already supplied a pre-built inner partition. Leaving any of these
    // empty lets build() auto-assemble a self-consistent sub-table from
    // the default UnitDescriptor / UnitBindingRecord / profile builders.
    PackageArtifactBuilder& unit_binding_table_bytes(std::vector<std::uint8_t> v);
    PackageArtifactBuilder& resolved_profile_table_bytes(std::vector<std::uint8_t> v);
    PackageArtifactBuilder& unit_descriptor_table_bytes(std::vector<std::uint8_t> v);
    PackageArtifactBuilder& registry_bytes(std::vector<std::uint8_t> v);

    // Unit identity for the default unit baked into auto-assembled sub-
    // tables. Setters here are ignored when a caller has already fed in
    // table-level override bytes above.
    PackageArtifactBuilder& default_unit_id(std::string v);
    PackageArtifactBuilder& default_unit_binding_record_id(std::string v);
    PackageArtifactBuilder& default_resolved_family_profile_id(std::string v);
    PackageArtifactBuilder& default_family_id(std::string v);
    PackageArtifactBuilder& default_policy_id(std::string v);
    PackageArtifactBuilder& default_unit_anti_downgrade_epoch(std::uint64_t v);

    PackageArtifactBuilder& payload_bytes(std::vector<std::uint8_t> v);
    PackageArtifactBuilder& signing_seed(std::array<std::uint8_t, 32> v);
    PackageArtifactBuilder& signing_public_key(std::array<std::uint8_t, 32> v);
    PackageArtifactBuilder& auth_key_id(std::string v);

    PackageArtifactAssembly build() const;

private:
    std::string id_;
    std::string schema_version_;
    std::string encoding_id_;
    std::uint64_t anti_downgrade_epoch_;
    std::uint64_t minimum_runtime_epoch_;

    // If non-empty the pre-built inner partition is used verbatim. Otherwise
    // build() assembles a CBOR map out of the four sub-table fields, with
    // any individually overridden sub-table swapped in.
    std::vector<std::uint8_t> inner_bytes_override_;
    std::vector<std::uint8_t> unit_binding_table_bytes_;
    std::vector<std::uint8_t> resolved_profile_table_bytes_;
    std::vector<std::uint8_t> unit_descriptor_table_bytes_;
    std::vector<std::uint8_t> registry_bytes_;

    // Default-unit specs used to auto-assemble sub-tables when they are
    // not overridden. build() keeps these in sync so the generated UBR,
    // descriptor, and profile all agree with each other.
    std::string default_unit_id_{"u-happy"};
    std::string default_record_id_{"ubr-happy"};
    std::string default_profile_id_{"rfp-happy"};
    std::string default_family_id_{"f1"};
    std::string default_policy_id_{"standard"};
    std::uint64_t default_unit_anti_downgrade_epoch_{1};

    std::vector<std::uint8_t> payload_bytes_;
    std::array<std::uint8_t, 32> signing_seed_;
    std::array<std::uint8_t, 32> signing_public_key_;
    std::string auth_key_id_;
};

// ─── OuterEnvelope ───────────────────────────────────────────────────────
//
// Assembles full artifact byte stream: 16-byte magic + 4-byte
// big-endian metadata length + canonical CBOR metadata + data
// partitions (pbr bytes, inner metadata, payload). All three data
// partitions are placed back-to-back after the envelope metadata.

struct OuterEnvelopeArtifact {
    std::vector<std::uint8_t> bytes;         // full on-disk stream
    std::size_t metadata_offset;             // after fixed header
    std::size_t metadata_length;
    std::size_t pbr_offset;
    std::size_t pbr_length;
    std::size_t inner_offset;
    std::size_t inner_length;
    std::size_t payload_offset;
    std::size_t payload_length;
};

class OuterEnvelopeBuilder {
public:
    OuterEnvelopeBuilder();  // installs happy-path defaults

    OuterEnvelopeBuilder& outer_format_version(std::uint64_t v);
    OuterEnvelopeBuilder& package_schema_version(std::string v);
    OuterEnvelopeBuilder& canonical_encoding_id(std::string v);
    OuterEnvelopeBuilder& section_table_shape_class(std::uint64_t v);

    OuterEnvelopeBuilder& pbr_bytes(std::vector<std::uint8_t> v);
    OuterEnvelopeBuilder& inner_partition_bytes(std::vector<std::uint8_t> v);
    OuterEnvelopeBuilder& payload_bytes(std::vector<std::uint8_t> v);

    // Negative-test escape hatch: let the caller inject an arbitrary text
    // field into the metadata map so tests can exercise the skeleton-leak
    // parser rule. Key id must not collide with a core envelope field.
    OuterEnvelopeBuilder& extra_text_field(std::uint64_t key, std::string v);

    OuterEnvelopeArtifact build() const;

private:
    std::uint64_t outer_format_version_{1};
    std::string schema_version_;
    std::string encoding_id_;
    std::uint64_t shape_class_{0};
    std::vector<std::uint8_t> pbr_bytes_;
    std::vector<std::uint8_t> inner_bytes_;
    std::vector<std::uint8_t> payload_bytes_;
    std::vector<std::pair<std::uint64_t, std::string>> extra_fields_;
};

}  // namespace VMPilot::Fixtures

#endif  // VMPILOT_RUNTIME_TEST_FIXTURES_FIXTURE_GENERATOR_HPP
