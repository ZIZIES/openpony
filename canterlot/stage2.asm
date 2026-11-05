bits 16
org 0x7E00

stage2:
    mov [boot_drive], dl
    mov si, msg
    call print

    call enable_a20
    call check_a20
    je .a20_fail

    mov si, a20_ok
    call print

    mov si, kdap
    mov dl, [boot_drive]
    mov ah, 0x42
    int 0x13
    jc .kfail
    jmp enter_pm

.kfail:
    mov si, k_fail
    call print
    jmp .hang

.a20_fail:
    mov si, a20_bad
    call print
.hang:
    hlt
    jmp .hang

; ---------- routines (16-bit) ----------

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

; ---------- switch to protected mode (still 16-bit code) ----------

enter_pm:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:pm_entry

gdt_start:
    dq 0
gdt_code:                   ; selector 0x08
    dw 0xFFFF, 0x0000
    db 0x00, 10011010b, 11001111b, 0x00
gdt_data:                   ; selector 0x10
    dw 0xFFFF, 0x0000
    db 0x00, 10010010b, 11001111b, 0x00
gdt_code64:                 ; selector 0x18
    dw 0xFFFF, 0x0000
    db 0x00, 10011010b, 10101111b, 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; ---------- data ----------

msg:        db "canterlot: stage 2 loaded", 13, 10, 0
a20_ok:     db "canterlot: a20 enabled", 13, 10, 0
a20_bad:    db "canterlot: a20 FAILED", 13, 10, 0
k_fail:     db "canterlot: kernel read failed", 13, 10, 0
pm_msg:     db "canterlot: protected mode!", 0
lm_msg:     db "canterlot: long mode!", 0
boot_drive: db 0

kdap:
    db 0x10, 0
    dw 64                   ; 64 sectors = 32KB
    dw 0x0000               ; offset
    dw 0x1000               ; segment -> physical 0x10000
    dq 5                    ; starting LBA

; ---------- 32-bit code, only from here down ----------

bits 32
pm_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000

    mov edi, 0xB8000 + 160*20
    mov esi, pm_msg
.loop:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0F
    mov [edi], ax
    add edi, 2
    jmp .loop
.done:

    ; --- build page tables at 0x1000 / 0x2000 / 0x3000 ---
    mov edi, 0x1000
    xor eax, eax
    mov ecx, 3 * 4096 / 4
    rep stosd

    mov dword [0x1000], 0x2000 | 0x03   ; pml4[0] -> pdpt
    mov dword [0x2000], 0x3000 | 0x03   ; pdpt[0] -> pd

    mov edi, 0x3000
    mov eax, 0x83                       ; present + writable + 2MB page
    mov ecx, 512
.map:
    mov [edi], eax
    add eax, 0x200000
    add edi, 8
    loop .map

    ; --- flip the switches ---
    mov eax, 0x1000
    mov cr3, eax

    mov eax, cr4
    or eax, 1 << 5                      ; PAE
    mov cr4, eax

    mov ecx, 0xC0000080                 ; EFER MSR
    rdmsr
    or eax, 1 << 8                      ; LME
    wrmsr

    mov eax, cr0
    or eax, 1 << 31                     ; PG
    mov cr0, eax

    jmp 0x18:long_entry

; ---------- 64-bit code ----------

bits 64
long_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov rdi, 0xB8000 + 160*21
    mov rsi, lm_msg
.loop:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0F
    mov [rdi], ax
    add rdi, 2
    jmp .loop
.done:
    mov rax, 0x10000        ; kernel entry (_start)
    jmp rax

times 2048 - ($ - $$) db 0