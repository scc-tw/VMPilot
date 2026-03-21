#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace VMPilot::SDK::ReferenceAnalyzer {

/// Symbolic expression representing the provenance of a register value.
/// Used by the SymbolicResolver for backward data-flow analysis.
///
/// Node types form a CLOSED set -- they represent fundamental data-flow
/// effects, not ISA-specific operations:
///   Unknown    -- cannot resolve
///   Constant   -- known concrete value
///   BinaryOp   -- lhs OP rhs (arithmetic/bitwise/shift)
///   Extend     -- sign/zero extend or truncate
///   Load       -- memory load: value at address children[0]
///   Opaque     -- system register, segment base, call result
struct SymExpr {
    enum class Kind : uint8_t {
        Unknown,
        Constant,
        BinaryOp,
        Extend,
        Load,
        Opaque,
    };

    /// Binary operators -- mathematical primitives (CLOSED set of 9).
    enum class BinOp : uint8_t {
        Add, Sub, Mul,      // Arithmetic
        And, Or, Xor,       // Bitwise
        Shl, Lsr, Asr,      // Shift (logical shift right, arithmetic shift right)
    };

    enum class ExtOp : uint8_t { SExt, ZExt, Trunc };

    enum class OpaqueKind : uint8_t {
        SystemReg,     // ARM64: MRS (e.g. TPIDR_EL0)
        SegmentBase,   // x86: FS / GS segment base
        CallResult,    // Return value of a function call
    };

    Kind kind = Kind::Unknown;

    // --- Per-kind payload ---
    BinOp bin_op = BinOp::Add;
    ExtOp ext_op = ExtOp::ZExt;
    OpaqueKind opaque_kind = OpaqueKind::SystemReg;
    uint64_t value = 0;           // Constant value
    unsigned reg_id = 0;          // OpaqueSource: segment/sysreg ID
    uint8_t bit_width = 0;        // Extend: target width in bits
    std::string symbol;           // CallResult: function name
    std::vector<SymExpr> children; // BinaryOp: [lhs, rhs], Load: [addr], Extend: [src]

    // === Factory methods ===
    static SymExpr unknown();
    static SymExpr constant(uint64_t v);
    static SymExpr binop(BinOp op, SymExpr lhs, SymExpr rhs);  // with constant folding
    static SymExpr load(SymExpr addr);
    static SymExpr extend(ExtOp op, SymExpr src, uint8_t width);
    static SymExpr opaque(OpaqueKind kind, unsigned id = 0, std::string symbol = {});

    // === Queries (for pattern detection in PatternLayer) ===

    bool isConstant() const { return kind == Kind::Constant; }
    bool isUnknown() const { return kind == Kind::Unknown; }

    /// Try to evaluate to a concrete value. Returns nullopt if symbolic.
    std::optional<uint64_t> tryEval() const;

    /// Does this expression involve a MemLoad at any depth?
    bool involvesLoad() const;

    /// Does this expression involve an opaque source of a specific kind?
    bool involvesOpaque(OpaqueKind k) const;

    /// Does this expression involve a call to a specific function?
    bool involvesCall(const std::string& name) const;

    /// Find the innermost Load node's address expression.
    const SymExpr* findLoadAddr() const;

    /// Find the first Opaque node of the given kind.
    const SymExpr* findOpaque(OpaqueKind k) const;
};

}  // namespace VMPilot::SDK::ReferenceAnalyzer
