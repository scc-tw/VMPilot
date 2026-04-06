#pragma once
#ifndef __RUNTIME_VM_INTRINSICS_HPP__
#define __RUNTIME_VM_INTRINSICS_HPP__

/// @file vm_intrinsics.hpp
/// @brief Intrinsic native call resolution for runtime-provided functions.
///
/// The NATIVE_CALL handler resolves targets via
///   target_offset + load_base_delta
/// which works for functions in the protected binary.  Runtime-internal
/// functions (e.g. TLS helpers) live in the runtime library and their
/// addresses are unknown at blob-build time.
///
/// Convention: the blob stores a sentinel value in target_offset to
/// indicate an intrinsic.  The runtime resolves it to the actual
/// function pointer at engine creation (VmEngine::create), before any
/// instruction executes.
///
/// ─── Backend Contract ────────────────────────────────────────────────
///
/// When the backend (SimpleBackend / LLVM backend) encounters a
/// segment-prefixed memory access in the protected region:
///
///   x86-64:  fs:[offset]              → TLS_READ64 / TLS_WRITE64
///   x86-32:  gs:[offset]              → TLS_READ32 / TLS_WRITE32
///   ARM64:   mrs TPIDR_EL0 + ldr/str  → TLS_READ64 / TLS_WRITE64
///
/// It must emit:
///
///   1. Load the TLS offset into r0.
///      For writes: offset in r0, value in r1.
///
///   2. A NATIVE_CALL instruction with insn.aux = transition entry index.
///
///   3. A TransitionEntry in the blob with:
///        target_offset = intrinsic_target(IntrinsicId::TLS_READ64)
///                      = INTRINSIC_BASE + id
///        arg_count     = te_pack_arg_count(1, 0, false, false, false)
///                        (2 for write variants)
///        call_site_ip  = instruction index of the NATIVE_CALL
///
/// The runtime resolves the sentinel to the actual function pointer at
/// engine creation time.  The result (for reads) is returned in r0.
/// ─────────────────────────────────────────────────────────────────────

#include <cstdint>

namespace VMPilot::Runtime {

/// Sentinel base for intrinsic target_offset values.
/// Top of the 64-bit address space — never a real function pointer.
inline constexpr uint64_t INTRINSIC_BASE = 0xFFFF'FFFF'FFFF'FF00ULL;

/// Well-known intrinsic IDs.
enum class IntrinsicId : uint8_t {
    TLS_READ64  = 0,   ///< vmpilot_tls_read64(offset) -> uint64_t
    TLS_WRITE64 = 1,   ///< vmpilot_tls_write64(offset, value) -> void
    TLS_READ32  = 2,   ///< vmpilot_tls_read32(offset) -> uint64_t
    TLS_WRITE32 = 3,   ///< vmpilot_tls_write32(offset, value) -> void

    COUNT               ///< must be last
};

/// Produce the sentinel target_offset for blob builders.
constexpr uint64_t intrinsic_target(IntrinsicId id) noexcept {
    return INTRINSIC_BASE + static_cast<uint8_t>(id);
}

/// Check whether a target_offset is an intrinsic sentinel.
constexpr bool is_intrinsic_target(uint64_t target_offset) noexcept {
    return target_offset >= INTRINSIC_BASE
        && target_offset < INTRINSIC_BASE + static_cast<uint8_t>(IntrinsicId::COUNT);
}

/// Resolve an intrinsic ID to the actual function pointer.
/// Returns nullptr for unknown IDs.
void* resolve_intrinsic(IntrinsicId id) noexcept;

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_VM_INTRINSICS_HPP__
