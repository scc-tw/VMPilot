// platform_call_arm64.S — AAPCS64 ARM64 native call trampoline.
//
// Loads integer args into x0-x7 and FP args into d0-d7,
// calls the target, and returns integer or FP result.
//
// extern "C" uint64_t platform_call(
//     void* target,           // x0
//     const uint64_t* int_regs, // x1 — 8 integer arg values
//     const uint64_t* fp_regs,  // x2 — 8 FP bit-patterns for d0-d7
//     uint8_t flags             // w3 — [5:0]=fp_count, [6]=returns_fp
// );

#if defined(__aarch64__)

    .text
    .globl platform_call
    .type  platform_call, %function
    .p2align 4

platform_call:
    // Save callee-saved registers and link register.
    stp     x29, x30, [sp, #-32]!
    mov     x29, sp
    stp     x19, x20, [sp, #16]

    // Save arguments.
    mov     x9,  x0             // x9  = target
    mov     x10, x1             // x10 = int_regs
    mov     x19, x3             // x19 = flags (callee-saved)

    // ── Load 8 FP args from fp_regs (x2) into d0-d7 ──
    ldp     d0, d1, [x2]
    ldp     d2, d3, [x2, #16]
    ldp     d4, d5, [x2, #32]
    ldp     d6, d7, [x2, #48]

    // ── Load 8 integer args from int_regs (x10) into x0-x7 ──
    ldp     x0, x1, [x10]
    ldp     x2, x3, [x10, #16]
    ldp     x4, x5, [x10, #32]
    ldp     x6, x7, [x10, #48]

    // ── Call target ──
    blr     x9

    // ── Handle return value ──
    // Integer result in x0.  If returns_fp (bit 6), copy d0 to x0.
    tst     w19, #0x40
    b.eq    .Lreturn_arm64
    fmov    x0, d0
.Lreturn_arm64:

    // Restore and return.
    ldp     x19, x20, [sp, #16]
    ldp     x29, x30, [sp], #32
    ret

    .size platform_call, . - platform_call

#endif  // __aarch64__
