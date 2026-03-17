/// nested_protect: Protected function calls another protected function.
///
/// Tests:
///   - segment() correctly identifies both regions without overlap
///   - RegionRefiner does not merge distinct non-overlapping regions
///   - group() produces 2 groups (inner + outer)
///   - Each group has correct enclosing symbol
///   - compile_binary() produces outputs for both functions
///   - Serializer round-trip preserves both units and their contexts

#include <iostream>

extern void VMPilot_Begin(char const*);
extern void VMPilot_End(char const*);

int inner_work(int x) {
    VMPilot_Begin(__FUNCTION__);
    int result = x * x;
    VMPilot_End(__FUNCTION__);
    return result;
}

int outer_work(int x) {
    VMPilot_Begin(__FUNCTION__);
    // Calls inner_work — the two protected regions are separate
    // (inner finishes before outer continues)
    int partial = inner_work(x);
    int result = partial + x + 1;
    VMPilot_End(__FUNCTION__);
    return result;
}

int main() {
    int v = 4;
    std::cout << "inner(" << v << ") = " << inner_work(v) << std::endl;
    std::cout << "outer(" << v << ") = " << outer_work(v) << std::endl;
    return 0;
}
