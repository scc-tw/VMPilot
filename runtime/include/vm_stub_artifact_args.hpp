#ifndef VMPILOT_RUNTIME_VM_STUB_ARTIFACT_ARGS_HPP
#define VMPILOT_RUNTIME_VM_STUB_ARTIFACT_ARGS_HPP

#include <cstddef>
#include <cstdint>

// POD handover for the redesigned per-unit entry stub.
//
// `vm_stub_entry_artifact` consumes a full signed artifact buffer plus a
// unit identifier and drives the Stage 4–7 acceptance pipeline before
// handing the payload to VmEngine. The legacy `vm_stub_entry(VmStubArgs*)`
// path (blob-header-flag dispatch) stays compiled for backward
// compatibility while existing tests migrate; new code should target the
// artifact path.

namespace VMPilot::Runtime {

constexpr std::uint32_t VM_STUB_ARTIFACT_ABI_VERSION = 2;

struct VmStubArtifactArgs {
    std::uint32_t   version;           // must equal VM_STUB_ARTIFACT_ABI_VERSION
    std::uint32_t   num_regs;
    std::int64_t    load_base_delta;

    const std::uint8_t* artifact_data;
    std::uint64_t        artifact_size;

    // Null-terminated UTF-8 unit identifier. The runtime looks this id up
    // in the artifact's UnitDescriptorTable.
    const char*         unit_id;

    // Payload-level encryption seed, still passed externally by the
    // loader. The artifact carries payload identity (SHA-256 + size)
    // which is verified before the seed is used.
    const std::uint8_t* stored_seed;

    const std::uint64_t* initial_regs;
};

}  // namespace VMPilot::Runtime

#endif  // VMPILOT_RUNTIME_VM_STUB_ARTIFACT_ARGS_HPP
