/// @file snake.cpp
/// @brief Terminal Snake game with VM-orchestrated control flow.
///
/// The game loop runs as VM bytecodes (3 basic blocks with JMP/JCC).
/// Game logic and I/O are host-side via NATIVE_CALL. The VM orchestrates:
///
///   BB 1 (init):    NATIVE_CALL game_init()    → JMP BB 2
///   BB 2 (loop):    NATIVE_CALL game_tick()    → if dead: JCC BB 3
///                   NATIVE_CALL game_draw()    → JMP BB 2
///   BB 3 (end):     NATIVE_CALL game_over()    → HALT
///
/// Build:  ninja -C build snake_example
/// Run:    ./build/bin/snake_example

#include "test_blob_builder.hpp"
#include "vm_engine.hpp"

#include <vm/vm_opcode.hpp>
#include <vm/vm_insn.hpp>

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#if !defined(_WIN32)
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#else
#include <conio.h>
#include <windows.h>
#endif

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Test;

// ═════════════════════════════════════════════════════════════════════════════
// Terminal I/O helpers
// ═════════════════════════════════════════════════════════════════════════════

#if !defined(_WIN32)
static struct termios g_orig;
static bool g_raw = false;
static void raw_on()  {
    if (g_raw) return;
    if (!isatty(STDIN_FILENO)) return;  // not a terminal (e.g., piped input)
    tcgetattr(0, &g_orig);
    auto t = g_orig;
    t.c_lflag &= ~static_cast<tcflag_t>(ICANON|ECHO); t.c_cc[VMIN]=0; t.c_cc[VTIME]=0;
    tcsetattr(0, TCSAFLUSH, &t); g_raw = true;
}
static void raw_off() { if (!g_raw) return; tcsetattr(0, TCSAFLUSH, &g_orig); g_raw = false; }
static char read_key() {
    raw_on(); fd_set fds; FD_ZERO(&fds); FD_SET(0, &fds);
    struct timeval tv = {0,0};
    if (select(1, &fds, nullptr, nullptr, &tv) > 0) { char c=0; if(read(0,&c,1)==1) return c; }
    return 0;
}
static void sleep_ms(int ms) { usleep(static_cast<useconds_t>(ms) * 1000); }
#else
static void raw_on()  {}
static void raw_off() {}
static char read_key() { return _kbhit() ? static_cast<char>(_getch()) : 0; }
static void sleep_ms(int ms) { Sleep(static_cast<DWORD>(ms)); }
#endif

// ═════════════════════════════════════════════════════════════════════════════
// Game state (host-side, managed via NATIVE_CALL)
// ═════════════════════════════════════════════════════════════════════════════

static struct {
    int hx, hy, dir, ax, ay, score;
    int w = 20, h = 10;
} G;

// ═════════════════════════════════════════════════════════════════════════════
// Native functions (8-arg signature for ABI trampoline)
// ═════════════════════════════════════════════════════════════════════════════

/// NATIVE 0: Initialize game state.
static uint64_t n_init(uint64_t,uint64_t,uint64_t,uint64_t,
                        uint64_t,uint64_t,uint64_t,uint64_t) {
    G.hx = G.w/2; G.hy = G.h/2; G.dir = 1; G.score = 0;
    G.ax = 1 + std::rand() % G.w;
    G.ay = 1 + std::rand() % G.h;
    return 0;
}

/// NATIVE 1: One game tick — read input, move, check collision.
/// Returns: 0=alive, 1=dead(wall), 2=quit
static uint64_t n_tick(uint64_t,uint64_t,uint64_t,uint64_t,
                        uint64_t,uint64_t,uint64_t,uint64_t) {
    char c = read_key();
    if (c == 'q' || c == 'Q') return 2;
    if (c == 'w' || c == 'W') G.dir = 0;
    if (c == 'd' || c == 'D') G.dir = 1;
    if (c == 's' || c == 'S') G.dir = 2;
    if (c == 'a' || c == 'A') G.dir = 3;

    switch (G.dir) { case 0: G.hy--; break; case 1: G.hx++; break;
                     case 2: G.hy++; break; case 3: G.hx--; break; }

    if (G.hx < 1 || G.hx > G.w || G.hy < 1 || G.hy > G.h) return 1;

    if (G.hx == G.ax && G.hy == G.ay) {
        G.score++;
        G.ax = 1 + std::rand() % G.w;
        G.ay = 1 + std::rand() % G.h;
    }
    return 0;
}

/// NATIVE 2: Draw frame + sleep.
static uint64_t n_draw(uint64_t,uint64_t,uint64_t,uint64_t,
                        uint64_t,uint64_t,uint64_t,uint64_t) {
    std::printf("\033[H\033[J");
    std::printf("Score: %d   [WASD=move, Q=quit]\n", G.score);
    for (int y = 0; y <= G.h+1; ++y) {
        for (int x = 0; x <= G.w+1; ++x) {
            if (y==0||y==G.h+1||x==0||x==G.w+1) std::putchar('#');
            else if (x==G.hx && y==G.hy) std::putchar('@');
            else if (x==G.ax && y==G.ay) std::putchar('*');
            else std::putchar(' ');
        }
        std::putchar('\n');
    }
    std::fflush(stdout);
    sleep_ms(150);
    return 0;
}

/// NATIVE 3: Game over — print final score.
static uint64_t n_over(uint64_t,uint64_t,uint64_t,uint64_t,
                        uint64_t,uint64_t,uint64_t,uint64_t) {
    raw_off();
    std::printf("\n  GAME OVER!  Score: %d\n\n", G.score);
    return static_cast<uint64_t>(G.score);
}

// ═════════════════════════════════════════════════════════════════════════════
// VM program: 3 basic blocks
// ═════════════════════════════════════════════════════════════════════════════

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    uint8_t seed[32];
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);

    auto none = []() -> uint8_t { return 0; };
    auto rr   = []() -> uint8_t {
        return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4));
    };
    auto jcc_ne = []() -> uint8_t {
        return static_cast<uint8_t>((VM_OPERAND_NONE << 6) | (VM_OPERAND_NONE << 4) | 1);
    };

    // BB 1: init → JMP loop
    TestBB bb1{}; bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    for (int i = 0; i < 32; ++i) bb1.epoch_seed[i] = static_cast<uint8_t>(0xB1 + i);
    bb1.instructions = {
        {VmOpcode::NATIVE_CALL, none(), 0, 0, 0},   // game_init()
        {VmOpcode::JMP,         none(), 0, 0, 2},   // → BB 2
    };

    // BB 2: tick → if dead JCC → BB 3, else draw → JMP BB 2
    TestBB bb2{}; bb2.bb_id = 2; bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF; bb2.flags = 0;
    for (int i = 0; i < 32; ++i) bb2.epoch_seed[i] = static_cast<uint8_t>(0xB2 + i);
    bb2.instructions = {
        {VmOpcode::NATIVE_CALL, none(), 0, 0, 1},   // tick() → r0 = status
        {VmOpcode::CMP,         rr(),   0, 10, 0},  // CMP r0, r10 (r10=0=alive)
        {VmOpcode::JCC,         jcc_ne(), 0, 0, 3}, // if r0 != 0 → BB 3 (gameover)
        {VmOpcode::NATIVE_CALL, none(), 0, 0, 2},   // draw()
        {VmOpcode::JMP,         none(), 0, 0, 2},   // → BB 2 (loop)
    };

    // BB 3: gameover → HALT
    TestBB bb3{}; bb3.bb_id = 3; bb3.epoch = 0;
    bb3.live_regs_bitmap = 0xFFFF; bb3.flags = 0;
    for (int i = 0; i < 32; ++i) bb3.epoch_seed[i] = static_cast<uint8_t>(0xB3 + i);
    bb3.instructions = {
        {VmOpcode::NATIVE_CALL, none(), 0, 0, 3},   // game_over()
        {VmOpcode::HALT,        none(), 0, 0, 0},
    };

    // Native call table: call_site_ip = GLOBAL instruction index
    // BB1: 2 insns (ip 0-1), BB2: 5 insns (ip 2-6), BB3: 2 insns (ip 7-8)
    std::vector<TestNativeCall> ncs = {
        {0, 0, reinterpret_cast<uint64_t>(&n_init)},  // ip 0: init
        {2, 0, reinterpret_cast<uint64_t>(&n_tick)},  // ip 2: tick
        {5, 0, reinterpret_cast<uint64_t>(&n_draw)},  // ip 5: draw
        {7, 0, reinterpret_cast<uint64_t>(&n_over)},  // ip 7: gameover
    };

    auto blob = build_test_blob(seed, {bb1, bb2, bb3}, {}, false, ncs);

    // r10 = 0 (for CMP against alive status)
    uint64_t regs[16] = {};
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed, 0, regs, 11);

    if (!engine) { std::fprintf(stderr, "VmEngine::create() failed\n"); return 1; }

    std::printf("=== VMPilot Snake ===\n");
    std::printf("WASD = move, Q = quit\n");
    std::printf("Starting in 2 seconds...\n");
    sleep_ms(2000);

    auto result = engine->execute();
    raw_off();

    if (!result) { std::fprintf(stderr, "execute() failed\n"); return 1; }
    return 0;
}
