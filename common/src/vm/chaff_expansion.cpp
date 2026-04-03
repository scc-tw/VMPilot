/// @file chaff_expansion.cpp
/// @brief Chaff NOP insertion for Doc 19 fixed-width superoperator.

#include <vm/chaff_expansion.hpp>

#include <cstdint>

namespace VMPilot::Common::VM {

// ─────────────────────────────────────────────────────────────────────────────
// SplitMix64 — fast deterministic PRNG for chaff field randomization
//
// WHY SplitMix64 (not BLAKE3 XOF):
//   Chaff field randomness is not a primary security boundary — the security
//   comes from SipHash encryption + Doc 17 ratchet entanglement.  The chaff
//   PRNG only needs to be unpredictable to an attacker who doesn't have the
//   seed (which is derived from stored_seed via BLAKE3).  SplitMix64 is
//   sufficient for this: it has full 64-bit period, excellent avalanche, and
//   is ~100× faster than BLAKE3 XOF per output word.
//
//   If the attacker has the seed, they can predict chaff fields — but they
//   can also decrypt all instructions, so chaff prediction adds nothing.
// ─────────────────────────────────────────────────────────────────────────────

static uint64_t splitmix64(uint64_t& state) noexcept {
    state += UINT64_C(0x9e3779b97f4a7c15);
    uint64_t z = state;
    z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
    return z ^ (z >> 31);
}

/// Generate one chaff NOP instruction with randomized fields.
static BuilderInstruction make_chaff_nop(uint64_t& rng) noexcept {
    uint64_t r = splitmix64(rng);
    BuilderInstruction nop{};
    nop.opcode = VmOpcode::NOP;
    // Random operand types (REG, REG) for ghost reads in enhanced NOP handler
    nop.flags = static_cast<uint8_t>(
        (VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4)
      | (r & 0x0F));  // random condition bits (ignored by NOP, diversifies encoding)
    nop.reg_a = static_cast<uint8_t>((r >> 8) & 0x0F);
    nop.reg_b = static_cast<uint8_t>((r >> 12) & 0x0F);
    // Random aux — this is the most important field for ratchet diversification.
    // Each chaff NOP's aux contributes unique entropy to the enc_state chain
    // via ST_{j+1} = SipHash(ST_j, full_plaintext_insn).  Different aux →
    // different instruction word → different next state.
    nop.aux = static_cast<uint32_t>(r >> 16);
    return nop;
}

// ─────────────────────────────────────────────────────────────────────────────
// expand_to_fixed_width
// ─────────────────────────────────────────────────────────────────────────────

void expand_to_fixed_width(std::vector<BuilderInstruction>& insns,
                           uint8_t N, uint64_t rng_seed) noexcept {
    if (N <= 1) return;  // N=1 (DebugPolicy): no expansion needed

    const size_t original_count = insns.size();
    std::vector<BuilderInstruction> expanded;
    expanded.reserve(original_count * N);

    uint64_t rng = rng_seed;

    for (size_t i = 0; i < original_count; ++i) {
        // Real instruction
        expanded.push_back(insns[i]);
        // N-1 chaff NOPs
        for (uint8_t j = 1; j < N; ++j)
            expanded.push_back(make_chaff_nop(rng));
    }

    insns = std::move(expanded);
}

// ─────────────────────────────────────────────────────────────────────────────
// pad_bb_dispatch_units
// ─────────────────────────────────────────────────────────────────────────────

void pad_bb_dispatch_units(std::vector<BuilderInstruction>& insns,
                           uint8_t N, uint8_t Q_D,
                           uint64_t rng_seed) noexcept {
    if (N == 0 || Q_D == 0) return;

    const uint32_t unit = static_cast<uint32_t>(N) * Q_D;
    const size_t current = insns.size();
    const size_t target = ((current + unit - 1) / unit) * unit;

    uint64_t rng = rng_seed;

    for (size_t i = current; i < target; ++i)
        insns.push_back(make_chaff_nop(rng));
}

}  // namespace VMPilot::Common::VM
