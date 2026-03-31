/// @file vm_stub_entry.cpp
/// @brief C-linkage entry point wired to VmEngine<Policy, Oram>.
///
/// This is the production execution path.  The loader-generated assembly
/// stubs call vm_stub_entry() which:
///   1. Validates the VmStubArgs
///   2. Selects the security policy based on the blob's debug flag
///   3. Creates a VmEngine and executes until HALT
///   4. Returns the decoded result in the platform return register
///
/// Policy selection layer:
///   Blob debug flag → DebugPolicy (native bridge arithmetic, no constant-time)
///   Otherwise       → StandardPolicy (MBA arithmetic, constant-time pipeline)
///
/// The policy is selected ONCE at entry — not per-instruction.
/// VmEngine is a template, so each policy variant is a separate
/// compilation unit with full dead-code elimination.

#include <vm_stub_entry.hpp>
#include <vm_engine.hpp>
#include <vm/blob_view.hpp>

#include <climits>
#include <cstdlib>

using namespace VMPilot::Common::VM;
using namespace VMPilot::Runtime;

// ─────────────────────────────────────────────────────────────────────────────
// Policy dispatch layer: template function per policy, called once at entry
// ─────────────────────────────────────────────────────────────────────────────

template<typename Policy>
static int64_t execute_with_policy(const VmStubArgs* args) noexcept {
    auto engine = VmEngine<Policy>::create(
        args->blob_data, args->blob_size,
        args->stored_seed,
        args->load_base_delta,
        args->initial_regs, args->num_regs);

    if (!engine.has_value()) {
#ifdef NDEBUG
        std::abort();
#else
        return INT64_MIN;
#endif
    }

    auto result = engine->execute();
    if (!result.has_value()) {
#ifdef NDEBUG
        std::abort();
#else
        return INT64_MIN;
#endif
    }

    return static_cast<int64_t>(result->return_value);
}

// ─────────────────────────────────────────────────────────────────────────────
// C-linkage entry point
// ─────────────────────────────────────────────────────────────────────────────

extern "C"
int64_t vm_stub_entry(const VmStubArgs* args) noexcept {

    // ── 1. Validation layer ─────────────────────────────────────────────

    if (!args || args->version != VM_STUB_ABI_VERSION) {
#ifdef NDEBUG
        std::abort();
#else
        return INT64_MIN;
#endif
    }

    if (!args->blob_data || args->blob_size < sizeof(BlobHeader)) {
#ifdef NDEBUG
        std::abort();
#else
        return INT64_MIN;
#endif
    }

    if (!args->stored_seed) {
#ifdef NDEBUG
        std::abort();
#else
        return INT64_MIN;
#endif
    }

    // ── 2. Policy selection layer ───────────────────────────────────────
    //
    // Peek at blob header to determine debug mode.  BlobView::create()
    // validates the full header; here we just need the debug flag to
    // select the policy.  This is a single branch executed once at entry,
    // not per-instruction.

    auto blob_or = BlobView::create(args->blob_data, args->blob_size);
    if (!blob_or.has_value()) {
#ifdef NDEBUG
        std::abort();
#else
        return INT64_MIN;
#endif
    }

    // ── 3. Dispatch to policy-specific execution ────────────────────────

    if (blob_or->is_debug()) {
        return execute_with_policy<DebugPolicy>(args);
    } else {
        return execute_with_policy<StandardPolicy>(args);
    }
}
