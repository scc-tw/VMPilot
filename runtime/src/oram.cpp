#include <oram.hpp>
#include <vm/vm_crypto.hpp>

#include <cstring>

namespace VMPilot::Runtime {

using Common::DiagnosticCode;
using Common::VM::VMContext;
using Common::VM::VM_OBLIVIOUS_SIZE;
using Common::VM::VM_ORAM_LINE_SIZE;
using Common::VM::VM_ORAM_NUM_LINES;
using Common::VM::Crypto::siphash_expand;

// ---------------------------------------------------------------------------
// Internal: scan all 64 cache lines with branchless CMOV masking.
//
// Both read and write perform the same access pattern to maintain the
// oblivious invariant.  Every access increments oram_nonce and re-encrypts
// all lines with the fresh nonce, so an observer cannot distinguish reads
// from writes by watching workspace changes.
// ---------------------------------------------------------------------------

static tl::expected<uint64_t, DiagnosticCode>
oram_access(VMContext& ctx, uint32_t addr, uint64_t write_value, bool is_write) noexcept {
    // Validation
    if (addr % 8 != 0)
        return tl::make_unexpected(DiagnosticCode::OramAddressUnaligned);
    if (addr >= VM_OBLIVIOUS_SIZE)
        return tl::make_unexpected(DiagnosticCode::OramAddressOutOfBounds);

    const uint32_t target_line   = addr / VM_ORAM_LINE_SIZE;    // 0..63
    const uint32_t target_offset = addr % VM_ORAM_LINE_SIZE;    // byte offset within line
    const uint32_t target_word   = target_offset / 8;           // word index within line

    const uint64_t old_nonce = ctx.oram_nonce;
    ctx.oram_nonce++;  // always increment — indistinguishable read vs write

    uint64_t read_result = 0;

    for (uint32_t line = 0; line < VM_ORAM_NUM_LINES; ++line) {
        uint8_t* line_ptr = ctx.oblivious_workspace + line * VM_ORAM_LINE_SIZE;

        // 1. Load 64 bytes
        uint64_t words[8];
        std::memcpy(words, line_ptr, 64);

        // 2. Decrypt with old nonce
        uint64_t old_ks[8];
        siphash_expand(ctx.oram_key, old_nonce, line, old_ks);
        for (int i = 0; i < 8; ++i)
            words[i] ^= old_ks[i];

        // 3. Branchless CMOV operate
        const bool is_target_line = (line == target_line);

        for (uint32_t w = 0; w < 8; ++w) {
            // mask is all-1s if this is the exact target word, else all-0s
            uint64_t mask = -static_cast<uint64_t>(
                static_cast<uint64_t>(is_target_line) &
                static_cast<uint64_t>(w == target_word));

            if (is_write) {
                // Replace target word with write_value, leave others unchanged
                words[w] = (words[w] & ~mask) | (write_value & mask);
            } else {
                // Extract target word into read_result
                read_result |= (words[w] & mask);
            }
        }

        // 4. Re-encrypt with fresh nonce
        uint64_t new_ks[8];
        siphash_expand(ctx.oram_key, ctx.oram_nonce, line, new_ks);
        for (int i = 0; i < 8; ++i)
            words[i] ^= new_ks[i];

        // 5. Store back
        std::memcpy(line_ptr, words, 64);
    }

    return read_result;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

tl::expected<uint64_t, DiagnosticCode>
oram_read(VMContext& ctx, uint32_t offset) noexcept {
    return oram_access(ctx, offset, 0, false);
}

tl::expected<void, DiagnosticCode>
oram_write(VMContext& ctx, uint32_t offset, uint64_t value) noexcept {
    auto result = oram_access(ctx, offset, value, true);
    if (!result)
        return tl::make_unexpected(result.error());
    return {};
}

tl::expected<void, DiagnosticCode>
oram_push(VMContext& ctx, uint64_t value) noexcept {
    if (ctx.vm_sp < 8)
        return tl::make_unexpected(DiagnosticCode::StackOverflow);
    ctx.vm_sp -= 8;
    return oram_write(ctx, static_cast<uint32_t>(ctx.vm_sp), value);
}

tl::expected<uint64_t, DiagnosticCode>
oram_pop(VMContext& ctx) noexcept {
    if (ctx.vm_sp >= VM_OBLIVIOUS_SIZE)
        return tl::make_unexpected(DiagnosticCode::StackUnderflow);
    auto val = oram_read(ctx, static_cast<uint32_t>(ctx.vm_sp));
    if (!val)
        return val;
    ctx.vm_sp += 8;
    return val;
}

}  // namespace VMPilot::Runtime
