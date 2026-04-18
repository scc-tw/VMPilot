#include "fixture_generator.hpp"

#include <cstring>
#include <utility>

#include "cbor_test_encoder.hpp"
#include "sign_ed25519_testonly.hpp"
#include "test_signing_key.hpp"

#include "vm/domain_labels.hpp"

namespace VMPilot::Fixtures {

namespace {

using VMPilot::Fixtures::Cbor::MapBuilder;
using VMPilot::Fixtures::Cbor::encode_bytes;
using VMPilot::Fixtures::Cbor::encode_text;
using VMPilot::Fixtures::Cbor::encode_uint;

// Deterministic filler so negative tests have predictable hash bytes to
// compare against. i-th byte = (i * mul + offset) mod 256.
std::array<std::uint8_t, 32> filled(std::uint8_t offset, std::uint8_t mul = 1) {
    std::array<std::uint8_t, 32> out{};
    for (std::size_t i = 0; i < 32; ++i) {
        out[i] = static_cast<std::uint8_t>((i * mul + offset) & 0xff);
    }
    return out;
}

std::vector<std::uint8_t> bytes_from(const std::array<std::uint8_t, 32>& a) {
    return std::vector<std::uint8_t>(a.begin(), a.end());
}

}  // namespace

// ─── PackageBindingRecordBuilder ────────────────────────────────────────

PackageBindingRecordBuilder::PackageBindingRecordBuilder()
    : id_{"pbr-fixture-happy-path"},
      schema_version_{"package-schema-v1"},
      encoding_id_{"canonical-metadata-bytes-v1"},
      unit_binding_table_hash_{filled(0x10)},
      resolved_profile_table_hash_{filled(0x20)},
      runtime_specialization_registry_hash_{filled(0x30)},
      artifact_layout_hash_{filled(0x40)},
      anti_downgrade_epoch_{1},
      minimum_runtime_epoch_{1},
      signing_seed_{TestKey::kPrivateSeed},
      signing_public_key_{TestKey::kPublicKey} {}

PackageBindingRecordBuilder&
PackageBindingRecordBuilder::package_binding_record_id(std::string v) {
    id_ = std::move(v);
    return *this;
}
PackageBindingRecordBuilder&
PackageBindingRecordBuilder::package_schema_version(std::string v) {
    schema_version_ = std::move(v);
    return *this;
}
PackageBindingRecordBuilder&
PackageBindingRecordBuilder::canonical_encoding_id(std::string v) {
    encoding_id_ = std::move(v);
    return *this;
}
PackageBindingRecordBuilder&
PackageBindingRecordBuilder::unit_binding_table_hash(std::array<std::uint8_t, 32> v) {
    unit_binding_table_hash_ = v;
    return *this;
}
PackageBindingRecordBuilder&
PackageBindingRecordBuilder::resolved_profile_table_hash(std::array<std::uint8_t, 32> v) {
    resolved_profile_table_hash_ = v;
    return *this;
}
PackageBindingRecordBuilder&
PackageBindingRecordBuilder::runtime_specialization_registry_hash(std::array<std::uint8_t, 32> v) {
    runtime_specialization_registry_hash_ = v;
    return *this;
}
PackageBindingRecordBuilder&
PackageBindingRecordBuilder::artifact_layout_hash(std::array<std::uint8_t, 32> v) {
    artifact_layout_hash_ = v;
    return *this;
}
PackageBindingRecordBuilder&
PackageBindingRecordBuilder::anti_downgrade_epoch(std::uint64_t v) {
    anti_downgrade_epoch_ = v;
    return *this;
}
PackageBindingRecordBuilder&
PackageBindingRecordBuilder::minimum_runtime_epoch(std::uint64_t v) {
    minimum_runtime_epoch_ = v;
    return *this;
}
PackageBindingRecordBuilder&
PackageBindingRecordBuilder::signing_seed(std::array<std::uint8_t, 32> v) {
    signing_seed_ = v;
    return *this;
}
PackageBindingRecordBuilder&
PackageBindingRecordBuilder::signing_public_key(std::array<std::uint8_t, 32> v) {
    signing_public_key_ = v;
    return *this;
}

SignedArtifact PackageBindingRecordBuilder::build() const {
    // Field id assignment (small unsigned integer keys, RFC 8949 canonical
    // order follows encoded key bytes which for small unsigned ints is
    // identical to numeric order):
    //   1 package_binding_record_id
    //   2 package_schema_version
    //   3 canonical_encoding_id
    //   4 unit_binding_table_hash
    //   5 resolved_profile_table_hash
    //   6 runtime_specialization_registry_hash
    //   7 artifact_layout_hash
    //   8 anti_downgrade_epoch
    //   9 minimum_runtime_epoch
    //
    // `binding_auth` is never in the hashed content; the signature below
    // covers canonical_bytes verbatim.

    MapBuilder m;
    m.put_uint(1, encode_text(id_));
    m.put_uint(2, encode_text(schema_version_));
    m.put_uint(3, encode_text(encoding_id_));
    m.put_uint(4, encode_bytes(bytes_from(unit_binding_table_hash_)));
    m.put_uint(5, encode_bytes(bytes_from(resolved_profile_table_hash_)));
    m.put_uint(6, encode_bytes(bytes_from(runtime_specialization_registry_hash_)));
    m.put_uint(7, encode_bytes(bytes_from(artifact_layout_hash_)));
    m.put_uint(8, encode_uint(anti_downgrade_epoch_));
    m.put_uint(9, encode_uint(minimum_runtime_epoch_));

    SignedArtifact out;
    out.canonical_bytes = m.build();
    out.covered_domain = std::string(VMPilot::DomainLabels::Auth::PackageBinding);
    out.signer_public_key = signing_public_key_;
    out.signature = Sign::sign_ed25519(signing_seed_, out.covered_domain, out.canonical_bytes);
    return out;
}

}  // namespace VMPilot::Fixtures
