#ifndef VMPILOT_RUNTIME_STATE_FILE_BACKED_STATE_PROVIDER_HPP
#define VMPILOT_RUNTIME_STATE_FILE_BACKED_STATE_PROVIDER_HPP

#include <string>

#include "state/persistent_state_provider.hpp"

// Debug / standard tier PersistentStateProvider — doc 17 §5.
//
// Honest capability claims:
//
//     cross_process_atomic     = true   (advisory lock + atomic rename)
//     crash_consistent         = true   (fsync before rename + parent dir fsync)
//     rollback_resistant       = false  (attacker with write access restores)
//     tamper_evident           = false  (CRC-32 detects corruption, not tamper)
//     hardware_bound           = false
//     policy_bound             = false
//     remote_authority         = false
//     signed_recovery_required = false
//
// Doc 17 §3.1 acceptance rule keeps this provider from satisfying
// highsec — the runtime MUST reject a highsec PolicyRequirement when
// paired with this provider.
//
// Unsupported deployments (doc 17 §5.4):
//
//   - NFS / SMB backing (advisory lock semantics diverge).
//   - Attacker-writable state paths (path integrity is a Stage B+
//     signed provisioning concern).

namespace VMPilot::Runtime::State {

class FileBackedStateProvider final : public PersistentStateProvider {
public:
    // `state_path` is the authoritative state file. The provider
    // derives a sibling lockfile at `state_path + ".lock"`. A missing
    // state file on first use is treated as a fresh-install — every
    // read returns default-initialised state and the first write
    // materialises the file.
    explicit FileBackedStateProvider(std::string state_path) noexcept;

    [[nodiscard]] tl::expected<void, StoreError>
    reserve_nonce(const std::array<std::uint8_t, 32>& nonce) noexcept override;

    [[nodiscard]] tl::expected<EpochState, StoreError>
    current_epoch_state() const noexcept override;

    [[nodiscard]] tl::expected<void, StoreError>
    advance_epoch_state(const EpochState& proposed) noexcept override;

    PersistenceCapability capabilities() const noexcept override;

private:
    std::string state_path_;
    std::string lock_path_;
};

}  // namespace VMPilot::Runtime::State

#endif  // VMPILOT_RUNTIME_STATE_FILE_BACKED_STATE_PROVIDER_HPP
