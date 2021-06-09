_TEXT    SEGMENT

extern stop_durability_damage_injection_return: qword
extern stop_durability_damage_original_jump: qword


PUBLIC CheatsASMFollow
PUBLIC stop_durability_damage_hook

; follows an address
CheatsASMFollow PROC

    mov rax, qword ptr[rcx]
    ret

CheatsASMFollow ENDP

; rcx = BaseB
; rdx = Homeward
CheatsASMHomewardWrapper PROC

    mov edx, 01h
    sub rsp, 38h
    mov rax, qword ptr[rcx+08h]     ; CheatsASMHomewardWrapperArguments->_Homeward
    mov rcx, qword ptr[rcx]         ; CheatsASMHomewardWrapperArguments->_BaseB
    call rax                        ; call _Homeward
    add rsp, 38h
    ret

CheatsASMHomewardWrapper ENDP

CheatsASMReviveCharWrapper PROC

    sub rsp, 28h
    mov rax, 1404EC430h
    call rax
    add rsp, 28h
    ret

CheatsASMReviveCharWrapper ENDP

CheatsASMKickPlayerWrapper PROC

    mov edx, 10044001h
    mov r8d, 01h
    sub rsp, 38h
    mov rax, 1406EDB70h
    call rax
    add rsp, 38h
    ret

CheatsASMKickPlayerWrapper ENDP

stop_durability_damage_hook PROC

    cmp dword ptr[r11+14h], r9d                     ; if the new durability value is NOT lower than the current value
    jle originalcode                                ;     we don't need to change the new value since it's the same or large than the current one
    mov r9d, dword ptr[r11+14h]                     ; else: set the new durability value to the current one (freezing it)
    
    originalcode:                                   ; since the long jump overwrites part of the function it is added here
                                                    ; the long jump overwrites the following four instructions
        mov dword ptr [r11+14h], r9d                ; updates the durability value (to new value - which was set to the current value)
        cmp r10d, dword ptr[rcx+20h]                ; instruction that was overwritten
        jge hopper                                  ; this was tricky, since the relative jump was overwritten we calculate the address it would have jumped to (stop_durability_damage_original_jump)
        mov rax, qword ptr[rcx+20h]                 ; last instruciton that was overwritten
        jmp stop_durability_damage_injection_return ; jump back to the original code (right after the long jump)

    hopper:
        jmp stop_durability_damage_original_jump    ; had to use a hopper & jmp as conditional jumps only support relative mode

stop_durability_damage_hook ENDP

_TEXT    ENDS

END
