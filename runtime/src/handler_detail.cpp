/// @file handler_detail.cpp
/// @brief Non-template handler helpers extracted from handler_impls.hpp.

#include "handler_impls.hpp"

namespace VMPilot::Runtime::detail {

bool evaluate_condition_ct(uint8_t flags, uint8_t cond) noexcept {
    // Precompute ALL 10 conditions into a bitmask, then select by index.
    // No data-dependent branch on `cond`.
    bool zf = (flags & 0x01) != 0;
    bool sf = (flags & 0x02) != 0;
    bool cf = (flags & 0x04) != 0;
    bool of = (flags & 0x08) != 0;

    uint16_t all = 0;
    all |= static_cast<uint16_t>(zf)                << 0;   // EQ
    all |= static_cast<uint16_t>(!zf)               << 1;   // NE
    all |= static_cast<uint16_t>(sf != of)           << 2;   // LT
    all |= static_cast<uint16_t>(sf == of)           << 3;   // GE
    all |= static_cast<uint16_t>(zf || (sf != of))   << 4;   // LE
    all |= static_cast<uint16_t>(!zf && (sf == of))  << 5;   // GT
    all |= static_cast<uint16_t>(cf)                 << 6;   // B
    all |= static_cast<uint16_t>(!cf)                << 7;   // AE
    all |= static_cast<uint16_t>(cf || zf)           << 8;   // BE
    all |= static_cast<uint16_t>(!cf && !zf)         << 9;   // A

    // Safe index: clamp to [0, 15], bits above 9 are 0 → returns false.
    return (all >> (cond & 0x0F)) & 1;
}

}  // namespace VMPilot::Runtime::detail
