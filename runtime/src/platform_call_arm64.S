// platform_call_arm64.S — AAPCS64 ARM64 native call trampoline.
//
// Reads PlatformCallDesc struct, loads x0-x7 + d0-d7 + stack args,
// calls target, returns integer or FP result.
//
// extern "C" uint64_t platform_call(const PlatformCallDesc* desc);
//   desc in x0
//
// extern "C" uint64_t platform_call_struct(
//     const PlatformCallDesc* desc,  // x0
//     void* struct_return_ptr         // x1 → loaded into x8
// );
//
// PlatformCallDesc offsets (verified via offsetof()):
//   0:   target
//   8:   int_regs[8]  (64B)
//   72:  fp_regs[8]   (64B)
//   144: stack_buf[128] (alignas(16) → 8B padding after fp_regs)
//   272: stack_size (uint16_t)
//   277: flags

#if defined(__aarch64__)

// Mach-O (macOS) requires a leading underscore on C symbols; ELF does not.
#if defined(__APPLE__)
#define SYM(name) _##name
#else
#define SYM(name) name
#endif

    .text

// ─────────────────────────────────────────────────────────────────────
// platform_call — standard call (no struct return)
// ─────────────────────────────────────────────────────────────────────

    .globl SYM(platform_call)
#if !defined(__APPLE__)
    .type  SYM(platform_call), %function
#endif
    .p2align 4

SYM(platform_call):
    stp     x29, x30, [sp, #-48]!
    mov     x29, sp
    stp     x19, x20, [sp, #16]
    str     x21, [sp, #32]

    mov     x19, x0             // x19 = desc (callee-saved)

    // ── 1. Allocate and copy stack args ──────────────────────────────
    ldrh    w20, [x19, #272]    // w20 = stack_size
    cbz     w20, .Lno_stack_arm64

    // Align stack_size to 16
    add     w20, w20, #15
    and     w20, w20, #-16
    sub     sp, sp, x20

    // Copy stack_buf to sp
    add     x0, x19, #144       // src = desc->stack_buf
    mov     x1, sp              // dst = sp
    mov     w2, w20             // count (aligned)
.Lcopy_loop:
    subs    w2, w2, #8
    b.lt    .Lcopy_done
    ldr     x3, [x0], #8
    str     x3, [x1], #8
    b       .Lcopy_loop
.Lcopy_done:

.Lno_stack_arm64:
    ldr     x9, [x19, #0]      // x9 = target

    // ── 2. Load 8 FP args ───────────────────────────────────────────
    ldp     d0, d1, [x19, #72]
    ldp     d2, d3, [x19, #88]
    ldp     d4, d5, [x19, #104]
    ldp     d6, d7, [x19, #120]

    // ── 3. Load 8 int args ──────────────────────────────────────────
    ldp     x0, x1, [x19, #8]
    ldp     x2, x3, [x19, #24]
    ldp     x4, x5, [x19, #40]
    ldp     x6, x7, [x19, #56]

    // ── 4. Call target ──────────────────────────────────────────────
    blr     x9

    // ── 5. Handle returns_fp ────────────────────────────────────────
    ldrb    w20, [x19, #277]    // flags
    tst     w20, #0x40
    b.eq    .Lreturn_arm64
    fmov    x0, d0
.Lreturn_arm64:

    // ── 6. Epilogue ─────────────────────────────────────────────────
    mov     sp, x29
    ldr     x21, [sp, #32]
    ldp     x19, x20, [sp, #16]
    ldp     x29, x30, [sp], #48
    ret

#if !defined(__APPLE__)
    .size SYM(platform_call), . - SYM(platform_call)
#endif

// ─────────────────────────────────────────────────────────────────────
// platform_call_struct — struct return variant (x8 = return pointer)
// ─────────────────────────────────────────────────────────────────────

    .globl SYM(platform_call_struct)
#if !defined(__APPLE__)
    .type  SYM(platform_call_struct), %function
#endif
    .p2align 4

SYM(platform_call_struct):
    stp     x29, x30, [sp, #-48]!
    mov     x29, sp
    stp     x19, x20, [sp, #16]
    str     x21, [sp, #32]

    mov     x19, x0             // desc
    mov     x8, x1              // struct return pointer → x8 (AAPCS dedicated)

    // Stack args
    ldrh    w20, [x19, #272]
    cbz     w20, .Lno_stack_struct

    add     w20, w20, #15
    and     w20, w20, #-16
    sub     sp, sp, x20

    add     x0, x19, #144
    mov     x1, sp
    mov     w2, w20
.Lcopy_struct:
    subs    w2, w2, #8
    b.lt    .Lcopy_struct_done
    ldr     x3, [x0], #8
    str     x3, [x1], #8
    b       .Lcopy_struct
.Lcopy_struct_done:

.Lno_stack_struct:
    ldr     x9, [x19, #0]      // target

    // FP args
    ldp     d0, d1, [x19, #72]
    ldp     d2, d3, [x19, #88]
    ldp     d4, d5, [x19, #104]
    ldp     d6, d7, [x19, #120]

    // Int args (x8 already set — does NOT shift x0-x7)
    ldp     x0, x1, [x19, #8]
    ldp     x2, x3, [x19, #24]
    ldp     x4, x5, [x19, #40]
    ldp     x6, x7, [x19, #56]

    blr     x9

    // Result: x0 = struct pointer (callee returns it per AAPCS)
    ldrb    w20, [x19, #277]
    tst     w20, #0x40
    b.eq    .Lreturn_struct
    fmov    x0, d0
.Lreturn_struct:

    mov     sp, x29
    ldr     x21, [sp, #32]
    ldp     x19, x20, [sp, #16]
    ldp     x29, x30, [sp], #48
    ret

#if !defined(__APPLE__)
    .size SYM(platform_call_struct), . - SYM(platform_call_struct)
#endif

#endif  // __aarch64__
