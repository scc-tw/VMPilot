#include "state/tpm_backed_state_provider.hpp"

// PR-B1 skeleton — every operation reports
// `StoreError::ProviderUnavailable` and `capabilities()` reports
// `operational_state == Unavailable`. On every platform, on every
// host, `satisfies_highsec(provider.capabilities())` is false.
//
// The only claim this provider makes while unavailable is
// `signed_recovery_required = true`: that's a static design property
// of the TPM path (doc 10 §6 + stage B plan §12), not a runtime
// availability claim. A host that later provisions a real TPM domain
// will flip every other bool to true and promote `operational_state`
// to `Available`; that happens in PR-B7 once the prepare-before-
// increment commit protocol is implemented end-to-end.

namespace VMPilot::Runtime::State {

TpmBackedStateProvider::TpmBackedStateProvider(TpmProviderConfig) noexcept {
    // PR-B2+ will open the TCTI and attempt to load the state domain
    // here. Until then constructing one is free of side effects so
    // callers can hold an instance without triggering TPM I/O on
    // platforms that don't have a TPM.
}

tl::expected<void, StoreError>
TpmBackedStateProvider::reserve_nonce(
    const std::array<std::uint8_t, 32>& /*nonce*/) noexcept {
    return tl::make_unexpected(StoreError::ProviderUnavailable);
}

tl::expected<EpochState, StoreError>
TpmBackedStateProvider::current_epoch_state() const noexcept {
    return tl::make_unexpected(StoreError::ProviderUnavailable);
}

tl::expected<void, StoreError>
TpmBackedStateProvider::advance_epoch_state(
    const EpochState& /*proposed*/) noexcept {
    return tl::make_unexpected(StoreError::ProviderUnavailable);
}

PersistenceCapability
TpmBackedStateProvider::capabilities() const noexcept {
    PersistenceCapability c{};
    c.cross_process_atomic     = false;
    c.crash_consistent         = false;
    c.rollback_resistant       = false;
    c.tamper_evident           = false;
    c.hardware_bound           = false;
    c.policy_bound             = false;
    c.signed_recovery_required = true;   // static design property
    c.operational_state        = ProviderOperationalState::Unavailable;
    return c;
}

tl::expected<void, StoreError>
TpmBackedStateProvider::reprovision(
    const ReprovisionTokenPlaceholder& /*token*/) noexcept {
    return tl::make_unexpected(StoreError::ProviderUnavailable);
}

}  // namespace VMPilot::Runtime::State
