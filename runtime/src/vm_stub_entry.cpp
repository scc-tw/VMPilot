#include <vm_stub_entry.hpp>
#include <vm_entry.hpp>

#include <climits>
#include <cstdlib>

using namespace VMPilot::Common::VM;
using namespace VMPilot::Runtime;

extern "C"
int64_t vm_stub_entry(const VmStubArgs* args) noexcept {

    // ---- 1. Version check ----
    // Reject stubs patched with a different loader version.
    // A mismatch means the struct layout disagrees — reading fields
    // from wrong offsets would produce garbage pointers and crash
    // unpredictably.  Better to fail fast with a clear signal.
    if (!args || args->version != VM_STUB_ABI_VERSION) {
#ifdef NDEBUG
        std::abort();
#else
        return INT64_MIN;
#endif
    }

    // ---- 2. Convert POD → C++ request ----
    // VmStubArgs is the minimal POD contract from the stub.
    // VmExecRequest adds config (deployment setting, not stub concern).
    VmExecRequest req;
    req.blob_data       = args->blob_data;
    req.blob_size       = args->blob_size;
    req.stored_seed     = args->stored_seed;
    req.load_base_delta = args->load_base_delta;
    req.initial_regs    = args->initial_regs;
    req.num_regs        = args->num_regs;
    // req.config uses default — deployment config is determined by
    // the blob header or compiled into the runtime, not passed through
    // the stub hot path.

    // ---- 3. Execute ----
    auto result = vm_execute(req);

    // ---- 4. Return or abort ----
    if (result.has_value())
        return static_cast<int64_t>(result->return_value);

    // VM error: tampered blob, integrity failure, invalid BB transition, etc.
    // Production: abort.  The original native caller has no concept of
    // VM errors.  Returning to compromised code would violate the
    // anti-tamper guarantee (D15§9, D13§G2).
    // Debug: return sentinel for test observability.
#ifdef NDEBUG
    std::abort();
#else
    return INT64_MIN;
#endif
}
