bits 16
org 0x7C00

    jmp 0x0000:start

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov [boot_drive], dl

    mov ax, 0x0003          ; 80x25 text mode (also clears the screen)
    int 0x10
    mov ax, 0x1112          ; load the 8x8 rom font -> 80x50 text mode
    xor bl, bl
    int 0x10

    mov si, banner
    call print

    mov si, dap
    mov dl, [boot_drive]
    mov ah, 0x42
    int 0x13
    jc disk_error

    mov si, m_stage2
    call log_ok

    mov dl, [boot_drive]
    jmp 0x0000:0x7E00

disk_error:
    mov si, err_msg
    call print

hang:
    hlt
    jmp hang

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

; prints "[  ok  ] " (ok in green) followed by the string in si
log_ok:
    push si
    mov si, s_open          ; "[ "
    call print
    mov ax, 0x0920          ; paint 4 cells green first...
    mov bx, 0x000A
    mov cx, 4
    int 0x10
    mov si, s_ok            ; ...then teletype keeps the attribute
    call print
    mov si, s_close         ; " ] "
    call print
    pop si
    call print
    ret

boot_drive: db 0
s_open:   db "[ ", 0
s_ok:     db " ok ", 0
s_close:  db " ] ", 0
banner:   db "canterlot 0.1 - openpony bootstrapper", 13, 10, 13, 10, 0
m_stage2: db "stage 1: stage 2 loaded from disk (lba 1, 4 sectors)", 13, 10, 0
err_msg:  db "[ fail ] stage 1: disk read error", 13, 10, 0

dap:
    db 0x10, 0
    dw 4
    dw 0x7E00
    dw 0x0000
    dq 1

times 510 - ($ - $$) db 0
dw 0xAA55