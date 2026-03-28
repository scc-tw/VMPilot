#include <vm/vm_encoding.hpp>

#include <blake3.h>

#include <cstring>
#include <numeric>

namespace VMPilot::Common::VM::Encoding {

// ─────────────────────────────────────────────────────────────────────────────
// Bijection primitives
// ─────────────────────────────────────────────────────────────────────────────

void generate_bijection(const uint8_t seed[32],
                        uint8_t table[256]) noexcept {
    // Identity permutation
    std::iota(table, table + 256, static_cast<uint8_t>(0));

    // Expand seed into 510 bytes of pseudorandom data via BLAKE3-XOF.
    // Fisher-Yates needs 255 swaps; we use 2 bytes per swap for reduced
    // modulo bias (bias < 256/65536 ≈ 0.4%, negligible for MCSP).
    uint8_t rand_bytes[512];
    blake3_hasher hasher;
    blake3_hasher_init_keyed(&hasher, seed);
    const char context[] = "bijection";
    blake3_hasher_update(&hasher, context, sizeof(context) - 1);
    blake3_hasher_finalize(&hasher, rand_bytes, sizeof(rand_bytes));

    // Fisher-Yates shuffle (Knuth TAOCP Vol 2 §3.4.2):
    // the unique O(n) algorithm that produces a uniform random permutation
    // from S_n when given uniform random swap indices.
    size_t ri = 0;
    for (int i = 255; i > 0; --i) {
        uint16_t r = (static_cast<uint16_t>(rand_bytes[ri]) << 8)
                   | static_cast<uint16_t>(rand_bytes[ri + 1]);
        ri += 2;
        uint8_t j = static_cast<uint8_t>(r % static_cast<uint16_t>(i + 1));

        uint8_t tmp = table[i];
        table[i] = table[j];
        table[j] = tmp;
    }
}

void invert_bijection(const uint8_t table[256],
                      uint8_t inverse[256]) noexcept {
    for (int i = 0; i < 256; ++i) {
        inverse[table[i]] = static_cast<uint8_t>(i);
    }
}

void compose_bijections(const uint8_t outer[256],
                        const uint8_t inner[256],
                        uint8_t result[256]) noexcept {
    for (int i = 0; i < 256; ++i) {
        result[i] = outer[inner[i]];
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Internal: derive a single byte-lane bijection from (parent_seed, r, k).
// ─────────────────────────────────────────────────────────────────────────────

namespace {

void derive_lane_seed(const uint8_t parent_seed[32],
                      uint8_t reg_index, uint8_t lane_index,
                      uint8_t out_seed[32]) noexcept {
    blake3_hasher hasher;
    blake3_hasher_init_keyed(&hasher, parent_seed);
    uint8_t input[2] = { reg_index, lane_index };
    blake3_hasher_update(&hasher, input, 2);
    blake3_hasher_finalize(&hasher, out_seed, 32);
}

}  // anonymous namespace

// ─────────────────────────────────────────────────────────────────────────────
// Per-BB register encoding
// ─────────────────────────────────────────────────────────────────────────────

void derive_register_tables(const uint8_t epoch_seed[32],
                            uint16_t live_regs_bitmap,
                            uint8_t encode[16][8][256],
                            uint8_t decode[16][8][256]) noexcept {
    for (uint8_t r = 0; r < 16; ++r) {
        if ((live_regs_bitmap & (1u << r)) == 0) continue;

        for (uint8_t k = 0; k < 8; ++k) {
            uint8_t lane_seed[32];
            derive_lane_seed(epoch_seed, r, k, lane_seed);
            generate_bijection(lane_seed, encode[r][k]);
            invert_bijection(encode[r][k], decode[r][k]);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Global memory encoding
// ─────────────────────────────────────────────────────────────────────────────

void derive_memory_tables(const uint8_t stored_seed[32],
                          uint8_t mem_encode[8][256],
                          uint8_t mem_decode[8][256]) noexcept {
    // mem_seed = BLAKE3(stored_seed, "mem")
    uint8_t mem_seed[32];
    blake3_hasher hasher;
    blake3_hasher_init_keyed(&hasher, stored_seed);
    const char ctx[] = "mem";
    blake3_hasher_update(&hasher, ctx, sizeof(ctx) - 1);
    blake3_hasher_finalize(&hasher, mem_seed, 32);

    for (uint8_t k = 0; k < 8; ++k) {
        uint8_t lane_seed[32];
        // Reuse derive_lane_seed with reg_index=0 but from mem_seed.
        // This is equivalent to BLAKE3(mem_seed, 0, k).
        derive_lane_seed(mem_seed, 0, k, lane_seed);
        generate_bijection(lane_seed, mem_encode[k]);
        invert_bijection(mem_encode[k], mem_decode[k]);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Domain conversion tables
// ─────────────────────────────────────────────────────────────────────────────

void derive_store_load_tables(
    const uint8_t reg_encode[16][8][256],
    const uint8_t reg_decode[16][8][256],
    const uint8_t mem_encode[8][256],
    const uint8_t mem_decode[8][256],
    uint16_t live_regs_bitmap,
    uint8_t store_tables[16][8][256],
    uint8_t load_tables[16][8][256]) noexcept {

    for (uint8_t r = 0; r < 16; ++r) {
        if ((live_regs_bitmap & (1u << r)) == 0) continue;

        for (uint8_t k = 0; k < 8; ++k) {
            // STORE = mem_encode ∘ reg_decode  (register → memory domain)
            compose_bijections(mem_encode[k], reg_decode[r][k],
                               store_tables[r][k]);
            // LOAD  = reg_encode ∘ mem_decode  (memory → register domain)
            compose_bijections(reg_encode[r][k], mem_decode[k],
                               load_tables[r][k]);
        }
    }
}

void derive_re_tables(
    const uint8_t decode_old[16][8][256],
    const uint8_t encode_new[16][8][256],
    uint16_t live_regs_bitmap,
    uint8_t re_tables[16][8][256]) noexcept {

    for (uint8_t r = 0; r < 16; ++r) {
        if ((live_regs_bitmap & (1u << r)) == 0) continue;

        for (uint8_t k = 0; k < 8; ++k) {
            // RE = encode_new ∘ decode_old  →  E_new(E_old^{-1}(x))
            compose_bijections(encode_new[r][k], decode_old[r][k],
                               re_tables[r][k]);
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Class A composition tables
// ─────────────────────────────────────────────────────────────────────────────

void build_composition_table(
    uint8_t (*op_fn)(uint8_t, uint8_t),
    const uint8_t encode_dst[8][256],
    const uint8_t decode_src_a[8][256],
    const uint8_t decode_src_b[8][256],
    uint8_t comp[8][256][256]) noexcept {

    for (uint8_t k = 0; k < 8; ++k) {
        for (int a = 0; a < 256; ++a) {
            uint8_t plain_a = decode_src_a[k][a];
            for (int b = 0; b < 256; ++b) {
                uint8_t plain_b = decode_src_b[k][b];
                uint8_t result = op_fn(plain_a, plain_b);
                comp[k][a][b] = encode_dst[k][result];
            }
        }
    }
}

void build_unary_composition_table(
    uint8_t (*op_fn)(uint8_t),
    const uint8_t encode_dst[8][256],
    const uint8_t decode_src[8][256],
    uint8_t comp[8][256]) noexcept {

    for (uint8_t k = 0; k < 8; ++k) {
        for (int a = 0; a < 256; ++a) {
            uint8_t plain = decode_src[k][a];
            comp[k][a] = encode_dst[k][op_fn(plain)];
        }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Opcode permutation (D4)
// ─────────────────────────────────────────────────────────────────────────────

void derive_opcode_permutation(const uint8_t epoch_seed[32],
                               uint8_t perm[256],
                               uint8_t perm_inv[256]) noexcept {
    // perm_seed = BLAKE3(epoch_seed, "perm")
    uint8_t perm_seed[32];
    blake3_hasher hasher;
    blake3_hasher_init_keyed(&hasher, epoch_seed);
    const char ctx[] = "perm";
    blake3_hasher_update(&hasher, ctx, sizeof(ctx) - 1);
    blake3_hasher_finalize(&hasher, perm_seed, 32);

    generate_bijection(perm_seed, perm);
    invert_bijection(perm, perm_inv);
}

}  // namespace VMPilot::Common::VM::Encoding
