// platform_call_win64.S — Windows x64 (Microsoft ABI) native call trampoline.
// GAS syntax for GCC/Clang targeting Windows (MinGW, Clang-CL with GAS).
//
// Windows x64 ABI:
//   4 register slots (shared int/FP): rcx/xmm0, rdx/xmm1, r8/xmm2, r9/xmm3
//   32-byte shadow space mandatory before CALL
//   Stack args at [rsp+40], [rsp+48], ...
//   Callee-saved: rbx, rbp, rdi, rsi, r12-r15, xmm6-xmm15
//
// extern "C" uint64_t platform_call(const PlatformCallDesc* desc);
//   desc in rcx (Win64 first arg)
//
// PlatformCallDesc offsets (verified via offsetof):
//   0:   target
//   8:   int_regs[8]  (64B)
//   72:  fp_regs[8]   (64B)
//   144: stack_buf[128] (128B, alignas 16)
//   272: stack_size (uint16_t)
//   277: flags ([5:0]=fp_count for AL, [6]=returns_fp)

#if defined(__x86_64__) && defined(_WIN32)

    .text
    .globl platform_call
    .def   platform_call; .scl 2; .type 32; .endef

platform_call:
    pushq   %rbp
    movq    %rsp, %rbp
    pushq   %r12
    pushq   %r13
    pushq   %rdi                // callee-saved on Windows (not SysV!)
    pushq   %rsi                // callee-saved on Windows

    movq    %rcx, %r12          // r12 = desc (callee-saved)

    // ── 1. Calculate total stack: shadow(32) + stack_buf + alignment ──
    movzwl  272(%r12), %eax     // stack_size
    addl    $32, %eax           // + shadow space
    addl    $15, %eax
    andl    $-16, %eax          // align 16
    subq    %rax, %rsp
    movq    %rax, %r13          // r13 = total allocated (for cleanup)

    // ── 2. Copy stack_buf to [rsp+32] (after shadow space) ──────────
    movzwl  272(%r12), %ecx     // stack_size
    testl   %ecx, %ecx
    jz      .Lno_stack_win64

    leaq    144(%r12), %rsi     // src = desc->stack_buf
    leaq    32(%rsp), %rdi      // dst = rsp + 32 (after shadow)
    rep     movsb

.Lno_stack_win64:
    movq    0(%r12), %r13       // r13 = target

    // ── 3. Load FP args into xmm0-3 (shared slot model) ────────────
    movq    72(%r12), %xmm0     // fp_regs[0] → xmm0 (slot 0)
    movq    80(%r12), %xmm1     // fp_regs[1] → xmm1 (slot 1)
    movq    88(%r12), %xmm2     // fp_regs[2] → xmm2 (slot 2)
    movq    96(%r12), %xmm3     // fp_regs[3] → xmm3 (slot 3)

    // ── 4. Load int args into rcx/rdx/r8/r9 ────────────────────────
    // Load in reverse order so rcx (which held desc) is last
    movq    32(%r12), %r9       // int_regs[3] → r9
    movq    24(%r12), %r8       // int_regs[2] → r8
    movq    16(%r12), %rdx      // int_regs[1] → rdx
    movq    8(%r12), %rcx       // int_regs[0] → rcx (clobbers desc ptr)

    // ── 5. Call target ──────────────────────────────────────────────
    call    *%r13

    // ── 6. Handle returns_fp ────────────────────────────────────────
    movzbl  277(%r12), %ecx     // flags
    testb   $0x40, %cl
    jz      .Lreturn_win64
    movq    %xmm0, %rax
.Lreturn_win64:

    // ── 7. Epilogue ─────────────────────────────────────────────────
    leaq    -32(%rbp), %rsp     // restore past 4 pushes
    popq    %rsi
    popq    %rdi
    popq    %r13
    popq    %r12
    popq    %rbp
    ret

    // ── platform_call_struct (Win64: struct ptr in rcx, shifts others) ──
    .globl platform_call_struct
    .def   platform_call_struct; .scl 2; .type 32; .endef

platform_call_struct:
    // rcx = desc, rdx = struct_return_ptr
    // On Win64, struct ptr goes in rcx (first arg), shifting others.
    // classify_args already put struct_ptr in int_regs[0] → rcx.
    // Just delegate to platform_call.
    jmp     platform_call

#endif  // __x86_64__ && _WIN32
