#ifndef VMPILOT_RUNTIME_BINDING_SIGNED_PARTITION_HPP
#define VMPILOT_RUNTIME_BINDING_SIGNED_PARTITION_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#include <vector>

#include <tl/expected.hpp>

#include "VMPilot_crypto.hpp"
#include "cbor/strict.hpp"
#include "trust_root.hpp"

// Signed-partition wrapper shared by every on-disk binding record:
//
//   [ canonical_bytes, binding_auth_map ]
//
// where `binding_auth_map` is a 5-field CBOR map over {kind, key_id,
// signature_alg_id, covered_domain, signature}. The five consumers
// that ship this wrapper (PackageBindingRecord, UnitBindingTable /
// UnitBindingRecord, RuntimeSpecializationRegistry partition,
// ReprovisionToken, MigrationToken) used to each reimplement the
// same ~80-line verify routine — collapsed here so each consumer
// only specifies (a) the trait that maps the shared rejection codes
// onto its own error enum and (b) the covered_domain string it
// expects.

namespace VMPilot::Runtime::Binding {

// ─── Auth field IDs (doc 06 §9.2; matching layout in every consumer) ────

constexpr std::uint64_t kSignedAuth_Kind             = 1;
constexpr std::uint64_t kSignedAuth_KeyId            = 2;
constexpr std::uint64_t kSignedAuth_SignatureAlgId   = 3;
constexpr std::uint64_t kSignedAuth_CoveredDomain    = 4;
constexpr std::uint64_t kSignedAuth_Signature        = 5;

constexpr std::string_view kSignedAuthKindVendorSignatureV1 = "vendor_signature_v1";
constexpr std::size_t kEd25519SignatureLen = 64;

// Non-owning view into a signed partition's two child elements. The
// pointers are valid as long as the caller keeps the decoded tree
// (the Value tree that came out of parse_strict) alive.
struct SignedPartitionView {
    const std::vector<std::uint8_t>* canonical_bytes;
    const std::vector<std::uint8_t>* signature_bytes;
};

// Consumer specializes this to map the shared rejection classes to
// its own error enum. Every consumer has a matching enumerator for
// these classes today (PBR/UBR/Registry/Tokens all carry them).
template <typename E>
struct SignedPartitionErrors;

namespace detail {

// Safe lookup — if the consumer hasn't declared a particular code
// (because the consumer never triggers it), the compile-time
// instantiation stays behind the `if (!cond) return err(...)` line
// that uses it.
template <typename E>
[[nodiscard]] constexpr E partition_malformed() noexcept {
    return SignedPartitionErrors<E>::partition_malformed;
}
template <typename E>
[[nodiscard]] constexpr E auth_kind_unsupported() noexcept {
    return SignedPartitionErrors<E>::auth_kind_unsupported;
}
template <typename E>
[[nodiscard]] constexpr E auth_key_id_mismatch() noexcept {
    return SignedPartitionErrors<E>::auth_key_id_mismatch;
}
template <typename E>
[[nodiscard]] constexpr E auth_signature_alg_mismatch() noexcept {
    return SignedPartitionErrors<E>::auth_signature_alg_mismatch;
}
template <typename E>
[[nodiscard]] constexpr E auth_covered_domain_mismatch() noexcept {
    return SignedPartitionErrors<E>::auth_covered_domain_mismatch;
}
template <typename E>
[[nodiscard]] constexpr E signature_wrong_size() noexcept {
    return SignedPartitionErrors<E>::signature_wrong_size;
}
template <typename E>
[[nodiscard]] constexpr E signature_invalid() noexcept {
    return SignedPartitionErrors<E>::signature_invalid;
}
template <typename E>
[[nodiscard]] constexpr E missing_core_field() noexcept {
    return SignedPartitionErrors<E>::missing_core_field;
}
template <typename E>
[[nodiscard]] constexpr E wrong_field_type() noexcept {
    return SignedPartitionErrors<E>::wrong_field_type;
}

}  // namespace detail

// Core routine. Decodes `data/size` as a strict-CBOR
// `[canonical_bytes, auth_map]` partition, verifies every auth_map
// field, and Ed25519-verifies the signature against `root`. On
// success returns a SignedPartitionView whose pointers reference
// into the outer Value tree; the caller keeps the tree alive (the
// template doesn't own it) and dispatches further parsing of the
// canonical bytes.
//
// Use this variant when the caller wants ownership of the decoded
// outer Value tree for other reasons; when the caller just needs
// canonical_bytes to feed into a second parse, see the convenience
// overload below that manages the outer tree internally.
template <typename E>
[[nodiscard]] inline tl::expected<SignedPartitionView, E>
verify_signed_partition_view(const VMPilot::Cbor::Value& partition,
                             const VMPilot::Runtime::VendorTrustRoot& root,
                             std::string_view expected_covered_domain) noexcept {
    using VMPilot::Cbor::Value;
    auto make_unexpected = [](E e) { return tl::make_unexpected(e); };

    if (partition.kind() != Value::Kind::Array) {
        return make_unexpected(detail::partition_malformed<E>());
    }
    if (partition.as_array().size() != 2) {
        return make_unexpected(detail::partition_malformed<E>());
    }

    const Value& canonical_v = partition.as_array()[0];
    const Value& auth_v      = partition.as_array()[1];
    if (canonical_v.kind() != Value::Kind::Bytes ||
        auth_v.kind()      != Value::Kind::Map) {
        return make_unexpected(detail::partition_malformed<E>());
    }

    auto kind_or =
        VMPilot::Cbor::require_text<E>(auth_v, kSignedAuth_Kind);
    if (!kind_or) return make_unexpected(kind_or.error());
    if (*kind_or != kSignedAuthKindVendorSignatureV1) {
        return make_unexpected(detail::auth_kind_unsupported<E>());
    }

    auto key_id_or =
        VMPilot::Cbor::require_text<E>(auth_v, kSignedAuth_KeyId);
    if (!key_id_or) return make_unexpected(key_id_or.error());
    if (*key_id_or != root.root_key_id) {
        return make_unexpected(detail::auth_key_id_mismatch<E>());
    }

    auto alg_or =
        VMPilot::Cbor::require_text<E>(auth_v, kSignedAuth_SignatureAlgId);
    if (!alg_or) return make_unexpected(alg_or.error());
    if (*alg_or != root.signature_alg_id) {
        return make_unexpected(detail::auth_signature_alg_mismatch<E>());
    }

    auto domain_or =
        VMPilot::Cbor::require_text<E>(auth_v, kSignedAuth_CoveredDomain);
    if (!domain_or) return make_unexpected(domain_or.error());
    if (*domain_or != expected_covered_domain) {
        return make_unexpected(detail::auth_covered_domain_mismatch<E>());
    }

    auto sig_or =
        VMPilot::Cbor::require_bytes_ref<E>(auth_v, kSignedAuth_Signature);
    if (!sig_or) return make_unexpected(sig_or.error());
    const auto& signature_bytes = **sig_or;
    if (signature_bytes.size() != kEd25519SignatureLen) {
        return make_unexpected(detail::signature_wrong_size<E>());
    }

    const auto& canonical_bytes = canonical_v.as_bytes();
    const std::vector<std::uint8_t> pubkey_vec(
        root.public_key, root.public_key + sizeof(root.public_key));
    const bool sig_ok = VMPilot::Crypto::Verify_Ed25519(
        pubkey_vec, signature_bytes,
        std::string(expected_covered_domain), canonical_bytes);
    if (!sig_ok) return make_unexpected(detail::signature_invalid<E>());

    SignedPartitionView out;
    out.canonical_bytes = &canonical_bytes;
    out.signature_bytes = &signature_bytes;
    return out;
}

}  // namespace VMPilot::Runtime::Binding

#endif  // VMPILOT_RUNTIME_BINDING_SIGNED_PARTITION_HPP
