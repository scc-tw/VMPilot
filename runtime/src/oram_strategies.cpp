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

/// Core ORAM scan — fully branchless on is_write.
///
/// WHY branchless is_write:
///   The original implementation branched on is_write inside the 64-line
///   inner loop.  An attacker with EM/DPA access could distinguish read
///   from write scans via the branch predictor's micro-architectural state.
///   The branchless version always performs BOTH the read accumulation AND
///   the conditional write, then selects via bitmask which word value to
///   keep.  The data-independent execution trace makes read and write
///   scans indistinguishable.
static uint64_t oram_access_impl(VmOramState& state, uint64_t addr,
                                  uint64_t write_value, bool is_write) noexcept {
    const uint32_t target_line   = static_cast<uint32_t>(addr / VM_ORAM_LINE_SIZE);
    const uint32_t target_word   = static_cast<uint32_t>((addr % VM_ORAM_LINE_SIZE) / 8);

    const uint64_t old_nonce = state.nonce;
    state.nonce++;  // always increment — indistinguishable read vs write

    const uint64_t w_sel = -static_cast<uint64_t>(is_write);  // all-1s if write, all-0s if read

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

        // 3. Branchless read + conditional write
        const bool is_target_line = (line == target_line);
        for (uint32_t w = 0; w < 8; ++w) {
            uint64_t mask = -static_cast<uint64_t>(
                static_cast<uint64_t>(is_target_line) &
                static_cast<uint64_t>(w == target_word));

            // Always accumulate read result (masked to target word only)
            read_result |= (words[w] & mask);

            // Compute written word (target word replaced with write_value)
            uint64_t written = (words[w] & ~mask) | (write_value & mask);

            // Branchless select: if is_write, use written; else keep original
            words[w] = (written & w_sel) | (words[w] & ~w_sel);
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

uint64_t RollingKeyOram::access(VmOramState& state, uint64_t addr,
                                uint64_t write_value, bool is_write) noexcept {
    return oram_access_impl(state, addr, write_value, is_write);
}

MemVal RollingKeyOram::read(VmOramState& state, uint64_t offset) noexcept {
    return MemVal(oram_access_impl(state, offset, 0, false));
}

void RollingKeyOram::write(VmOramState& state, uint64_t offset, MemVal val) noexcept {
    oram_access_impl(state, offset, val.bits, true);
}

void RollingKeyOram::dummy_scan(VmOramState& state) noexcept {
    (void)oram_access_impl(state, 0, 0, false);
}

uint64_t DirectOram::access(VmOramState& state, uint64_t addr,
                             uint64_t write_value, bool is_write) noexcept {
    // Direct indexed — no oblivious scan.  Branchless read + conditional write.
    uint64_t val = 0;
    if (addr + 8 <= VM_OBLIVIOUS_SIZE) {
        std::memcpy(&val, state.workspace + addr, 8);
        if (is_write)
            std::memcpy(state.workspace + addr, &write_value, 8);
    }
    return val;
}

}  // namespace VMPilot::Runtime
