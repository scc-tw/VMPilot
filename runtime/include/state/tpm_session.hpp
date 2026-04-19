#ifndef VMPILOT_RUNTIME_STATE_TPM_SESSION_HPP
#define VMPILOT_RUNTIME_STATE_TPM_SESSION_HPP

#include <cstdint>
#include <memory>

#include "state/persistent_state_provider.hpp"

// TPM session abstraction — doc 17a Stage B plan §2 (library) + §13 PR-B2.
//
// `ITpmSession` is the dependency-inversion boundary the rest of the
// Stage B code builds on. Concrete implementations:
//
//     TssEsapiSession   — real tpm2-tss ESAPI over TCTI (Linux).
//                         Compiled only when VMPILOT_HAS_TPM2_TSS is
//                         defined; otherwise open_tpm_session()
//                         returns StoreError::ProviderUnavailable.
//
//     MockTpmSession    — in-memory simulator. Lands with PR-B4 once
//                         NV-counter / seal operations join the
//                         interface, so unit tests don't need swtpm
//                         to cover the non-TPM parts of Stage B.
//
// Putting the interface here — rather than inside
// tpm_backed_state_provider.cpp — means Stage B can grow the surface
// incrementally (PR-B4: nv_read_counter / nv_increment; PR-B5: seal /
// unseal; PR-B6: policy_authorize; ...) without the provider ever
// touching a raw ESYS_CONTEXT*. The provider consumes `ITpmSession`;
// the tpm2-tss linkage never leaves this file's implementation.

namespace VMPilot::Runtime::State {

// Abstract TPM session. Every method is noexcept and returns
// `tl::expected` so the provider's acceptance rule can map each
// failure mode onto the right `StoreError` (see doc 17a §6).
//
// PR-B2 surface is deliberately minimal: only a liveness / smoke
// query. Later PRs extend this interface rather than replacing it —
// any future mock or real impl that wants to satisfy the interface
// must cover every operation the provider relies on.
class ITpmSession {
public:
    virtual ~ITpmSession() = default;

    ITpmSession(const ITpmSession&)            = delete;
    ITpmSession& operator=(const ITpmSession&) = delete;
    ITpmSession(ITpmSession&&)                 = delete;
    ITpmSession& operator=(ITpmSession&&)      = delete;

    // Returns the TPM's raw 4-byte manufacturer identifier packed
    // big-endian into a `std::uint32_t` (TPM2_PT_MANUFACTURER). The
    // decode matches the TCG convention: the byte that appears first
    // on the wire ends up in the MSB, so a little-endian host sees
    // e.g. ASCII "IBM " as 0x49424D20.
    //
    // Any non-TPM2_RC_SUCCESS from the underlying TPM surfaces as a
    // `StoreError`; see the concrete impl for the exact mapping.
    [[nodiscard]] virtual tl::expected<std::uint32_t, StoreError>
    get_manufacturer() noexcept = 0;

protected:
    ITpmSession() noexcept = default;
};

// Opens an ITpmSession using the system default TCTI (`nullptr` conf
// to `Tss2_TctiLdr_Initialize`). PR-B3 layers explicit TCTI selection
// on top via the VMPILOT_TPM_TCTI env var; PR-B2 accepts whatever the
// system picks so the smoke path is as short as possible.
//
// Returns `StoreError::ProviderUnavailable` on platforms without
// tpm2-tss (macOS, Windows-with-VMPILOT_ENABLE_TPM=OFF) and on Linux
// hosts whose TCTI init / ESAPI init fail — doc 17a §4 forbids a
// skeleton session from passing highsec, and doc 17 §3.1 enforces
// that via the acceptance rule at the provider level.
[[nodiscard]] tl::expected<std::unique_ptr<ITpmSession>, StoreError>
open_tpm_session() noexcept;

}  // namespace VMPilot::Runtime::State

#endif  // VMPILOT_RUNTIME_STATE_TPM_SESSION_HPP
