; platform_call_win64.asm — Windows x64 (Microsoft ABI) trampoline for MSVC.
; MASM syntax (Intel notation, ml64.exe).
;
; Windows x64 ABI:
;   4 register slots (shared int/FP): rcx/xmm0, rdx/xmm1, r8/xmm2, r9/xmm3
;   32-byte shadow space mandatory before CALL
;   Stack args at [rsp+40], [rsp+48], ...
;
; PlatformCallDesc offsets:
;   0:   target
;   8:   int_regs[8]  (64B)
;   72:  fp_regs[8]   (64B)
;   144: stack_buf[128] (128B)
;   272: stack_size (uint16_t)
;   277: flags

_TEXT SEGMENT

; extern "C" uint64_t platform_call(const PlatformCallDesc* desc);
;   desc in rcx
platform_call PROC FRAME
    push    rbp
    .pushreg rbp
    mov     rbp, rsp
    push    r12
    .pushreg r12
    push    r13
    .pushreg r13
    push    rdi
    .pushreg rdi
    push    rsi
    .pushreg rsi
    .endprolog

    mov     r12, rcx            ; r12 = desc

    ; 1. Allocate shadow(32) + stack_buf + alignment
    movzx   eax, WORD PTR [r12 + 272]   ; stack_size
    add     eax, 32             ; + shadow space
    add     eax, 15
    and     eax, NOT 15         ; align 16
    sub     rsp, rax

    ; 2. Copy stack_buf to [rsp+32]
    movzx   ecx, WORD PTR [r12 + 272]
    test    ecx, ecx
    jz      no_stack

    lea     rsi, [r12 + 144]    ; src = stack_buf
    lea     rdi, [rsp + 32]     ; dst = after shadow
    rep     movsb

no_stack:
    mov     r13, QWORD PTR [r12]         ; target

    ; 3. Load FP regs (shared slots)
    movq    xmm0, QWORD PTR [r12 + 72]
    movq    xmm1, QWORD PTR [r12 + 80]
    movq    xmm2, QWORD PTR [r12 + 88]
    movq    xmm3, QWORD PTR [r12 + 96]

    ; 4. Load int regs (rcx last — clobbers desc)
    mov     r9,  QWORD PTR [r12 + 32]
    mov     r8,  QWORD PTR [r12 + 24]
    mov     rdx, QWORD PTR [r12 + 16]
    mov     rcx, QWORD PTR [r12 + 8]

    ; 5. Call
    call    r13

    ; 6. returns_fp check
    movzx   ecx, BYTE PTR [r12 + 277]
    test    cl, 40h
    jz      int_ret
    movq    rax, xmm0
int_ret:

    ; 7. Epilogue
    lea     rsp, [rbp - 32]     ; unwind past 4 pushes
    pop     rsi
    pop     rdi
    pop     r13
    pop     r12
    pop     rbp
    ret
platform_call ENDP

; extern "C" uint64_t platform_call_struct(desc, struct_ptr);
; Win64: struct_ptr already in int_regs[0] via classify_args → delegate
platform_call_struct PROC
    jmp     platform_call
platform_call_struct ENDP

_TEXT ENDS
END
