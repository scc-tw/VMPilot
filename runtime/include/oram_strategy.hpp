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

    /// Unified ORAM access — always performs a full 64-line scan.
    ///
    /// WHY unified (Doc 19 pipeline-level normalization):
    ///   The dispatch_unit pipeline calls this once per sub-instruction,
    ///   unconditionally.  Branchless MUX in the pipeline selects the
    ///   address, value, and direction based on the decoded opcode:
    ///     PUSH: addr=vm_sp-8, value=encoded, is_write=true
    ///     POP:  addr=vm_sp,   value=0,       is_write=false
    ///     else: addr=0,       value=0,       is_write=false (dummy)
    ///
    ///   Every sub-instruction does exactly 1 scan at the same cost.
    ///   PUSH/POP handlers no longer call read/write directly.
    ///
    /// @return  read result (meaningful for POP; 0 for PUSH/dummy)
    [[nodiscard]] static uint64_t access(VmOramState& state, uint64_t addr,
                                         uint64_t write_value,
                                         bool is_write) noexcept;
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

    /// Unified access for DirectOram — direct indexed, no oblivious scan.
    [[nodiscard]] static uint64_t access(VmOramState& state, uint64_t addr,
                                         uint64_t write_value,
                                         bool is_write) noexcept;
};

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_ORAM_STRATEGY_HPP__
