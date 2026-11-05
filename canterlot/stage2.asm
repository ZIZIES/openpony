bits 16
org 0x7E00

stage2:
    mov si, msg
    call print

    call enable_a20
    call check_a20
    je .a20_fail

    mov si, a20_ok
    call print
    jmp .hang

.a20_fail:
    mov si, a20_bad
    call print
.hang:
    hlt
    jmp .hang

; ---------- routines ----------

print:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    xor bx, bx
    int 0x10
    jmp print
.done:
    ret

enable_a20:
    in al, 0x92
    or al, 2
    and al, 0xFE
    out 0x92, al
    ret

check_a20:                  ; returns ZF=1 if A20 is OFF
    push ds
    push es
    xor ax, ax
    mov ds, ax
    mov ax, 0xFFFF
    mov es, ax
    mov si, 0x0500
    mov di, 0x0510
    mov byte [ds:si], 0x00
    mov byte [es:di], 0xFF
    cmp byte [ds:si], 0xFF
    pop es
    pop ds
    ret

; ---------- data ----------

msg:     db "canterlot: stage 2 loaded", 13, 10, 0
a20_ok:  db "canterlot: a20 enabled", 13, 10, 0
a20_bad: db "canterlot: a20 FAILED", 13, 10, 0

times 2048 - ($ - $$) db 0