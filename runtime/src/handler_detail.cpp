/// @file handler_detail.cpp
/// @brief Non-template handler helpers extracted from handler_impls.hpp.

#include "handler_impls.hpp"

namespace VMPilot::Runtime::detail {

bool evaluate_condition(uint8_t flags, uint8_t cond) noexcept {
    bool zf = (flags & 0x01) != 0;
    bool sf = (flags & 0x02) != 0;
    bool cf = (flags & 0x04) != 0;
    bool of = (flags & 0x08) != 0;
    switch (cond) {
        case 0: return zf;           // EQ  / Z
        case 1: return !zf;          // NE  / NZ
        case 2: return sf != of;     // LT  (signed)
        case 3: return sf == of;     // GE  (signed)
        case 4: return zf || (sf != of);  // LE (signed)
        case 5: return !zf && (sf == of); // GT (signed)
        case 6: return cf;           // B   (unsigned below)
        case 7: return !cf;          // AE  (unsigned above-or-equal)
        case 8: return cf || zf;     // BE  (unsigned below-or-equal)
        case 9: return !cf && !zf;   // A   (unsigned above)
        default: return false;
    }
}

}  // namespace VMPilot::Runtime::detail
