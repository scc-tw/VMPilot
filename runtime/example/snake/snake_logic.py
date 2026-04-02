"""Snake game logic — compiled to VMPilot VM bytecode by py2vmp.py.

Restricted Python subset understood by the compiler:
  - Types:      int only (uint64 in VM)
  - Statements: assignment, if/elif/else, while, break, del, pass
  - Expressions: + - * // %, == != < > <= >=, and / or / not
  - Calls:      native_xxx(args) → NATIVE_CALL
  - del var:    explicitly frees the register (important for tight loops)

Usage:
    python py2vmp.py snake_logic.py -o snake_vm.gen.hpp
"""

# ─── Compiler Configuration ─────────────────────────────────────────────
# These dicts are read by py2vmp.py from the AST — not executed at runtime.

REGISTER_MAP = {
    # Game state — dedicated registers (never used as temps)
    'hx':        8,   # head X
    'hy':        9,   # head Y
    'direction': 10,  # 0=up 1=right 2=down 3=left
    'ax':        11,  # apple X
    'ay':        12,  # apple Y
    'score':     13,  # score counter
    'body_len':  14,  # number of body segments
}

CONST_REGS = {
    # Constant registers — never modified by compiled code
    'ZERO': (6, 0),
    'ONE':  (7, 1),
}

TEMP_REGS = [15, 5, 4, 3, 2, 1]   # local temps (r0 reserved for native call)
# Ordered high→low so long-lived locals (status, grew) land on r15/r5
# which are outside the 6-arg native_draw range [0..5], reducing spills.

NATIVE_MAP = {
    # name → slot index (order must match snake.cpp native table)
    'native_read_key':       0,
    'native_body_init':      1,   # (cx, cy, length)
    'native_body_get_x':     2,   # (index) → x
    'native_body_get_y':     3,   # (index) → y
    'native_body_shift':     4,   # (j) → body[j] = body[j-1]  (auto-extends)
    'native_body_set_head':  5,   # (x, y) → body[0] = {x, y}
    'native_body_trim':      6,   # (len) → resize body to len
    'native_random':         7,   # (lo, hi) → random in [lo, hi]
    'native_draw':           8,   # (hx, hy, ax, ay, score, body_len)
    'native_sleep':          9,   # () → sleep one frame
    'native_place_apple':    10,  # (body_len) → returns (ax << 16) | ay
}

# ─── Module Constants (resolved at compile time) ────────────────────────

GW = 30          # grid width  (playfield columns 1..GW)
GH = 15          # grid height (playfield rows    1..GH)

# ─── Init ───────────────────────────────────────────────────────────────

hx = GW // 2          # 15
hy = GH // 2          # 7
direction = 1          # start moving right
score = 0
body_len = 3

# Initialise body array (native — manages the C++ vector)
native_body_init(hx, hy, body_len)

# Place first apple (native — needs rand())
ax = native_random(1, GW)
ay = native_random(1, GH)

# ─── Game Loop ──────────────────────────────────────────────────────────

status = 0             # 0=alive  1=dead  2=quit

while status == 0:

    # ── Read input ──────────────────────────────────────────────────
    key = native_read_key()

    # ── Quit check ──────────────────────────────────────────────────
    if key == 113 or key == 81:          # 'q' or 'Q'
        status = 2

    # ── Direction update (with 180° guard) ──────────────────────────
    if status == 0:
        new_dir = direction
        if key == 119:                   # 'w'
            new_dir = 0
        elif key == 115:                 # 's'
            new_dir = 2
        elif key == 97:                  # 'a'
            new_dir = 3
        elif key == 100:                 # 'd'
            new_dir = 1

        # Prevent 180° reversal: (new_dir+2)%4 must differ from direction
        reverse = (new_dir + 2) % 4
        if reverse != direction:
            direction = new_dir
        del new_dir, reverse
    del key

    # ── Movement ────────────────────────────────────────────────────
    if status == 0:
        if direction == 0:
            hy = hy - 1
        elif direction == 1:
            hx = hx + 1
        elif direction == 2:
            hy = hy + 1
        elif direction == 3:
            hx = hx - 1

        # ── Wall collision ──────────────────────────────────────────
        if hx < 1 or hx > GW or hy < 1 or hy > GH:
            status = 1

    # ── Apple check (must run before body collision to know grew) ───
    grew = 0
    if status == 0:
        if hx == ax and hy == ay:
            score = score + 1
            grew = 1

    # ── Body collision (exclude tail if it will move away) ──────────
    if status == 0:
        check_len = body_len
        if grew == 0:
            check_len = body_len - 1
        i = 0
        while i < check_len:
            bx = native_body_get_x(i)
            by = native_body_get_y(i)
            if bx == hx and by == hy:
                status = 1
            del bx, by
            i = i + 1
        del i, check_len

    # ── Body update ─────────────────────────────────────────────────
    if status == 0:
        # Grow first so the shift loop can copy the old tail into the
        # new slot (native_body_shift auto-extends the C++ vector).
        if grew == 1:
            body_len = body_len + 1

        # Shift body segments backward (tail → head)
        j = body_len - 1
        while j > 0:
            native_body_shift(j)
            j = j - 1
        del j
        native_body_set_head(hx, hy)

        if grew == 0:
            native_body_trim(body_len)

        # Place new apple (native handles rand + collision avoidance)
        if grew == 1:
            packed = native_place_apple(body_len)
            ax = packed // 65536
            ay = packed % 65536
            del packed

        # Free grew before the 6-arg draw call (frees a register in
        # the arg range so the spill has room).
        del grew

        # ── Draw + frame delay ──────────────────────────────────────
        native_draw(hx, hy, ax, ay, score, body_len)
        native_sleep()
    del grew  # no-op if already freed inside the if-block

# ── After loop: HALT is emitted automatically by the compiler ───────────
