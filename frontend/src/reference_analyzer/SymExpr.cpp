#include "SymExpr.hpp"

#include <algorithm>

namespace VMPilot::SDK::ReferenceAnalyzer {

// === Factory methods ===

SymExpr SymExpr::unknown() {
    SymExpr e;
    e.kind = Kind::Unknown;
    return e;
}

SymExpr SymExpr::constant(uint64_t v) {
    SymExpr e;
    e.kind = Kind::Constant;
    e.value = v;
    return e;
}

SymExpr SymExpr::binop(BinOp op, SymExpr lhs, SymExpr rhs) {
    // Constant folding: if both children are constants, evaluate immediately.
    if (lhs.isConstant() && rhs.isConstant()) {
        uint64_t l = lhs.value;
        uint64_t r = rhs.value;
        uint64_t result = 0;
        switch (op) {
            case BinOp::Add: result = l + r; break;
            case BinOp::Sub: result = l - r; break;
            case BinOp::Mul: result = l * r; break;
            case BinOp::And: result = l & r; break;
            case BinOp::Or:  result = l | r; break;
            case BinOp::Xor: result = l ^ r; break;
            case BinOp::Shl: result = l << r; break;
            case BinOp::Lsr: result = l >> r; break;
            case BinOp::Asr:
                result = static_cast<uint64_t>(
                    static_cast<int64_t>(l) >> r);
                break;
        }
        return constant(result);
    }

    // Algebraic simplification: x + 0 = x
    if (op == BinOp::Add && rhs.isConstant() && rhs.value == 0)
        return lhs;
    // Algebraic simplification: 0 + x = x
    if (op == BinOp::Add && lhs.isConstant() && lhs.value == 0)
        return rhs;

    // Algebraic simplification: x - 0 = x
    if (op == BinOp::Sub && rhs.isConstant() && rhs.value == 0)
        return lhs;

    // Algebraic simplification: x * 1 = x
    if (op == BinOp::Mul && rhs.isConstant() && rhs.value == 1)
        return lhs;
    // Algebraic simplification: 1 * x = x
    if (op == BinOp::Mul && lhs.isConstant() && lhs.value == 1)
        return rhs;

    // Algebraic simplification: x * 0 = 0
    if (op == BinOp::Mul && rhs.isConstant() && rhs.value == 0)
        return constant(0);
    // Algebraic simplification: 0 * x = 0
    if (op == BinOp::Mul && lhs.isConstant() && lhs.value == 0)
        return constant(0);

    SymExpr e;
    e.kind = Kind::BinaryOp;
    e.bin_op = op;
    e.children.reserve(2);
    e.children.push_back(std::move(lhs));
    e.children.push_back(std::move(rhs));
    return e;
}

SymExpr SymExpr::load(SymExpr addr) {
    SymExpr e;
    e.kind = Kind::Load;
    e.children.reserve(1);
    e.children.push_back(std::move(addr));
    return e;
}

SymExpr SymExpr::extend(ExtOp op, SymExpr src, uint8_t width) {
    // Fold constants: SExt/ZExt/Trunc of a constant = computed constant.
    if (src.isConstant()) {
        uint64_t v = src.value;
        switch (op) {
            case ExtOp::ZExt:
                // Zero-extend: mask to width bits (if width < 64)
                if (width < 64)
                    v = v & ((uint64_t{1} << width) - 1);
                return constant(v);
            case ExtOp::SExt: {
                // Sign-extend from the current bit representation to width bits.
                // Interpret the value in its original bit-width and sign-extend.
                if (width < 64) {
                    uint64_t mask = (uint64_t{1} << width) - 1;
                    v = v & mask;
                    // Check sign bit at position width-1
                    if (v & (uint64_t{1} << (width - 1))) {
                        // Sign extend: fill upper bits with 1s
                        v |= ~mask;
                    }
                }
                return constant(v);
            }
            case ExtOp::Trunc:
                // Truncate: mask to width bits
                if (width < 64)
                    v = v & ((uint64_t{1} << width) - 1);
                return constant(v);
        }
        return constant(v);  // unreachable, satisfies compiler
    }

    SymExpr e;
    e.kind = Kind::Extend;
    e.ext_op = op;
    e.bit_width = width;
    e.children.reserve(1);
    e.children.push_back(std::move(src));
    return e;
}

SymExpr SymExpr::opaque(OpaqueKind kind, unsigned id, std::string sym) {
    SymExpr e;
    e.kind = Kind::Opaque;
    e.opaque_kind = kind;
    e.reg_id = id;
    e.symbol = std::move(sym);
    return e;
}

// === Queries ===

std::optional<uint64_t> SymExpr::tryEval() const {
    switch (kind) {
        case Kind::Constant:
            return value;
        case Kind::BinaryOp: {
            if (children.size() != 2)
                return std::nullopt;
            auto l = children[0].tryEval();
            auto r = children[1].tryEval();
            if (!l || !r)
                return std::nullopt;
            switch (bin_op) {
                case BinOp::Add: return *l + *r;
                case BinOp::Sub: return *l - *r;
                case BinOp::Mul: return *l * *r;
                case BinOp::And: return *l & *r;
                case BinOp::Or:  return *l | *r;
                case BinOp::Xor: return *l ^ *r;
                case BinOp::Shl: return *l << *r;
                case BinOp::Lsr: return *l >> *r;
                case BinOp::Asr:
                    return static_cast<uint64_t>(
                        static_cast<int64_t>(*l) >> *r);
            }
            return std::nullopt;
        }
        case Kind::Extend: {
            if (children.size() != 1)
                return std::nullopt;
            auto v = children[0].tryEval();
            if (!v)
                return std::nullopt;
            switch (ext_op) {
                case ExtOp::ZExt:
                    if (bit_width < 64)
                        return *v & ((uint64_t{1} << bit_width) - 1);
                    return *v;
                case ExtOp::SExt: {
                    uint64_t val = *v;
                    if (bit_width < 64) {
                        uint64_t mask = (uint64_t{1} << bit_width) - 1;
                        val = val & mask;
                        if (val & (uint64_t{1} << (bit_width - 1)))
                            val |= ~mask;
                    }
                    return val;
                }
                case ExtOp::Trunc:
                    if (bit_width < 64)
                        return *v & ((uint64_t{1} << bit_width) - 1);
                    return *v;
            }
            return std::nullopt;
        }
        case Kind::Unknown:
        case Kind::Load:
        case Kind::Opaque:
            return std::nullopt;
    }
    return std::nullopt;
}

bool SymExpr::involvesLoad() const {
    if (kind == Kind::Load)
        return true;
    return std::any_of(children.begin(), children.end(),
                       [](const SymExpr& c) { return c.involvesLoad(); });
}

bool SymExpr::involvesOpaque(OpaqueKind k) const {
    if (kind == Kind::Opaque && opaque_kind == k)
        return true;
    return std::any_of(children.begin(), children.end(),
                       [k](const SymExpr& c) { return c.involvesOpaque(k); });
}

bool SymExpr::involvesCall(const std::string& name) const {
    if (kind == Kind::Opaque && opaque_kind == OpaqueKind::CallResult &&
        (name.empty() || symbol == name))
        return true;
    return std::any_of(children.begin(), children.end(),
                       [&name](const SymExpr& c) { return c.involvesCall(name); });
}

const SymExpr* SymExpr::findLoadAddr() const {
    if (kind == Kind::Load && !children.empty())
        return &children[0];
    for (const auto& c : children) {
        const SymExpr* found = c.findLoadAddr();
        if (found)
            return found;
    }
    return nullptr;
}

const SymExpr* SymExpr::findOpaque(OpaqueKind k) const {
    if (kind == Kind::Opaque && opaque_kind == k)
        return this;
    for (const auto& c : children) {
        const SymExpr* found = c.findOpaque(k);
        if (found)
            return found;
    }
    return nullptr;
}

}  // namespace VMPilot::SDK::ReferenceAnalyzer
