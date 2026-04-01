/// @file dry_run.cpp
/// @brief Minimal dry-run: 42 + 58 = 100.
///
/// Build:  ninja -C build dry_run_example
/// Run:    ./build/bin/dry_run_example

#include "program_builder.hpp"
#include "vm_engine.hpp"

#include <cstdio>
#include <cstdlib>

using namespace VMPilot::Runtime;
using namespace VMPilot::Example;

int main() {
    uint8_t seed[32];
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);

    ProgramBuilder prog(seed);
    prog.bb(1)
        .load_const(0, 42)
        .load_const(1, 58)
        .add(0, 1)
        .halt()
        .end_bb();

    auto blob = prog.build();

    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed);
    if (!engine) { std::fprintf(stderr, "create failed\n"); return 1; }

    auto result = engine->execute();
    if (!result) { std::fprintf(stderr, "execute failed\n"); return 1; }

    std::printf("42 + 58 = %lu\n", static_cast<unsigned long>(result->return_value));
    return (result->return_value == 100) ? 0 : 1;
}
