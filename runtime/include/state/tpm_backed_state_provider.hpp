#ifndef VMPILOT_RUNTIME_STATE_TPM_BACKED_STATE_PROVIDER_HPP
#define VMPILOT_RUNTIME_STATE_TPM_BACKED_STATE_PROVIDER_HPP

#include <string>

#include "state/persistent_state_provider.hpp"

// Highsec-local PersistentStateProvider backed by a TPM 2.0 monotonic
// counter plus sealed state records — doc 17 §6 + stage B plan.
//
// PR-B1 (this file) ships only the class shape. Every operation
// returns `StoreError::ProviderUnavailable` and `capabilities()`
// reports `operational_state == Unavailable`, which means
// `satisfies_highsec(...)` rejects the provider on every platform.
// That honest-unavailable posture is what the Stage B plan §4 calls
// "compiled but unavailable" — no host is surprised at runtime, and
// no skeleton build passes the highsec gate.
//
// Later PRs fill this in incrementally:
//
//     B2  tpm2-tss ESAPI + TCTI integration
//     B3  TCTI selection + swtpm harness
//     B4  NV counter provisioning / read / increment
//     B5  Sealing parent + sealed state record
//     B6  Signed PCR policy / PolicyAuthorize
//     B7  Prepare-before-increment commit protocol
//     B8  Crash injection + RecoveryRequired branches
//     B9  ReprovisionToken integration
//
// Consumers may include this header today; the class will not become
// highsec-accepting until B7 lands with a real backend.

namespace VMPilot::Runtime::State {

// Placeholder config type. Fields land with their respective PRs:
//
//     tcti          — B2/B3 (device / tbs / swtpm selector)
//     state_dir     — B2 (per-domain slot + metadata layout)
//     domain_id     — B4 (state domain identity)
//     enrollment_id — B4 (enrollment identity within a domain)
//
// Kept as a struct with no required fields so PR-B1 can compile a
// no-op constructor without committing to a layout that later PRs
// will refine.
struct TpmProviderConfig {};

// Placeholder provisioning request. Fields land with B4/B5/B6 as the
// NV counter / sealing parent / PCR policy provisioning routines are
// implemented.
struct TpmProvisioningRequest {};

class TpmBackedStateProvider final : public PersistentStateProvider {
public:
    // PR-B1 ctor: accept a config by value and record nothing. Later
    // PRs will attempt TCTI open + domain load here; until then every
    // op fails closed so no caller mistakes construction success for
    // availability.
    explicit TpmBackedStateProvider(TpmProviderConfig cfg) noexcept;

    // PR-B1 stubs — each returns StoreError::ProviderUnavailable.
    [[nodiscard]] tl::expected<void, StoreError>
    reserve_nonce(const std::array<std::uint8_t, 32>& nonce) noexcept override;

    [[nodiscard]] tl::expected<EpochState, StoreError>
    current_epoch_state() const noexcept override;

    [[nodiscard]] tl::expected<void, StoreError>
    advance_epoch_state(const EpochState& proposed) noexcept override;

    PersistenceCapability capabilities() const noexcept override;

    // Signed recovery entry point (doc 10 §6 + stage B plan §12).
    // Stubbed in PR-B1; real TPM domain rotation lands in PR-B9.
    struct ReprovisionTokenPlaceholder {};
    [[nodiscard]] tl::expected<void, StoreError>
    reprovision(const ReprovisionTokenPlaceholder& token) noexcept;
};

}  // namespace VMPilot::Runtime::State

#endif  // VMPILOT_RUNTIME_STATE_TPM_BACKED_STATE_PROVIDER_HPP
