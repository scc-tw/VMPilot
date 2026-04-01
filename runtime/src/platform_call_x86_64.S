// platform_call_x86_64.S — System V x86-64 ABI native call trampoline.
//
// Loads integer args into rdi/rsi/rdx/rcx/r8/r9 and FP args into xmm0-7,
// sets AL for variadic, calls the target, and returns integer or FP result.
//
// extern "C" uint64_t platform_call(
//     void* target,           // rdi
//     const uint64_t* int_regs, // rsi — 8 integer arg values
//     const uint64_t* fp_regs,  // rdx — 8 FP bit-patterns for xmm0-7
//     uint8_t flags             // cl  — [5:0]=fp_count, [6]=returns_fp
// );

#if defined(__x86_64__) && !defined(_WIN32)

    .text
    .globl platform_call
    .type  platform_call, @function
    .p2align 4

platform_call:
    // Prologue: save callee-saved r12, align stack for inner call.
    // Entry: rsp is 8-misaligned (return addr pushed by caller).
    // pushq rbp + pushq r12 = -16 → aligned.
    pushq   %rbp
    movq    %rsp, %rbp
    pushq   %r12

    // Save arguments that will be clobbered by register loading.
    movq    %rdi, %r10          // r10 = target
    movq    %rsi, %r11          // r11 = int_regs
    movzbl  %cl, %r12d          // r12 = flags (zero-extend byte)

    // ── Load 8 FP args from fp_regs (rdx) into xmm0-7 ──
    movq     0(%rdx), %xmm0
    movq     8(%rdx), %xmm1
    movq    16(%rdx), %xmm2
    movq    24(%rdx), %xmm3
    movq    32(%rdx), %xmm4
    movq    40(%rdx), %xmm5
    movq    48(%rdx), %xmm6
    movq    56(%rdx), %xmm7

    // ── Set AL = fp_count (lower 6 bits of flags) ──
    // System V ABI: for variadic functions, AL = number of vector (xmm)
    // registers used.  Non-variadic functions ignore AL, so always setting
    // it is harmless and simplifies the code.
    movl    %r12d, %eax
    andb    $0x3F, %al

    // ── Load 6 integer args from int_regs (r11) into rdi..r9 ──
    // Load in reverse order so rdi (which held target) is loaded last.
    movq    40(%r11), %r9
    movq    32(%r11), %r8
    movq    24(%r11), %rcx
    movq    16(%r11), %rdx
    movq     8(%r11), %rsi
    movq     0(%r11), %rdi

    // ── Call target ──
    // Stack is 16-byte aligned (pushq rbp + pushq r12 = 16 bytes).
    call    *%r10

    // ── Handle return value ──
    // Integer result is already in rax.
    // If returns_fp (bit 6 of flags), copy xmm0 bit-pattern to rax.
    testb   $0x40, %r12b
    jz      .Lreturn
    movq    %xmm0, %rax
.Lreturn:

    // Epilogue
    popq    %r12
    popq    %rbp
    ret

    .size platform_call, . - platform_call

#endif  // __x86_64__ && !_WIN32
