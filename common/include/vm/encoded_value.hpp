#pragma once
#ifndef __COMMON_VM_ENCODED_VALUE_HPP__
#define __COMMON_VM_ENCODED_VALUE_HPP__

/// @file encoded_value.hpp
/// @brief Phantom-typed encoding domains — compile-time domain separation.
///
/// The VM ISA (doc 15 §3.1) defines two distinct encoding domains:
///
///   1. Register Domain (per-BB): values in VM registers are encoded with
///      per-BB bijective LUTs derived from epoch_seed.  Encoding changes
///      at BB boundaries via RE_TABLE (encoding phi nodes, §3.6).
///
///   2. Global Memory Domain (cross-BB): values in guest memory and the
///      ORAM workspace are encoded with a fixed global bijection derived
///      from stored_seed.  This domain is invariant across BB transitions.
///
/// Mixing values from different domains silently corrupts data:
///   For independent random bijections f,g on {0..255},
///     Pr[f^{-1}(g(x)) = x] = 1/256 per byte lane
///   so Pr[full 8-lane match] = (1/256)^8 ≈ 2^{-64}.
///
/// This header provides phantom-typed wrappers `Encoded<Domain>` that
/// make domain mixing a compile-time error.  The ONLY way to convert
/// between domains is through explicit conversion functions that apply
/// the correct composed bijection (STORE_TABLE, LOAD_TABLE, RE_TABLE).
///
/// Runtime overhead: zero.  `Encoded<D>` has the same size, alignment,
/// and ABI as `uint64_t`.  The domain tag exists only in the type system.

#include <cstdint>
#include <type_traits>

namespace VMPilot::Common::VM {

// ─────────────────────────────────────────────────────────────────────────────
// Domain tags
// ─────────────────────────────────────────────────────────────────────────────

/// Encoding domain — phantom type tag, zero runtime cost.
enum class Domain : uint8_t {
    Register,     ///< Per-BB register encoding (E_BB, changes at epoch boundaries)
    Memory,       ///< Global memory encoding (E_G, fixed for entire execution)
    Plaintext,    ///< Unencoded native value (exists only transiently in Class C bridge)
};

// ─────────────────────────────────────────────────────────────────────────────
// Encoded<D> — the core phantom-typed value
// ─────────────────────────────────────────────────────────────────────────────

/// A 64-bit value encoded in domain D.
///
/// No implicit conversion between domains.  The only way to cross
/// a domain boundary is through the explicit conversion functions below.
///
/// Zero-overhead: same size and alignment as uint64_t.
template<Domain D>
struct Encoded {
    uint64_t bits;

    /// Default-initialise to zero (encoded zero).
    constexpr Encoded() noexcept : bits(0) {}

    /// Construct from raw bits.  Explicit to prevent accidental wrapping.
    constexpr explicit Encoded(uint64_t raw) noexcept : bits(raw) {}

    /// Access byte lane k ∈ [0,7] for composition table lookups.
    [[nodiscard]] constexpr uint8_t lane(unsigned k) const noexcept {
        return static_cast<uint8_t>(bits >> (k * 8));
    }

    /// Set byte lane k ∈ [0,7].
    constexpr void set_lane(unsigned k, uint8_t v) noexcept {
        bits = (bits & ~(uint64_t{0xFF} << (k * 8)))
             | (uint64_t{v} << (k * 8));
    }

    /// Same-domain equality (e.g. comparing two RegVal values).
    [[nodiscard]] constexpr bool operator==(Encoded other) const noexcept {
        return bits == other.bits;
    }
    [[nodiscard]] constexpr bool operator!=(Encoded other) const noexcept {
        return bits != other.bits;
    }
};

// Verify zero-overhead: same size as uint64_t.
static_assert(sizeof(Encoded<Domain::Register>)  == sizeof(uint64_t));
static_assert(sizeof(Encoded<Domain::Memory>)    == sizeof(uint64_t));
static_assert(sizeof(Encoded<Domain::Plaintext>) == sizeof(uint64_t));
static_assert(std::is_trivially_copyable_v<Encoded<Domain::Register>>);
static_assert(std::is_trivially_copyable_v<Encoded<Domain::Memory>>);
static_assert(std::is_trivially_copyable_v<Encoded<Domain::Plaintext>>);

/// Convenient type aliases used throughout the codebase.
using RegVal   = Encoded<Domain::Register>;
using MemVal   = Encoded<Domain::Memory>;
using PlainVal = Encoded<Domain::Plaintext>;

// ─────────────────────────────────────────────────────────────────────────────
// Byte-lane LUT types
//
// Thin wrappers around raw arrays to make conversion function signatures
// self-documenting.  These are non-owning views.
// ─────────────────────────────────────────────────────────────────────────────

/// 8-lane × 256-entry bijection table for a single register.
///
/// This is the fundamental encoding primitive.  Each lane k maps a byte
/// through an independent random bijection: out[k] = table[k][in[k]].
struct ByteLaneLUT {
    const uint8_t (*lanes)[256];  // points to uint8_t[8][256]

    /// Apply this LUT to a 64-bit value, lane by lane.
    [[nodiscard]] constexpr uint64_t apply(uint64_t val) const noexcept {
        uint64_t result = 0;
        for (unsigned k = 0; k < 8; ++k) {
            auto byte = static_cast<uint8_t>(val >> (k * 8));
            result |= uint64_t{lanes[k][byte]} << (k * 8);
        }
        return result;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
// Domain conversion functions
//
// These are the ONLY legal ways to cross domain boundaries.
// Each applies a composed bijection with no plaintext intermediate
// (except encode/decode to/from Plaintext, which is the acknowledged
// security boundary for Class C operations and native bridges).
// ─────────────────────────────────────────────────────────────────────────────

/// Register → Memory domain.  STORE_TABLE = E_G ∘ E_BB^{-1} (doc 15 §3.6).
/// No plaintext intermediate.
[[nodiscard]] inline MemVal store_convert(ByteLaneLUT store_tbl, RegVal v) noexcept {
    return MemVal{store_tbl.apply(v.bits)};
}

/// Memory → Register domain.  LOAD_TABLE = E_BB ∘ E_G^{-1} (doc 15 §3.6).
/// No plaintext intermediate.
[[nodiscard]] inline RegVal load_convert(ByteLaneLUT load_tbl, MemVal v) noexcept {
    return RegVal{load_tbl.apply(v.bits)};
}

/// Plaintext → Register domain.  encode = E_BB (per-register, per-lane).
/// Plaintext exists transiently in the caller (Class C, LOAD_CTX, initial regs).
[[nodiscard]] inline RegVal encode_register(ByteLaneLUT encode_tbl, PlainVal v) noexcept {
    return RegVal{encode_tbl.apply(v.bits)};
}

/// Register → Plaintext.  decode = E_BB^{-1} (per-register, per-lane).
/// Plaintext materialises transiently — only for Class C bridge operations
/// and final result extraction.
[[nodiscard]] inline PlainVal decode_register(ByteLaneLUT decode_tbl, RegVal v) noexcept {
    return PlainVal{decode_tbl.apply(v.bits)};
}

/// Plaintext → Memory domain.  encode = E_G (global, per-lane).
[[nodiscard]] inline MemVal encode_memory(ByteLaneLUT encode_tbl, PlainVal v) noexcept {
    return MemVal{encode_tbl.apply(v.bits)};
}

/// Memory → Plaintext.  decode = E_G^{-1} (global, per-lane).
[[nodiscard]] inline PlainVal decode_memory(ByteLaneLUT decode_tbl, MemVal v) noexcept {
    return PlainVal{decode_tbl.apply(v.bits)};
}

/// Re-encode across epoch boundary.  RE_TABLE = E_new ∘ E_old^{-1} (doc 15 §3.6).
/// No plaintext intermediate — maps directly between two register domains.
[[nodiscard]] inline RegVal reencode(ByteLaneLUT re_tbl, RegVal v) noexcept {
    return RegVal{re_tbl.apply(v.bits)};
}

// ─────────────────────────────────────────────────────────────────────────────
// Composition table result: same-domain RegVal → RegVal (Class A operations)
// ─────────────────────────────────────────────────────────────────────────────

/// Apply a binary composition table (Class A: AND, OR, XOR).
///
///   comp[k][a][b] = E_dst[k]( E_a^{-1}[k](a) OP E_b^{-1}[k](b) )
///
/// Input and output are all in Register Domain.  Zero plaintext.
[[nodiscard]] inline RegVal apply_binary_comp(
    const uint8_t comp[8][256][256],
    RegVal a, RegVal b) noexcept
{
    RegVal result;
    for (unsigned k = 0; k < 8; ++k) {
        result.set_lane(k, comp[k][a.lane(k)][b.lane(k)]);
    }
    return result;
}

/// Apply a unary composition table (Class A: NOT).
///
///   comp[k][a] = E_dst[k]( OP(E_src^{-1}[k](a)) )
///
/// Input and output are in Register Domain.  Zero plaintext.
[[nodiscard]] inline RegVal apply_unary_comp(
    const uint8_t comp[8][256],
    RegVal a) noexcept
{
    RegVal result;
    for (unsigned k = 0; k < 8; ++k) {
        result.set_lane(k, comp[k][a.lane(k)]);
    }
    return result;
}

/// Apply a byte-lane LUT to a RegVal, producing a RegVal (same domain).
/// Used for intra-BB operations where source and destination share encoding.
[[nodiscard]] inline RegVal apply_lut(ByteLaneLUT tbl, RegVal v) noexcept {
    return RegVal{tbl.apply(v.bits)};
}

}  // namespace VMPilot::Common::VM

#endif  // __COMMON_VM_ENCODED_VALUE_HPP__
