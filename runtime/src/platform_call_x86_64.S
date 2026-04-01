// platform_call_x86_64.S — System V x86-64 ABI native call trampoline.
//
// Reads PlatformCallDesc struct by offset, loads int/FP regs + stack args,
// calls target, returns integer or FP result.
//
// extern "C" uint64_t platform_call(const PlatformCallDesc* desc);
//   desc in rdi (first SysV arg)
//
// PlatformCallDesc layout (offsets — verified via offsetof()):
//   0:   target (void*)
//   8:   int_regs[8]  (64 bytes)
//   72:  fp_regs[8]   (64 bytes)
//   144: stack_buf[128] (128 bytes, alignas(16) adds 8B padding after fp_regs)
//   272: stack_size (uint16_t)
//   274: total_args (uint8_t)
//   275: reg_int_count (uint8_t)
//   276: fp_count (uint8_t)
//   277: flags (uint8_t) — [5:0]=fp_count for AL, [6]=returns_fp

#if defined(__x86_64__) && !defined(_WIN32)

    .text
    .globl platform_call
    .type  platform_call, @function
    .p2align 4

platform_call:
    pushq   %rbp
    movq    %rsp, %rbp
    pushq   %r12
    pushq   %r13

    movq    %rdi, %r12          // r12 = desc

    // ── 1. Allocate and copy stack args ──────────────────────────────
    movzwl  272(%r12), %eax     // eax = stack_size
    testl   %eax, %eax
    jz      .Lno_stack_sysv

    // Align: need (3 pushes + alloc + call) to be 16-aligned at callee.
    // 3 pushes = 24B, entry was 8-misaligned → 32 total.
    // After alloc N + call 8 → 32+N+8 = 40+N. Need 40+N ≡ 0 (mod 16).
    // N is rounded to 16 → 40 mod 16 = 8. Add 8 to fix.
    addl    $15+8, %eax
    andl    $-16, %eax
    subq    %rax, %rsp

    // Copy stack_buf to [rsp]
    leaq    144(%r12), %rsi     // src = desc->stack_buf
    movq    %rsp, %rdi          // dst = rsp
    movzwl  272(%r12), %ecx     // count = stack_size
    rep     movsb

    jmp     .Lload_regs_sysv

.Lno_stack_sysv:
    // Ensure 16-byte alignment for call.
    // On entry: rsp was 8-misaligned (return addr). After 3 pushes: 8+24=32 → aligned.
    // CALL will push 8 → 40 → misaligned. Need sub 8 to make it 48 → aligned at callee entry.
    subq    $8, %rsp

.Lload_regs_sysv:
    movq    0(%r12), %r13       // r13 = target

    // ── 2. Load 8 FP args ───────────────────────────────────────────
    movq     72(%r12), %xmm0
    movq     80(%r12), %xmm1
    movq     88(%r12), %xmm2
    movq     96(%r12), %xmm3
    movq    104(%r12), %xmm4
    movq    112(%r12), %xmm5
    movq    120(%r12), %xmm6
    movq    128(%r12), %xmm7

    // ── 3. Set AL = fp_count for variadic ───────────────────────────
    movzbl  277(%r12), %eax     // flags
    andb    $0x3F, %al          // AL = fp_count

    // ── 4. Load 6 int args (rdi last — it holds desc pointer) ───────
    movq    48(%r12), %r9       // int_regs[5]
    movq    40(%r12), %r8       // int_regs[4]
    movq    32(%r12), %rcx      // int_regs[3]
    movq    24(%r12), %rdx      // int_regs[2]
    movq    16(%r12), %rsi      // int_regs[1]
    movq     8(%r12), %rdi      // int_regs[0] — LAST (clobbers desc ptr)

    // ── 5. Call target ──────────────────────────────────────────────
    call    *%r13

    // ── 6. Handle returns_fp ────────────────────────────────────────
    movzbl  277(%r12), %ecx     // reload flags (rdi was clobbered)
    testb   $0x40, %cl
    jz      .Lreturn_sysv
    movq    %xmm0, %rax         // FP result → rax (bit-cast)
.Lreturn_sysv:

    // ── 7. Epilogue ─────────────────────────────────────────────────
    leaq    -16(%rbp), %rsp     // restore rsp (skip stack args)
    popq    %r13
    popq    %r12
    popq    %rbp
    ret

    .size platform_call, . - platform_call

// ─────────────────────────────────────────────────────────────────────
// platform_call_struct — on x86-64 SysV, struct ptr is already in
// int_regs[0] via classify_args (rdi).  Just delegate to platform_call.
// ─────────────────────────────────────────────────────────────────────

    .globl platform_call_struct
    .type  platform_call_struct, @function

platform_call_struct:
    // rdi = desc (same as platform_call), rsi = struct_ptr (ignored — already in int_regs[0])
    jmp     platform_call

    .size platform_call_struct, . - platform_call_struct

#endif  // __x86_64__ && !_WIN32
