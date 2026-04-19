#include "state/tpm_session.hpp"

#include <memory>

#ifdef VMPILOT_HAS_TPM2_TSS
extern "C" {
#include <tss2/tss2_esys.h>
#include <tss2/tss2_tctildr.h>
}
#endif

// tpm2-tss ESAPI-backed concrete session, gated behind
// VMPILOT_HAS_TPM2_TSS. When the define is absent (Windows default,
// macOS, or any build that leaves VMPILOT_ENABLE_TPM=OFF),
// `open_tpm_session()` returns `ProviderUnavailable` so the provider
// layer fails closed at the acceptance rule.

namespace VMPilot::Runtime::State {

#ifdef VMPILOT_HAS_TPM2_TSS

namespace {

// RAII wrapper around ESAPI + TCTI handles. Order matters on teardown:
// finalize ESAPI first (drops references into TCTI), then the TCTI.
// Missing that order has been the root cause of at least one swtpm
// hang in other projects, so keep the dtor explicit rather than
// relying on any future refactor.
class TssEsapiSession final : public ITpmSession {
public:
    static tl::expected<std::unique_ptr<ITpmSession>, StoreError>
    open(const char* tcti_conf) noexcept {
        TSS2_TCTI_CONTEXT* tcti = nullptr;
        TSS2_RC rc = Tss2_TctiLdr_Initialize(tcti_conf, &tcti);
        if (rc != TSS2_RC_SUCCESS || tcti == nullptr) {
            return tl::make_unexpected(StoreError::ProviderUnavailable);
        }

        ESYS_CONTEXT* esys = nullptr;
        rc = Esys_Initialize(&esys, tcti, nullptr);
        if (rc != TSS2_RC_SUCCESS || esys == nullptr) {
            Tss2_TctiLdr_Finalize(&tcti);
            return tl::make_unexpected(StoreError::ProviderUnavailable);
        }

        // `new (std::nothrow)` keeps the factory noexcept even under
        // std::bad_alloc; on OOM we unwind the TPM handles so no
        // device reference leaks. Downstream callers own the
        // unique_ptr and release via the dtor below.
        auto* raw = new (std::nothrow) TssEsapiSession{esys, tcti};
        if (raw == nullptr) {
            Esys_Finalize(&esys);
            Tss2_TctiLdr_Finalize(&tcti);
            return tl::make_unexpected(StoreError::IoError);
        }
        return std::unique_ptr<ITpmSession>{raw};
    }

    ~TssEsapiSession() override {
        if (esys_ != nullptr) {
            Esys_Finalize(&esys_);
        }
        if (tcti_ != nullptr) {
            Tss2_TctiLdr_Finalize(&tcti_);
        }
    }

    tl::expected<std::uint32_t, StoreError>
    get_manufacturer() noexcept override {
        TPMS_CAPABILITY_DATA* cap = nullptr;
        TPMI_YES_NO more = TPM2_NO;
        TSS2_RC rc = Esys_GetCapability(
            esys_,
            ESYS_TR_NONE, ESYS_TR_NONE, ESYS_TR_NONE,
            TPM2_CAP_TPM_PROPERTIES,
            TPM2_PT_MANUFACTURER,
            /*count=*/1,
            &more,
            &cap);
        if (rc != TSS2_RC_SUCCESS || cap == nullptr) {
            if (cap != nullptr) Esys_Free(cap);
            return tl::make_unexpected(StoreError::IoError);
        }

        // Expect exactly one tagged property = TPM2_PT_MANUFACTURER.
        const auto& props = cap->data.tpmProperties;
        if (cap->capability != TPM2_CAP_TPM_PROPERTIES ||
            props.count != 1 ||
            props.tpmProperty[0].property != TPM2_PT_MANUFACTURER) {
            Esys_Free(cap);
            return tl::make_unexpected(StoreError::Corrupt);
        }

        const std::uint32_t value = props.tpmProperty[0].value;
        Esys_Free(cap);
        return value;
    }

private:
    TssEsapiSession(ESYS_CONTEXT* esys, TSS2_TCTI_CONTEXT* tcti) noexcept
        : esys_(esys), tcti_(tcti) {}

    ESYS_CONTEXT*      esys_;
    TSS2_TCTI_CONTEXT* tcti_;
};

}  // namespace

tl::expected<std::unique_ptr<ITpmSession>, StoreError>
open_tpm_session() noexcept {
    // `nullptr` = Tss2_TctiLdr_Initialize picks the system default.
    // PR-B3 introduces VMPILOT_TPM_TCTI env-var selection.
    return TssEsapiSession::open(nullptr);
}

#else  // !VMPILOT_HAS_TPM2_TSS

tl::expected<std::unique_ptr<ITpmSession>, StoreError>
open_tpm_session() noexcept {
    return tl::make_unexpected(StoreError::ProviderUnavailable);
}

#endif

}  // namespace VMPilot::Runtime::State
