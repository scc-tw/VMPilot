#include "state/persistent_state_provider.hpp"

// Interface-side translation unit for the PersistentStateProvider
// hierarchy. Concrete providers live in sibling TUs (Stage A shipped
// FileBackedStateProvider; Stage B adds TpmBackedStateProvider; Stage
// C adds CloudAttestedStateProvider).
//
// Keeping `satisfies_highsec` here — rather than inlining in the
// header — means the acceptance rule is linkage-time stable: every
// consumer that links VMPilot_Runtime sees the same definition, and
// any future evolution (e.g. a Stage C clause for a distinct remote-
// authority path) is a single-TU edit.

namespace VMPilot::Runtime::State {

bool satisfies_highsec(const PersistenceCapability& c) noexcept {
    return c.operational_state == ProviderOperationalState::Available
        && c.cross_process_atomic
        && c.crash_consistent
        && c.rollback_resistant
        && c.tamper_evident
        && c.hardware_bound
        && c.policy_bound
        && c.signed_recovery_required;
}

}  // namespace VMPilot::Runtime::State
