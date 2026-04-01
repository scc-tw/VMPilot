/// @file snake.cpp
/// @brief 2D terminal Snake running inside the VMPilot VM.
///
/// Architecture:
///   - Terminal I/O (raw mode, key reading) handled ONLY in main().
///   - Game state and logic in host-side native functions via NATIVE_CALL.
///   - VM orchestrates the game loop across 3 basic blocks (init/loop/end).
///
/// Controls: WASD or arrow keys. Q to quit.
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
#include <cstring>
#include <ctime>
#include <deque>
#include <utility>

// ─── Platform terminal I/O ──────────────────────────────────────────────────

#if defined(_WIN32)
#include <conio.h>
#include <windows.h>

static void term_raw_on()  {}
static void term_raw_off() {}

static int term_read_key() {
    if (!_kbhit()) return 0;
    int c = _getch();
    if (c == 0 || c == 0xE0) {
        int ext = _getch();
        switch (ext) {
            case 72: return 'w';  // up
            case 77: return 'd';  // right
            case 80: return 's';  // down
            case 75: return 'a';  // left
        }
        return 0;
    }
    return c;
}

static void term_sleep_ms(int ms) { Sleep(static_cast<DWORD>(ms)); }

#else
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

static struct termios g_saved_termios;
static bool g_raw_active = false;

static void term_raw_on() {
    if (g_raw_active || !isatty(STDIN_FILENO)) return;
    tcgetattr(STDIN_FILENO, &g_saved_termios);
    struct termios raw = g_saved_termios;
    raw.c_lflag &= ~static_cast<tcflag_t>(ICANON | ECHO);
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    g_raw_active = true;
}

static void term_raw_off() {
    if (!g_raw_active) return;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_saved_termios);
    g_raw_active = false;
}

/// Non-blocking key read. Returns ASCII char, or 0 if no input.
/// Handles arrow key escape sequences: ESC [ A/B/C/D → w/s/d/a
static int term_read_key() {
    auto has_data = []() -> bool {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        struct timeval tv = {0, 0};
        return select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0;
    };
    auto getc_ = []() -> int {
        unsigned char c = 0;
        return (read(STDIN_FILENO, &c, 1) == 1) ? c : -1;
    };

    if (!has_data()) return 0;
    int c = getc_();
    if (c < 0) return 0;

    // Arrow keys: ESC [ {A,B,C,D}
    if (c == 0x1B) {
        if (!has_data()) return 0x1B;  // bare ESC
        int c2 = getc_();
        if (c2 != '[') return 0;
        if (!has_data()) return 0;
        int c3 = getc_();
        switch (c3) {
            case 'A': return 'w';  // up
            case 'B': return 's';  // down
            case 'C': return 'd';  // right
            case 'D': return 'a';  // left
        }
        return 0;
    }
    return c;
}

static void term_sleep_ms(int ms) {
    usleep(static_cast<useconds_t>(ms) * 1000);
}
#endif

// ─── Game state ─────────────────────────────────────────────────────────────

using namespace VMPilot::Runtime;
using namespace VMPilot::Common::VM;
using namespace VMPilot::Test;

static constexpr int GW = 30, GH = 15;

static struct GameState {
    std::deque<std::pair<int,int>> body;  // front = head
    int dir    = 1;     // 0=up 1=right 2=down 3=left
    int ax     = 0;     // apple
    int ay     = 0;
    int score  = 0;
    int status = 0;     // 0=alive 1=dead 2=quit

    int hx() const { return body.front().first;  }
    int hy() const { return body.front().second; }

    bool hits_body(int x, int y) const {
        for (auto& s : body)
            if (s.first == x && s.second == y) return true;
        return false;
    }

    void place_apple() {
        do {
            ax = 1 + std::rand() % GW;
            ay = 1 + std::rand() % GH;
        } while (hits_body(ax, ay));
    }
} G;

/// Shared key buffer — written by main's key-reading loop,
/// read by n_tick inside the VM. Avoids terminal syscalls from trampoline.
static volatile int g_last_key = 0;

// ─── Native functions (called via NATIVE_CALL from VM) ──────────────────────

/// NATIVE 0: init
static uint64_t n_init(uint64_t,uint64_t,uint64_t,uint64_t,
                        uint64_t,uint64_t,uint64_t,uint64_t) {
    G.body.clear();
    G.dir = 1; G.score = 0; G.status = 0;
    int cx = GW / 2, cy = GH / 2;
    G.body.push_back({cx, cy});
    G.body.push_back({cx-1, cy});
    G.body.push_back({cx-2, cy});
    G.place_apple();
    return 0;
}

/// NATIVE 1: tick — read buffered key, move, check collisions.
/// Returns: 0=alive, 1=dead, 2=quit
static uint64_t n_tick(uint64_t,uint64_t,uint64_t,uint64_t,
                        uint64_t,uint64_t,uint64_t,uint64_t) {
    // Read key from shared buffer (set by main's polling loop)
    int c = g_last_key;
    g_last_key = 0;

    if (c == 'q' || c == 'Q') { G.status = 2; return 2; }

    int nd = G.dir;
    if (c == 'w' || c == 'W') nd = 0;
    if (c == 'd' || c == 'D') nd = 1;
    if (c == 's' || c == 'S') nd = 2;
    if (c == 'a' || c == 'A') nd = 3;
    if ((nd + 2) % 4 != G.dir) G.dir = nd;  // prevent 180°

    int nx = G.hx(), ny = G.hy();
    switch (G.dir) {
        case 0: ny--; break; case 1: nx++; break;
        case 2: ny++; break; case 3: nx--; break;
    }

    if (nx < 1 || nx > GW || ny < 1 || ny > GH) { G.status = 1; return 1; }
    if (G.hits_body(nx, ny)) { G.status = 1; return 1; }

    G.body.push_front({nx, ny});
    if (nx == G.ax && ny == G.ay) {
        G.score++;
        G.place_apple();
    } else {
        G.body.pop_back();
    }
    return 0;
}

/// NATIVE 2: draw + sleep
static uint64_t n_draw(uint64_t,uint64_t,uint64_t,uint64_t,
                        uint64_t,uint64_t,uint64_t,uint64_t) {
    std::printf("\033[H");  // cursor home (no full clear → less flicker)
    std::printf("  VMPilot Snake  |  Score: %d  |  Length: %zu  |  WASD/Arrows  Q=quit\n",
                G.score, G.body.size());

    for (int y = 0; y <= GH + 1; ++y) {
        for (int x = 0; x <= GW + 1; ++x) {
            if (y == 0 || y == GH+1 || x == 0 || x == GW+1) {
                std::putchar('#');
            } else if (x == G.hx() && y == G.hy()) {
                std::putchar('@');
            } else if (x == G.ax && y == G.ay) {
                std::putchar('*');
            } else {
                bool is_seg = false;
                for (size_t i = 1; i < G.body.size(); ++i) {
                    if (G.body[i].first == x && G.body[i].second == y) {
                        is_seg = true; break;
                    }
                }
                std::putchar(is_seg ? 'o' : ' ');
            }
        }
        std::putchar('\n');
    }
    std::fflush(stdout);
    term_sleep_ms(100);
    return 0;
}

/// NATIVE 3: game over — just returns score (terminal cleanup in main)
static uint64_t n_over(uint64_t,uint64_t,uint64_t,uint64_t,
                        uint64_t,uint64_t,uint64_t,uint64_t) {
    return static_cast<uint64_t>(G.score);
}

// ─── VM program ─────────────────────────────────────────────────────────────

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    uint8_t seed[32];
    for (int i = 0; i < 32; ++i) seed[i] = static_cast<uint8_t>(i + 1);

    auto f_none   = []() -> uint8_t { return 0; };
    auto f_rr     = []() -> uint8_t {
        return static_cast<uint8_t>((VM_OPERAND_REG << 6) | (VM_OPERAND_REG << 4));
    };
    auto f_jcc_ne = []() -> uint8_t {
        return static_cast<uint8_t>((VM_OPERAND_NONE << 6) | (VM_OPERAND_NONE << 4) | 1);
    };

    // BB 1: init → JMP BB 2
    TestBB bb1{}; bb1.bb_id = 1; bb1.epoch = 0;
    bb1.live_regs_bitmap = 0xFFFF; bb1.flags = 0;
    for (int i = 0; i < 32; ++i) bb1.epoch_seed[i] = static_cast<uint8_t>(0xB1 + i);
    bb1.instructions = {
        {VmOpcode::NATIVE_CALL, f_none(), 0, 0, 0},
        {VmOpcode::JMP,         f_none(), 0, 0, 2},
    };

    // BB 2: tick → check → draw → loop
    TestBB bb2{}; bb2.bb_id = 2; bb2.epoch = 0;
    bb2.live_regs_bitmap = 0xFFFF; bb2.flags = 0;
    for (int i = 0; i < 32; ++i) bb2.epoch_seed[i] = static_cast<uint8_t>(0xB2 + i);
    bb2.instructions = {
        {VmOpcode::NATIVE_CALL, f_none(),   0,  0, 1},  // tick → r0
        {VmOpcode::CMP,         f_rr(),     0, 10, 0},  // CMP r0, r10 (0)
        {VmOpcode::JCC,         f_jcc_ne(), 0,  0, 3},  // if r0 ≠ 0 → BB 3
        {VmOpcode::NATIVE_CALL, f_none(),   0,  0, 2},  // draw
        {VmOpcode::JMP,         f_none(),   0,  0, 2},  // loop
    };

    // BB 3: gameover → HALT
    TestBB bb3{}; bb3.bb_id = 3; bb3.epoch = 0;
    bb3.live_regs_bitmap = 0xFFFF; bb3.flags = 0;
    for (int i = 0; i < 32; ++i) bb3.epoch_seed[i] = static_cast<uint8_t>(0xB3 + i);
    bb3.instructions = {
        {VmOpcode::NATIVE_CALL, f_none(), 0, 0, 3},
        {VmOpcode::HALT,        f_none(), 0, 0, 0},
    };

    // Native call table (global instruction indices)
    // BB1: ip 0-1, BB2: ip 2-6, BB3: ip 7-8
    std::vector<TestNativeCall> ncs = {
        {0, 0, reinterpret_cast<uint64_t>(&n_init)},
        {2, 0, reinterpret_cast<uint64_t>(&n_tick)},
        {5, 0, reinterpret_cast<uint64_t>(&n_draw)},
        {7, 0, reinterpret_cast<uint64_t>(&n_over)},
    };

    auto blob = build_test_blob(seed, {bb1, bb2, bb3}, {}, false, ncs);

    uint64_t regs[16] = {};  // r10 = 0 for CMP
    auto engine = VmEngine<DebugPolicy, DirectOram>::create(
        blob.data(), blob.size(), seed, 0, regs, 11);
    if (!engine) { std::fprintf(stderr, "VmEngine::create() failed\n"); return 1; }

    // ── Pre-game screen ─────────────────────────────────────────────────
    std::printf("\033[2J\033[H");
    std::printf("╔═══════════════════════════════╗\n");
    std::printf("║       VMPilot Snake           ║\n");
    std::printf("╠═══════════════════════════════╣\n");
    std::printf("║  @ = you   o = body           ║\n");
    std::printf("║  * = apple # = wall           ║\n");
    std::printf("║                               ║\n");
    std::printf("║  WASD or Arrow Keys to move   ║\n");
    std::printf("║  Q to quit                    ║\n");
    std::printf("╚═══════════════════════════════╝\n");
    std::printf("\n  Starting in 2 seconds...\n");
    term_sleep_ms(2000);

    // ── Enter raw mode HERE (not inside NATIVE_CALL) ────────────────────
    // tcgetattr/tcsetattr crash under ASan when called from inside the
    // platform_call ASM trampoline due to stack red-zone conflicts.
    term_raw_on();
    std::printf("\033[2J");

    // ── Key polling thread alternative: single-threaded cooperative ──────
    // The VM runs step-by-step. Between steps we poll the keyboard and
    // store the result in g_last_key for n_tick to read.
    // Since execute() runs to completion, we use step() instead.

    bool running = true;
    while (running) {
        // Poll keyboard (in main, not inside trampoline)
        int key = term_read_key();
        if (key) g_last_key = key;

        // Run one VM instruction
        auto step_result = engine->step();
        if (!step_result) {
            running = false;
        } else if (*step_result == VmResult::Halted) {
            running = false;
        }
    }

    // ── Restore terminal ────────────────────────────────────────────────
    term_raw_off();

    std::printf("\n  GAME OVER!  Score: %d  |  Length: %zu\n\n",
                G.score, G.body.size());

    return 0;
}
