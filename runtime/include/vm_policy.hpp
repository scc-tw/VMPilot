#pragma once
#ifndef __RUNTIME_VM_POLICY_HPP__
#define __RUNTIME_VM_POLICY_HPP__

/// @file vm_policy.hpp
/// @brief Compile-time security policies for VmEngine.
///
/// Each policy is a traits struct that controls:
///   - constant_time: D3 uniform pipeline enforced unconditionally (true) or optimised (false)
///   - fusion_granularity: superoperator window size (1 = no fusion, 2-4 = fused handlers)
///
/// Policy selection is a compile-time template parameter on VmEngine<Policy, Oram>.
/// This enables dead-code elimination: unused handler variants are not compiled.
///
/// Security argument:
///   HighSecPolicy enforces all four security dimensions (D1-D4) at full strength.
///   StandardPolicy is identical but with smaller fusion window (faster compilation).
///   DebugPolicy disables D2/D3 features for ease of debugging — NOT for production.
///   static_assert in VmEngine ensures only valid policies are instantiated.
///
/// NOTE: MBA (Mixed Boolean Arithmetic) was removed from runtime handlers.
///   MBA obfuscation of arithmetic (ADD/SUB/NEG) is now the responsibility of the
///   compiler backend / serializer, where it can be applied as a static instruction
///   transform without runtime timing cost.  The FPE + key ratchet + ORAM pipeline
///   already protects register state at runtime; runtime MBA only added handler cost
///   variance that increased timing side-channel leakage (between_σ: 12K→34K ns).

#include <cstdint>

namespace VMPilot::Runtime {

// ─────────────────────────────────────────────────────────────────────────────
// Security class tags (for handler trait annotations)
// ─────────────────────────────────────────────────────────────────────────────

/// Classification of handler security level (doc 15 §3.4).
///
/// NOTE: Class B (MBA decomposition) has been removed from runtime.
/// Former Class B opcodes (ADD, SUB, NEG) now use native arithmetic
/// at runtime.  MBA obfuscation is applied at compile time instead.
enum class SecurityClass : uint8_t {
    A,   ///< Zero plaintext — homomorphic composition table (AND, OR, XOR, NOT)
    C,   ///< Native bridge — full plaintext register-transient (MUL, DIV, ADD, SUB, NEG, ...)
};

// ─────────────────────────────────────────────────────────────────────────────
// Policy traits
// ─────────────────────────────────────────────────────────────────────────────

/// Maximum security: constant-time pipeline, fusion=4.
///
/// All four dimensions active:
///   D1: Per-BB SipHash encryption (always on)
///   D2: MCSP-hard byte-lane LUT encoding
///   D3: Uniform 12-step pipeline, superoperator fusion window = 4
///   D4: Two-layer PRP opcode mutation (always on)
struct HighSecPolicy {
    static constexpr bool    constant_time        = true;
    static constexpr uint8_t fusion_granularity   = 4;

    /// Validate policy invariants at compile time.
    static constexpr bool validate() noexcept {
        return fusion_granularity >= 1 && fusion_granularity <= 4;
    }
};

/// Standard security: same as HighSec but fusion=2 (smaller compile overhead).
struct StandardPolicy {
    static constexpr bool    constant_time        = true;
    static constexpr uint8_t fusion_granularity   = 2;

    static constexpr bool validate() noexcept {
        return fusion_granularity >= 1 && fusion_granularity <= 4;
    }
};

/// Debug policy: no constant-time pipeline.
///
/// Useful for development and testing.  NOT production-safe:
///   D2: encoding tables still active (values are encoded)
///   D3: pipeline NOT constant-time (data-dependent branches allowed)
struct DebugPolicy {
    static constexpr bool    constant_time        = false;
    static constexpr uint8_t fusion_granularity   = 1;

    static constexpr bool validate() noexcept { return true; }
};

// ─────────────────────────────────────────────────────────────────────────────
// Default ORAM selection per policy (forward declaration)
// ─────────────────────────────────────────────────────────────────────────────

// Defined in oram_strategy.hpp.  Declared here so policies can reference them.
struct RollingKeyOram;
struct DirectOram;

/// Map each policy to its default ORAM strategy.
///
/// HighSec/Standard → RollingKeyOram (full IND-CPA security)
/// Debug → DirectOram (fast, no oblivious access pattern)
template<typename Policy>
struct DefaultOramFor;

template<> struct DefaultOramFor<HighSecPolicy>  { using type = RollingKeyOram; };
template<> struct DefaultOramFor<StandardPolicy>  { using type = RollingKeyOram; };
template<> struct DefaultOramFor<DebugPolicy>     { using type = DirectOram; };

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_VM_POLICY_HPP__
