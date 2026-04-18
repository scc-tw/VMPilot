#include "fixture_generator.hpp"

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <utility>

#include "cbor_test_encoder.hpp"
#include "sign_ed25519_testonly.hpp"
#include "test_signing_key.hpp"

#include "cbor/strict.hpp"
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
      signing_public_key_{TestKey::kPublicKey},
      auth_key_id_{"vmpilot-dev-rfc8032-test1"} {}

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
PackageBindingRecordBuilder&
PackageBindingRecordBuilder::auth_key_id(std::string v) {
    auth_key_id_ = std::move(v);
    return *this;
}

namespace {

std::vector<std::uint8_t>
encode_package_binding_auth(const std::array<std::uint8_t, 64>& signature,
                            const std::string& key_id) {
    // Field IDs match runtime/src/binding/package.cpp.
    VMPilot::Fixtures::Cbor::MapBuilder auth;
    auth.put_uint(1, VMPilot::Fixtures::Cbor::encode_text("vendor_signature_v1"));
    auth.put_uint(2, VMPilot::Fixtures::Cbor::encode_text(key_id));
    auth.put_uint(3, VMPilot::Fixtures::Cbor::encode_text("ed25519-pure-v1"));
    auth.put_uint(4, VMPilot::Fixtures::Cbor::encode_text(
        std::string(VMPilot::DomainLabels::Auth::PackageBinding)));
    std::vector<std::uint8_t> sig_vec(signature.begin(), signature.end());
    auth.put_uint(5, VMPilot::Fixtures::Cbor::encode_bytes(sig_vec));
    return auth.build();
}

std::vector<std::uint8_t>
wrap_partition_array(const std::vector<std::uint8_t>& canonical_bytes,
                     const std::vector<std::uint8_t>& auth_bytes) {
    const auto canonical_element =
        VMPilot::Fixtures::Cbor::encode_bytes(canonical_bytes);
    return VMPilot::Fixtures::Cbor::encode_array({canonical_element, auth_bytes});
}

}  // namespace

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

std::vector<std::uint8_t>
PackageBindingRecordBuilder::build_partition_bytes() const {
    const auto signed_ = build();
    const auto auth_bytes = encode_package_binding_auth(signed_.signature, auth_key_id_);
    return wrap_partition_array(signed_.canonical_bytes, auth_bytes);
}

// ─── PackageArtifactBuilder ─────────────────────────────────────────────

PackageArtifactBuilder::PackageArtifactBuilder()
    : id_{"pbr-fixture-happy-path"},
      schema_version_{"package-schema-v1"},
      encoding_id_{"canonical-metadata-bytes-v1"},
      anti_downgrade_epoch_{1},
      minimum_runtime_epoch_{1},
      inner_bytes_(std::vector<std::uint8_t>(64, 0xAB)),
      payload_bytes_(std::vector<std::uint8_t>(96, 0xCD)),
      signing_seed_{TestKey::kPrivateSeed},
      signing_public_key_{TestKey::kPublicKey},
      auth_key_id_{"vmpilot-dev-rfc8032-test1"} {}

PackageArtifactBuilder&
PackageArtifactBuilder::package_binding_record_id(std::string v) {
    id_ = std::move(v); return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::package_schema_version(std::string v) {
    schema_version_ = std::move(v); return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::canonical_encoding_id(std::string v) {
    encoding_id_ = std::move(v); return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::anti_downgrade_epoch(std::uint64_t v) {
    anti_downgrade_epoch_ = v; return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::minimum_runtime_epoch(std::uint64_t v) {
    minimum_runtime_epoch_ = v; return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::inner_partition_bytes(std::vector<std::uint8_t> v) {
    inner_bytes_ = std::move(v); return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::payload_bytes(std::vector<std::uint8_t> v) {
    payload_bytes_ = std::move(v); return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::signing_seed(std::array<std::uint8_t, 32> v) {
    signing_seed_ = v; return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::signing_public_key(std::array<std::uint8_t, 32> v) {
    signing_public_key_ = v; return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::auth_key_id(std::string v) {
    auth_key_id_ = std::move(v); return *this;
}

PackageArtifactAssembly PackageArtifactBuilder::build() const {
    // Inner-partition-derived hashes — doc 06 §6.1 `unit_binding_table_hash`
    // and friends. Domain labels differ so the three digests don't
    // collide despite covering the same byte range. Stages 6/7 will
    // introduce real per-table byte slicing; the contract surface stays
    // the same.
    const auto ubt_hash = VMPilot::Cbor::domain_hash_sha256(
        VMPilot::DomainLabels::Hash::UnitBindingTable, inner_bytes_);
    const auto rpt_hash = VMPilot::Cbor::domain_hash_sha256(
        VMPilot::DomainLabels::Hash::ResolvedProfileTable, inner_bytes_);
    const auto reg_hash = VMPilot::Cbor::domain_hash_sha256(
        VMPilot::DomainLabels::Hash::RuntimeSpecRegistry, inner_bytes_);

    // Two-pass artifact_layout_hash computation:
    //
    //  pass 1: size the PBR partition using a placeholder artifact layout
    //          hash and placeholder signature. PBR canonical length and
    //          auth length are deterministic (fixed-size hashes, fixed
    //          signature size), so partition size won't shift between
    //          placeholder and real content.
    //  pass 2: build the outer envelope with the placeholder partition so
    //          the envelope body bytes stabilize; hash them to produce
    //          the real artifact_layout_hash.
    //  pass 3: rebuild the PBR with the real hash, re-sign, re-wrap; the
    //          partition size equals what the envelope commits to, so
    //          the envelope body stays byte-identical.

    const std::array<std::uint8_t, 32> placeholder_hash{};
    PackageBindingRecordBuilder placeholder_pb;
    placeholder_pb.package_binding_record_id(id_)
                  .package_schema_version(schema_version_)
                  .canonical_encoding_id(encoding_id_)
                  .unit_binding_table_hash(ubt_hash)
                  .resolved_profile_table_hash(rpt_hash)
                  .runtime_specialization_registry_hash(reg_hash)
                  .artifact_layout_hash(placeholder_hash)
                  .anti_downgrade_epoch(anti_downgrade_epoch_)
                  .minimum_runtime_epoch(minimum_runtime_epoch_)
                  .signing_seed(signing_seed_)
                  .signing_public_key(signing_public_key_)
                  .auth_key_id(auth_key_id_);
    const auto placeholder_partition = placeholder_pb.build_partition_bytes();

    OuterEnvelopeBuilder oeb1;
    oeb1.pbr_bytes(placeholder_partition)
        .inner_partition_bytes(inner_bytes_)
        .payload_bytes(payload_bytes_);
    const auto placeholder_envelope = oeb1.build();

    // artifact_layout_hash commits to the envelope body (fixed header +
    // canonical CBOR metadata). With locators and metadata stable from the
    // placeholder build, the body is exactly what the real build will
    // produce once the PBR is swapped in.
    const std::size_t envelope_body_end =
        placeholder_envelope.metadata_offset + placeholder_envelope.metadata_length;
    const auto artifact_layout_hash = VMPilot::Cbor::domain_hash_sha256(
        VMPilot::DomainLabels::Hash::ArtifactLayout,
        placeholder_envelope.bytes.data(), envelope_body_end);

    PackageBindingRecordBuilder real_pb;
    real_pb.package_binding_record_id(id_)
           .package_schema_version(schema_version_)
           .canonical_encoding_id(encoding_id_)
           .unit_binding_table_hash(ubt_hash)
           .resolved_profile_table_hash(rpt_hash)
           .runtime_specialization_registry_hash(reg_hash)
           .artifact_layout_hash(artifact_layout_hash)
           .anti_downgrade_epoch(anti_downgrade_epoch_)
           .minimum_runtime_epoch(minimum_runtime_epoch_)
           .signing_seed(signing_seed_)
           .signing_public_key(signing_public_key_)
           .auth_key_id(auth_key_id_);
    const auto real_partition = real_pb.build_partition_bytes();

    if (real_partition.size() != placeholder_partition.size()) {
        throw std::runtime_error(
            "PackageArtifactBuilder: partition size shifted between placeholder and real PBR");
    }

    OuterEnvelopeBuilder oeb2;
    oeb2.pbr_bytes(real_partition)
        .inner_partition_bytes(inner_bytes_)
        .payload_bytes(payload_bytes_);
    const auto final_envelope = oeb2.build();

    PackageArtifactAssembly out;
    out.bytes = final_envelope.bytes;
    out.metadata_offset = final_envelope.metadata_offset;
    out.metadata_length = final_envelope.metadata_length;
    out.envelope_body_end = final_envelope.metadata_offset + final_envelope.metadata_length;
    out.pbr_offset = final_envelope.pbr_offset;
    out.pbr_length = final_envelope.pbr_length;
    out.inner_offset = final_envelope.inner_offset;
    out.inner_length = final_envelope.inner_length;
    out.payload_offset = final_envelope.payload_offset;
    out.payload_length = final_envelope.payload_length;
    out.unit_binding_table_hash = ubt_hash;
    out.resolved_profile_table_hash = rpt_hash;
    out.runtime_specialization_registry_hash = reg_hash;
    out.artifact_layout_hash = artifact_layout_hash;
    out.signer_public_key = signing_public_key_;
    return out;
}

}  // namespace VMPilot::Fixtures
