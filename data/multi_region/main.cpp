/// multi_region: Three distinct protected functions.
///
/// Tests:
///   - segment() finds 3 separate protected regions
///   - group() produces 3 groups (one per function name)
///   - Each group has exactly 1 canonical site
///   - build_units() produces 3 CompilationUnits
///   - Full compile_binary() succeeds with 3 outputs

#include <iostream>

extern void VMPilot_Begin(char const*);
extern void VMPilot_End(char const*);

int compute_alpha(int x) {
    VMPilot_Begin(__FUNCTION__);
    int result = x * 2 + 1;
    VMPilot_End(__FUNCTION__);
    return result;
}

int compute_beta(int x) {
    VMPilot_Begin(__FUNCTION__);
    int result = x * x;
    VMPilot_End(__FUNCTION__);
    return result;
}

int compute_gamma(int x) {
    VMPilot_Begin(__FUNCTION__);
    int result = compute_alpha(x) + compute_beta(x);
    VMPilot_End(__FUNCTION__);
    return result;
}

int main() {
    int v = 5;
    std::cout << "alpha(" << v << ") = " << compute_alpha(v) << std::endl;
    std::cout << "beta(" << v << ") = " << compute_beta(v) << std::endl;
    std::cout << "gamma(" << v << ") = " << compute_gamma(v) << std::endl;
    return 0;
}
