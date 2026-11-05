ENTRY(_start)
SECTIONS {
    . = 0x10000;
    .text   : { *(.text.entry) *(.text*) }
    .rodata : { *(.rodata*) }
    .data   : { *(.data*) }
    .bss    : { *(COMMON) *(.bss*) }
    /DISCARD/ : { *(.eh_frame) *(.comment) *(.note*) }
}