#pragma once
#ifndef __LOADER_STUB_ARGS_LAYOUT_HPP__
#define __LOADER_STUB_ARGS_LAYOUT_HPP__

/// @file StubArgsLayout.hpp
/// @brief Target-arch byte offsets for VmStubArgs fields.
///
/// StubEmitter implementations use these traits to generate machine code
/// that builds a VmStubArgs struct on the stack.  Each specialization
/// defines the byte offset of every field as the target arch's ABI sees it.
///
/// The runtime side uses the C++ VmStubArgs struct definition directly
/// (compiled for the same target arch, so layout matches).  These traits
/// exist because the LOADER may cross-compile: a 64-bit host patching a
/// 32-bit target binary.  In that scenario, sizeof(void*) on the host
/// differs from the target, so the loader cannot #include vm_stub_args.hpp
/// and use offsetof().  Instead, it uses StubArgsLayout<TargetArch,Mode>
/// which hardcodes the correct target offsets.
///
/// Layout consistency between this file and vm_stub_args.hpp is verified
/// by static_assert in test code on native builds (where host == target).

#include <file_type_parser.hpp>

#include <cstddef>

namespace VMPilot::Loader {

/// Primary template — intentionally undefined.  Must specialize.
template <Common::FileArch Arch, Common::FileMode Mode>
struct StubArgsLayout;

// =====================================================================
// x86-64 (SysV ABI, pointers = 8 bytes)
// =====================================================================
template <>
struct StubArgsLayout<Common::FileArch::X86, Common::FileMode::MODE_64> {
    static constexpr std::size_t total_size          = 64;

    // Fixed header (same on all targets)
    static constexpr std::size_t off_version         = 0;   // uint32_t  (4)
    static constexpr std::size_t off_num_regs        = 4;   // uint8_t   (1)
    // reserved_u8[3] at offset 5-7

    // Scalars requiring strict alignment
    static constexpr std::size_t off_load_base_delta = 8;   // int64_t   (8)

    // Pointers (8 bytes each on 64-bit)
    static constexpr std::size_t off_blob_data       = 16;  // uint8_t*  (8)
    static constexpr std::size_t off_stored_seed     = 24;  // uint8_t*  (8)
    static constexpr std::size_t off_initial_regs    = 32;  // uint64_t* (8)

    // Remaining scalars
    static constexpr std::size_t off_blob_size       = 40;  // uint32_t  (4)
    // pad_[20] at offset 44-63
};

// =====================================================================
// x86-32 (cdecl, pointers = 4 bytes)
//
// Note: int64_t is at offset 8, which is 8-byte aligned.  This is
// required by MSVC x86_32 where __int64 has 8-byte alignment.
// GCC x86_32 uses 4-byte alignment for long long by default, so
// offset 8 satisfies both compilers.
// =====================================================================
template <>
struct StubArgsLayout<Common::FileArch::X86, Common::FileMode::MODE_32> {
    static constexpr std::size_t total_size          = 64;

    static constexpr std::size_t off_version         = 0;   // uint32_t  (4)
    static constexpr std::size_t off_num_regs        = 4;   // uint8_t   (1)
    // reserved_u8[3] at offset 5-7

    static constexpr std::size_t off_load_base_delta = 8;   // int64_t   (8)

    // Pointers (4 bytes each on 32-bit)
    static constexpr std::size_t off_blob_data       = 16;  // uint8_t*  (4)
    static constexpr std::size_t off_stored_seed     = 20;  // uint8_t*  (4)
    static constexpr std::size_t off_initial_regs    = 24;  // uint64_t* (4)

    static constexpr std::size_t off_blob_size       = 28;  // uint32_t  (4)
    // pad_[32] at offset 32-63
};

// =====================================================================
// ARM64 (AAPCS64, pointers = 8 bytes)
//
// Same layout as x86-64: both are LP64 with 8-byte pointers.
// Separate specialization so ARM64 ILP32 (e.g., watchOS) can
// diverge independently if needed in the future.
// =====================================================================
template <>
struct StubArgsLayout<Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN> {
    static constexpr std::size_t total_size          = 64;

    static constexpr std::size_t off_version         = 0;
    static constexpr std::size_t off_num_regs        = 4;

    static constexpr std::size_t off_load_base_delta = 8;

    static constexpr std::size_t off_blob_data       = 16;
    static constexpr std::size_t off_stored_seed     = 24;
    static constexpr std::size_t off_initial_regs    = 32;

    static constexpr std::size_t off_blob_size       = 40;
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_STUB_ARGS_LAYOUT_HPP__
