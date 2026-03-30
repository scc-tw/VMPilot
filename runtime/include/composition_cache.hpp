#pragma once
#ifndef __RUNTIME_COMPOSITION_CACHE_HPP__
#define __RUNTIME_COMPOSITION_CACHE_HPP__

/// @file composition_cache.hpp
/// @brief Cache for lazily-built Class A composition tables and MBA tables.
///
/// Why lazy build preserves Class A "zero plaintext":
///   Tables are built by iterating ALL 256x256 input combinations --
///   a generic precomputation independent of actual register data.
///   During instruction execution, a single comp[k][a][b] lookup
///   maps (encoded_a, encoded_b) -> encoded_result with no plaintext.
///
/// Cleared on every BB transition (encoding tables change per-epoch).

#include <vm/vm_context.hpp>

#include <cstdint>
#include <cstring>

namespace VMPilot::Runtime {

class CompositionCache {
public:
    struct BinaryKey {
        uint8_t op;      ///< operation identifier
        uint8_t dst;     ///< destination register
        uint8_t src_a;   ///< source register a
        uint8_t src_b;   ///< source register b
        bool operator==(const BinaryKey& o) const noexcept {
            return op == o.op && dst == o.dst
                && src_a == o.src_a && src_b == o.src_b;
        }
    };

    struct UnaryKey {
        uint8_t op;
        uint8_t dst;
        uint8_t src;
        bool operator==(const UnaryKey& o) const noexcept {
            return op == o.op && dst == o.dst && src == o.src;
        }
    };

    struct MbaKey {
        uint8_t dst;
        uint8_t src;
        bool operator==(const MbaKey& o) const noexcept {
            return dst == o.dst && src == o.src;
        }
    };

    struct MbaEntry {
        MbaKey key{};
        uint8_t shl1_intra[8][256];     ///< intra-lane shift left by 1
        uint8_t carry_extract[8][256];   ///< extract carry bit (bit 7)
        uint8_t inject_0[8][256];        ///< clear bit 0 of encoded value
        uint8_t inject_1[8][256];        ///< set bit 0 of encoded value
        bool valid = false;
    };

    /// Clear all cached tables. Called on every enter_basic_block().
    void clear() noexcept;

    /// Get or build a binary composition table (XOR, AND, OR).
    /// Returns pointer to comp[8][256][256].
    const uint8_t (*get_binary(
        uint8_t op_id, uint8_t dst, uint8_t src_a, uint8_t src_b,
        uint8_t (*op_fn)(uint8_t, uint8_t),
        const Common::VM::VMContext& ctx))[256][256];

    /// Get or build a unary composition table (NOT).
    /// Returns pointer to comp[8][256].
    const uint8_t (*get_unary(
        uint8_t op_id, uint8_t dst, uint8_t src,
        uint8_t (*op_fn)(uint8_t),
        const Common::VM::VMContext& ctx))[256];

    /// Get or build MBA tables for a (dst, src) register pair.
    const MbaEntry& get_mba(
        uint8_t dst, uint8_t src,
        const Common::VM::VMContext& ctx);

private:
    static constexpr size_t MAX_BINARY = 16;
    static constexpr size_t MAX_UNARY  = 8;
    static constexpr size_t MAX_MBA    = 8;

    struct BinaryEntry {
        BinaryKey key{};
        uint8_t table[8][256][256];  // 512KB
        bool valid = false;
    };

    struct UnaryEntry {
        UnaryKey key{};
        uint8_t table[8][256];  // 2KB
        bool valid = false;
    };

    BinaryEntry binary_[MAX_BINARY];
    size_t binary_count_ = 0;
    UnaryEntry unary_[MAX_UNARY];
    size_t unary_count_ = 0;
    MbaEntry mba_[MAX_MBA];
    size_t mba_count_ = 0;
};

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_COMPOSITION_CACHE_HPP__
