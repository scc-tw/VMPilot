#include "fixture_generator.hpp"

#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <utility>

#include "cbor_test_encoder.hpp"
#include "sign_ed25519_testonly.hpp"
#include "test_signing_key.hpp"

#include "VMPilot_crypto.hpp"
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

std::string default_family_specific_unwind_surface_ref(
    std::string_view family_id) {
    if (family_id == "f1") return "f1-unwind-surface-v1";
    if (family_id == "f2") return "f2-unwind-surface-v1";
    if (family_id == "f3") return "f3-unwind-surface-v1";
    return std::string(family_id) + "-unwind-surface-v1";
}


std::vector<std::uint8_t> encode_exception_unwind_contract(
    const ExceptionUnwindContractSpec& spec,
    std::string_view family_id) {
    using namespace VMPilot::Fixtures::Cbor;

    MapBuilder eh;
    eh.put_uint(1, encode_text(spec.eh_contract_version));
    eh.put_uint(2, encode_text(
        VMPilot::Runtime::EH::to_text(spec.executable_eh_status)));
    eh.put_uint(3, encode_text(spec.planned_executable_eh_epoch));
    eh.put_uint(4, encode_text(
        VMPilot::Runtime::EH::to_text(spec.cross_protected_frame_unwind)));
    eh.put_uint(5, encode_text(
        VMPilot::Runtime::EH::to_text(spec.native_boundary_unwind_behavior)));
    eh.put_uint(6, encode_text(
        VMPilot::Runtime::EH::to_text(spec.handler_table_status)));
    eh.put_uint(7, encode_text(
        VMPilot::Runtime::EH::to_text(spec.cleanup_table_status)));
    eh.put_uint(8, encode_text(spec.frame_contract_ref));
    eh.put_uint(9, encode_text(spec.stackmap_contract_ref));
    eh.put_uint(10, encode_text(spec.resume_contract_ref));
    eh.put_uint(11, encode_text(spec.verifier_rules_ref));
    eh.put_uint(12, encode_text(
        spec.family_specific_unwind_surface_ref.empty()
            ? default_family_specific_unwind_surface_ref(family_id)
            : spec.family_specific_unwind_surface_ref));

    if (!spec.critical_extensions.empty()) {
        std::vector<std::vector<std::uint8_t>> ext_bytes;
        ext_bytes.reserve(spec.critical_extensions.size());
        for (const auto& extension : spec.critical_extensions) {
            ext_bytes.push_back(encode_text(extension));
        }
        eh.put_uint(13, encode_array(ext_bytes));
    }

    return eh.build();
}

}  // namespace

// ─── PolicyRequirement canonical CBOR encoder ───────────────────────────

std::vector<std::uint8_t>
encode_policy_requirement(const PolicyRequirementSpec& spec) {
    using namespace VMPilot::Fixtures::Cbor;

    std::vector<std::vector<std::uint8_t>> family_items;
    family_items.reserve(spec.required_family_set.size());
    for (auto f : spec.required_family_set) {
        family_items.push_back(
            encode_text(VMPilot::DomainLabels::to_text(f)));
    }
    std::vector<std::vector<std::uint8_t>> class_items;
    class_items.reserve(spec.allowed_provider_classes.size());
    for (auto c : spec.allowed_provider_classes) {
        class_items.push_back(
            encode_text(VMPilot::Runtime::Provider::to_text(c)));
    }

    MapBuilder m;
    m.put_uint(1,  encode_text(spec.requirement_version));
    m.put_uint(2,  encode_uint(
        static_cast<std::uint64_t>(spec.required_policy_floor)));
    m.put_uint(3,  encode_array(family_items));
    m.put_uint(4,  encode_uint(spec.require_hardware_bound      ? 1u : 0u));
    m.put_uint(5,  encode_uint(spec.require_non_exportable_key  ? 1u : 0u));
    m.put_uint(6,  encode_uint(spec.require_online_freshness    ? 1u : 0u));
    m.put_uint(7,  encode_uint(spec.require_remote_attestation  ? 1u : 0u));
    m.put_uint(8,  encode_uint(
        static_cast<std::uint64_t>(spec.require_recovery_model)));
    m.put_uint(9,  encode_array(class_items));
    m.put_uint(10, encode_uint(spec.minimum_provider_epoch));
    return m.build();
}

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

// ─── UnitDescriptor / UnitBindingRecord / ResolvedFamilyProfile ─────────

UnitDescriptorBuilder::UnitDescriptorBuilder()
    : descriptor_version_{"descriptor-v1"},
      unit_id_{"u-happy"},
      unit_identity_hash_{filled(0x50)},
      resolved_family_profile_id_{"rfp-happy"},
      unit_binding_record_id_{"ubr-happy"} {}

UnitDescriptorBuilder& UnitDescriptorBuilder::descriptor_version(std::string v) {
    descriptor_version_ = std::move(v); return *this;
}
UnitDescriptorBuilder& UnitDescriptorBuilder::unit_id(std::string v) {
    unit_id_ = std::move(v); return *this;
}
UnitDescriptorBuilder& UnitDescriptorBuilder::unit_identity_hash(std::array<std::uint8_t, 32> v) {
    unit_identity_hash_ = v; return *this;
}
UnitDescriptorBuilder& UnitDescriptorBuilder::family_id(VMPilot::DomainLabels::FamilyId v) {
    family_id_ = v; return *this;
}
UnitDescriptorBuilder& UnitDescriptorBuilder::requested_policy_id(VMPilot::DomainLabels::PolicyId v) {
    requested_policy_id_ = v; return *this;
}
UnitDescriptorBuilder& UnitDescriptorBuilder::resolved_family_profile_id(std::string v) {
    resolved_family_profile_id_ = std::move(v); return *this;
}
UnitDescriptorBuilder& UnitDescriptorBuilder::payload_sha256(std::array<std::uint8_t, 32> v) {
    payload_sha256_ = v; return *this;
}
UnitDescriptorBuilder& UnitDescriptorBuilder::payload_size(std::uint64_t v) {
    payload_size_ = v; return *this;
}
UnitDescriptorBuilder& UnitDescriptorBuilder::unit_binding_record_id(std::string v) {
    unit_binding_record_id_ = std::move(v); return *this;
}

std::vector<std::uint8_t> UnitDescriptorBuilder::build() const {
    using namespace VMPilot::Fixtures::Cbor;

    MapBuilder pi;
    pi.put_uint(1, encode_bytes(bytes_from(payload_sha256_)));
    pi.put_uint(2, encode_uint(payload_size_));
    const auto pi_bytes = pi.build();

    MapBuilder m;
    m.put_uint(1, encode_text(descriptor_version_));
    m.put_uint(2, encode_text(unit_id_));
    m.put_uint(3, encode_bytes(bytes_from(unit_identity_hash_)));
    m.put_uint(4, encode_text(VMPilot::DomainLabels::to_text(family_id_)));
    m.put_uint(5, encode_text(VMPilot::DomainLabels::to_text(requested_policy_id_)));
    m.put_uint(6, encode_text(resolved_family_profile_id_));
    m.put_uint(7, pi_bytes);
    m.put_uint(8, encode_text(unit_binding_record_id_));
    return m.build();
}

ResolvedFamilyProfileBuilder::ResolvedFamilyProfileBuilder()
    : profile_id_{"rfp-happy"},
      profile_revision_{"rev1"},
      runtime_specialization_id_{"f1-standard-v1"},
      semantic_contract_version_{"semantic-contract-v1"} {}

ResolvedFamilyProfileBuilder&
ResolvedFamilyProfileBuilder::profile_id(std::string v) {
    profile_id_ = std::move(v); return *this;
}
ResolvedFamilyProfileBuilder&
ResolvedFamilyProfileBuilder::family_id(VMPilot::DomainLabels::FamilyId v) {
    family_id_ = v; return *this;
}
ResolvedFamilyProfileBuilder&
ResolvedFamilyProfileBuilder::requested_policy_id(VMPilot::DomainLabels::PolicyId v) {
    requested_policy_id_ = v; return *this;
}
ResolvedFamilyProfileBuilder&
ResolvedFamilyProfileBuilder::profile_revision(std::string v) {
    profile_revision_ = std::move(v); return *this;
}
ResolvedFamilyProfileBuilder&
ResolvedFamilyProfileBuilder::runtime_specialization_id(std::string v) {
    runtime_specialization_id_ = std::move(v); return *this;
}
ResolvedFamilyProfileBuilder&
ResolvedFamilyProfileBuilder::semantic_contract_version(std::string v) {
    semantic_contract_version_ = std::move(v); return *this;
}
ResolvedFamilyProfileBuilder&
ResolvedFamilyProfileBuilder::exception_unwind_contract(ExceptionUnwindContractSpec v) {
    exception_unwind_contract_ = std::move(v); return *this;
}

std::vector<std::uint8_t> ResolvedFamilyProfileBuilder::build() const {
    // Minimal Layer-1 skeleton for Stages 8-9: dispatch header fields plus
    // correctness_legality_contract.exception_unwind_contract.
    using namespace VMPilot::Fixtures::Cbor;

    const auto family_text = VMPilot::DomainLabels::to_text(family_id_);
    MapBuilder correctness_legality_contract;
    correctness_legality_contract.put_uint(
        1, encode_text(semantic_contract_version_));
    correctness_legality_contract.put_uint(
        2, encode_exception_unwind_contract(
               exception_unwind_contract_, family_text));

    MapBuilder m;
    m.put_uint(1, encode_text(profile_id_));
    m.put_uint(2, encode_text(family_text));
    m.put_uint(3, encode_text(VMPilot::DomainLabels::to_text(requested_policy_id_)));
    m.put_uint(4, encode_text(profile_revision_));
    m.put_uint(5, encode_text(runtime_specialization_id_));
    m.put_uint(6, correctness_legality_contract.build());
    return m.build();
}

UnitBindingRecordBuilder::UnitBindingRecordBuilder()
    : unit_binding_record_id_{"ubr-happy"},
      unit_identity_hash_{filled(0x50)},
      unit_descriptor_hash_{filled(0x60)},
      resolved_family_profile_id_{"rfp-happy"},
      resolved_family_profile_content_hash_{filled(0x70)},
      payload_sha256_{filled(0x80)},
      payload_size_{0},
      anti_downgrade_epoch_{1} {}

UnitBindingRecordBuilder&
UnitBindingRecordBuilder::unit_binding_record_id(std::string v) {
    unit_binding_record_id_ = std::move(v); return *this;
}
UnitBindingRecordBuilder&
UnitBindingRecordBuilder::unit_identity_hash(std::array<std::uint8_t, 32> v) {
    unit_identity_hash_ = v; return *this;
}
UnitBindingRecordBuilder&
UnitBindingRecordBuilder::unit_descriptor_hash(std::array<std::uint8_t, 32> v) {
    unit_descriptor_hash_ = v; return *this;
}
UnitBindingRecordBuilder&
UnitBindingRecordBuilder::family_id(VMPilot::DomainLabels::FamilyId v) {
    family_id_ = v; return *this;
}
UnitBindingRecordBuilder&
UnitBindingRecordBuilder::requested_policy_id(VMPilot::DomainLabels::PolicyId v) {
    requested_policy_id_ = v; return *this;
}
UnitBindingRecordBuilder&
UnitBindingRecordBuilder::resolved_family_profile_id(std::string v) {
    resolved_family_profile_id_ = std::move(v); return *this;
}
UnitBindingRecordBuilder&
UnitBindingRecordBuilder::resolved_family_profile_content_hash(std::array<std::uint8_t, 32> v) {
    resolved_family_profile_content_hash_ = v; return *this;
}
UnitBindingRecordBuilder&
UnitBindingRecordBuilder::payload_sha256(std::array<std::uint8_t, 32> v) {
    payload_sha256_ = v; return *this;
}
UnitBindingRecordBuilder&
UnitBindingRecordBuilder::payload_size(std::uint64_t v) {
    payload_size_ = v; return *this;
}
UnitBindingRecordBuilder&
UnitBindingRecordBuilder::anti_downgrade_epoch(std::uint64_t v) {
    anti_downgrade_epoch_ = v; return *this;
}
UnitBindingRecordBuilder&
UnitBindingRecordBuilder::binding_auth(UnitBindingAuthSpec v) {
    binding_auth_ = std::move(v); return *this;
}

std::vector<std::uint8_t> UnitBindingRecordBuilder::build() const {
    using namespace VMPilot::Fixtures::Cbor;

    MapBuilder pi;
    pi.put_uint(1, encode_bytes(bytes_from(payload_sha256_)));
    pi.put_uint(2, encode_uint(payload_size_));
    const auto pi_bytes = pi.build();

    // Fields 1..9 — this is the "canonical UBR content" that
    // record_hash commits to. binding_auth is NOT part of it; the
    // wrapper array below attaches auth as a sibling element.
    MapBuilder m;
    m.put_uint(1, encode_text(unit_binding_record_id_));
    m.put_uint(2, encode_bytes(bytes_from(unit_identity_hash_)));
    m.put_uint(3, encode_bytes(bytes_from(unit_descriptor_hash_)));
    m.put_uint(4, encode_text(VMPilot::DomainLabels::to_text(family_id_)));
    m.put_uint(5, encode_text(VMPilot::DomainLabels::to_text(requested_policy_id_)));
    m.put_uint(6, encode_text(resolved_family_profile_id_));
    m.put_uint(7, encode_bytes(bytes_from(resolved_family_profile_content_hash_)));
    m.put_uint(8, pi_bytes);
    m.put_uint(9, encode_uint(anti_downgrade_epoch_));
    const auto canonical_bytes = m.build();

    // Auto-populate record_hash unless the caller explicitly set a
    // non-zero one — negative tests that want to ship a mismatched
    // hash (to exercise the verifier) can still do so.
    UnitBindingAuthSpec auth_spec = binding_auth_;
    const std::array<std::uint8_t, 32> zero{};
    if (auth_spec.record_hash == zero) {
        auth_spec.record_hash = VMPilot::Cbor::domain_hash_sha256(
            VMPilot::DomainLabels::Hash::UnitBindingRecord, canonical_bytes);
    }

    MapBuilder auth;
    auth.put_uint(1, encode_text(auth_spec.kind));
    auth.put_uint(2, encode_bytes(bytes_from(auth_spec.unit_binding_table_hash)));
    auth.put_uint(3, encode_uint(auth_spec.inclusion_index));
    auth.put_uint(4, encode_bytes(bytes_from(auth_spec.record_hash)));
    const auto auth_bytes = auth.build();

    // Wrapper: [canonical_bytes, auth_map].
    return encode_array({encode_bytes(canonical_bytes), auth_bytes});
}

std::vector<std::uint8_t>
build_unit_binding_table_bytes(const std::vector<std::vector<std::uint8_t>>& ubr_bytes) {
    return VMPilot::Fixtures::Cbor::encode_array(ubr_bytes);
}

std::vector<std::uint8_t>
build_unit_descriptor_table_bytes(
    const std::vector<std::pair<std::string, std::vector<std::uint8_t>>>& entries) {
    using namespace VMPilot::Fixtures::Cbor;
    MapBuilder m;
    for (const auto& [unit_id, bytes] : entries) {
        m.put_text(unit_id, encode_bytes(bytes));
    }
    return m.build();
}

std::vector<std::uint8_t>
build_resolved_profile_table_bytes(
    const std::vector<std::pair<std::string, std::vector<std::uint8_t>>>& entries) {
    using namespace VMPilot::Fixtures::Cbor;
    MapBuilder m;
    for (const auto& [profile_id, bytes] : entries) {
        m.put_text(profile_id, encode_bytes(bytes));
    }
    return m.build();
}

// ─── RuntimeSpecializationRegistryBuilder ───────────────────────────────

RuntimeSpecializationRegistryBuilder::RuntimeSpecializationRegistryBuilder()
    : registry_version_{"registry-v1"},
      runtime_build_id_{"vmpilot-dev-runtime-build"},
      package_schema_version_{"package-schema-v1"},
      registry_epoch_{1},
      signing_seed_{TestKey::kPrivateSeed},
      auth_key_id_{"vmpilot-dev-rfc8032-test1"} {
    // Default entry covers F1 × standard with a happy-path happy-profile.
    // Tests that exercise non-default family/policy combinations call
    // clear_entries() first.
    RegistryEntrySpec e;
    e.runtime_specialization_id = "f1-standard-v1";
    e.family_id = VMPilot::DomainLabels::FamilyId::F1;
    e.requested_policy_id = VMPilot::DomainLabels::PolicyId::Standard;
    e.profile_revision = "rev1";
    entries_.push_back(std::move(e));
}

RuntimeSpecializationRegistryBuilder&
RuntimeSpecializationRegistryBuilder::registry_version(std::string v) {
    registry_version_ = std::move(v); return *this;
}
RuntimeSpecializationRegistryBuilder&
RuntimeSpecializationRegistryBuilder::runtime_build_id(std::string v) {
    runtime_build_id_ = std::move(v); return *this;
}
RuntimeSpecializationRegistryBuilder&
RuntimeSpecializationRegistryBuilder::package_schema_version(std::string v) {
    package_schema_version_ = std::move(v); return *this;
}
RuntimeSpecializationRegistryBuilder&
RuntimeSpecializationRegistryBuilder::registry_epoch(std::uint64_t v) {
    registry_epoch_ = v; return *this;
}
RuntimeSpecializationRegistryBuilder&
RuntimeSpecializationRegistryBuilder::add_entry(RegistryEntrySpec e) {
    // If the spec carries a PolicyRequirement, materialize canonical
    // bytes + hash here so tests don't have to do it manually.
    if (e.policy_requirement.has_value()) {
        e.provider_requirement_canonical_bytes =
            encode_policy_requirement(*e.policy_requirement);
        e.provider_requirement_hash = VMPilot::Cbor::domain_hash_sha256(
            VMPilot::DomainLabels::Hash::PolicyRequirement,
            e.provider_requirement_canonical_bytes);
    }
    entries_.push_back(std::move(e)); return *this;
}
RuntimeSpecializationRegistryBuilder&
RuntimeSpecializationRegistryBuilder::clear_entries() {
    entries_.clear(); return *this;
}
RuntimeSpecializationRegistryBuilder&
RuntimeSpecializationRegistryBuilder::signing_seed(std::array<std::uint8_t, 32> v) {
    signing_seed_ = v; return *this;
}
RuntimeSpecializationRegistryBuilder&
RuntimeSpecializationRegistryBuilder::auth_key_id(std::string v) {
    auth_key_id_ = std::move(v); return *this;
}

std::vector<std::uint8_t>
RuntimeSpecializationRegistryBuilder::build_canonical_bytes() const {
    // Entry field IDs must match runtime/src/registry/registry.cpp.
    using namespace VMPilot::Fixtures::Cbor;

    std::vector<std::vector<std::uint8_t>> entry_bytes;
    entry_bytes.reserve(entries_.size());
    for (const auto& e : entries_) {
        MapBuilder m;
        m.put_uint(1,  encode_text(e.runtime_specialization_id));
        m.put_uint(2,  encode_text(VMPilot::DomainLabels::to_text(e.family_id)));
        m.put_uint(3,  encode_text(
            VMPilot::DomainLabels::to_text(e.requested_policy_id)));
        m.put_uint(4,  encode_text(e.profile_revision));
        m.put_uint(5,  encode_text(e.semantic_contract_version));
        m.put_uint(6,  encode_text(e.execution_contract_ref));
        m.put_uint(7,  encode_bytes(std::vector<std::uint8_t>(
            e.required_runtime_primitives_hash.begin(),
            e.required_runtime_primitives_hash.end())));
        m.put_uint(8,  encode_bytes(std::vector<std::uint8_t>(
            e.required_runtime_helpers_hash.begin(),
            e.required_runtime_helpers_hash.end())));
        m.put_uint(9,  encode_bytes(std::vector<std::uint8_t>(
            e.provider_requirement_hash.begin(),
            e.provider_requirement_hash.end())));
        m.put_uint(10, encode_bytes(std::vector<std::uint8_t>(
            e.accepted_profile_content_hash.begin(),
            e.accepted_profile_content_hash.end())));
        m.put_uint(11, encode_uint(e.diagnostic_visibility_class));
        m.put_uint(12, encode_uint(e.enabled_in_this_runtime ? 1 : 0));
        m.put_uint(13, encode_bytes(e.provider_requirement_canonical_bytes));
        entry_bytes.push_back(m.build());
    }

    MapBuilder header;
    header.put_uint(1, encode_text(registry_version_));
    header.put_uint(2, encode_text(runtime_build_id_));
    header.put_uint(3, encode_text(package_schema_version_));
    header.put_uint(4, encode_uint(registry_epoch_));
    header.put_uint(5, encode_array(entry_bytes));
    return header.build();
}

std::vector<std::uint8_t>
RuntimeSpecializationRegistryBuilder::build() const {
    const auto canonical = build_canonical_bytes();
    const auto sig = Sign::sign_ed25519(
        signing_seed_,
        VMPilot::DomainLabels::Auth::RuntimeSpecRegistry,
        canonical);

    VMPilot::Fixtures::Cbor::MapBuilder auth;
    auth.put_uint(1, VMPilot::Fixtures::Cbor::encode_text("vendor_signature_v1"));
    auth.put_uint(2, VMPilot::Fixtures::Cbor::encode_text(auth_key_id_));
    auth.put_uint(3, VMPilot::Fixtures::Cbor::encode_text("ed25519-pure-v1"));
    auth.put_uint(4, VMPilot::Fixtures::Cbor::encode_text(
        std::string(VMPilot::DomainLabels::Auth::RuntimeSpecRegistry)));
    std::vector<std::uint8_t> sig_vec(sig.begin(), sig.end());
    auth.put_uint(5, VMPilot::Fixtures::Cbor::encode_bytes(sig_vec));
    const auto auth_bytes = auth.build();

    return VMPilot::Fixtures::Cbor::encode_array(
        {VMPilot::Fixtures::Cbor::encode_bytes(canonical), auth_bytes});
}

std::array<std::uint8_t, 32>
sha256_of(const std::vector<std::uint8_t>& bytes) {
    const auto digest = VMPilot::Crypto::SHA256(bytes, {});
    std::array<std::uint8_t, 32> out{};
    if (digest.size() == 32) std::memcpy(out.data(), digest.data(), 32);
    return out;
}

std::vector<std::uint8_t>
build_inner_partition_bytes(const std::vector<std::uint8_t>& unit_binding_table_bytes,
                            const std::vector<std::uint8_t>& resolved_profile_table_bytes,
                            const std::vector<std::uint8_t>& registry_bytes,
                            const std::vector<std::uint8_t>& unit_descriptor_table_bytes) {
    using namespace VMPilot::Fixtures::Cbor;
    MapBuilder m;
    m.put_uint(1, encode_bytes(unit_binding_table_bytes));
    m.put_uint(2, encode_bytes(resolved_profile_table_bytes));
    m.put_uint(3, encode_bytes(registry_bytes));
    m.put_uint(4, encode_bytes(unit_descriptor_table_bytes));
    return m.build();
}

// ─── PackageArtifactBuilder ─────────────────────────────────────────────

PackageArtifactBuilder::PackageArtifactBuilder()
    : id_{"pbr-fixture-happy-path"},
      schema_version_{"package-schema-v1"},
      encoding_id_{"canonical-metadata-bytes-v1"},
      anti_downgrade_epoch_{1},
      minimum_runtime_epoch_{1},
      // registry_bytes_ intentionally left empty. build() either uses the
      // caller's override via registry_bytes() setter, or auto-generates a
      // registry whose single entry carries the real profile hash
      // (accepted_profile_content_hash) so Stage 8 dispatch can verify
      // without a second fix-up pass.
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
    inner_bytes_override_ = std::move(v); return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::unit_binding_table_bytes(std::vector<std::uint8_t> v) {
    unit_binding_table_bytes_ = std::move(v); return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::resolved_profile_table_bytes(std::vector<std::uint8_t> v) {
    resolved_profile_table_bytes_ = std::move(v); return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::unit_descriptor_table_bytes(std::vector<std::uint8_t> v) {
    unit_descriptor_table_bytes_ = std::move(v); return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::registry_bytes(std::vector<std::uint8_t> v) {
    registry_bytes_ = std::move(v); return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::default_unit_id(std::string v) {
    default_unit_id_ = std::move(v); return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::default_unit_binding_record_id(std::string v) {
    default_record_id_ = std::move(v); return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::default_resolved_family_profile_id(std::string v) {
    default_profile_id_ = std::move(v); return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::default_family_id(std::string v) {
    default_family_id_ = std::move(v); return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::default_policy_id(std::string v) {
    default_policy_id_ = std::move(v); return *this;
}
PackageArtifactBuilder&
PackageArtifactBuilder::default_unit_anti_downgrade_epoch(std::uint64_t v) {
    default_unit_anti_downgrade_epoch_ = v; return *this;
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
    // ── 1. Compute the default unit's payload identity from payload_bytes_.
    //       Used downstream by the default descriptor + UBR builders.
    const auto payload_sha_vec = VMPilot::Crypto::SHA256(payload_bytes_, {});
    std::array<std::uint8_t, 32> payload_sha{};
    if (payload_sha_vec.size() == 32) {
        std::memcpy(payload_sha.data(), payload_sha_vec.data(), 32);
    }
    const std::uint64_t payload_size = payload_bytes_.size();

    // Stable fictitious unit identity hash — domain-separated from any real
    // hash label used elsewhere. Tests that want cross-unit mismatches
    // pass override sub-tables.
    const std::array<std::uint8_t, 32> unit_identity{
        0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
        0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
        0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    };

    // ── 2. Auto-assemble each inner sub-table when no override is given.
    //       Defaults produce a single-unit package whose descriptor, UBR,
    //       and profile all reference each other consistently.

    std::vector<std::uint8_t> descriptor_bytes;
    std::vector<std::uint8_t> profile_bytes;
    std::vector<std::uint8_t> unit_binding_table_bytes = unit_binding_table_bytes_;
    std::vector<std::uint8_t> resolved_profile_table_bytes = resolved_profile_table_bytes_;
    std::vector<std::uint8_t> unit_descriptor_table_bytes = unit_descriptor_table_bytes_;

    if (unit_binding_table_bytes.empty() ||
        resolved_profile_table_bytes.empty() ||
        unit_descriptor_table_bytes.empty()) {

        const auto fam_enum =
            VMPilot::DomainLabels::parse_family_id(default_family_id_)
                .value_or(VMPilot::DomainLabels::FamilyId::F1);
        const auto pol_enum =
            VMPilot::DomainLabels::parse_policy_id(default_policy_id_)
                .value_or(VMPilot::DomainLabels::PolicyId::Standard);

        descriptor_bytes = UnitDescriptorBuilder{}
            .descriptor_version("descriptor-v1")
            .unit_id(default_unit_id_)
            .unit_identity_hash(unit_identity)
            .family_id(fam_enum)
            .requested_policy_id(pol_enum)
            .resolved_family_profile_id(default_profile_id_)
            .payload_sha256(payload_sha)
            .payload_size(payload_size)
            .unit_binding_record_id(default_record_id_)
            .build();

        profile_bytes = ResolvedFamilyProfileBuilder{}
            .profile_id(default_profile_id_)
            .family_id(fam_enum)
            .requested_policy_id(pol_enum)
            .build();

        const auto descriptor_hash = VMPilot::Cbor::domain_hash_sha256(
            VMPilot::DomainLabels::Hash::UnitDescriptor, descriptor_bytes);
        const auto profile_hash = VMPilot::Cbor::domain_hash_sha256(
            VMPilot::DomainLabels::Hash::ResolvedFamilyProfile, profile_bytes);

        UnitBindingAuthSpec auth;
        // unit_binding_table_hash inside binding_auth is informational —
        // acceptance does not cryptographically verify it against the UBT
        // (cyclic definition; doc 06 §9.3 intent is audit-debug only).
        // A stable sentinel reads consistently in hex dumps.
        for (std::size_t i = 0; i < 32; ++i) auth.unit_binding_table_hash[i] = 0xAA;
        auth.inclusion_index = 0;
        // record_hash left zero — UnitBindingRecordBuilder auto-computes
        // domain_hash("unit-binding-record-v1", canonical_bytes_minus_auth)
        // which the parser verifies.

        const auto ubr_bytes = UnitBindingRecordBuilder{}
            .unit_binding_record_id(default_record_id_)
            .unit_identity_hash(unit_identity)
            .unit_descriptor_hash(descriptor_hash)
            .family_id(fam_enum)
            .requested_policy_id(pol_enum)
            .resolved_family_profile_id(default_profile_id_)
            .resolved_family_profile_content_hash(profile_hash)
            .payload_sha256(payload_sha)
            .payload_size(payload_size)
            .anti_downgrade_epoch(default_unit_anti_downgrade_epoch_)
            .binding_auth(auth)
            .build();

        if (unit_binding_table_bytes.empty()) {
            unit_binding_table_bytes = build_unit_binding_table_bytes({ubr_bytes});
        }
        if (unit_descriptor_table_bytes.empty()) {
            unit_descriptor_table_bytes =
                build_unit_descriptor_table_bytes({{default_unit_id_, descriptor_bytes}});
        }
        if (resolved_profile_table_bytes.empty()) {
            resolved_profile_table_bytes =
                build_resolved_profile_table_bytes({{default_profile_id_, profile_bytes}});
        }
    }

    // ── 2b. Registry: when no override is supplied, emit a registry whose
    //        single default entry carries the real profile content hash.
    //        This lets Stage 8 dispatch verify
    //        `entry.accepted_profile_content_hash ==
    //        ubr.resolved_family_profile_content_hash` without a second
    //        fix-up pass on the caller's side.
    std::vector<std::uint8_t> registry_bytes = registry_bytes_;
    if (registry_bytes.empty()) {
        std::array<std::uint8_t, 32> computed_profile_hash{};
        if (!profile_bytes.empty()) {
            computed_profile_hash = VMPilot::Cbor::domain_hash_sha256(
                VMPilot::DomainLabels::Hash::ResolvedFamilyProfile, profile_bytes);
        }
        RuntimeSpecializationRegistryBuilder reg;
        reg.clear_entries();
        RegistryEntrySpec e;
        e.runtime_specialization_id = "f1-standard-v1";
        e.family_id                 =
            VMPilot::DomainLabels::parse_family_id(default_family_id_)
                .value_or(VMPilot::DomainLabels::FamilyId::F1);
        e.requested_policy_id       =
            VMPilot::DomainLabels::parse_policy_id(default_policy_id_)
                .value_or(VMPilot::DomainLabels::PolicyId::Standard);
        e.profile_revision          = "rev1";
        e.accepted_profile_content_hash = computed_profile_hash;
        reg.add_entry(e);
        registry_bytes = reg.build();
    }

    // ── 3. Inner partition CBOR map, honouring any explicit override.
    const std::vector<std::uint8_t> inner_bytes =
        inner_bytes_override_.empty()
            ? build_inner_partition_bytes(unit_binding_table_bytes,
                                          resolved_profile_table_bytes,
                                          registry_bytes,
                                          unit_descriptor_table_bytes)
            : inner_bytes_override_;

    // ── 4. Per-sub-table hashes. Domain-separated so the three digests
    //       differ even when two happen to cover identical bytes.
    std::array<std::uint8_t, 32> ubt_hash{};
    std::array<std::uint8_t, 32> rpt_hash{};
    std::array<std::uint8_t, 32> reg_hash{};
    if (inner_bytes_override_.empty()) {
        ubt_hash = VMPilot::Cbor::domain_hash_sha256(
            VMPilot::DomainLabels::Hash::UnitBindingTable, unit_binding_table_bytes);
        rpt_hash = VMPilot::Cbor::domain_hash_sha256(
            VMPilot::DomainLabels::Hash::ResolvedProfileTable, resolved_profile_table_bytes);
        reg_hash = VMPilot::Cbor::domain_hash_sha256(
            VMPilot::DomainLabels::Hash::RuntimeSpecRegistry, registry_bytes);
    } else {
        // Override path — tests that go here have typically set specific
        // hash mismatches as part of their expected failure. Fall back to
        // hashing the whole partition blob; Stage 5 acceptance tries to
        // parse it as a CBOR map and will reject on PbrPartitionMalformed
        // if the shape is wrong.
        ubt_hash = VMPilot::Cbor::domain_hash_sha256(
            VMPilot::DomainLabels::Hash::UnitBindingTable, inner_bytes);
        rpt_hash = VMPilot::Cbor::domain_hash_sha256(
            VMPilot::DomainLabels::Hash::ResolvedProfileTable, inner_bytes);
        reg_hash = VMPilot::Cbor::domain_hash_sha256(
            VMPilot::DomainLabels::Hash::RuntimeSpecRegistry, inner_bytes);
    }

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
        .inner_partition_bytes(inner_bytes)
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
        .inner_partition_bytes(inner_bytes)
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
