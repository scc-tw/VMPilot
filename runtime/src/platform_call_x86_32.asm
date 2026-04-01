; platform_call_x86_32.asm — x86-32 cdecl/stdcall trampoline for MSVC (ml.exe).
; MASM syntax, 32-bit.
;
; x86-32: ALL args on stack. No register args (cdecl/stdcall).
; FP return via x87 ST(0) → fstp to get double as edx:eax.

.MODEL FLAT, C

; PlatformCallDesc offsets (same as 64-bit due to uint64_t arrays):
;   0:   target (void*, 4B on 32-bit)
;   144: stack_buf[128]
;   272: stack_size (uint16_t)
;   277: flags
;   278: convention

.CODE

; extern "C" uint64_t platform_call(const PlatformCallDesc* desc);
;   desc at [esp+4]
platform_call PROC
    push    ebp
    mov     ebp, esp
    push    esi
    push    edi
    push    ebx

    mov     ebx, [ebp + 8]      ; ebx = desc

    ; Allocate and copy stack args
    movzx   ecx, WORD PTR [ebx + 272]   ; stack_size
    test    ecx, ecx
    jz      no_stack

    add     ecx, 3
    and     ecx, NOT 3           ; 4-byte align
    sub     esp, ecx

    lea     esi, [ebx + 144]     ; src = stack_buf
    mov     edi, esp             ; dst = esp
    movzx   ecx, WORD PTR [ebx + 272]
    rep     movsb

no_stack:
    ; Call target
    mov     eax, DWORD PTR [ebx]
    call    eax

    ; Convention check
    movzx   ecx, BYTE PTR [ebx + 278]
    test    ecx, ecx
    jnz     skip_cleanup

    ; cdecl cleanup
    movzx   ecx, WORD PTR [ebx + 272]
    add     ecx, 3
    and     ecx, NOT 3
    add     esp, ecx

skip_cleanup:
    ; Check returns_fp
    movzx   ecx, BYTE PTR [ebx + 277]
    test    cl, 40h
    jz      int_ret

    ; x87 return → edx:eax
    sub     esp, 8
    fstp    QWORD PTR [esp]
    mov     eax, DWORD PTR [esp]
    mov     edx, DWORD PTR [esp + 4]
    add     esp, 8

int_ret:
    pop     ebx
    pop     edi
    pop     esi
    pop     ebp
    ret
platform_call ENDP

; platform_call_struct: delegate (struct ptr already in stack_buf)
platform_call_struct PROC
    jmp     platform_call
platform_call_struct ENDP

END
