#ifndef VMPILOT_COMMON_VM_DOMAIN_LABELS_HPP
#define VMPILOT_COMMON_VM_DOMAIN_LABELS_HPP

#include <string_view>

// Domain separation labels for the redesigned artifact binding layer.
//
// Two distinct namespaces. Auth labels (signature `covered_domain`) and hash
// labels (prepended to canonical bytes before `H(...)`) must never alias each
// other; a verifier that reuses a hash label as a `covered_domain`, or vice
// versa, is a correctness bug.
//
// Contract sources:
//   docs/research/zh-tw/family-redesign/06-binding-record-and-payload-identity.md §9.4
//   docs/research/zh-tw/family-redesign/07-canonical-metadata-bytes.md §4

namespace VMPilot::DomainLabels {

// ─── Auth labels (signature covered_domain) ──────────────────────────────
//
// Used as the `covered_domain` field inside `binding_auth` objects and as
// the length-prefixed message prefix for `Verify_Ed25519`.

namespace Auth {
    constexpr std::string_view PackageBinding       = "package-binding-v1";
    constexpr std::string_view UnitBinding          = "unit-binding-v1";
    constexpr std::string_view RuntimeSpecRegistry  = "runtime-specialization-registry-v1";
    constexpr std::string_view ProviderEntitlement  = "provider-entitlement-v1";
    constexpr std::string_view ReprovisionToken     = "reprovision-token-v1";
    constexpr std::string_view MigrationToken       = "migration-token-v1";
}  // namespace Auth

// ─── Hash labels (domain_hash prefix) ────────────────────────────────────
//
// Prepended to canonical bytes before SHA-256 (or future upgraded hash) so
// that the same bytes under different domains never collide.

namespace Hash {
    constexpr std::string_view UnitIdentity            = "unit-identity-v1";
    constexpr std::string_view UnitDescriptor          = "unit-descriptor-v1";
    constexpr std::string_view ResolvedFamilyProfile   = "resolved-family-profile-v1";
    constexpr std::string_view UnitBindingRecord       = "unit-binding-record-v1";
    constexpr std::string_view PackageBindingRecord    = "package-binding-record-v1";
    constexpr std::string_view RuntimeSpecRegistry     = "runtime-specialization-registry-v1";
    constexpr std::string_view ArtifactLayout          = "artifact-layout-v1";
}  // namespace Hash

}  // namespace VMPilot::DomainLabels

#endif  // VMPILOT_COMMON_VM_DOMAIN_LABELS_HPP
