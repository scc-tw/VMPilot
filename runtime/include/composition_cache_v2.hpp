#pragma once
#ifndef __RUNTIME_COMPOSITION_CACHE_V2_HPP__
#define __RUNTIME_COMPOSITION_CACHE_V2_HPP__

/// @file composition_cache_v2.hpp
/// @brief Composition table cache for the new VmEpoch-based architecture.
///
/// Functionally identical to composition_cache.hpp but takes RegTables
/// instead of VMContext for table lookups.  This decouples the cache
/// from the monolithic VMContext.

#include "vm_state.hpp"
#include <vm/vm_encoding.hpp>

#include <cstdint>
#include <cstring>

namespace VMPilot::Runtime {

/// Lazily-built composition tables for Class A (bitwise) and Class B (MBA) operations.
///
/// Owned by VmEpoch, allocated on heap.  Cleared on every BB transition
/// because encoding tables change per-epoch.
struct CompositionCache {

    struct BinaryKey {
        uint8_t op, dst, src_a, src_b;
        bool operator==(const BinaryKey& o) const noexcept {
            return op == o.op && dst == o.dst && src_a == o.src_a && src_b == o.src_b;
        }
    };

    struct MbaEntry {
        uint8_t dst, src;
        uint8_t shl1_intra[8][256];
        uint8_t carry_extract[8][256];
        uint8_t inject_0[8][256];
        uint8_t inject_1[8][256];
        bool valid = false;
    };

    // ── Binary composition (AND, OR, XOR) ───────────────────────────────

    /// Get or build a binary composition table for (op, dst, src_a, src_b).
    /// Returns pointer to comp[8][256][256] (512KB).
    const uint8_t (*get_binary(
        uint8_t op_id, uint8_t dst, uint8_t src_a, uint8_t src_b,
        uint8_t (*op_fn)(uint8_t, uint8_t),
        const RegTables& tables))[256][256]
    {
        BinaryKey key{op_id, dst, src_a, src_b};
        for (size_t i = 0; i < binary_count_; ++i) {
            if (binary_[i].valid && binary_[i].key == key)
                return binary_[i].table;
        }
        // Build new entry
        size_t idx = binary_count_ < MAX_BINARY ? binary_count_++ : 0;
        auto& e = binary_[idx];
        e.key = key;
        Common::VM::Encoding::build_composition_table(
            op_fn, tables.encode[dst], tables.decode[src_a], tables.decode[src_b],
            e.table);
        e.valid = true;
        return e.table;
    }

    // ── Unary composition (NOT) ─────────────────────────────────────────

    const uint8_t (*get_unary(
        uint8_t op_id, uint8_t dst, uint8_t src,
        uint8_t (*op_fn)(uint8_t),
        const RegTables& tables))[256]
    {
        for (size_t i = 0; i < unary_count_; ++i) {
            if (unary_[i].valid && unary_[i].op == op_id
                && unary_[i].dst == dst && unary_[i].src == src)
                return unary_[i].table;
        }
        size_t idx = unary_count_ < MAX_UNARY ? unary_count_++ : 0;
        auto& e = unary_[idx];
        e.op = op_id; e.dst = dst; e.src = src;
        Common::VM::Encoding::build_unary_composition_table(
            op_fn, tables.encode[dst], tables.decode[src], e.table);
        e.valid = true;
        return e.table;
    }

    // ── MBA tables (SHL1 for ADD/SUB/NEG) ───────────────────────────────

    const MbaEntry& get_mba(uint8_t dst, uint8_t src, const RegTables& tables) {
        for (size_t i = 0; i < mba_count_; ++i) {
            if (mba_[i].valid && mba_[i].dst == dst && mba_[i].src == src)
                return mba_[i];
        }
        size_t idx = mba_count_ < MAX_MBA ? mba_count_++ : 0;
        auto& e = mba_[idx];
        e.dst = dst; e.src = src;
        // Build MBA tables for cross-lane carry propagation
        for (int k = 0; k < 8; ++k) {
            for (int x = 0; x < 256; ++x) {
                uint8_t plain = tables.decode[src][k][x];
                e.shl1_intra[k][x] = tables.encode[dst][k][(plain << 1) & 0xFF];
                e.carry_extract[k][x] = (plain >> 7) & 1;
                uint8_t plain_dst = tables.decode[dst][k][x];
                e.inject_0[k][x] = tables.encode[dst][k][plain_dst & 0xFE];
                e.inject_1[k][x] = tables.encode[dst][k][plain_dst | 0x01];
            }
        }
        e.valid = true;
        return e;
    }

    void clear() noexcept {
        binary_count_ = unary_count_ = mba_count_ = 0;
        for (auto& e : binary_) e.valid = false;
        for (auto& e : unary_) e.valid = false;
        for (auto& e : mba_) e.valid = false;
    }

private:
    static constexpr size_t MAX_BINARY = 16;
    static constexpr size_t MAX_UNARY  = 8;
    static constexpr size_t MAX_MBA    = 8;

    struct BinaryEntry {
        BinaryKey key{};
        uint8_t table[8][256][256];
        bool valid = false;
    };

    struct UnaryEntry {
        uint8_t op = 0, dst = 0, src = 0;
        uint8_t table[8][256];
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

#endif  // __RUNTIME_COMPOSITION_CACHE_V2_HPP__
