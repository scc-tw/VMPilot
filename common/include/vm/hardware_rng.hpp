#pragma once
#ifndef __COMMON_VM_HARDWARE_RNG_HPP__
#define __COMMON_VM_HARDWARE_RNG_HPP__

/// @file hardware_rng.hpp
/// @brief Hardware random number generation for per-execution nonces.
///
/// WHY HARDWARE RNG:
///
///   Doc 16's forward-secrecy chain requires a per-execution nonce in
///   bb_chain_state[0].  If this nonce is deterministic (e.g., zero), then
///   two executions of the same blob with the same seed produce identical
///   FPE keys — an attacker who captures one execution's register values
///   can decode all future executions.
///
///   RDRAND (x86) / RNDR (ARM64) provides a hardware-backed, non-deterministic
///   64-bit random value that differs across executions.  This makes bb_chain_state
///   (and therefore all FPE keys) unique per execution, even with the same blob+seed.
///
/// FALLBACK HIERARCHY (platform-specific, see common/src/vm/hardware_rng_*.cpp):
///
///   x86:    RDRAND → /dev/urandom | BCryptGenRandom → zero
///   ARM64:  RNDR   → /dev/urandom → zero
///   Darwin: RNDR (runtime-detected via sysctl) → arc4random_buf → /dev/urandom → zero

#include <cstdint>

namespace VMPilot::Common::VM {

/// Generate a 64-bit hardware random value.
///
/// Thread-safe: RDRAND/RNDR are per-core; OS fallbacks are thread-safe.
/// Not suitable for key material — use BLAKE3 KDF for that.
uint64_t hardware_random_u64() noexcept;

}  // namespace VMPilot::Common::VM

#endif  // __COMMON_VM_HARDWARE_RNG_HPP__
