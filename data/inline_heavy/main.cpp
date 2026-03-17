/// inline_heavy: A protected function that should be inlined into
/// multiple distinct call sites.
///
/// Tests:
///   - segment() finds multiple copies of "protected_op" at different addresses
///   - group() produces 1 group with multiple sites
///   - Canonical detection: the site whose enclosing symbol matches
///     "protected_op" is canonical; others are inlined copies
///   - RegionRefiner handles contained/overlapping copies correctly
///   - Serializer round-trip preserves all sites

#include <iostream>

extern void VMPilot_Begin(char const*);
extern void VMPilot_End(char const*);

// Force inlining so the compiler duplicates this into each call site.
// Each copy creates a separate protected region at a different address.
#ifdef _MSC_VER
#define FORCE_INLINE __forceinline
#else
#define FORCE_INLINE __attribute__((always_inline)) inline
#endif

FORCE_INLINE int protected_op(int x) {
    VMPilot_Begin(__FUNCTION__);
    int result = x * x + x + 1;
    VMPilot_End(__FUNCTION__);
    return result;
}

// --- Distinct call sites that should each contain an inlined copy ---

int caller_add(int x) {
    return protected_op(x) + 10;
}

int caller_mul(int x) {
    return protected_op(x) * 2;
}

int caller_chain(int x) {
    return protected_op(protected_op(x));
}

int main() {
    int v = 3;
    std::cout << "add(" << v << ") = " << caller_add(v) << std::endl;
    std::cout << "mul(" << v << ") = " << caller_mul(v) << std::endl;
    std::cout << "chain(" << v << ") = " << caller_chain(v) << std::endl;
    return 0;
}
