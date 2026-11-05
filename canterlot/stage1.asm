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

    mov si, msg
    call print

    mov si, dap
    mov dl, [boot_drive]
    mov ah, 0x42
    int 0x13
    jc disk_error

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

boot_drive: db 0
msg:     db "canterlot: hello from openpony", 13, 10, 0
err_msg: db "disk read failed", 13, 10, 0

dap:
    db 0x10, 0
    dw 4
    dw 0x7E00
    dw 0x0000
    dq 1

times 510 - ($ - $$) db 0
dw 0xAA55