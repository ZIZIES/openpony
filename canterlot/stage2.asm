bits 16
org 0x7E00

%define E820_BUF 0x5000     ; u32 count, then 24-byte entries from +0x10

stage2:
    mov [boot_drive], dl

    call enable_a20
    call check_a20
    je .a20_fail
    mov si, m_a20
    call log_ok

    call detect_memory
    jc .mem_fail
    mov si, m_mem
    call log_ok

    mov si, kdap
    mov dl, [boot_drive]
    mov ah, 0x42
    int 0x13
    jc .kfail
    mov si, m_kernel
    call log_ok

    jmp enter_pm

.a20_fail:
    mov si, e_a20
    jmp .die
.mem_fail:
    mov si, e_mem
    jmp .die
.kfail:
    mov si, e_kernel
.die:
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

log_ok:                     ; "[  ok  ] " + string in si
    push si
    mov si, s_open
    call print
    mov ax, 0x0920
    mov bx, 0x000A
    mov cx, 4
    int 0x10
    mov si, s_ok
    call print
    mov si, s_close
    call print
    pop si
    call print
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

detect_memory:              ; bios e820 memory map -> E820_BUF, CF=1 on failure
    mov dword [E820_BUF], 0
    mov di, E820_BUF + 16
    xor ebx, ebx            ; continuation value, 0 = start of list
    xor bp, bp              ; entry count
.next:
    mov dword [di + 20], 1  ; acpi attrs default (some bioses return only 20 bytes)
    mov eax, 0xE820
    mov edx, 0x534D4150     ; 'SMAP'
    mov ecx, 24
    int 0x15
    jc .end
    cmp eax, 0x534D4150
    jne .fail
    jcxz .skip              ; empty entry
    inc bp
    add di, 24
    cmp bp, 32
    jae .end
.skip:
    test ebx, ebx
    jnz .next
.end:
    test bp, bp
    jz .fail
    movzx eax, bp
    mov [E820_BUF], eax
    clc
    ret
.fail:
    stc
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

s_open:   db "[ ", 0
s_ok:     db " ok ", 0
s_close:  db " ] ", 0
m_a20:    db "stage 2: a20 gate enabled", 13, 10, 0
m_mem:    db "stage 2: memory map read (bios e820)", 13, 10, 0
m_kernel: db "stage 2: kernel loaded at 0x10000 (64 sectors)", 13, 10, 0
e_a20:    db "[ fail ] stage 2: a20 gate would not enable", 13, 10, 0
e_mem:    db "[ fail ] stage 2: bios e820 memory map failed", 13, 10, 0
e_kernel: db "[ fail ] stage 2: kernel read failed", 13, 10, 0
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

    mov rax, 0x10000        ; kernel entry (_start)
    jmp rax

times 2048 - ($ - $$) db 0