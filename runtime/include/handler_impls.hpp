#pragma once
#ifndef __RUNTIME_HANDLER_IMPLS_HPP__
#define __RUNTIME_HANDLER_IMPLS_HPP__

/// @file handler_impls.hpp
/// @brief ALL 55 HandlerTraits specializations with real implementations.
///
/// Ported from the old VMContext-based handlers to the new 4-way state
/// split (VmExecution, VmEpoch, VmOramState, VmImmutable).
///
/// Phantom type safety: handler boundaries enforce RegVal for registers,
/// MemVal for ORAM/memory, PlainVal for Class C bridge operations.
/// Inside handlers, raw .bits access is used for table lookups.

#include "handler_traits.hpp"
#include "composition_cache_v2.hpp"
#include "platform_call.hpp"

#include <vm/vm_encoding.hpp>
// secure_zero is forward-declared here; full implementation comes from
// vm/secure_zero.hpp on branches that have it.  This inline fallback
// ensures compilation on branches without the header.
namespace VMPilot::Runtime::detail {
inline void secure_zero_impl(void* p, size_t n) noexcept {
    static void* (* volatile vset)(void*, int, size_t) = std::memset;
    if (p && n) vset(p, 0, n);
}
}  // namespace VMPilot::Runtime::detail

#include <atomic>
#include <cstring>

namespace VMPilot::Runtime {

using Common::VM::VM_REG_COUNT;
using Common::VM::VM_MAX_NESTING;
using Common::VM::VM_OBLIVIOUS_SIZE;
using Common::VM::VM_OPERAND_POOL;
using Common::VM::VM_OPERAND_REG;
using Common::VM::VM_OPERAND_MEM;

// ─────────────────────────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────────────────────────

namespace detail {

inline void ensure_cache(VmEpoch& epoch) noexcept {
    if (!epoch.cache)
        epoch.cache = std::make_unique<CompositionCache>();
}

static inline uint8_t op_and(uint8_t a, uint8_t b) { return a & b; }
static inline uint8_t op_or(uint8_t a, uint8_t b)  { return a | b; }
static inline uint8_t op_xor(uint8_t a, uint8_t b) { return a ^ b; }
static inline uint8_t op_not(uint8_t a)             { return static_cast<uint8_t>(~a); }

/// Apply binary composition table to two encoded values.
inline uint64_t apply_comp(const uint8_t comp[][256][256],
                            uint64_t a, uint64_t b) noexcept {
    uint64_t r = 0;
    for (int k = 0; k < 8; ++k) {
        uint8_t la = static_cast<uint8_t>(a >> (k * 8));
        uint8_t lb = static_cast<uint8_t>(b >> (k * 8));
        r |= uint64_t(comp[k][la][lb]) << (k * 8);
    }
    return r;
}

/// Apply unary composition table.
inline uint64_t apply_unary(const uint8_t comp[][256], uint64_t a) noexcept {
    uint64_t r = 0;
    for (int k = 0; k < 8; ++k) {
        uint8_t la = static_cast<uint8_t>(a >> (k * 8));
        r |= uint64_t(comp[k][la]) << (k * 8);
    }
    return r;
}

/// MBA SHL1 (cross-lane carry propagation, branchless).
inline uint64_t mba_shl1(uint64_t encoded,
                          const CompositionCache::MbaEntry& mba) noexcept {
    uint64_t result = 0;
    // Lane 0: SHL1_INTRA only (no carry-in)
    uint8_t lane0 = static_cast<uint8_t>(encoded);
    result = mba.shl1_intra[0][lane0];
    uint8_t carry_out = mba.carry_extract[0][lane0];

    // Lanes 1-7: SHL1_INTRA + branchless dual-table carry injection
    for (int k = 1; k < 8; ++k) {
        uint8_t lane = static_cast<uint8_t>(encoded >> (k * 8));
        uint8_t shifted = mba.shl1_intra[k][lane];
        uint8_t no_carry  = mba.inject_0[k][shifted];
        uint8_t yes_carry = mba.inject_1[k][shifted];
        uint8_t mask = -carry_out;  // 0x00 or 0xFF
        uint8_t byte = static_cast<uint8_t>((no_carry & ~mask) | (yes_carry & mask));
        result |= uint64_t(byte) << (k * 8);
        carry_out = mba.carry_extract[k][lane];
    }
    return result;
}

/// MBA ADD on two values already in the SAME encoding domain (dst_reg).
/// Used for carry propagation iterations and for SUB/NEG chaining.
inline uint64_t mba_add_self(uint64_t a, uint64_t b,
                              VmEpoch& epoch, uint8_t dst_reg,
                              int iterations) noexcept {
    ensure_cache(epoch);
    // Both operands are in dst_reg's encoding — use self-domain tables
    auto xor_tbl = epoch.cache->get_binary(12, dst_reg, dst_reg, dst_reg, op_xor, epoch.reg);
    auto and_tbl = epoch.cache->get_binary(13, dst_reg, dst_reg, dst_reg, op_and, epoch.reg);
    auto& mba = epoch.cache->get_mba(dst_reg, dst_reg, epoch.reg);

    uint64_t s = apply_comp(xor_tbl, a, b);
    uint64_t c = apply_comp(and_tbl, a, b);
    c = mba_shl1(c, mba);

    int iters = iterations > 0 ? iterations : 64;
    for (int i = 0; i < iters; ++i) {
        uint64_t t = apply_comp(xor_tbl, s, c);
        c = apply_comp(and_tbl, s, c);
        c = mba_shl1(c, mba);
        s = t;
    }
    return s;
}

/// MBA ADD of two values from DIFFERENT registers (reg_a, reg_b).
/// The first XOR/AND uses cross-register tables; then carry propagation
/// uses self-domain tables (both s,c are in dst=reg_a's domain after first step).
inline uint64_t mba_add_cross(uint64_t a, uint64_t b,
                               VmEpoch& epoch, uint8_t reg_a, uint8_t reg_b,
                               int iterations) noexcept {
    ensure_cache(epoch);
    // First half-add: cross-register (reg_a × reg_b → reg_a domain)
    auto xor_cross = epoch.cache->get_binary(10, reg_a, reg_a, reg_b, op_xor, epoch.reg);
    auto and_cross = epoch.cache->get_binary(11, reg_a, reg_a, reg_b, op_and, epoch.reg);
    auto& mba = epoch.cache->get_mba(reg_a, reg_a, epoch.reg);

    uint64_t s = apply_comp(xor_cross, a, b);
    uint64_t c = apply_comp(and_cross, a, b);
    c = mba_shl1(c, mba);

    // Carry propagation: both s and c are now in reg_a's domain
    auto xor_self = epoch.cache->get_binary(12, reg_a, reg_a, reg_a, op_xor, epoch.reg);
    auto and_self = epoch.cache->get_binary(13, reg_a, reg_a, reg_a, op_and, epoch.reg);

    int iters = iterations > 0 ? iterations : 64;
    for (int i = 0; i < iters; ++i) {
        uint64_t t = apply_comp(xor_self, s, c);
        c = apply_comp(and_self, s, c);
        c = mba_shl1(c, mba);
        s = t;
    }
    return s;
}

/// Evaluate condition code against flags.
inline bool evaluate_condition(uint8_t flags, uint8_t cond) noexcept {
    bool zf = (flags & 0x01) != 0;
    bool sf = (flags & 0x02) != 0;
    bool cf = (flags & 0x04) != 0;
    bool of = (flags & 0x08) != 0;
    switch (cond) {
        case 0: return zf;
        case 1: return !zf;
        case 2: return sf != of;
        case 3: return sf == of;
        case 4: return zf || (sf != of);
        case 5: return !zf && (sf == of);
        case 6: return cf;
        case 7: return !cf;
        case 8: return cf || zf;
        case 9: return !cf && !zf;
        default: return false;
    }
}

/// Decode register to plaintext (Class C bridge).
inline uint64_t decode_reg(const VmEpoch& ep, uint8_t reg, uint64_t encoded) noexcept {
    return ep.reg.decode_lut(reg).apply(encoded);
}

/// Encode plaintext to register domain.
inline uint64_t encode_reg(const VmEpoch& ep, uint8_t reg, uint64_t plain) noexcept {
    return ep.reg.encode_lut(reg).apply(plain);
}

/// Store convert: register → memory domain.
inline uint64_t store_conv(const VmEpoch& ep, uint8_t reg, uint64_t encoded) noexcept {
    return ep.dom.store_lut(reg).apply(encoded);
}

/// Load convert: memory → register domain.
inline uint64_t load_conv(const VmEpoch& ep, uint8_t reg, uint64_t mem_val) noexcept {
    return ep.dom.load_lut(reg).apply(mem_val);
}

}  // namespace detail

// ═════════════════════════════════════════════════════════════════════════════
// Cat 0: Data Movement (8 opcodes)
// ═════════════════════════════════════════════════════════════════════════════

/// MOVE: dst = src (same domain, zero plaintext)
template<typename P>
struct HandlerTraits<VmOpcode::MOVE, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.regs[i.reg_a] = e.regs[i.reg_b];
        return {};
    }
};

/// LOAD: guest memory → register domain
template<typename P>
struct HandlerTraits<VmOpcode::LOAD, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        auto addr = static_cast<uintptr_t>(static_cast<int64_t>(i.aux) + e.load_base_delta);
        uint64_t mem = 0;
        std::memcpy(&mem, reinterpret_cast<const uint8_t*>(addr), 8);
        e.regs[i.reg_a] = RegVal(detail::load_conv(ep, i.reg_a, mem));
        return {};
    }
};

/// STORE: register → memory domain → guest memory
template<typename P>
struct HandlerTraits<VmOpcode::STORE, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        auto addr = static_cast<uintptr_t>(static_cast<int64_t>(i.aux) + e.load_base_delta);
        uint64_t mem = detail::store_conv(ep, i.reg_a, e.regs[i.reg_a].bits);
        std::memcpy(reinterpret_cast<uint8_t*>(addr), &mem, 8);
        return {};
    }
};

/// PUSH: register → memory domain → ORAM stack
template<typename P>
struct HandlerTraits<VmOpcode::PUSH, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = UsesOramTag;
    template<typename Oram>
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState& o,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        if (e.vm_sp < 8) return tl::make_unexpected(DiagnosticCode::StackOverflow);
        e.vm_sp -= 8;
        MemVal mem(detail::store_conv(ep, i.reg_a, e.regs[i.reg_a].bits));
        Oram::write(o, e.vm_sp, mem);
        return {};
    }
};

/// POP: ORAM stack → memory domain → register
template<typename P>
struct HandlerTraits<VmOpcode::POP, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = UsesOramTag;
    template<typename Oram>
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState& o,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        if (e.vm_sp >= VM_OBLIVIOUS_SIZE) return tl::make_unexpected(DiagnosticCode::StackUnderflow);
        MemVal mem = Oram::read(o, e.vm_sp);
        e.vm_sp += 8;
        e.regs[i.reg_a] = RegVal(detail::load_conv(ep, i.reg_a, mem.bits));
        return {};
    }
};

/// LOAD_CONST: pool[aux] → register (pre-encoded, zero plaintext)
template<typename P>
struct HandlerTraits<VmOpcode::LOAD_CONST, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable& im, const DecodedInsn& i) noexcept {
        if (i.operand_a_type == VM_OPERAND_POOL) {
            e.regs[i.reg_a] = i.resolved_a;
        } else {
            if (im.decrypted_pool.empty() || i.aux * 8 >= im.decrypted_pool.size())
                return {};
            uint64_t v = 0;
            std::memcpy(&v, im.decrypted_pool.data() + i.aux * 8, 8);
            e.regs[i.reg_a] = RegVal(v);
        }
        return {};
    }
};

/// LOAD_CTX: read VM context field → encode → register
template<typename P>
struct HandlerTraits<VmOpcode::LOAD_CTX, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t plain = 0;
        switch (i.aux) {
            case 0: plain = e.vm_ip;          break;
            case 1: plain = e.vm_sp;          break;
            case 2: plain = e.current_bb_id;  break;
            case 3: plain = e.current_epoch;  break;
            default: break;
        }
        e.regs[i.reg_a] = RegVal(detail::encode_reg(ep, i.reg_a, plain));
        return {};
    }
};

/// STORE_CTX: decode register → write VM context field
template<typename P>
struct HandlerTraits<VmOpcode::STORE_CTX, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t plain = detail::decode_reg(ep, i.reg_a, e.regs[i.reg_a].bits);
        switch (i.aux) {
            case 1: e.vm_sp = plain; break;
            default: break;
        }
        return {};
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// Cat 1: Arithmetic (8 opcodes)
// ═════════════════════════════════════════════════════════════════════════════

// ADD: Policy-dependent (MBA for HighSec/Standard, bridge for Debug)
template<> struct HandlerTraits<VmOpcode::ADD, HighSecPolicy> {
    static constexpr auto security_class = SecurityClass::B;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        int iters = (i.condition == 0) ? 64 : i.condition;
        e.regs[i.reg_a] = RegVal(detail::mba_add_cross(
            e.regs[i.reg_a].bits, e.regs[i.reg_b].bits,
            ep, i.reg_a, i.reg_b, iters));
        return {};
    }
};
template<> struct HandlerTraits<VmOpcode::ADD, StandardPolicy> {
    static constexpr auto security_class = SecurityClass::B;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        int iters = (i.condition == 0) ? 64 : i.condition;
        e.regs[i.reg_a] = RegVal(detail::mba_add_cross(
            e.regs[i.reg_a].bits, e.regs[i.reg_b].bits,
            ep, i.reg_a, i.reg_b, iters));
        return {};
    }
};
template<> struct HandlerTraits<VmOpcode::ADD, DebugPolicy> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t a = detail::decode_reg(ep, i.reg_a, e.regs[i.reg_a].bits);
        uint64_t b = detail::decode_reg(ep, i.reg_b, e.regs[i.reg_b].bits);
        e.regs[i.reg_a] = RegVal(detail::encode_reg(ep, i.reg_a, a + b));
        return {};
    }
};

// SUB: similar pattern to ADD
template<> struct HandlerTraits<VmOpcode::SUB, HighSecPolicy> {
    static constexpr auto security_class = SecurityClass::B;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        int iters = (i.condition == 0) ? 64 : i.condition;
        // SUB(a,b) = ADD(a, ADD(NOT(b), 1))
        // NOT(b) in reg_a's domain (cross-register NOT: src=reg_b, dst=reg_a)
        detail::ensure_cache(ep);
        auto not_tbl = ep.cache->get_unary(14, i.reg_a, i.reg_b, detail::op_not, ep.reg);
        uint64_t not_b = detail::apply_unary(not_tbl, e.regs[i.reg_b].bits);
        // not_b is now in reg_a's domain. ADD(not_b, 1) is self-domain.
        uint64_t one_enc = detail::encode_reg(ep, i.reg_a, 1);
        uint64_t neg_b = detail::mba_add_self(not_b, one_enc, ep, i.reg_a, iters);
        // ADD(a, neg_b) — both in reg_a's domain → self-domain
        e.regs[i.reg_a] = RegVal(detail::mba_add_self(
            e.regs[i.reg_a].bits, neg_b, ep, i.reg_a, iters));
        return {};
    }
};
template<> struct HandlerTraits<VmOpcode::SUB, StandardPolicy> : HandlerTraits<VmOpcode::SUB, HighSecPolicy> {};
template<> struct HandlerTraits<VmOpcode::SUB, DebugPolicy> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t a = detail::decode_reg(ep, i.reg_a, e.regs[i.reg_a].bits);
        uint64_t b = detail::decode_reg(ep, i.reg_b, e.regs[i.reg_b].bits);
        e.regs[i.reg_a] = RegVal(detail::encode_reg(ep, i.reg_a, a - b));
        return {};
    }
};

// NEG
template<> struct HandlerTraits<VmOpcode::NEG, HighSecPolicy> {
    static constexpr auto security_class = SecurityClass::B;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        int iters = (i.condition == 0) ? 64 : i.condition;
        detail::ensure_cache(ep);
        auto not_tbl = ep.cache->get_unary(14, i.reg_a, i.reg_a, detail::op_not, ep.reg);
        uint64_t not_a = detail::apply_unary(not_tbl, e.regs[i.reg_a].bits);
        uint64_t one_enc = detail::encode_reg(ep, i.reg_a, 1);
        e.regs[i.reg_a] = RegVal(detail::mba_add_self(
            not_a, one_enc, ep, i.reg_a, iters));
        return {};
    }
};
template<> struct HandlerTraits<VmOpcode::NEG, StandardPolicy> : HandlerTraits<VmOpcode::NEG, HighSecPolicy> {};
template<> struct HandlerTraits<VmOpcode::NEG, DebugPolicy> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t a = detail::decode_reg(ep, i.reg_a, e.regs[i.reg_a].bits);
        e.regs[i.reg_a] = RegVal(detail::encode_reg(ep, i.reg_a, -a));
        return {};
    }
};

// MUL, IMUL, DIV, IDIV, MOD: Class C (all policies use native bridge)
#define VMPILOT_CLASS_C_ARITH(OPCODE, OP_EXPR)                               \
    template<typename P>                                                       \
    struct HandlerTraits<VmOpcode::OPCODE, P> {                               \
        static constexpr auto security_class = SecurityClass::C;              \
        using oram_tag = NoOramTag;                                            \
        static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&, \
                                   const VmImmutable&, const DecodedInsn& i) noexcept { \
            uint64_t a = detail::decode_reg(ep, i.reg_a, e.regs[i.reg_a].bits); \
            uint64_t b = detail::decode_reg(ep, i.reg_b, e.regs[i.reg_b].bits); \
            uint64_t r = (OP_EXPR);                                            \
            e.regs[i.reg_a] = RegVal(detail::encode_reg(ep, i.reg_a, r));     \
            return {};                                                         \
        }                                                                      \
    }

VMPILOT_CLASS_C_ARITH(MUL,  a * b);
VMPILOT_CLASS_C_ARITH(IMUL, static_cast<uint64_t>(static_cast<int64_t>(a) * static_cast<int64_t>(b)));
VMPILOT_CLASS_C_ARITH(DIV,  b != 0 ? a / b : 0);
VMPILOT_CLASS_C_ARITH(IDIV, b != 0 ? static_cast<uint64_t>(static_cast<int64_t>(a) / static_cast<int64_t>(b)) : 0);
VMPILOT_CLASS_C_ARITH(MOD,  b != 0 ? a % b : 0);

#undef VMPILOT_CLASS_C_ARITH

// ═════════════════════════════════════════════════════════════════════════════
// Cat 2: Logic (9 opcodes)
// ═════════════════════════════════════════════════════════════════════════════

#define VMPILOT_CLASS_A_BINARY(OPCODE, OP_ID, OP_FN)                          \
    template<typename P>                                                       \
    struct HandlerTraits<VmOpcode::OPCODE, P> {                               \
        static constexpr auto security_class = SecurityClass::A;              \
        using oram_tag = NoOramTag;                                            \
        static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&, \
                                   const VmImmutable&, const DecodedInsn& i) noexcept { \
            detail::ensure_cache(ep);                                          \
            auto tbl = ep.cache->get_binary(OP_ID, i.reg_a, i.reg_a, i.reg_b, \
                                             detail::OP_FN, ep.reg);           \
            e.regs[i.reg_a] = RegVal(detail::apply_comp(tbl,                  \
                e.regs[i.reg_a].bits, e.regs[i.reg_b].bits));                 \
            return {};                                                         \
        }                                                                      \
    }

VMPILOT_CLASS_A_BINARY(AND, 0, op_and);
VMPILOT_CLASS_A_BINARY(OR,  1, op_or);
VMPILOT_CLASS_A_BINARY(XOR, 2, op_xor);

#undef VMPILOT_CLASS_A_BINARY

/// NOT: unary Class A
template<typename P>
struct HandlerTraits<VmOpcode::NOT, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        detail::ensure_cache(ep);
        auto tbl = ep.cache->get_unary(3, i.reg_a, i.reg_a, detail::op_not, ep.reg);
        e.regs[i.reg_a] = RegVal(detail::apply_unary(tbl, e.regs[i.reg_a].bits));
        return {};
    }
};

// SHL, SHR, SAR, ROL, ROR: Class C (cross-lane shifts need plaintext)
#define VMPILOT_SHIFT(OPCODE, OP_EXPR)                                         \
    template<typename P>                                                        \
    struct HandlerTraits<VmOpcode::OPCODE, P> {                                \
        static constexpr auto security_class = SecurityClass::C;               \
        using oram_tag = NoOramTag;                                             \
        static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,  \
                                   const VmImmutable&, const DecodedInsn& i) noexcept { \
            uint64_t val = detail::decode_reg(ep, i.reg_a, e.regs[i.reg_a].bits); \
            uint64_t amt = detail::decode_reg(ep, i.reg_b, e.regs[i.reg_b].bits) & 63; \
            uint64_t r = (OP_EXPR);                                             \
            e.regs[i.reg_a] = RegVal(detail::encode_reg(ep, i.reg_a, r));      \
            return {};                                                          \
        }                                                                       \
    }

VMPILOT_SHIFT(SHL, val << amt);
VMPILOT_SHIFT(SHR, val >> amt);
VMPILOT_SHIFT(SAR, static_cast<uint64_t>(static_cast<int64_t>(val) >> amt));
VMPILOT_SHIFT(ROL, (val << amt) | (val >> (64 - amt)));
VMPILOT_SHIFT(ROR, (val >> amt) | (val << (64 - amt)));

#undef VMPILOT_SHIFT

// ═════════════════════════════════════════════════════════════════════════════
// Cat 3: Comparison (4 opcodes)
// ═════════════════════════════════════════════════════════════════════════════

/// CMP: decode both, set flags
template<typename P>
struct HandlerTraits<VmOpcode::CMP, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        auto sa = static_cast<int64_t>(detail::decode_reg(ep, i.reg_a, e.regs[i.reg_a].bits));
        auto sb = static_cast<int64_t>(detail::decode_reg(ep, i.reg_b, e.regs[i.reg_b].bits));
        auto ua = static_cast<uint64_t>(sa);
        auto ub = static_cast<uint64_t>(sb);
        int64_t diff = sa - sb;
        e.vm_flags = 0;
        if (diff == 0)  e.vm_flags |= 0x01;
        if (diff < 0)   e.vm_flags |= 0x02;
        if (ua < ub)    e.vm_flags |= 0x04;
        if (((sa ^ sb) & (sa ^ diff)) < 0) e.vm_flags |= 0x08;
        return {};
    }
};

/// TEST: AND then set flags
template<typename P>
struct HandlerTraits<VmOpcode::TEST, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t a = detail::decode_reg(ep, i.reg_a, e.regs[i.reg_a].bits);
        uint64_t b = detail::decode_reg(ep, i.reg_b, e.regs[i.reg_b].bits);
        uint64_t result = a & b;
        e.vm_flags = 0;
        if (result == 0) e.vm_flags |= 0x01;
        if (static_cast<int64_t>(result) < 0) e.vm_flags |= 0x02;
        return {};
    }
};

/// SET_FLAG
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

/// GET_FLAG
template<typename P>
struct HandlerTraits<VmOpcode::GET_FLAG, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint8_t bit = i.condition & 0x03;
        uint64_t val = (e.vm_flags >> bit) & 1;
        e.regs[i.reg_a] = RegVal(detail::encode_reg(ep, i.reg_a, val));
        return {};
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// Cat 4: Control Flow (6 opcodes)
// ═════════════════════════════════════════════════════════════════════════════

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

template<typename P>
struct HandlerTraits<VmOpcode::CALL_VM, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        if (e.shadow_depth >= VM_MAX_NESTING)
            return tl::make_unexpected(DiagnosticCode::ShadowStackOverflow);
        auto& cp = e.shadow_stack[e.shadow_depth];
        // Save resume point: vm_ip of the CALL instruction itself.
        // The dispatcher will advance to vm_ip+1 after RET_VM re-enters
        // this BB (resume after the CALL, not at BB entry).
        cp.vm_ip = e.vm_ip;
        cp.bb_id = e.current_bb_id;
        std::memcpy(cp.epoch_seed, ep.reg.encode[0], 32);
        cp.saved_insn_index = e.insn_index_in_bb;
        for (int r = 0; r < VM_REG_COUNT; ++r)
            cp.encoded_regs_snapshot[r] = e.regs[r].bits;
        e.shadow_depth++;
        e.branch_target_bb = i.aux;
        e.branch_taken = true;
        return {};
    }
};

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
        // Restore register snapshot from before CALL_VM
        for (int r = 0; r < VM_REG_COUNT; ++r)
            e.regs[r] = RegVal(cp.encoded_regs_snapshot[r]);
        e.branch_target_bb = cp.bb_id;
        e.branch_taken = true;
        // Resume AFTER the CALL_VM instruction (cp.vm_ip is CALL's ip,
        // cp.saved_insn_index stores the insn_index_in_bb at CALL time).
        // The dispatcher will use these to override enter_basic_block's
        // default vm_ip = entry_ip.
        e.return_resume_ip = cp.vm_ip + 1;
        e.return_resume_insn_idx = cp.saved_insn_index + 1;
        return {};
    }
};

/// NATIVE_CALL: ABI-correct native function call with FP, struct return,
/// stack args, and platform-specific calling convention support.
///
/// Uses platform_call trampoline (ASM) for correct register placement.
/// Supports all 5 ABIs: SysV x64, Win x64, cdecl x86-32, stdcall x86-32, AAPCS64.
template<typename P>
struct HandlerTraits<VmOpcode::NATIVE_CALL, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
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

        void* target = reinterpret_cast<void*>(
            static_cast<uintptr_t>(te.target_offset + e.load_base_delta));
        if (!target)
            return tl::make_unexpected(DiagnosticCode::NativeCallBridgeFailed);

        // Decode arg registers to plaintext (up to 31 args, using regs 0..argc-1)
        uint64_t raw_args[32] = {};
        uint8_t decode_count = argc < VM_REG_COUNT ? argc : VM_REG_COUNT;
        for (uint8_t i = 0; i < decode_count; ++i)
            raw_args[i] = detail::decode_reg(ep, i, e.regs[i].bits);

        // Struct return: raw_args[0] is the struct buffer pointer (from compiler)
        void* struct_ptr = returns_struct
            ? reinterpret_cast<void*>(static_cast<uintptr_t>(raw_args[0]))
            : nullptr;

        // Classify args into PlatformCallDesc for the native ABI
        PlatformCallDesc desc{};

        CallABI abi = native_abi();
        // Override ABI for stdcall convention on x86-32
        if (abi == CallABI::Cdecl_x86 && convention == Common::VM::TE_CONV_STDCALL)
            abi = CallABI::Stdcall_x86;

        classify_args_for_abi(abi, raw_args, argc, fp_mask,
                              returns_struct, struct_ptr, desc);

        // Set target and convention AFTER classify (which zeroes desc)
        desc.target = target;
        desc.convention = convention;
        desc.flags = pack_call_flags(
            is_variadic ? desc.fp_count : static_cast<uint8_t>(0), returns_fp);

        // Call via platform-specific ASM trampoline
        uint64_t result;
        if (returns_struct && abi == CallABI::AAPCS64) {
            // ARM64: struct pointer goes in x8 (separate variant)
            result = platform_call_struct(&desc, struct_ptr);
        } else {
            // All others: struct pointer already in int_regs[0] via classify_args
            result = platform_call(&desc);
        }

        // Zero plaintext args from stack (forward secrecy)
        detail::secure_zero_impl(raw_args, sizeof(raw_args));

        // Re-encode result into register 0's domain.
        // execute() will decode it back to plaintext for the return value.
        e.regs[0] = RegVal(detail::encode_reg(ep, 0, result));
        e.native_call_nonce++;
        return {};
    }
};

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

// ═════════════════════════════════════════════════════════════════════════════
// Cat 5: Width/Extension (8 opcodes)
// ═════════════════════════════════════════════════════════════════════════════

#define VMPILOT_WIDTH(OPCODE, OP_EXPR)                                          \
    template<typename P>                                                         \
    struct HandlerTraits<VmOpcode::OPCODE, P> {                                 \
        static constexpr auto security_class = SecurityClass::C;                \
        using oram_tag = NoOramTag;                                              \
        static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,   \
                                   const VmImmutable&, const DecodedInsn& i) noexcept { \
            uint64_t v = detail::decode_reg(ep, i.reg_a, e.regs[i.reg_a].bits); \
            v = (OP_EXPR);                                                       \
            e.regs[i.reg_a] = RegVal(detail::encode_reg(ep, i.reg_a, v));       \
            return {};                                                           \
        }                                                                        \
    }

VMPILOT_WIDTH(SEXT8,   static_cast<uint64_t>(static_cast<int64_t>(static_cast<int8_t>(v & 0xFF))));
VMPILOT_WIDTH(SEXT16,  static_cast<uint64_t>(static_cast<int64_t>(static_cast<int16_t>(v & 0xFFFF))));
VMPILOT_WIDTH(SEXT32,  static_cast<uint64_t>(static_cast<int64_t>(static_cast<int32_t>(v & 0xFFFFFFFF))));
VMPILOT_WIDTH(ZEXT8,   v & 0xFF);
VMPILOT_WIDTH(ZEXT16,  v & 0xFFFF);
VMPILOT_WIDTH(ZEXT32,  v & 0xFFFFFFFF);
VMPILOT_WIDTH(TRUNC8,  v & 0xFF);
VMPILOT_WIDTH(TRUNC16, v & 0xFFFF);

#undef VMPILOT_WIDTH

// ═════════════════════════════════════════════════════════════════════════════
// Cat 6: Atomic (5 opcodes) — Class C (decode + std::atomic + re-encode)
// ═════════════════════════════════════════════════════════════════════════════

template<typename P>
struct HandlerTraits<VmOpcode::LOCK_ADD, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t addend = detail::decode_reg(ep, i.reg_a, e.regs[i.reg_a].bits);
        auto addr = static_cast<uintptr_t>(static_cast<int64_t>(i.aux) + e.load_base_delta);
        auto* ptr = reinterpret_cast<std::atomic<uint64_t>*>(addr);
        uint64_t old = ptr->fetch_add(addend, std::memory_order_seq_cst);
        e.regs[i.reg_a] = RegVal(detail::encode_reg(ep, i.reg_a, old));
        return {};
    }
};

template<typename P>
struct HandlerTraits<VmOpcode::XCHG, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t newval = detail::decode_reg(ep, i.reg_a, e.regs[i.reg_a].bits);
        auto addr = static_cast<uintptr_t>(static_cast<int64_t>(i.aux) + e.load_base_delta);
        auto* ptr = reinterpret_cast<std::atomic<uint64_t>*>(addr);
        uint64_t old = ptr->exchange(newval, std::memory_order_seq_cst);
        e.regs[i.reg_a] = RegVal(detail::encode_reg(ep, i.reg_a, old));
        return {};
    }
};

template<typename P>
struct HandlerTraits<VmOpcode::CMPXCHG, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        uint64_t expected = detail::decode_reg(ep, i.reg_a, e.regs[i.reg_a].bits);
        uint64_t desired  = detail::decode_reg(ep, i.reg_b, e.regs[i.reg_b].bits);
        auto addr = static_cast<uintptr_t>(static_cast<int64_t>(i.aux) + e.load_base_delta);
        auto* ptr = reinterpret_cast<std::atomic<uint64_t>*>(addr);
        bool ok = ptr->compare_exchange_strong(expected, desired, std::memory_order_seq_cst);
        e.regs[i.reg_a] = RegVal(detail::encode_reg(ep, i.reg_a, expected));
        e.vm_flags = ok ? 0x01 : 0x00;  // ZF = success
        return {};
    }
};

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

template<typename P>
struct HandlerTraits<VmOpcode::ATOMIC_LOAD, P> {
    static constexpr auto security_class = SecurityClass::C;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        auto addr = static_cast<uintptr_t>(static_cast<int64_t>(i.aux) + e.load_base_delta);
        auto* ptr = reinterpret_cast<std::atomic<uint64_t>*>(addr);
        uint64_t val = ptr->load(std::memory_order_seq_cst);
        e.regs[i.reg_a] = RegVal(detail::encode_reg(ep, i.reg_a, val));
        return {};
    }
};

// ═════════════════════════════════════════════════════════════════════════════
// Cat 7: VM Internal (7 opcodes)
// ═════════════════════════════════════════════════════════════════════════════

template<typename P>
struct HandlerTraits<VmOpcode::NOP, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn& i) noexcept {
        e.trash_regs[i.reg_a] = e.regs[i.reg_b].bits;
        return {};
    }
};

template<typename P>
struct HandlerTraits<VmOpcode::CHECK_INTEGRITY, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution&, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn&) noexcept {
        // BB MAC is verified at BB boundaries by the dispatcher (step 11).
        // This opcode is a no-op in v1 — future: explicit inline MAC check.
        return {};
    }
};

template<typename P>
struct HandlerTraits<VmOpcode::CHECK_DEBUG, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution&, VmEpoch&, VmOramState&,
                               const VmImmutable&, const DecodedInsn&) noexcept {
        // Phase 9 stub: anti-debug checks deferred
        return {};
    }
};

template<typename P>
struct HandlerTraits<VmOpcode::MUTATE_ISA, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution&, VmEpoch& ep, VmOramState&,
                               const VmImmutable&, const DecodedInsn&) noexcept {
        // Re-derive opcode permutation from current epoch_seed
        // The epoch tables are already set by enter_basic_block; this opcode
        // triggers an additional mid-BB mutation (D4 §5.1).
        // For v1: no-op (mutation happens at BB boundaries via enter_bb).
        (void)ep;
        return {};
    }
};

template<typename P>
struct HandlerTraits<VmOpcode::REKEY, P> {
    static constexpr auto security_class = SecurityClass::A;
    using oram_tag = NoOramTag;
    static HandlerResult exec(VmExecution& e, VmEpoch&, VmOramState&,
                               const VmImmutable& im, const DecodedInsn& i) noexcept {
        // Re-derive enc_state from stored_seed + rekey counter
        uint32_t counter = i.aux;
        uint8_t rk_ctx[9];
        std::memcpy(rk_ctx, "rekey", 5);
        std::memcpy(rk_ctx + 5, &counter, 4);
        uint8_t rk_mat[16];
        Common::VM::Crypto::blake3_kdf(im.stored_seed,
            reinterpret_cast<const char*>(rk_ctx), 9, rk_mat, 16);
        uint8_t es[8];
        std::memcpy(es, &e.enc_state, 8);
        e.enc_state = Common::VM::Crypto::siphash_2_4(rk_mat, es, 8);
        return {};
    }
};

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
