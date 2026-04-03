#pragma once
#ifndef __RUNTIME_ORAM_STRATEGY_HPP__
#define __RUNTIME_ORAM_STRATEGY_HPP__

/// @file oram_strategy.hpp
/// @brief ORAM access strategies — orthogonal to security policy.
///
/// Two strategies:
///
///   RollingKeyOram — Full oblivious access: scans all 64 cache lines on every
///                    read/write, re-encrypts entire 4KB workspace with fresh
///                    keystream.  IND-CPA secure under SipHash PRF assumption.
///
///   DirectOram    — Direct indexed access (no oblivious scan).  Leaks access
///                   patterns via cache-timing side channels.  Values are still
///                   in Global Memory Encoding (MCSP-hard), so data content is
///                   protected but access addresses are not.
///
/// ORAM strategy is a separate template parameter on VmEngine to allow:
///   - VmEngine<HighSec, DirectOram>   for fast testing of HighSec handlers
///   - VmEngine<Standard, RollingKeyOram>  for paranoid deployments
///
/// Security argument (orthogonality, doc 15 §11.6):
///   IND-CPA of rolling-keystream ORAM is independent of MCSP value encoding.
///   Composing them provides defense in depth:
///     - ORAM disabled: values still MCSP-encoded
///     - MCSP broken: access patterns still hidden by ORAM
///   Adv_IND-CPA(A) ≤ negl(s) under PRF assumption on SipHash, regardless
///   of whether A can break MCSP encoding.

#include <vm/encoded_value.hpp>
#include <vm/vm_crypto.hpp>

#include <cstdint>

namespace VMPilot::Runtime {

using Common::VM::MemVal;

// Forward declaration
struct VmOramState;

// ─────────────────────────────────────────────────────────────────────────────
// RollingKeyOram — Full oblivious access (IND-CPA)
// ─────────────────────────────────────────────────────────────────────────────

/// Rolling-keystream ORAM: every access scans all 64 cache lines.
///
/// Algorithm (doc 15 §5.3, Block-Streaming):
///   1. For each of 64 cache lines:
///      a. Decrypt 64 bytes with old keystream (SipHash-expand)
///      b. Branchless CMOV: read or write at target offset
///      c. Re-encrypt with new keystream (fresh nonce)
///      d. Store back
///   2. Increment nonce (monotonic — never repeats)
///
/// Properties:
///   - Access pattern oblivious: all 64 lines always touched
///   - Diff attack defeated: rolling nonce → all 4KB avalanche on every write
///   - No RAM spillage: 64-byte cache line fits in 8 GPRs
///   - IND-CPA: Pr[Adv distinguishes] ≤ negl(s) under SipHash PRF
struct RollingKeyOram {
    static constexpr bool oblivious = true;

    /// Read an 8-byte MemVal from the oblivious workspace at `offset`.
    /// Scans all 64 cache lines (constant-time access pattern).
    [[nodiscard]] static MemVal read(VmOramState& state, uint64_t offset) noexcept;

    /// Write an 8-byte MemVal to the oblivious workspace at `offset`.
    /// Scans all 64 cache lines and re-encrypts entire workspace.
    static void write(VmOramState& state, uint64_t offset, MemVal val) noexcept;

    /// Unconditional dummy scan (Doc 19 §C.1, ORAM Invariant).
    ///
    /// WHY: every dispatch_unit must produce the same ORAM access pattern
    /// regardless of opcode mix.  A DU with PUSH/POP triggers real ORAM
    /// scans; a DU with only ALU ops does not.  The dummy scan ensures
    /// at least 1 full scan per DU, normalizing the memory bus frequency
    /// to constant rate (Doc 19 Appendix C.4).
    ///
    /// Implementation: read-equivalent — full 64-line scan + re-encrypt +
    /// nonce bump, identical cost to a real read.  Result discarded.
    static void dummy_scan(VmOramState& state) noexcept;
};

// ─────────────────────────────────────────────────────────────────────────────
// DirectOram — Fast direct access (NOT oblivious)
// ─────────────────────────────────────────────────────────────────────────────

/// Direct indexed ORAM: no oblivious scan, no re-encryption.
///
/// Values are still in Global Memory Encoding (MCSP-hard), so data content
/// is protected.  But access patterns are visible to cache-timing attackers.
///
/// Use for: debug builds, unit testing handler logic, performance benchmarks.
/// Do NOT use in production deployments where cache-timing is a threat.
struct DirectOram {
    static constexpr bool oblivious = false;

    /// Read 8 bytes from workspace at `offset` (direct indexed access).
    [[nodiscard]] static MemVal read(VmOramState& state, uint64_t offset) noexcept;

    /// Write 8 bytes to workspace at `offset` (direct indexed access).
    static void write(VmOramState& state, uint64_t offset, MemVal val) noexcept;

    /// No-op dummy scan — DirectOram does not need timing normalization.
    /// DebugPolicy::constant_time == false, so timing leaks are acceptable.
    static void dummy_scan(VmOramState&) noexcept {}
};

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_ORAM_STRATEGY_HPP__
