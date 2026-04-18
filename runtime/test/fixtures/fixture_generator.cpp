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

// ─── OuterEnvelopeBuilder ───────────────────────────────────────────────

namespace {

constexpr std::array<std::uint8_t, 16> kOuterMagic = {
    'V', 'M', 'P', 'I', 'L', 'O', 'T', '_',
    'A', 'R', 'T', 'I', 'F', 'A', 'C', 'T',
};

void append_u32_be(std::vector<std::uint8_t>& out, std::uint32_t v) {
    out.push_back(static_cast<std::uint8_t>((v >> 24) & 0xff));
    out.push_back(static_cast<std::uint8_t>((v >> 16) & 0xff));
    out.push_back(static_cast<std::uint8_t>((v >>  8) & 0xff));
    out.push_back(static_cast<std::uint8_t>(v & 0xff));
}

std::vector<std::uint8_t> encode_locator_map(std::uint64_t offset, std::uint64_t length) {
    VMPilot::Fixtures::Cbor::MapBuilder m;
    m.put_uint(1, VMPilot::Fixtures::Cbor::encode_uint(offset));
    m.put_uint(2, VMPilot::Fixtures::Cbor::encode_uint(length));
    return m.build();
}

}  // namespace

OuterEnvelopeBuilder::OuterEnvelopeBuilder()
    : schema_version_{"package-schema-v1"},
      encoding_id_{"canonical-metadata-bytes-v1"},
      shape_class_{1},
      pbr_bytes_{PackageBindingRecordBuilder{}.build().canonical_bytes},
      // Inner partition and payload placeholders: arbitrary bytes sized
      // enough that the locator fits-and-doesn't-overlap arithmetic has
      // something meaningful to check.
      inner_bytes_(std::vector<std::uint8_t>(64, 0xAB)),
      payload_bytes_(std::vector<std::uint8_t>(96, 0xCD)) {}

OuterEnvelopeBuilder& OuterEnvelopeBuilder::outer_format_version(std::uint64_t v) {
    outer_format_version_ = v;
    return *this;
}
OuterEnvelopeBuilder& OuterEnvelopeBuilder::package_schema_version(std::string v) {
    schema_version_ = std::move(v);
    return *this;
}
OuterEnvelopeBuilder& OuterEnvelopeBuilder::canonical_encoding_id(std::string v) {
    encoding_id_ = std::move(v);
    return *this;
}
OuterEnvelopeBuilder& OuterEnvelopeBuilder::section_table_shape_class(std::uint64_t v) {
    shape_class_ = v;
    return *this;
}
OuterEnvelopeBuilder& OuterEnvelopeBuilder::pbr_bytes(std::vector<std::uint8_t> v) {
    pbr_bytes_ = std::move(v);
    return *this;
}
OuterEnvelopeBuilder& OuterEnvelopeBuilder::inner_partition_bytes(std::vector<std::uint8_t> v) {
    inner_bytes_ = std::move(v);
    return *this;
}
OuterEnvelopeBuilder& OuterEnvelopeBuilder::payload_bytes(std::vector<std::uint8_t> v) {
    payload_bytes_ = std::move(v);
    return *this;
}
OuterEnvelopeBuilder& OuterEnvelopeBuilder::extra_text_field(std::uint64_t key, std::string v) {
    extra_fields_.emplace_back(key, std::move(v));
    return *this;
}

OuterEnvelopeArtifact OuterEnvelopeBuilder::build() const {
    // Step 1: compute where each data partition will live so the metadata
    // can carry correct offsets before we know the metadata length itself.
    // We resolve this with a fixed-point: first assemble metadata with
    // placeholder offsets to learn `metadata_length`, then rebuild with
    // real offsets. Two passes is cheap and keeps the helper linear.

    auto build_metadata = [&](std::uint64_t pbr_off, std::uint64_t pbr_len,
                              std::uint64_t inner_off, std::uint64_t inner_len,
                              std::uint64_t payload_off, std::uint64_t payload_len) {
        MapBuilder m;
        m.put_uint(1, encode_uint(outer_format_version_));
        m.put_uint(2, encode_text(schema_version_));
        m.put_uint(3, encode_text(encoding_id_));
        m.put_uint(4, encode_uint(shape_class_));
        m.put_uint(5, encode_locator_map(pbr_off, pbr_len));
        m.put_uint(6, encode_locator_map(inner_off, inner_len));
        m.put_uint(7, encode_locator_map(payload_off, payload_len));
        for (const auto& [k, v] : extra_fields_) {
            m.put_uint(k, encode_text(v));
        }
        return m.build();
    };

    // First pass: placeholder offsets just to learn encoded metadata length.
    auto trial_meta = build_metadata(0, pbr_bytes_.size(),
                                     0, inner_bytes_.size(),
                                     0, payload_bytes_.size());
    const std::size_t fixed_header_size = 20;
    std::size_t meta_len_guess = trial_meta.size();

    // Compute offsets given the guessed metadata length.
    std::uint64_t pbr_off = fixed_header_size + meta_len_guess;
    std::uint64_t inner_off = pbr_off + pbr_bytes_.size();
    std::uint64_t payload_off = inner_off + inner_bytes_.size();

    // Second pass: rebuild metadata with real offsets; length may change if
    // the new integers encode to different widths (24..0xff bumps by one
    // byte). Iterate until stable — converges in at most two passes for
    // anything realistic.
    for (int iter = 0; iter < 4; ++iter) {
        auto meta = build_metadata(pbr_off, pbr_bytes_.size(),
                                   inner_off, inner_bytes_.size(),
                                   payload_off, payload_bytes_.size());
        if (meta.size() == meta_len_guess) {
            OuterEnvelopeArtifact out;
            out.bytes.reserve(fixed_header_size + meta.size() + pbr_bytes_.size() +
                              inner_bytes_.size() + payload_bytes_.size());
            out.bytes.insert(out.bytes.end(), kOuterMagic.begin(), kOuterMagic.end());
            append_u32_be(out.bytes, static_cast<std::uint32_t>(meta.size()));
            out.bytes.insert(out.bytes.end(), meta.begin(), meta.end());
            out.bytes.insert(out.bytes.end(), pbr_bytes_.begin(), pbr_bytes_.end());
            out.bytes.insert(out.bytes.end(), inner_bytes_.begin(), inner_bytes_.end());
            out.bytes.insert(out.bytes.end(), payload_bytes_.begin(), payload_bytes_.end());
            out.metadata_offset = fixed_header_size;
            out.metadata_length = meta.size();
            out.pbr_offset = pbr_off;
            out.pbr_length = pbr_bytes_.size();
            out.inner_offset = inner_off;
            out.inner_length = inner_bytes_.size();
            out.payload_offset = payload_off;
            out.payload_length = payload_bytes_.size();
            return out;
        }
        meta_len_guess = meta.size();
        pbr_off = fixed_header_size + meta_len_guess;
        inner_off = pbr_off + pbr_bytes_.size();
        payload_off = inner_off + inner_bytes_.size();
    }
    // Defensive fallback: should be unreachable since offsets monotonically
    // grow and every CBOR width bump is bounded by a small constant.
    throw std::runtime_error("OuterEnvelopeBuilder: metadata length failed to converge");
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
