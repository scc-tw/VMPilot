#ifndef __LOADER_ARCH_TRAITS_HPP__
#define __LOADER_ARCH_TRAITS_HPP__
#pragma once

/// @file ArchTraits.hpp
/// @brief Compile-time per-{arch, mode} constants.
///
/// Defines calling convention, callee-saved registers, argument registers,
/// branch limits, and pointer sizes for each supported architecture.
///
/// Source: RM§2.E1 (Calling Convention Matrix).

#include <file_type_parser.hpp>

#include <array>
#include <cstdint>
#include <climits>

namespace VMPilot::Loader {

template <Common::FileArch Arch, Common::FileMode Mode>
struct ArchTraits;

// ---------------------------------------------------------------------------
// x86-64 SysV ABI (Linux, macOS)
// RM§2.E1: rdi, rsi, rdx, rcx, r8, r9 | rbx, rbp, r12-r15
// ---------------------------------------------------------------------------
template <>
struct ArchTraits<Common::FileArch::X86, Common::FileMode::MODE_64> {
    static constexpr size_t  min_region_size  = 5;       // JMP rel32
    static constexpr int64_t max_branch_dist  = INT32_MAX;
    static constexpr size_t  ptr_size         = 8;

    // D10§GAP7: "Save ALL callee-saved registers"
    static constexpr std::array<uint8_t, 6> callee_saved = {
        3,   // rbx
        5,   // rbp
        12,  // r12
        13,  // r13
        14,  // r14
        15,  // r15
    };

    // RM§2.E1: System V arg order
    static constexpr std::array<uint8_t, 6> arg_regs = {
        7,  // rdi
        6,  // rsi
        2,  // rdx
        1,  // rcx
        8,  // r8
        9,  // r9
    };

    static constexpr uint8_t return_reg = 0;  // rax
    static constexpr uint8_t gpr_count  = 16;
};

// ---------------------------------------------------------------------------
// x86 32-bit cdecl (args on stack)
// ---------------------------------------------------------------------------
template <>
struct ArchTraits<Common::FileArch::X86, Common::FileMode::MODE_32> {
    static constexpr size_t  min_region_size  = 5;       // JMP rel32
    static constexpr int64_t max_branch_dist  = INT32_MAX;
    static constexpr size_t  ptr_size         = 4;

    static constexpr std::array<uint8_t, 4> callee_saved = {
        3,  // ebx
        5,  // ebp
        6,  // esi
        7,  // edi
    };

    // cdecl: all args on stack
    static constexpr std::array<uint8_t, 0> arg_regs = {};

    static constexpr uint8_t return_reg = 0;  // eax
    static constexpr uint8_t gpr_count  = 8;
};

// ---------------------------------------------------------------------------
// ARM64 AAPCS64 (Linux, macOS)
// RM§2.E1: x0-x7 args | x19-x28 callee-saved, x29(fp), x30(lr)
// ---------------------------------------------------------------------------
template <>
struct ArchTraits<Common::FileArch::ARM64, Common::FileMode::MODE_LITTLE_ENDIAN> {
    static constexpr size_t  min_region_size  = 4;       // B imm26
    static constexpr int64_t max_branch_dist  = 128LL * 1024 * 1024;
    static constexpr size_t  ptr_size         = 8;

    static constexpr std::array<uint8_t, 12> callee_saved = {
        19, 20, 21, 22, 23, 24, 25, 26, 27, 28,  // x19-x28
        29,  // x29 (fp)
        30,  // x30 (lr)
    };

    static constexpr std::array<uint8_t, 8> arg_regs = {
        0, 1, 2, 3, 4, 5, 6, 7,  // x0-x7
    };

    static constexpr uint8_t return_reg = 0;  // x0
    static constexpr uint8_t gpr_count  = 31; // x0-x30
};

}  // namespace VMPilot::Loader

#endif  // __LOADER_ARCH_TRAITS_HPP__
