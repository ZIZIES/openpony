bits 64
global _start
extern kmain

section .text.entry
_start:
    mov rsp, 0x90000
    call kmain
.hang:
    hlt
    jmp .hang