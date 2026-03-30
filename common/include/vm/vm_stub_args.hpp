#pragma once
#ifndef __COMMON_VM_STUB_ARGS_HPP__
#define __COMMON_VM_STUB_ARGS_HPP__

/// @file vm_stub_args.hpp
/// @brief POD struct for the entry-stub -> runtime handover.
///
/// This is the ONLY interface between loader-generated machine-code
/// stubs and the runtime library.  The struct is built on the stack
/// by hand-assembled stubs at runtime, then consumed by vm_stub_entry().
///
/// Loader side: does NOT #include this header.  Stub emitters use
/// StubArgsLayout<Arch,Mode> traits (loader/include/StubArgsLayout.hpp)
/// which define the byte offsets for the target arch.  This avoids
/// host-vs-target sizeof mismatches during cross-compilation.
///
/// Runtime side: #includes this header normally.  The runtime is always
/// compiled for the same target arch as the stub, so sizeof/offsetof
/// are guaranteed correct.
///
/// Design constraints:
///   - POD only (no constructors, vtable, or std:: types).
///   - int64_t at offset 8: 8-byte aligned on all compilers including
///     MSVC x86_32 (which requires __int64 8-byte alignment, unlike
///     GCC which uses 4-byte alignment for long long by default).
///   - All padding is explicit — no compiler-inserted gaps.
///     This avoids MSVC warning C4324 (structure padded due to
///     alignment specifier).
///   - Exactly 64 bytes on all targets (one cache line).
///   - Append-only evolution: new fields consume pad_[] bytes,
///     VM_STUB_ABI_VERSION increments to reject incompatible stubs.

#include <cstdint>
#include <cstddef>

namespace VMPilot::Common::VM {

/// ABI contract version.  Increment when VmStubArgs layout changes.
/// vm_stub_entry() rejects stubs with a mismatched version.
constexpr uint32_t VM_STUB_ABI_VERSION = 1;

/// POD handover struct: entry stub (loader) -> vm_stub_entry (runtime).
///
/// Field ordering rationale:
///   1. version + num_regs + explicit pad (8 bytes total)
///   2. int64_t load_base_delta at offset 8 — guaranteed 8-byte aligned
///      on all compilers (including MSVC x86_32 where __int64 requires
///      8-byte alignment)
///   3. Pointers next — naturally aligned (8B on 64-bit, 4B on 32-bit)
///   4. uint32_t blob_size last before padding
///   5. Explicit pad_[] to reach exactly 64 bytes
///
/// Memory layout (64-bit targets):
///   offset  0: version          uint32_t        (4)
///   offset  4: num_regs         uint8_t         (1)
///   offset  5: reserved_u8[3]                   (3)
///   offset  8: load_base_delta  int64_t         (8)
///   offset 16: blob_data        const uint8_t*  (8)
///   offset 24: stored_seed      const uint8_t*  (8)
///   offset 32: initial_regs     const uint64_t* (8)
///   offset 40: blob_size        uint32_t        (4)
///   offset 44: pad_[20]                         (20)
///   TOTAL: 64 bytes
///
/// Memory layout (32-bit targets):
///   offset  0: version          uint32_t        (4)
///   offset  4: num_regs         uint8_t         (1)
///   offset  5: reserved_u8[3]                   (3)
///   offset  8: load_base_delta  int64_t         (8)
///   offset 16: blob_data        const uint8_t*  (4)
///   offset 20: stored_seed      const uint8_t*  (4)
///   offset 24: initial_regs     const uint64_t* (4)
///   offset 28: blob_size        uint32_t        (4)
///   offset 32: pad_[32]                         (32)
///   TOTAL: 64 bytes
struct VmStubArgs {
    // --- Fixed header (offset 0-7, same on all targets) ---
    uint32_t        version;          ///< Must equal VM_STUB_ABI_VERSION
    uint8_t         num_regs;         ///< Number of saved GPRs
    uint8_t         reserved_u8[3];   ///< Zero. Explicit alignment padding.

    // --- Scalars requiring strict alignment ---
    int64_t         load_base_delta;  ///< actual_VA - static_VA (D13§D3)

    // --- Pointers (size varies with target: 8B on 64-bit, 4B on 32-bit) ---
    const uint8_t*  blob_data;        ///< Pointer to blob in injected payload
    const uint8_t*  stored_seed;      ///< Pointer to 32-byte seed in payload
    const uint64_t* initial_regs;     ///< Pointer to GPR save area on stack

    // --- Remaining scalars ---
    uint32_t        blob_size;        ///< Blob size in bytes

    // --- Explicit tail padding to 64 bytes ---
    // Computed at compile time from actual field sizes, so automatically
    // correct for both 32-bit (pad = 32) and 64-bit (pad = 20) targets.
    uint8_t         pad_[64
        - 4                       // version
        - 1                       // num_regs
        - 3                       // reserved_u8
        - sizeof(int64_t)         // load_base_delta
        - sizeof(const uint8_t*)  // blob_data
        - sizeof(const uint8_t*)  // stored_seed
        - sizeof(const uint64_t*) // initial_regs
        - sizeof(uint32_t)        // blob_size
    ];
};

// --- Compile-time guarantees ---
static_assert(sizeof(VmStubArgs) == 64,
              "VmStubArgs must be exactly 64 bytes (one cache line)");
static_assert(alignof(VmStubArgs) <= 64,
              "VmStubArgs must not require alignment greater than a cache line");
static_assert(offsetof(VmStubArgs, load_base_delta) == 8,
              "load_base_delta must be at offset 8 for 8-byte alignment on all compilers");
static_assert(offsetof(VmStubArgs, load_base_delta) % 8 == 0,
              "load_base_delta must be 8-byte aligned (MSVC x86_32 requires this for __int64)");

}  // namespace VMPilot::Common::VM

#endif  // __COMMON_VM_STUB_ARGS_HPP__
