#!/usr/bin/env python3
"""Dry-run simulator for snake_logic.py.

Interprets the same restricted Python that py2vmp.py compiles, but
executes it directly in Python with mock native functions.  Prints
each frame so you can verify the game logic without compiling C++.

Usage:
    python dry_run.py                 # run 20 frames, no input
    python dry_run.py --frames 50     # run 50 frames
    python dry_run.py --keys wdddsss  # simulate key presses
"""

from __future__ import annotations

import argparse
import random
import sys
from typing import List, Tuple

# ─── Game state (mirrors the C++ host) ──────────────────────────────────

GW = 30
GH = 15

body: List[Tuple[int, int]] = []
key_queue: List[int] = []
frame_idx = 0


# ─── Mock native functions ──────────────────────────────────────────────

def native_read_key() -> int:
    if key_queue:
        return key_queue.pop(0)
    return 0

def native_body_init(cx: int, cy: int, length: int) -> int:
    body.clear()
    for i in range(length):
        body.append((cx - i, cy))
    return 0

def native_body_get_x(i: int) -> int:
    return body[i][0]

def native_body_get_y(i: int) -> int:
    return body[i][1]

def native_body_shift(j: int) -> int:
    while j >= len(body):
        body.append(body[-1])  # auto-extend
    if j > 0:
        body[j] = body[j - 1]
    return 0

def native_body_set_head(x: int, y: int) -> int:
    if body:
        body[0] = (x, y)
    return 0

def native_body_trim(length: int) -> int:
    while len(body) > length:
        body.pop()
    return 0

def native_random(lo: int, hi: int) -> int:
    return random.randint(lo, hi)

def native_draw(hx: int, hy: int, ax: int, ay: int, score: int, body_len: int) -> int:
    global frame_idx
    frame_idx += 1
    print(f"\n── Frame {frame_idx}  Score={score}  Len={body_len}  "
          f"Head=({hx},{hy})  Apple=({ax},{ay}) ──")

    for y in range(0, GH + 2):
        row = []
        for x in range(0, GW + 2):
            if y == 0 or y == GH + 1 or x == 0 or x == GW + 1:
                row.append('#')
            elif x == hx and y == hy:
                row.append('@')
            elif x == ax and y == ay:
                row.append('*')
            else:
                is_seg = False
                for k in range(1, len(body)):
                    if body[k][0] == x and body[k][1] == y:
                        is_seg = True
                        break
                row.append('o' if is_seg else ' ')
        print(''.join(row))
    return 0

def native_sleep() -> int:
    return 0  # no actual sleep in dry run

def native_place_apple(body_len_arg: int) -> int:
    while True:
        ax = random.randint(1, GW)
        ay = random.randint(1, GH)
        collision = False
        for i in range(min(body_len_arg, len(body))):
            if body[i][0] == ax and body[i][1] == ay:
                collision = True
                break
        if not collision:
            return (ax << 16) | ay


# ─── Execute the game logic ─────────────────────────────────────────────

def run(max_frames: int, keys: str) -> None:
    global key_queue
    random.seed(42)

    # Pre-load key queue (one key per frame)
    key_queue = [ord(c) for c in keys]

    # ── Init ──
    hx = GW // 2
    hy = GH // 2
    direction = 1
    score = 0
    body_len = 3

    native_body_init(hx, hy, body_len)
    ax = native_random(1, GW)
    ay = native_random(1, GH)

    status = 0
    frames = 0

    while status == 0:
        if frames >= max_frames:
            print(f"\n[dry_run] Stopped after {max_frames} frames (snake still alive)")
            return

        key = native_read_key()

        # Quit check
        if key == 113 or key == 81:
            status = 2

        # Direction update
        if status == 0:
            new_dir = direction
            if key == 119:   new_dir = 0
            elif key == 115: new_dir = 2
            elif key == 97:  new_dir = 3
            elif key == 100: new_dir = 1
            reverse = (new_dir + 2) % 4
            if reverse != direction:
                direction = new_dir

        # Movement
        if status == 0:
            if direction == 0:   hy -= 1
            elif direction == 1: hx += 1
            elif direction == 2: hy += 1
            elif direction == 3: hx -= 1

            if hx < 1 or hx > GW or hy < 1 or hy > GH:
                status = 1

        # Apple check (before collision — need grew for tail exclusion)
        if status == 0:
            grew = 0
            if hx == ax and hy == ay:
                score += 1
                grew = 1

        # Body collision (exclude tail if it will move away)
        if status == 0:
            check_len = body_len
            if grew == 0:
                check_len = body_len - 1
            for i in range(check_len):
                if body[i][0] == hx and body[i][1] == hy:
                    status = 1
                    break

        # Body update
        if status == 0:
            if grew == 1:
                body_len += 1

            j = body_len - 1
            while j > 0:
                native_body_shift(j)
                j -= 1
            native_body_set_head(hx, hy)

            if grew == 0:
                native_body_trim(body_len)

            if grew == 1:
                packed = native_place_apple(body_len)
                ax = packed // 65536
                ay = packed % 65536

            native_draw(hx, hy, ax, ay, score, body_len)
            native_sleep()
            frames += 1

    reason = "QUIT" if status == 2 else "DEAD"
    print(f"\n[dry_run] GAME OVER ({reason})  Score={score}  Length={body_len}  "
          f"Head=({hx},{hy})  Frames={frames}")


def main() -> None:
    ap = argparse.ArgumentParser(description="Dry-run snake game logic")
    ap.add_argument("--frames", type=int, default=20,
                    help="max frames to simulate (default 20)")
    ap.add_argument("--keys", type=str, default="",
                    help="simulated key presses (one per frame, e.g. 'wdddsss')")
    args = ap.parse_args()
    run(args.frames, args.keys)


if __name__ == "__main__":
    main()
