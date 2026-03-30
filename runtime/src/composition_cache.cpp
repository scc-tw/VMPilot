/// @file composition_cache.cpp
/// @brief Implementation of the composition table cache.
///
/// Class A security guarantee: tables are built by enumerating all
/// 256x256 (binary) or 256 (unary) input combinations. During instruction
/// execution, only encoded-domain lookups occur -- no plaintext appears
/// in any CPU register.

#include <composition_cache.hpp>
#include <vm/vm_encoding.hpp>

#include <cstring>

namespace VMPilot::Runtime {

using namespace Common::VM;
using namespace Common::VM::Encoding;

// ---------------------------------------------------------------------------
// clear
// ---------------------------------------------------------------------------

void CompositionCache::clear() noexcept {
    for (size_t i = 0; i < MAX_BINARY; ++i)
        binary_[i].valid = false;
    binary_count_ = 0;

    for (size_t i = 0; i < MAX_UNARY; ++i)
        unary_[i].valid = false;
    unary_count_ = 0;

    for (size_t i = 0; i < MAX_MBA; ++i)
        mba_[i].valid = false;
    mba_count_ = 0;
}

// ---------------------------------------------------------------------------
// get_binary
// ---------------------------------------------------------------------------

const uint8_t (*CompositionCache::get_binary(
    uint8_t op_id, uint8_t dst, uint8_t src_a, uint8_t src_b,
    uint8_t (*op_fn)(uint8_t, uint8_t),
    const VMContext& ctx))[256][256]
{
    BinaryKey key{op_id, dst, src_a, src_b};

    // Linear search for existing entry
    for (size_t i = 0; i < binary_count_; ++i) {
        if (binary_[i].valid && binary_[i].key == key)
            return binary_[i].table;
    }

    // Build new entry; evict oldest if full
    size_t slot = binary_count_;
    if (slot >= MAX_BINARY) slot = MAX_BINARY - 1;  // reuse last slot
    else ++binary_count_;

    binary_[slot].key = key;
    build_composition_table(
        op_fn,
        ctx.reg_encode[dst],
        ctx.reg_decode[src_a],
        ctx.reg_decode[src_b],
        binary_[slot].table);
    binary_[slot].valid = true;

    return binary_[slot].table;
}

// ---------------------------------------------------------------------------
// get_unary
// ---------------------------------------------------------------------------

const uint8_t (*CompositionCache::get_unary(
    uint8_t op_id, uint8_t dst, uint8_t src,
    uint8_t (*op_fn)(uint8_t),
    const VMContext& ctx))[256]
{
    UnaryKey key{op_id, dst, src};

    for (size_t i = 0; i < unary_count_; ++i) {
        if (unary_[i].valid && unary_[i].key == key)
            return unary_[i].table;
    }

    size_t slot = unary_count_;
    if (slot >= MAX_UNARY) slot = MAX_UNARY - 1;
    else ++unary_count_;

    unary_[slot].key = key;
    build_unary_composition_table(
        op_fn,
        ctx.reg_encode[dst],
        ctx.reg_decode[src],
        unary_[slot].table);
    unary_[slot].valid = true;

    return unary_[slot].table;
}

// ---------------------------------------------------------------------------
// get_mba
// ---------------------------------------------------------------------------

const CompositionCache::MbaEntry& CompositionCache::get_mba(
    uint8_t dst, uint8_t src,
    const VMContext& ctx)
{
    MbaKey key{dst, src};

    for (size_t i = 0; i < mba_count_; ++i) {
        if (mba_[i].valid && mba_[i].key == key)
            return mba_[i];
    }

    size_t slot = mba_count_;
    if (slot >= MAX_MBA) slot = MAX_MBA - 1;
    else ++mba_count_;

    mba_[slot].key = key;

    // Derive MBA tables:
    //   SHL1_INTRA[k][x]    = encode[dst][k][ (decode[src][k][x] << 1) & 0xFF ]
    //   CARRY_EXTRACT[k][x] = (decode[src][k][x] >> 7) & 1
    //   INJECT_0[k][x]      = encode[dst][k][ decode[dst][k][x] & 0xFE ]
    //   INJECT_1[k][x]      = encode[dst][k][ decode[dst][k][x] | 0x01 ]
    for (int k = 0; k < 8; ++k) {
        for (int x = 0; x < 256; ++x) {
            uint8_t plain_src = ctx.reg_decode[src][k][x];
            mba_[slot].shl1_intra[k][x] =
                ctx.reg_encode[dst][k][static_cast<uint8_t>((plain_src << 1) & 0xFF)];
            mba_[slot].carry_extract[k][x] =
                static_cast<uint8_t>((plain_src >> 7) & 1);

            uint8_t plain_dst = ctx.reg_decode[dst][k][x];
            mba_[slot].inject_0[k][x] =
                ctx.reg_encode[dst][k][static_cast<uint8_t>(plain_dst & 0xFE)];
            mba_[slot].inject_1[k][x] =
                ctx.reg_encode[dst][k][static_cast<uint8_t>(plain_dst | 0x01)];
        }
    }
    mba_[slot].valid = true;

    return mba_[slot];
}

}  // namespace VMPilot::Runtime
