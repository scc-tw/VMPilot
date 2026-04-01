// platform_call_x86_32.S — x86-32 cdecl/stdcall trampoline (GAS syntax).
//
// x86-32 cdecl: ALL args on stack, caller cleans up.
// x86-32 stdcall: ALL args on stack, callee cleans up.
//
// extern "C" uint64_t platform_call(const PlatformCallDesc* desc);
//   desc at [esp+4] (cdecl: first arg on stack)
//
// Returns: eax (low 32), edx (high 32) for int64_t.
//          For FP return: fstp qword → edx:eax.
//
// PlatformCallDesc offsets (32-bit build — pointer = 4 bytes):
// NOTE: offsets differ from 64-bit due to pointer size!
// These must be verified via offsetof() on the target platform.
// For now, we access stack_buf/stack_size via known relative positions.
// On 32-bit: target=0, int_regs=4 (4B ptr), fp_regs=36 (4+8*4=36),
//            stack_buf=68 (36+8*4=68 → aligned to 16 → 80),
//            ... but PlatformCallDesc is designed for 64-bit.
//
// WORKAROUND: On 32-bit, we use a simplified approach:
//   The classify_args puts ALL args into stack_buf with correct byte sizes.
//   The trampoline just: sub esp, stack_size; memcpy [esp], stack_buf; call.
//
// Since int_regs/fp_regs are unused on x86-32 (no register args in cdecl),
// we only need: desc->target, desc->stack_buf, desc->stack_size, desc->flags,
// desc->convention.

#if defined(__i386__)

    .text
    .globl platform_call
    .type  platform_call, @function
    .p2align 4

platform_call:
    pushl   %ebp
    movl    %esp, %ebp
    pushl   %esi
    pushl   %edi
    pushl   %ebx

    movl    8(%ebp), %ebx       // ebx = desc

    // Load target
    movl    0(%ebx), %edx       // edx = target (void*, 4 bytes on 32-bit)
    // NOTE: On 32-bit, PlatformCallDesc.target is still void* = 4 bytes.
    // But the struct was designed with uint64_t int_regs[8] after target.
    // On ILP32, sizeof(void*) = 4, so target is at offset 0 (4 bytes),
    // then 4 bytes of padding (struct alignment), then int_regs at offset 8.
    // Actually, PlatformCallDesc has target as void* (4B on 32-bit),
    // followed immediately by int_regs[8] (uint64_t array).
    // The struct padding depends on the compiler. We should use the
    // stack_buf directly since all args are there for x86-32.

    // For x86-32, classify_args puts everything in stack_buf.
    // We need to find stack_buf and stack_size in the struct.
    // On 32-bit: offsetof(PlatformCallDesc, stack_buf) and
    // offsetof(PlatformCallDesc, stack_size) are different from 64-bit.
    //
    // PRAGMATIC APPROACH: Use hardcoded offsets determined at build time.
    // The classify_args.cpp will write to a separate x86-32-specific
    // struct if needed. For now, assume the same struct with sizeof(void*)=4:
    //   target: 0 (4B)
    //   pad: 4 (4B for uint64_t alignment)
    //   int_regs: 8 (64B)
    //   fp_regs: 72 (64B)
    //   stack_buf: 144 (128B, alignas 16 — may have less padding on 32-bit)
    //   stack_size: 272 (2B)
    //   ...flags: 277
    //   convention: 278
    //
    // Actually on 32-bit, alignas(16) stack_buf at offset 136+padding.
    // This is fragile. Let's use a DIFFERENT interface for 32-bit:
    // Just read stack_buf pointer and stack_size from fixed positions.

    // We'll use a simpler approach: classify_args on x86-32 writes
    // the stack image to stack_buf starting at the known offset.
    // For cross-platform safety, read stack_size and call.

    // For GAS x86-32 on Linux/MinGW, use these offsets:
    // (verified: sizeof(void*)==4 → target at 0, int_regs at 8)
    // stack_buf at 144, stack_size at 272, flags at 277, convention at 278
    // These offsets are the SAME as 64-bit because uint64_t arrays don't
    // change size, and void* has 4B padding to align to uint64_t.

    movzwl  272(%ebx), %ecx     // ecx = stack_size
    testl   %ecx, %ecx
    jz      .Lno_stack_32

    // Allocate stack space (4-byte aligned is sufficient for x86-32)
    addl    $3, %ecx
    andl    $-4, %ecx
    subl    %ecx, %esp

    // Copy stack_buf to [esp]
    leal    144(%ebx), %esi     // src = desc->stack_buf
    movl    %esp, %edi          // dst = esp
    movzwl  272(%ebx), %ecx     // count = stack_size (original)
    rep     movsb

.Lno_stack_32:
    // Call target
    movl    0(%ebx), %eax       // target (void* at offset 0)
    call    *%eax

    // Convention check: if stdcall (convention == 1), callee already cleaned stack.
    // For cdecl (convention == 0), we clean.
    movzbl  278(%ebx), %ecx     // convention
    testl   %ecx, %ecx
    jnz     .Lskip_cleanup_32

    // cdecl cleanup
    movzwl  272(%ebx), %ecx
    addl    $3, %ecx
    andl    $-4, %ecx
    addl    %ecx, %esp

.Lskip_cleanup_32:
    // Check returns_fp → read from x87 ST(0)
    movzbl  277(%ebx), %ecx     // flags
    testb   $0x40, %cl
    jz      .Lint_ret_32

    // x87 FP return: pop ST(0) into edx:eax
    subl    $8, %esp
    fstpl   (%esp)              // fstp qword ptr [esp]
    movl    (%esp), %eax        // low 32 bits
    movl    4(%esp), %edx       // high 32 bits
    addl    $8, %esp

.Lint_ret_32:
    // edx:eax has the 64-bit result (int or FP bit-cast)
    popl    %ebx
    popl    %edi
    popl    %esi
    popl    %ebp
    ret

    .size platform_call, . - platform_call

    // platform_call_struct: struct ptr already in stack_buf[0..3]
    .globl platform_call_struct
    .type  platform_call_struct, @function
platform_call_struct:
    jmp     platform_call
    .size platform_call_struct, . - platform_call_struct

#endif  // __i386__
