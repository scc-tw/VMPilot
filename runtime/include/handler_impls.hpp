#pragma once
#ifndef __RUNTIME_HANDLER_IMPLS_HPP__
#define __RUNTIME_HANDLER_IMPLS_HPP__

/// @file handler_impls.hpp
/// @brief ALL 55 HandlerTraits specializations — doc 16 forward-secrecy architecture.
///
/// Architecture change from doc 15 to doc 16:
///
///   Old (doc 15): Handlers receive encoded RegVal operands and must decode/encode
///   internally.  Class A handlers use composition tables, Class B uses MBA
///   decomposition, Class C bridges to plaintext.  Per-policy specializations
///   for ADD/SUB/NEG.  CompositionCache (~8MB) allocated per-epoch.
///
///   New (doc 16): The step() pipeline FPE-decodes operands BEFORE the handler
///   call (plain_a, plain_b in DecodedInsn) and FPE-encodes the destination
///   register AFTER the handler returns.  Handlers operate exclusively on
///   plaintext values.  The A/B/C classification is now moot — all handlers
///   are uniform.  CompositionCache, MBA tables, and composition helpers are
///   eliminated entirely.
///
/// Security argument: plaintext exists in CPU registers for the duration of
/// handler execution (~10-50ns).  The pipeline's FPE key ratchet (Speck64/XEX)
/// re-encodes ALL 16 registers after every instruction, providing forward
/// secrecy — a memory snapshot after instruction N reveals nothing about
/// register values before instruction N.
///
/// Handler contract:
///   - Receives pre-decoded i.plain_a, i.plain_b (pipeline did FPE_Decode)
///   - Computes on plaintext, writes result: e.regs[dst] = RegVal(result)
///   - The value is temporarily plaintext in regs[] for ~nanoseconds
///   - Pipeline immediately FPE_Encodes regs[dst] after handler returns
///   - Then key ratchet re-encodes all 16 registers
///
/// Memory domain:
///   STORE/PUSH: plaintext -> im.mem.encode_lut() -> MemVal for guest/ORAM
///   LOAD/POP:   MemVal from guest/ORAM -> im.mem.decode_lut() -> plaintext

#include "handler_traits.hpp"
#include "eh_guard.hpp"
#include "platform_call.hpp"

#include <vm/encoded_value.hpp>
#include <vm/secure_zero.hpp>
#include <vm/xex_speck64.hpp>

#include <atomic>
#include <cstring>

namespace VMPilot::Runtime {

using Common::VM::VM_REG_COUNT;
using Common::VM::VM_MAX_NESTING;
using Common::VM::VM_OBLIVIOUS_SIZE;
using Common::VM::VM_OPERAND_POOL;
using Common::VM::VM_OPERAND_REG;
using Common::VM::VM_OPERAND_MEM;
using Common::VM::MemVal;
using Common::VM::PlainVal;

// ---------------------------------------------------------------------------
// Internal helpers (encoding-free)
// ---------------------------------------------------------------------------

namespace detail {

/// Branchless condition evaluation — precomputes all 10 conditions, selects via
/// lookup table indexed by cond.  Constant-time: all conditions are
/// always evaluated regardless of which one is actually needed.
bool evaluate_condition_ct(uint8_t flags, uint8_t cond) noexcept;

}  // namespace detail

// ===========================================================================
// Cat 0: Data Movement (8 opcodes)
// ===========================================================================

/// MOVE: dst = src.
/// Pipeline decoded src into plain_b; we store it as the new dst value.
/// Pipeline will FPE-encode regs[dst] afterward.
template<typename P>
struct HandlerTraits<VmOpcode::MOVE, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(i.plain_b);
        return {};
    }
};

/// LOAD: guest memory -> register.
/// Read raw bytes from guest address, decode from global memory domain to
/// plaintext.  Pipeline will FPE-encode the result into regs[dst].
template<typename P>
struct HandlerTraits<VmOpcode::LOAD, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable& im, const DecodedInsn& i) noexcept {
        // SECURITY NOTE: There are intentionally no bounds checks here.
        // VMPilot's threat model assumes it is obfuscating trusted C++ code
        // (Man-At-The-End attack model), not sandboxing untrusted code.
        // The protected code must legitimately access the host process's
        // global data, stack, and heap. Restricting memory access would
        // break standard C++ execution capabilities.
        auto addr = static_cast<uintptr_t>(static_cast<int64_t>(i.aux) + e.load_base_delta);
        uint64_t raw = 0;
        std::memcpy(&raw, reinterpret_cast<const uint8_t*>(addr), 8);
        // Guest memory is in global memory encoding; decode to plaintext
        uint64_t plain = im.mem.decode_lut().apply(raw);
        e.regs[i.reg_a] = RegVal(plain);
        return {};
    }
};

/// STORE: register -> guest memory.
/// plain_a was pre-decoded by pipeline.  Encode to global memory domain,
/// then write to guest address.  No register result.
template<typename P>
struct HandlerTraits<VmOpcode::STORE, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable& im, const DecodedInsn& i) noexcept {
        // SECURITY NOTE: Intentional lack of bounds checking.
        // Similar to LOAD, the VM must have unrestricted access to the
        // host process's memory space to correctly execute the obfuscated
        // application logic. This is not a sandbox.
        auto addr = static_cast<uintptr_t>(static_cast<int64_t>(i.aux) + e.load_base_delta);
        // Encode plaintext to global memory domain for guest storage
        uint64_t mem = im.mem.encode_lut().apply(i.plain_a);
        std::memcpy(reinterpret_cast<uint8_t*>(addr), &mem, 8);
        return {};
    }
};

/// PUSH: register -> ORAM stack.
///
/// Doc 19 pipeline-level ORAM: the ORAM write has already been executed
/// by Phase D.oram in dispatch_unit (branchless, before handler dispatch).
/// The handler only updates vm_sp.  No direct Oram::write call.
template<typename P>
struct HandlerTraits<VmOpcode::PUSH, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = UsesOramTag;
    template<typename Oram>
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn&) noexcept {
        if (e.vm_sp < 8) return tl::make_unexpected(DiagnosticCode::StackOverflow);
        e.vm_sp -= 8;
        // ORAM write already done by pipeline Phase D.oram at (vm_sp - 8)
        // with encode_lut().apply(plain_a).
        return {};
    }
};

/// POP: ORAM stack -> register.
///
/// Doc 19 pipeline-level ORAM: the ORAM read has already been executed
/// by Phase D.oram in dispatch_unit.  The result is in exec.oram_read_result.
/// The handler decodes and writes to the destination register.
template<typename P>
struct HandlerTraits<VmOpcode::POP, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = UsesOramTag;
    template<typename Oram>
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable& im, const DecodedInsn& i) noexcept {
        if (e.vm_sp >= VM_OBLIVIOUS_SIZE) return tl::make_unexpected(DiagnosticCode::StackUnderflow);
        // ORAM read already done by pipeline Phase D.oram at vm_sp.
        // Result is in e.oram_read_result (raw MemVal bits).
        e.vm_sp += 8;
        uint64_t plain = im.mem.decode_lut().apply(e.oram_read_result);
        e.regs[i.reg_a] = RegVal(plain);
        return {};
    }
};

/// LOAD_CONST: constant pool -> register.
/// Pool values are SipHash-encrypted at rest but decrypted to plaintext at
/// load time.  The pipeline will FPE-encode regs[dst].
template<typename P>
struct HandlerTraits<VmOpcode::LOAD_CONST, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable& im, const DecodedInsn& i) noexcept {
        if (i.operand_a_type == VM_OPERAND_POOL) {
            // resolved_a was pre-decoded by pipeline into plain_a
            e.regs[i.reg_a] = RegVal(i.plain_a);
        } else {
            if (im.decrypted_pool.empty() || i.aux * 8 >= im.decrypted_pool.size())
                return {};
            uint64_t v = 0;
            std::memcpy(&v, im.decrypted_pool.data() + i.aux * 8, 8);
            // Pool stores plaintext after SipHash decryption
            e.regs[i.reg_a] = RegVal(v);
        }
        return {};
    }
};

/// LOAD_CTX: read VM context field -> register.
/// Context fields (IP, SP, bb_id, epoch) are already plaintext.
/// Pipeline will FPE-encode regs[dst].
template<typename P>
struct HandlerTraits<VmOpcode::LOAD_CTX, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t val = 0;
        switch (i.aux) {
            case 0: val = e.vm_ip;          break;
            case 1: val = e.vm_sp;          break;
            case 2: val = e.current_bb_id;  break;
            case 3: val = e.current_epoch;  break;
            default: break;
        }
        e.regs[i.reg_a] = RegVal(val);
        return {};
    }
};

/// STORE_CTX: register -> VM context field.
/// plain_a is pre-decoded.  Write plaintext directly to context field.
/// No register result.
template<typename P>
struct HandlerTraits<VmOpcode::STORE_CTX, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        switch (i.aux) {
            case 1: e.vm_sp = i.plain_a; break;
            default: break;
        }
        return {};
    }
};

// ===========================================================================
// Cat 1: Arithmetic (8 opcodes)
//
// Doc 15 §3.4 classification:
//   Class C (ADD, SUB, NEG, MUL, IMUL, DIV, IDIV, MOD) — native arithmetic
//
// Policy wiring:
//   P::constant_time     → DIV/IDIV/MOD use branchless zero-safe division
//   !P::constant_time    → DIV/IDIV/MOD use branch-based zero check (DebugPolicy)
//
// NOTE: MBA decomposition was removed from runtime handlers.
//   MBA obfuscation is now applied at compile time (serializer/linker).
//   Runtime uses native arithmetic — the FPE + key ratchet + ORAM pipeline
//   protects register state; runtime MBA only added timing side-channel leakage.
// ===========================================================================

namespace detail {

/// Constant-time zero-safe division:  avoids data-dependent branch.
/// Returns a/b when b != 0, 0 when b == 0.
inline uint64_t ct_div(uint64_t a, uint64_t b) noexcept {
    // mask = 0 when b==0, all-ones when b!=0
    uint64_t nz   = b | (-b);          // MSB set iff b != 0
    uint64_t mask = static_cast<uint64_t>(
        -static_cast<int64_t>(nz >> 63));
    // safe_b is 1 when b==0 (prevents hardware divide-by-zero), b otherwise
    uint64_t safe_b = b | (~mask & 1);
    return (a / safe_b) & mask;
}

/// Constant-time zero-safe signed division.
///
/// Handles two UB cases branchlessly:
///   1. b == 0:                     returns 0 (zero-safe)
///   2. a == INT64_MIN && b == -1:  returns INT64_MIN (two's complement wrap)
///
/// WHY case 2: INT64_MIN / -1 = INT64_MAX+1 overflows int64_t.
///   On x86 this triggers #DE (divide error). C++ says it's UB.
///   We return INT64_MIN — the two's complement wrapped result.
inline uint64_t ct_idiv(int64_t a, int64_t b) noexcept {
    uint64_t ua = static_cast<uint64_t>(a);
    uint64_t ub = static_cast<uint64_t>(b);

    // Branchless equality: (~(x | -x)) >> 63 == 1 iff x == 0
    auto eq_zero = [](uint64_t x) -> uint64_t {
        return (~(x | -x)) >> 63;
    };

    // b == 0 check
    uint64_t b_is_zero = eq_zero(ub);
    uint64_t b_nz_mask = -static_cast<uint64_t>(!b_is_zero);  // all-ones if b != 0

    // Overflow check: a == INT64_MIN && b == -1
    uint64_t a_is_min  = eq_zero(ua ^ static_cast<uint64_t>(INT64_MIN));
    uint64_t b_is_neg1 = eq_zero(ub ^ UINT64_MAX);
    uint64_t overflow  = a_is_min & b_is_neg1;
    uint64_t ov_mask   = -overflow;  // all-ones if overflow

    // safe_b: replace b with 1 when b==0 OR overflow (avoids #DE and UB)
    //   Normal:   safe_b = b
    //   b==0:     safe_b = 1 → a/1 = a, masked to 0 by b_nz_mask
    //   Overflow: safe_b = 1 → INT64_MIN/1 = INT64_MIN (correct wrap)
    uint64_t poison = b_is_zero | overflow;          // 1 if either bad case
    uint64_t poison_mask = -poison;                   // all-ones if bad
    int64_t safe_b = static_cast<int64_t>(
        (1 & poison_mask) | (ub & ~poison_mask));

    uint64_t quotient = static_cast<uint64_t>(a / safe_b);

    // Result selection:
    //   b==0:     return 0
    //   Overflow: return INT64_MIN (= ua)
    //   Normal:   return quotient
    uint64_t result = (quotient & ~ov_mask) | (ua & ov_mask);
    return result & b_nz_mask;
}

/// Constant-time zero-safe modulus.
inline uint64_t ct_mod(uint64_t a, uint64_t b) noexcept {
    uint64_t nz   = b | (-b);
    uint64_t mask = static_cast<uint64_t>(
        -static_cast<int64_t>(nz >> 63));
    uint64_t safe_b = b | (~mask & 1);
    return (a % safe_b) & mask;
}

}  // namespace detail

/// ADD: dst = a + b
template<typename P>
struct HandlerTraits<VmOpcode::ADD, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(i.plain_a + i.plain_b);
        return {};
    }
};

/// SUB: dst = a - b
template<typename P>
struct HandlerTraits<VmOpcode::SUB, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(i.plain_a - i.plain_b);
        return {};
    }
};

/// MUL: dst = a * b (unsigned) — native for all policies (MBA mul is future work)
template<typename P>
struct HandlerTraits<VmOpcode::MUL, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(i.plain_a * i.plain_b);
        return {};
    }
};

/// IMUL: dst = a * b (signed) — native for all policies
template<typename P>
struct HandlerTraits<VmOpcode::IMUL, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        auto sa = static_cast<int64_t>(i.plain_a);
        auto sb = static_cast<int64_t>(i.plain_b);
        e.regs[i.reg_a] = RegVal(static_cast<uint64_t>(sa * sb));
        return {};
    }
};

/// DIV: dst = a / b (unsigned), zero-safe
template<typename P>
struct HandlerTraits<VmOpcode::DIV, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(detail::ct_div(i.plain_a, i.plain_b));
        return {};
    }
};

/// IDIV: dst = a / b (signed), zero-safe
template<typename P>
struct HandlerTraits<VmOpcode::IDIV, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        auto sa = static_cast<int64_t>(i.plain_a);
        auto sb = static_cast<int64_t>(i.plain_b);
        e.regs[i.reg_a] = RegVal(detail::ct_idiv(sa, sb));
        return {};
    }
};

/// NEG: dst = -a (two's complement)
template<typename P>
struct HandlerTraits<VmOpcode::NEG, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(-i.plain_a);
        return {};
    }
};

/// MOD: dst = a % b (unsigned), zero-safe
template<typename P>
struct HandlerTraits<VmOpcode::MOD, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(detail::ct_mod(i.plain_a, i.plain_b));
        return {};
    }
};

// ===========================================================================
// Cat 2: Logic (9 opcodes)  [doc 15 §3.4: Class A — zero plaintext]
//
// Class A: byte-lane LUT composition in the encoding domain, zero plaintext
// exposure.  In the current doc 16 architecture, the pipeline provides
// plaintext operands and Class A composition tables are NOT yet wired.
// The FPE key ratchet provides equivalent forward secrecy.
//
// TODO: When superoperator infrastructure lands, Class A handlers should
//       operate on encoded values directly via composition tables,
//       bypassing the plaintext decode/encode phases entirely.
// ===========================================================================

/// AND: dst = a & b
template<typename P>
struct HandlerTraits<VmOpcode::AND, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(i.plain_a & i.plain_b);
        return {};
    }
};

/// OR: dst = a | b
template<typename P>
struct HandlerTraits<VmOpcode::OR, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(i.plain_a | i.plain_b);
        return {};
    }
};

/// XOR: dst = a ^ b
template<typename P>
struct HandlerTraits<VmOpcode::XOR, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(i.plain_a ^ i.plain_b);
        return {};
    }
};

/// NOT: dst = ~a
template<typename P>
struct HandlerTraits<VmOpcode::NOT, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(~i.plain_a);
        return {};
    }
};

/// SHL: dst = a << (b & 63)
template<typename P>
struct HandlerTraits<VmOpcode::SHL, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t amt = i.plain_b & 63;
        e.regs[i.reg_a] = RegVal(i.plain_a << amt);
        return {};
    }
};

/// SHR: dst = a >> (b & 63)  (logical)
template<typename P>
struct HandlerTraits<VmOpcode::SHR, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t amt = i.plain_b & 63;
        e.regs[i.reg_a] = RegVal(i.plain_a >> amt);
        return {};
    }
};

/// SAR: dst = a >> (b & 63)  (arithmetic, sign-extending)
template<typename P>
struct HandlerTraits<VmOpcode::SAR, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t amt = i.plain_b & 63;
        e.regs[i.reg_a] = RegVal(static_cast<uint64_t>(
            static_cast<int64_t>(i.plain_a) >> amt));
        return {};
    }
};

/// ROL: dst = rotate_left(a, b & 63)
///
/// Uses the standard UB-free rotate idiom: (x << n) | (x >> ((-n) & 63)).
/// The naive (x >> (64 - n)) is undefined behavior when n == 0 because
/// shifting a uint64_t by 64 violates C++17 §8.8 [expr.shift].
/// The (-n) & 63 trick maps n=0 to >>0 (identity), avoiding the UB while
/// remaining constant-time (no branch on shift amount).
template<typename P>
struct HandlerTraits<VmOpcode::ROL, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t amt = i.plain_b & 63;
        e.regs[i.reg_a] = RegVal((i.plain_a << amt) | (i.plain_a >> ((-amt) & 63)));
        return {};
    }
};

/// ROR: dst = rotate_right(a, b & 63)
///
/// Same UB-free idiom as ROL — see ROL comment for rationale.
template<typename P>
struct HandlerTraits<VmOpcode::ROR, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t amt = i.plain_b & 63;
        e.regs[i.reg_a] = RegVal((i.plain_a >> amt) | (i.plain_a << ((-amt) & 63)));
        return {};
    }
};

// ===========================================================================
// Cat 3: Comparison (4 opcodes)  [doc 15 §3.4: Class A — zero plaintext]
//
// CMP and TEST set flags from pre-decoded plaintext operands.
// No register result — pipeline skips FPE-encode for these.
//
// When P::constant_time is true, flag computation uses branchless
// arithmetic to prevent timing side-channels on the comparison result.
// ===========================================================================

namespace detail {

/// Branchless flag computation for CMP.
/// Each flag bit is computed via arithmetic, no branches.
inline uint8_t ct_cmp_flags(uint64_t a, uint64_t b) noexcept {
    auto sa = static_cast<int64_t>(a);
    auto sb = static_cast<int64_t>(b);
    int64_t diff = sa - sb;

    // ZF: diff == 0  →  1 iff all bits zero
    uint64_t zf = static_cast<uint64_t>(
        (static_cast<int64_t>(~(diff | -diff))) >> 63) & 1;

    // SF: sign bit of diff
    uint64_t sf = (static_cast<uint64_t>(diff) >> 63) & 1;

    // CF: unsigned borrow (a < b), branchless.
    // ~a & b catches all bits where b has a 1 and a has 0 (borrow sources).
    // (a ^ b) has bits that differ; half-borrow propagation via AND with
    // the XOR detects full borrow.  The MSB of the combined expression
    // gives the borrow-out.
    uint64_t diff_u = a - b;
    uint64_t cf = ((~a & b) | ((~(a ^ b)) & diff_u)) >> 63;

    // OF: signed overflow
    uint64_t of = (static_cast<uint64_t>((sa ^ sb) & (sa ^ diff)) >> 63) & 1;

    return static_cast<uint8_t>(zf | (sf << 1) | (cf << 2) | (of << 3));
}

}  // namespace detail

/// CMP: set flags from signed/unsigned comparison of a and b.
template<typename P>
struct HandlerTraits<VmOpcode::CMP, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.vm_flags = detail::ct_cmp_flags(i.plain_a, i.plain_b);
        return {};
    }
};

/// TEST: AND then set flags (no register write).
template<typename P>
struct HandlerTraits<VmOpcode::TEST, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t result = i.plain_a & i.plain_b;
        uint64_t zf = static_cast<uint64_t>(
            (static_cast<int64_t>(~(result | -result))) >> 63) & 1;
        uint64_t sf = (result >> 63) & 1;
        e.vm_flags = static_cast<uint8_t>(zf | (sf << 1));
        return {};
    }
};

/// SET_FLAG: set a specific flag bit.
/// Pure flag manipulation — no register or encoding involvement.
template<typename P>
struct HandlerTraits<VmOpcode::SET_FLAG, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint8_t bit = i.condition & 0x03;
        uint8_t val = static_cast<uint8_t>(i.aux & 1);
        e.vm_flags = static_cast<uint8_t>((e.vm_flags & ~(1u << bit)) | (val << bit));
        return {};
    }
};

/// GET_FLAG: read a flag bit into register as 0 or 1.
/// Pipeline will FPE-encode regs[dst].
template<typename P>
struct HandlerTraits<VmOpcode::GET_FLAG, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint8_t bit = i.condition & 0x03;
        uint64_t val = (e.vm_flags >> bit) & 1;
        e.regs[i.reg_a] = RegVal(val);
        return {};
    }
};

// ===========================================================================
// Cat 4: Control Flow (6 opcodes)
//
// Branch/call/ret handlers modify execution flow state (branch_target_bb,
// shadow_stack).  They do NOT produce register results — pipeline skips
// FPE-encode.  NATIVE_CALL is the exception: it writes a result to reg 0.
// ===========================================================================

/// JMP: unconditional branch to BB at i.aux.
template<typename P>
struct HandlerTraits<VmOpcode::JMP, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.branch_target_bb = i.aux;
        e.branch_taken = true;
        return {};
    }
};

/// JCC: conditional branch — evaluate condition against vm_flags.
/// [doc 14 D3: branchless CMOV — no data-dependent branch on condition result]
template<typename P>
struct HandlerTraits<VmOpcode::JCC, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        // Branchless: always write both fields.  The pipeline reads
        // branch_taken to decide whether to transition; when false the
        // values of branch_target_bb are harmlessly ignored.
        bool cond = detail::evaluate_condition_ct(e.vm_flags, i.condition);
        uint32_t mask = -static_cast<uint32_t>(cond);  // 0 or 0xFFFFFFFF
        e.branch_target_bb = (i.aux & mask) | (e.branch_target_bb & ~mask);
        e.branch_taken     = cond;
        return {};
    }
};

/// CALL_VM: save context to shadow stack, branch to callee BB.
/// Register snapshot stores current FPE-encoded values (the snapshot
/// is opaque to the handler — the pipeline manages encoding state).
template<typename P>
struct HandlerTraits<VmOpcode::CALL_VM, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        if (e.shadow_depth >= VM_MAX_NESTING)
            return tl::make_unexpected(DiagnosticCode::ShadowStackOverflow);
        auto& cp = e.shadow_stack[e.shadow_depth];
        cp.vm_ip = e.vm_ip;
        cp.bb_id = e.current_bb_id;
        // Doc 16: save the epoch_seed for opcode permutation context
        std::memcpy(cp.epoch_seed, &ep.opcode_perm, 32);
        cp.saved_insn_index = e.insn_index_in_bb;
        // Doc 16: save current FPE key so RET_VM can decode the snapshot.
        // The register snapshot is in insn_fpe_key's domain at this moment.
        std::memcpy(cp.saved_insn_fpe_key, e.insn_fpe_key, 16);
        for (int r = 0; r < VM_REG_COUNT; ++r)
            cp.encoded_regs_snapshot[r] = e.regs[r].bits;
        e.shadow_depth++;
        e.branch_target_bb = i.aux;
        e.branch_taken = true;
        return {};
    }
};

/// RET_VM: restore context from shadow stack, branch to caller BB.
/// Restores the opaque register snapshot saved by CALL_VM.
template<typename P>
struct HandlerTraits<VmOpcode::RET_VM, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn&) noexcept {
        if (e.shadow_depth == 0)
            return tl::make_unexpected(DiagnosticCode::StackUnderflow);
        e.shadow_depth--;
        auto& cp = e.shadow_stack[e.shadow_depth];
        for (int r = 0; r < VM_REG_COUNT; ++r)
            e.regs[r] = RegVal(cp.encoded_regs_snapshot[r]);
        // Restore FPE key to the caller's domain (doc 16 §5 step 4).
        // The register snapshot is ciphertext under saved_insn_fpe_key.
        // enter_basic_block will use exec.insn_fpe_key as the "old" key
        // for re-encoding — it must match the snapshot's domain, not the
        // callee's current key.  Without this, FPE domain translation
        // breaks and all registers become garbage after RET_VM.
        std::memcpy(e.insn_fpe_key, cp.saved_insn_fpe_key, 16);
        e.branch_target_bb = cp.bb_id;
        e.branch_taken = true;
        e.return_resume_ip = cp.vm_ip + 1;
        e.return_resume_insn_idx = cp.saved_insn_index + 1;
        return {};
    }
};

/// NATIVE_CALL: ABI-correct native function call with FP, struct return,
/// stack args, and platform-specific calling convention support.
///
/// Uses PlatformCallDesc + platform_call trampoline (ASM) for correct
/// register placement across all ABIs: SysV x64, Win x64, cdecl/stdcall
/// x86-32, AAPCS64.
///
/// The result is written as plaintext to reg 0; the pipeline FPE-encodes
/// it after handler return.
template<typename P>
struct HandlerTraits<VmOpcode::NATIVE_CALL, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable& im, const DecodedInsn& insn) noexcept {
        if (insn.aux >= im.native_calls.size())
            return tl::make_unexpected(DiagnosticCode::NativeCallBridgeFailed);

        const auto& te = im.native_calls[insn.aux];
        uint8_t argc        = Common::VM::te_arg_count(te);
        uint8_t fp_mask     = Common::VM::te_fp_mask(te);
        bool is_variadic    = Common::VM::te_is_variadic(te);
        bool returns_fp     = Common::VM::te_returns_fp(te);
        bool returns_struct = Common::VM::te_returns_struct(te);
        uint8_t convention  = Common::VM::te_convention(te);

        auto target = reinterpret_cast<void*>(
            static_cast<uintptr_t>(te.target_offset + e.load_base_delta));
        if (!target)
            return tl::make_unexpected(DiagnosticCode::NativeCallBridgeFailed);

        // FPE-decode arg registers to plaintext (Class C — acknowledged).
        SecureLocal<Crypto::Speck64_RoundKeys> nc_rk;
        Crypto::Speck64_KeySchedule(e.insn_fpe_key, nc_rk.val);
        SecureLocal<Crypto::XEX_Tweaks> nc_tw;
        Crypto::XEX_ComputeTweaks(nc_rk.val, nc_tw.val);

        uint64_t raw_args[32] = {};
        uint8_t decode_count = argc < VM_REG_COUNT ? argc : VM_REG_COUNT;
        for (uint8_t i = 0; i < decode_count; ++i)
            raw_args[i] = Crypto::FPE_Decode(nc_rk.val, nc_tw.val, i,
                                              e.regs[i].bits);
        // nc_rk, nc_tw auto-zeroed by SecureLocal

        // Struct return: raw_args[0] is the struct buffer pointer
        void* struct_ptr = returns_struct
            ? reinterpret_cast<void*>(static_cast<uintptr_t>(raw_args[0]))
            : nullptr;

        // Classify args into PlatformCallDesc for the native ABI
        PlatformCallDesc desc{};
        CallABI abi = native_abi();
        if (abi == CallABI::Cdecl_x86 && convention == Common::VM::TE_CONV_STDCALL)
            abi = CallABI::Stdcall_x86;

        classify_args_for_abi(abi, raw_args, argc, fp_mask,
                              returns_struct, struct_ptr, desc);

        desc.target = target;
        desc.convention = convention;
        desc.flags = pack_call_flags(
            is_variadic ? desc.fp_count : static_cast<uint8_t>(0), returns_fp);

        // Call via platform-specific ASM trampoline
        auto result_or = EH::guarded_platform_call(
            &desc, returns_struct && abi == CallABI::AAPCS64, struct_ptr);

        // Zero plaintext args from stack (forward secrecy)
        secure_zero(raw_args, sizeof(raw_args));

        if (!result_or) {
            return tl::make_unexpected(result_or.error());
        }

        // Store plaintext result; pipeline will FPE-encode reg 0
        e.regs[0] = RegVal(*result_or);
        e.native_call_nonce++;
        return {};
    }
};

/// HALT: stop execution.
template<typename P>
struct HandlerTraits<VmOpcode::HALT, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn&) noexcept {
        e.halted = true;
        return {};
    }
};

// ===========================================================================
// Cat 5: Width/Extension (8 opcodes)
//
// All width operations are trivial on plaintext — just mask or sign-extend.
// ===========================================================================

/// SEXT8: sign-extend from 8 bits
template<typename P>
struct HandlerTraits<VmOpcode::SEXT8, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(static_cast<uint64_t>(
            static_cast<int64_t>(static_cast<int8_t>(i.plain_a & 0xFF))));
        return {};
    }
};

/// SEXT16: sign-extend from 16 bits
template<typename P>
struct HandlerTraits<VmOpcode::SEXT16, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(static_cast<uint64_t>(
            static_cast<int64_t>(static_cast<int16_t>(i.plain_a & 0xFFFF))));
        return {};
    }
};

/// SEXT32: sign-extend from 32 bits
template<typename P>
struct HandlerTraits<VmOpcode::SEXT32, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(static_cast<uint64_t>(
            static_cast<int64_t>(static_cast<int32_t>(i.plain_a & 0xFFFFFFFF))));
        return {};
    }
};

/// ZEXT8: zero-extend from 8 bits
template<typename P>
struct HandlerTraits<VmOpcode::ZEXT8, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(i.plain_a & 0xFF);
        return {};
    }
};

/// ZEXT16: zero-extend from 16 bits
template<typename P>
struct HandlerTraits<VmOpcode::ZEXT16, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(i.plain_a & 0xFFFF);
        return {};
    }
};

/// ZEXT32: zero-extend from 32 bits
template<typename P>
struct HandlerTraits<VmOpcode::ZEXT32, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(i.plain_a & 0xFFFFFFFF);
        return {};
    }
};

/// TRUNC8: truncate to 8 bits (same as ZEXT8 for unsigned)
template<typename P>
struct HandlerTraits<VmOpcode::TRUNC8, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(i.plain_a & 0xFF);
        return {};
    }
};

/// TRUNC16: truncate to 16 bits
template<typename P>
struct HandlerTraits<VmOpcode::TRUNC16, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = RegVal(i.plain_a & 0xFFFF);
        return {};
    }
};

// ===========================================================================
// Cat 6: Atomic (5 opcodes)
//
// Guest atomic memory uses native plaintext (hardware atomicity requires
// actual values).  P::constant_time wires branchless flag computation
// for CMPXCHG.
// ===========================================================================

/// LOCK_ADD: atomic fetch-add on guest memory.
/// plain_a = addend.  Returns old value in reg_a.
template<typename P>
struct HandlerTraits<VmOpcode::LOCK_ADD, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        auto addr = static_cast<uintptr_t>(static_cast<int64_t>(i.aux) + e.load_base_delta);
        auto* ptr = reinterpret_cast<std::atomic<uint64_t>*>(addr);
        uint64_t old = ptr->fetch_add(i.plain_a, std::memory_order_seq_cst);
        e.regs[i.reg_a] = RegVal(old);
        return {};
    }
};

/// XCHG: atomic exchange on guest memory.
/// plain_a = new value.  Returns old value in reg_a.
template<typename P>
struct HandlerTraits<VmOpcode::XCHG, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        auto addr = static_cast<uintptr_t>(static_cast<int64_t>(i.aux) + e.load_base_delta);
        auto* ptr = reinterpret_cast<std::atomic<uint64_t>*>(addr);
        uint64_t old = ptr->exchange(i.plain_a, std::memory_order_seq_cst);
        e.regs[i.reg_a] = RegVal(old);
        return {};
    }
};

/// CMPXCHG: atomic compare-and-swap on guest memory.
/// plain_a = expected, plain_b = desired.
/// Returns old value (expected after CAS) in reg_a; ZF = success.
/// [doc 15: branchless CMOV for D3 constant-time]
template<typename P>
struct HandlerTraits<VmOpcode::CMPXCHG, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t expected = i.plain_a;
        uint64_t desired  = i.plain_b;
        auto addr = static_cast<uintptr_t>(static_cast<int64_t>(i.aux) + e.load_base_delta);
        auto* ptr = reinterpret_cast<std::atomic<uint64_t>*>(addr);
        bool ok = ptr->compare_exchange_strong(expected, desired, std::memory_order_seq_cst);
        e.regs[i.reg_a] = RegVal(expected);
        e.vm_flags = static_cast<uint8_t>(ok);
        return {};
    }
};

/// FENCE: full memory barrier.  No register result.
template<typename P>
struct HandlerTraits<VmOpcode::FENCE, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution&, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn&) noexcept {
        std::atomic_thread_fence(std::memory_order_seq_cst);
        return {};
    }
};

/// ATOMIC_LOAD: atomic load from guest memory.
/// Returns loaded value in reg_a.
template<typename P>
struct HandlerTraits<VmOpcode::ATOMIC_LOAD, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        auto addr = static_cast<uintptr_t>(static_cast<int64_t>(i.aux) + e.load_base_delta);
        auto* ptr = reinterpret_cast<std::atomic<uint64_t>*>(addr);
        uint64_t val = ptr->load(std::memory_order_seq_cst);
        e.regs[i.reg_a] = RegVal(val);
        return {};
    }
};

// ===========================================================================
// Cat 7: VM Internal (7 opcodes)
//
// Internal VM control ops.  Most are no-ops or modify metadata.
// The key ratchet and FPE pipeline run regardless of these.
// ===========================================================================

/// NOP: write to trash register file (GSS chaff — makes NOP
/// indistinguishable from real ops in cache/memory traces).
///
/// WHY ghost ALU + ghost flags (constant_time mode, Doc 19 §2):
///   In the fixed-width SO model, every dispatch unit contains 1 real
///   instruction + N-1 chaff NOPs.  If the NOP handler has measurably
///   different timing from arithmetic handlers (ADD, SUB, CMP), an
///   attacker with EM/DPA access can distinguish chaff from real by
///   timing the handler execution.  The ghost ALU performs the same
///   ADD + flag computation as ADD/CMP handlers, writing results to
///   trash_regs (never to real regs or flags).  This equalizes the
///   micro-architectural footprint within timing cluster 1.
template<typename P>
struct HandlerTraits<VmOpcode::NOP, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        // Ghost ALU: same computation as ADD handler to equalize
        // micro-architectural side effects (pipeline stalls, power trace).
        uint64_t ghost_result = i.plain_a + i.plain_b;
        e.trash_regs[i.reg_a] = ghost_result;

        // Ghost flags: CMP/TEST handlers write to vm_flags on every
        // execution.  NOP must produce equivalent flag-write timing
        // without touching real vm_flags.
        e.trash_regs[(i.reg_b) & 0x0F] = ghost_result & 0x0F;
        return {};
    }
};

/// CHECK_INTEGRITY: BB MAC verified by dispatcher at BB boundaries.
/// This opcode is a no-op in v1 — future: explicit inline MAC check.
template<typename P>
struct HandlerTraits<VmOpcode::CHECK_INTEGRITY, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution&, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn&) noexcept {
        return {};
    }
};

/// CHECK_DEBUG: anti-debug stub (phase 9 deferred).
template<typename P>
struct HandlerTraits<VmOpcode::CHECK_DEBUG, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution&, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn&) noexcept {
        return {};
    }
};

/// MUTATE_ISA: trigger mid-BB opcode permutation re-derivation.
/// In doc 16, the per-instruction FPE key ratchet subsumes this —
/// the opcode permutation changes implicitly with every ratchet step.
/// Retained as a no-op for ISA compatibility.
template<typename P>
struct HandlerTraits<VmOpcode::MUTATE_ISA, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution&, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn&) noexcept {
        return {};
    }
};

/// REKEY: derive new enc_state from stored_seed + rekey counter.
/// This strengthens the SipHash chain mid-BB by mixing in fresh
/// keying material.  The enc_state feeds the FPE key schedule.
template<typename P>
struct HandlerTraits<VmOpcode::REKEY, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable& im, const DecodedInsn& i) noexcept {
        uint32_t counter = i.aux;
        uint8_t rk_ctx[9];
        std::memcpy(rk_ctx, "rekey", 5);
        std::memcpy(rk_ctx + 5, &counter, 4);
        uint8_t rk_mat[16];
        // Use pre-derived rekey_key (stored_seed was zeroed at init)
        Common::VM::Crypto::blake3_keyed_hash(im.rekey_key,
            rk_ctx, 9, rk_mat, 16);
        uint8_t es[8];
        std::memcpy(es, &e.enc_state, 8);
        e.enc_state = Common::VM::Crypto::siphash_2_4(rk_mat, es, 8);
        return {};
    }
};

/// SAVE_EPOCH: snapshot current state to shadow stack (no branch).
/// Used for speculative execution and error recovery.
template<typename P>
struct HandlerTraits<VmOpcode::SAVE_EPOCH, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn&) noexcept {
        if (e.shadow_depth >= VM_MAX_NESTING)
            return tl::make_unexpected(DiagnosticCode::ShadowStackOverflow);
        auto& cp = e.shadow_stack[e.shadow_depth];
        cp.vm_ip = e.vm_ip;
        cp.bb_id = e.current_bb_id;
        cp.salt = 0;
        for (int r = 0; r < VM_REG_COUNT; ++r)
            cp.encoded_regs_snapshot[r] = e.regs[r].bits;
        e.shadow_depth++;
        return {};
    }
};

/// RESYNC: restore register snapshot from shadow stack (no branch).
/// Counterpart to SAVE_EPOCH.
template<typename P>
struct HandlerTraits<VmOpcode::RESYNC, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn&) noexcept {
        if (e.shadow_depth == 0)
            return tl::make_unexpected(DiagnosticCode::StackUnderflow);
        e.shadow_depth--;
        auto& cp = e.shadow_stack[e.shadow_depth];
        for (int r = 0; r < VM_REG_COUNT; ++r)
            e.regs[r] = RegVal(cp.encoded_regs_snapshot[r]);
        return {};
    }
};

}  // namespace VMPilot::Runtime

#endif  // __RUNTIME_HANDLER_IMPLS_HPP__
