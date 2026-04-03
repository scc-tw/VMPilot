/// @file oram_strategies.cpp
/// @brief ORAM strategy implementations for the new VmOramState.

#include "oram_strategy.hpp"
#include "vm_state.hpp"

#include <vm/vm_crypto.hpp>
#include <vm/vm_context.hpp>  // VM_OBLIVIOUS_SIZE constants

#include <cstring>

namespace VMPilot::Runtime {

using namespace Common::VM;
using namespace Common::VM::Crypto;

// ─────────────────────────────────────────────────────────────────────────────
// DirectOram — simple indexed access (NOT oblivious)
// ─────────────────────────────────────────────────────────────────────────────

MemVal DirectOram::read(VmOramState& state, uint64_t offset) noexcept {
    if (offset + 8 > VM_OBLIVIOUS_SIZE) return MemVal(0);
    uint64_t val = 0;
    std::memcpy(&val, state.workspace + offset, 8);
    return MemVal(val);
}

void DirectOram::write(VmOramState& state, uint64_t offset, MemVal val) noexcept {
    if (offset + 8 > VM_OBLIVIOUS_SIZE) return;
    std::memcpy(state.workspace + offset, &val.bits, 8);
}

// ─────────────────────────────────────────────────────────────────────────────
// RollingKeyOram — full oblivious scan (IND-CPA)
// ─────────────────────────────────────────────────────────────────────────────

static uint64_t oram_access_impl(VmOramState& state, uint64_t addr,
                                  uint64_t write_value, bool is_write) noexcept {
    const uint32_t target_line   = static_cast<uint32_t>(addr / VM_ORAM_LINE_SIZE);
    const uint32_t target_word   = static_cast<uint32_t>((addr % VM_ORAM_LINE_SIZE) / 8);

    const uint64_t old_nonce = state.nonce;
    state.nonce++;  // always increment — indistinguishable read vs write

    uint64_t read_result = 0;

    for (uint32_t line = 0; line < VM_ORAM_NUM_LINES; ++line) {
        uint8_t* line_ptr = state.workspace + line * VM_ORAM_LINE_SIZE;

        // 1. Load 64 bytes
        uint64_t words[8];
        std::memcpy(words, line_ptr, 64);

        // 2. Decrypt with old nonce
        uint64_t old_ks[8];
        siphash_expand(state.key, old_nonce, line, old_ks);
        for (int i = 0; i < 8; ++i)
            words[i] ^= old_ks[i];

        // 3. Branchless CMOV operate
        const bool is_target_line = (line == target_line);
        for (uint32_t w = 0; w < 8; ++w) {
            uint64_t mask = -static_cast<uint64_t>(
                static_cast<uint64_t>(is_target_line) &
                static_cast<uint64_t>(w == target_word));

            if (is_write) {
                words[w] = (words[w] & ~mask) | (write_value & mask);
            } else {
                read_result |= (words[w] & mask);
            }
        }

        // 4. Re-encrypt with fresh nonce
        uint64_t new_ks[8];
        siphash_expand(state.key, state.nonce, line, new_ks);
        for (int i = 0; i < 8; ++i)
            words[i] ^= new_ks[i];

        // 5. Store back
        std::memcpy(line_ptr, words, 64);
    }

    return read_result;
}

MemVal RollingKeyOram::read(VmOramState& state, uint64_t offset) noexcept {
    return MemVal(oram_access_impl(state, offset, 0, false));
}

void RollingKeyOram::write(VmOramState& state, uint64_t offset, MemVal val) noexcept {
    oram_access_impl(state, offset, val.bits, true);
}

void RollingKeyOram::dummy_scan(VmOramState& state) noexcept {
    // Read-equivalent: full 64-line scan + re-encrypt + nonce bump.
    // Result discarded — the scan's purpose is timing normalization.
    (void)oram_access_impl(state, 0, 0, false);
}

}  // namespace VMPilot::Runtime
