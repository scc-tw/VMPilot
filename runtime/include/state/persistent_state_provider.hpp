#ifndef VMPILOT_RUNTIME_STATE_PERSISTENT_STATE_PROVIDER_HPP
#define VMPILOT_RUNTIME_STATE_PERSISTENT_STATE_PROVIDER_HPP

#include <array>
#include <cstdint>

#include <tl/expected.hpp>

// Runtime persistent state trust-root interface — doc 17.
//
// The runtime's consumed-nonce set and epoch lattice are tier-mapped
// local state. Their trust root is NOT artifact authenticity; it is the
// provider below. Doc 17 §3 lays out which PersistenceCapability claims
// each tier requires, and §3.1 gives the acceptance rule that keeps
// highsec off a file-backed provider:
//
//     if policy_requirement.policy_floor == highsec:
//       require capabilities().rollback_resistant
//       require capabilities().tamper_evident
//       require capabilities().hardware_bound
//               OR capabilities().remote_authority
//
// Three provider families (doc 17 §5-§7):
//     - FileBackedStateProvider      (debug / standard)  — Stage A PR-2
//     - TpmBackedStateProvider       (highsec local)     — Stage B
//     - CloudAttestedStateProvider   (highsec cloud)     — Stage C
//
// This PR ships only the interface. Implementations follow.

namespace VMPilot::Runtime::State {

// Capability claims the provider makes at provisioning time. Every
// field is an honest boolean — a provider MUST NOT report a claim it
// cannot back. Doc 17 §3.1 turns these into a fail-closed gate so
// misreporting is detectable by the acceptance rule rather than by
// silent security downgrade.
struct PersistenceCapability {
    // `reserve_nonce` and `advance_epoch_state` are atomic across every
    // cooperating process sharing this provider's backing store.
    bool cross_process_atomic;

    // A crash at any point between the start and end of a mutation
    // leaves a readable prior-or-new state; it does not silently load
    // zero state or partially-written state.
    bool crash_consistent;

    // An offline attacker that can rewind the backing bytes to an
    // earlier captured copy cannot make the provider accept it. Per
    // doc 17 §2, file lock alone, sealed blob alone, and KMS unwrap
    // alone do NOT qualify — only a TPM NV monotonic counter or a
    // remote monotonic authority does.
    bool rollback_resistant;

    // Offline byte-level tampering of the backing store produces a
    // detectable failure (unseal failure, signature mismatch, NV Name
    // mismatch, ...), not silent acceptance of mutated state.
    bool tamper_evident;

    // Freshness is rooted in hardware local to this machine (TPM, TEE,
    // HSM with hardware custody). Required or `remote_authority` for
    // highsec (§3.1).
    bool hardware_bound;

    // Unseal / access is conditioned on a signed PCR / boot policy
    // (PolicyAuthorize-style, not exact-PCR binding). Only meaningful
    // when `hardware_bound == true`.
    bool policy_bound;

    // Freshness is rooted in a remote attestation-gated monotonic
    // authority (Nitro/KMS-style, Azure Attestation, bespoke remote
    // state service). Required or `hardware_bound` for highsec (§3.1).
    bool remote_authority;

    // TPM clear, hardware migration, VM migration, NV Name mismatch,
    // sealed-parent loss, and the other §6.5 triggers must be resolved
    // by `ReprovisionToken` (doc 10 §6). No support-override unsigned
    // recovery path.
    bool signed_recovery_required;
};

// Persistent epoch state — the runtime's advertised current epoch and
// its minimum-accepted floor (doc 06 §10). Both move monotonically
// forward; `advance_epoch_state` rejects any proposal that rewinds
// either field against disk-truth.
struct EpochState {
    std::uint64_t runtime_epoch{0};
    std::uint64_t minimum_accepted_epoch{0};
};

// Failure modes surfaced by the provider. Stage B / C providers may
// grow this enum (e.g. `RecoveryRequired` for TPM Name mismatch), which
// is why `noexcept` + tl::expected is used rather than a fixed set of
// boolean returns.
enum class StoreError : std::uint8_t {
    IoError             = 1,  // backing store unreachable / write failed
    Corrupt             = 2,  // magic / version / crc / schema mismatch
    EpochRollbackDenied = 3,  // proposed epoch strictly less than disk value
    NonceAlreadyPresent = 4,  // reserve_nonce saw the nonce already consumed
};

// Tier-mapped runtime persistent state provider.
//
// Every method is `noexcept`. Mutating methods are reload-under-lock:
// the provider reloads the authoritative state from its backing store
// at the start of each operation and commits atomically before
// returning success. Doc 17 §4 is the definitive specification; callers
// rely on:
//
//   - reserve_nonce          : atomic check-and-mark. If the nonce is
//                              already consumed the provider returns
//                              `NonceAlreadyPresent`; otherwise it is
//                              marked and durably persisted before
//                              success is returned.
//
//   - current_epoch_state    : returns the most recently
//                              durably-committed epoch state.
//
//   - advance_epoch_state    : monotonic update over disk-truth (not
//                              in-memory state). Rolling back either
//                              field fails with `EpochRollbackDenied`.
//
//   - capabilities           : immutable once the provider is
//                              constructed.
class PersistentStateProvider {
public:
    virtual ~PersistentStateProvider() = default;

    [[nodiscard]] virtual tl::expected<void, StoreError>
    reserve_nonce(const std::array<std::uint8_t, 32>& nonce) noexcept = 0;

    [[nodiscard]] virtual tl::expected<EpochState, StoreError>
    current_epoch_state() const noexcept = 0;

    [[nodiscard]] virtual tl::expected<void, StoreError>
    advance_epoch_state(const EpochState& proposed) noexcept = 0;

    virtual PersistenceCapability capabilities() const noexcept = 0;
};

}  // namespace VMPilot::Runtime::State

#endif  // VMPILOT_RUNTIME_STATE_PERSISTENT_STATE_PROVIDER_HPP
