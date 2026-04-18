#ifndef VMPILOT_RUNTIME_TEST_FIXTURES_FIXTURE_GENERATOR_HPP
#define VMPILOT_RUNTIME_TEST_FIXTURES_FIXTURE_GENERATOR_HPP

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

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

// ─── RuntimeSpecializationRegistry builder ──────────────────────────────
//
// Assembles the canonical CBOR bytes that make up the registry sub-table
// inside the inner partition. Entries follow doc 08 §3.2.

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

    std::vector<std::uint8_t> build() const;

private:
    std::string registry_version_;
    std::string runtime_build_id_;
    std::string package_schema_version_;
    std::uint64_t registry_epoch_;
    std::vector<RegistryEntrySpec> entries_;
};

// Build the inner partition CBOR map that wraps the three sub-tables.
// The UBT and profile-table payloads are arbitrary opaque bytes for now;
// Stages 7+ will populate them with real content. `registry_bytes` should
// be the output of RuntimeSpecializationRegistryBuilder::build().
std::vector<std::uint8_t>
build_inner_partition_bytes(const std::vector<std::uint8_t>& unit_binding_table_bytes,
                            const std::vector<std::uint8_t>& resolved_profile_table_bytes,
                            const std::vector<std::uint8_t>& registry_bytes);

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
    // already supplied a pre-built inner partition.
    PackageArtifactBuilder& unit_binding_table_bytes(std::vector<std::uint8_t> v);
    PackageArtifactBuilder& resolved_profile_table_bytes(std::vector<std::uint8_t> v);
    PackageArtifactBuilder& registry_bytes(std::vector<std::uint8_t> v);

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
    // build() assembles a CBOR map out of the three sub-table fields.
    std::vector<std::uint8_t> inner_bytes_override_;
    std::vector<std::uint8_t> unit_binding_table_bytes_;
    std::vector<std::uint8_t> resolved_profile_table_bytes_;
    std::vector<std::uint8_t> registry_bytes_;

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
