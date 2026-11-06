#include "types.h"
#include "io.h"
#include "console.h"

#define VGA   ((volatile u16 *)0xB8000)
#define COLS  80
#define ROWS  50                /* 80x50 text mode, set up by canterlot stage 1 */
#define COM1  0x3F8

static int cursor;
static u16 attr = C_GREY << 8;  /* current color, already shifted into the high byte */

/* ---- serial ---- */
static void serial_putc(char c) {
    while (!(inb(COM1 + 5) & 0x20)) ;
    outb(COM1, (u8)c);
}

void serial_init(void) {
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);
    outb(COM1 + 0, 0x03);
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
}

/* ---- vga ---- */
static void hw_cursor(void) {           /* move the blinking cursor to `cursor` */
    outb(0x3D4, 0x0F);
    outb(0x3D5, (u8)(cursor & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (u8)(cursor >> 8));
}

static void scroll(void) {
    for (int i = 0; i < COLS * (ROWS - 1); i++)
        VGA[i] = VGA[i + COLS];
    for (int i = COLS * (ROWS - 1); i < COLS * ROWS; i++)
        VGA[i] = (C_GREY << 8) | ' ';
    cursor = COLS * (ROWS - 1);
}

void ksetcolor(u8 color) { attr = (u16)color << 8; }

void kclear(void) {
    for (int i = 0; i < COLS * ROWS; i++)
        VGA[i] = (C_GREY << 8) | ' ';
    cursor = 0;
    hw_cursor();
}

void kset_cursor(int row, int col) {
    cursor = row * COLS + col;
    hw_cursor();
}

void knewline(void) {
    cursor += COLS - (cursor % COLS);
    if (cursor >= COLS * ROWS) scroll();
}

static void kbackspace(void) {
    if (cursor % COLS == 0) return;
    cursor--;
    VGA[cursor] = (C_GREY << 8) | ' ';
    serial_putc('\b'); serial_putc(' '); serial_putc('\b');
}

void kputc(char c) {
    if (c == '\n') {
        serial_putc('\r');
        serial_putc('\n');
        knewline();
    } else if (c == '\b') {
        kbackspace();
    } else {
        serial_putc(c);
        VGA[cursor++] = attr | (u8)c;
        if (cursor >= COLS * ROWS) scroll();
    }
    hw_cursor();
}

void kputs(const char *s) {
    while (*s) kputc(*s++);
}