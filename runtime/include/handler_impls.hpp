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

/// Evaluate condition code against flags register.
/// Pure predicate logic — no encoding involvement.
/// Implementation: handler_detail.cpp.
bool evaluate_condition(uint8_t flags, uint8_t cond) noexcept;

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
/// Encode plaintext to memory domain, write via ORAM.  No register result.
template<typename P>
struct HandlerTraits<VmOpcode::PUSH, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = UsesOramTag;
    template<typename Oram>
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState& o,
                               const VmImmutable& im, const DecodedInsn& i) noexcept {
        if (e.vm_sp < 8) return tl::make_unexpected(DiagnosticCode::StackOverflow);
        e.vm_sp -= 8;
        MemVal mem(im.mem.encode_lut().apply(i.plain_a));
        Oram::write(o, e.vm_sp, mem);
        return {};
    }
};

/// POP: ORAM stack -> register.
/// Read MemVal from ORAM, decode from memory domain to plaintext.
/// Pipeline will FPE-encode regs[dst].
template<typename P>
struct HandlerTraits<VmOpcode::POP, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = UsesOramTag;
    template<typename Oram>
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState& o,
                               const VmImmutable& im, const DecodedInsn& i) noexcept {
        if (e.vm_sp >= VM_OBLIVIOUS_SIZE) return tl::make_unexpected(DiagnosticCode::StackUnderflow);
        MemVal mem = Oram::read(o, e.vm_sp);
        e.vm_sp += 8;
        uint64_t plain = im.mem.decode_lut().apply(mem.bits);
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
//   Class B (ADD, SUB, NEG) — MBA decomposition, 7 register-transient carry bits
//   Class C (MUL, IMUL, DIV, IDIV, MOD) — native bridge, full plaintext transient
//
// Policy wiring:
//   P::use_mba           → Class B ops use MBA decomposition (XOR+AND+SHL1 chain)
//                           Depth controlled by P::fusion_granularity (1-4 levels)
//   P::constant_time     → Class C ops use branchless zero-safe division
//   !P::use_mba          → Class B ops fall back to native bridge (DebugPolicy)
//   !P::constant_time    → Class C ops use branch-based zero check (DebugPolicy)
// ===========================================================================

namespace detail {

/// MBA addition: a + b = (a ^ b) + 2*(a & b), applied recursively.
///
/// Each level replaces one addition with XOR + AND + shift + addition,
/// progressively shredding the carry chain.  After `depth` levels the
/// residual addition handles the remaining carries.
///
/// Constant-time: every level executes unconditionally (no early-out on
/// zero carry) to prevent timing side-channels.
inline uint64_t mba_add(uint64_t a, uint64_t b, uint8_t depth) noexcept {
    for (uint8_t i = 0; i < depth; ++i) {
        uint64_t x = a ^ b;           // sum without carries
        uint64_t c = (a & b) << 1;    // carry chain shifted
        a = x;
        b = c;
    }
    return a + b;   // residual native add for remaining carries
}

/// MBA subtraction: a - b = a + (~b) + 1, then MBA the addition.
inline uint64_t mba_sub(uint64_t a, uint64_t b, uint8_t depth) noexcept {
    return mba_add(a, ~b + 1, depth);
}

/// MBA negation: -a = ~a + 1, then MBA the increment.
inline uint64_t mba_neg(uint64_t a, uint8_t depth) noexcept {
    return mba_add(~a, 1, depth);
}

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
inline uint64_t ct_idiv(int64_t a, int64_t b) noexcept {
    uint64_t ub = static_cast<uint64_t>(b);
    uint64_t nz   = ub | (-ub);
    uint64_t mask = static_cast<uint64_t>(
        -static_cast<int64_t>(nz >> 63));
    int64_t safe_b = static_cast<int64_t>(ub | (~mask & 1));
    return static_cast<uint64_t>(a / safe_b) & mask;
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

/// ADD: dst = a + b  [doc 15 §3.4: Class B — MBA decomposition]
template<typename P>
struct HandlerTraits<VmOpcode::ADD, P> {
    static constexpr auto security_class = SecurityClass::B;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        if constexpr (P::use_mba)
            e.regs[i.reg_a] = RegVal(
                detail::mba_add(i.plain_a, i.plain_b, P::fusion_granularity));
        else
            e.regs[i.reg_a] = RegVal(i.plain_a + i.plain_b);
        return {};
    }
};

/// SUB: dst = a - b  [doc 15 §3.4: Class B — MBA decomposition]
template<typename P>
struct HandlerTraits<VmOpcode::SUB, P> {
    static constexpr auto security_class = SecurityClass::B;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        if constexpr (P::use_mba)
            e.regs[i.reg_a] = RegVal(
                detail::mba_sub(i.plain_a, i.plain_b, P::fusion_granularity));
        else
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
        if constexpr (P::constant_time)
            e.regs[i.reg_a] = RegVal(detail::ct_div(i.plain_a, i.plain_b));
        else
            e.regs[i.reg_a] = RegVal(i.plain_b != 0 ? i.plain_a / i.plain_b : 0);
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
        if constexpr (P::constant_time)
            e.regs[i.reg_a] = RegVal(detail::ct_idiv(sa, sb));
        else
            e.regs[i.reg_a] = RegVal(sb != 0 ? static_cast<uint64_t>(sa / sb) : 0);
        return {};
    }
};

/// NEG: dst = -a (two's complement)  [doc 15 §3.4: Class B — MBA decomposition]
template<typename P>
struct HandlerTraits<VmOpcode::NEG, P> {
    static constexpr auto security_class = SecurityClass::B;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        if constexpr (P::use_mba)
            e.regs[i.reg_a] = RegVal(
                detail::mba_neg(i.plain_a, P::fusion_granularity));
        else
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
        if constexpr (P::constant_time)
            e.regs[i.reg_a] = RegVal(detail::ct_mod(i.plain_a, i.plain_b));
        else
            e.regs[i.reg_a] = RegVal(i.plain_b != 0 ? i.plain_a % i.plain_b : 0);
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
template<typename P>
struct HandlerTraits<VmOpcode::ROL, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t amt = i.plain_b & 63;
        e.regs[i.reg_a] = RegVal((i.plain_a << amt) | (i.plain_a >> (64 - amt)));
        return {};
    }
};

/// ROR: dst = rotate_right(a, b & 63)
template<typename P>
struct HandlerTraits<VmOpcode::ROR, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t amt = i.plain_b & 63;
        e.regs[i.reg_a] = RegVal((i.plain_a >> amt) | (i.plain_a << (64 - amt)));
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

    // CF: unsigned borrow (a < b)
    uint64_t cf = (a < b) ? 1 : 0;  // compiled to SBB on x86

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
        if constexpr (P::constant_time) {
            e.vm_flags = detail::ct_cmp_flags(i.plain_a, i.plain_b);
        } else {
            auto sa = static_cast<int64_t>(i.plain_a);
            auto sb = static_cast<int64_t>(i.plain_b);
            auto ua = i.plain_a;
            auto ub = i.plain_b;
            int64_t diff = sa - sb;
            e.vm_flags = 0;
            if (diff == 0)  e.vm_flags |= 0x01;
            if (diff < 0)   e.vm_flags |= 0x02;
            if (ua < ub)    e.vm_flags |= 0x04;
            if (((sa ^ sb) & (sa ^ diff)) < 0) e.vm_flags |= 0x08;
        }
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
        if constexpr (P::constant_time) {
            uint64_t zf = static_cast<uint64_t>(
                (static_cast<int64_t>(~(result | -result))) >> 63) & 1;
            uint64_t sf = (result >> 63) & 1;
            e.vm_flags = static_cast<uint8_t>(zf | (sf << 1));
        } else {
            e.vm_flags = 0;
            if (result == 0) e.vm_flags |= 0x01;
            if (static_cast<int64_t>(result) < 0) e.vm_flags |= 0x02;
        }
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
template<typename P>
struct HandlerTraits<VmOpcode::JCC, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        if (detail::evaluate_condition(e.vm_flags, i.condition)) {
            e.branch_target_bb = i.aux;
            e.branch_taken = true;
        }
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
        uint64_t result;
        if (returns_struct && abi == CallABI::AAPCS64) {
            result = platform_call_struct(&desc, struct_ptr);
        } else {
            result = platform_call(&desc);
        }

        // Zero plaintext args from stack (forward secrecy)
        secure_zero(raw_args, sizeof(raw_args));

        // Store plaintext result; pipeline will FPE-encode reg 0
        e.regs[0] = RegVal(result);
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
// Atomic operations read/write guest memory at physical addresses.
// The pipeline pre-decodes plain_a (the value operand) and plain_b
// (for CMPXCHG expected/desired).  Guest atomic memory is NOT in the
// global memory encoding domain — it uses native plaintext because
// atomic ops require hardware atomicity guarantees on the actual value.
// ===========================================================================

/// LOCK_ADD: atomic fetch-add on guest memory.
/// plain_a = addend.  Returns old value in reg_a.
template<typename P>
struct HandlerTraits<VmOpcode::LOCK_ADD, P> {
    static constexpr auto security_class = SecurityClass::A;
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
        e.vm_flags = ok ? 0x01 : 0x00;
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
template<typename P>
struct HandlerTraits<VmOpcode::NOP, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.trash_regs[i.reg_a] = i.plain_b;
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
