/// @file arithmetic.cpp
/// @brief All 4 basic arithmetic operations: ADD, SUB, MUL, DIV.
///
/// Build:  ninja -C build arithmetic_example
/// Run:    ./build/bin/arithmetic_example

#include "program_builder.hpp"
#include "vm_engine.hpp"
#include <vm/xex_speck64.hpp>

#include <cstdio>

using namespace VMPilot::Runtime;
using namespace VMPilot::Example;
using namespace VMPilot::Common::VM::Crypto;

int main() {
    uint8_t seed[32];
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);

    // r0=100(ADD), r1=20(operand), r2=100(SUB), r3=100(MUL), r4=100(DIV)
    ProgramBuilder prog(seed);
    prog.bb(1)
        .load_const(0, 100)     // r0 = 100
        .load_const(1, 20)      // r1 = 20
        .load_const(2, 100)     // r2 = 100
        .load_const(3, 100)     // r3 = 100
        .load_const(4, 100)     // r4 = 100
        .add(0, 1)              // r0 = 100 + 20 = 120
        .sub(2, 1)              // r2 = 100 - 20 = 80
        .mul(3, 1)              // r3 = 100 * 20 = 2000
        .div(4, 1)              // r4 = 100 / 20 = 5
        .halt()
        .end_bb();

    auto blob = prog.build();
    auto engine = VmEngine<StandardPolicy, RollingKeyOram>::create(blob.data(), blob.size(), seed);
    if (!engine) { std::fprintf(stderr, "create failed\n"); return 1; }

    auto result = engine->execute();
    if (!result) { std::fprintf(stderr, "execute failed\n"); return 1; }

    // r0 returned by execute(); decode r2-r4 via FPE
    const auto& exec = engine->execution();
    uint64_t r0 = result->return_value;
    uint64_t r2 = FPE_Decode(exec.insn_fpe_key, 2, exec.regs[2].bits);
    uint64_t r3 = FPE_Decode(exec.insn_fpe_key, 3, exec.regs[3].bits);
    uint64_t r4 = FPE_Decode(exec.insn_fpe_key, 4, exec.regs[4].bits);

    std::printf("ADD: 100 + 20 = %lu\n", static_cast<unsigned long>(r0));
    std::printf("SUB: 100 - 20 = %lu\n", static_cast<unsigned long>(r2));
    std::printf("MUL: 100 * 20 = %lu\n", static_cast<unsigned long>(r3));
    std::printf("DIV: 100 / 20 = %lu\n", static_cast<unsigned long>(r4));

    bool ok = (r0 == 120 && r2 == 80 && r3 == 2000 && r4 == 5);
    std::printf(ok ? "\nSUCCESS\n" : "\nFAIL\n");
    return ok ? 0 : 1;
}
