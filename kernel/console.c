#include "types.h"
#include "io.h"
#include "console.h"

#define VGA   ((volatile u16 *)0xB8000)
#define COLS  80
#define ROWS  25
#define COM1  0x3F8
#define ATTR  0x0F00            /* white on black */

static int cursor;

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
static void scroll(void) {
    for (int i = 0; i < COLS * (ROWS - 1); i++)
        VGA[i] = VGA[i + COLS];
    for (int i = COLS * (ROWS - 1); i < COLS * ROWS; i++)
        VGA[i] = ATTR | ' ';
    cursor = COLS * (ROWS - 1);
}

void kclear(void) {
    for (int i = 0; i < COLS * ROWS; i++)
        VGA[i] = ATTR | ' ';
    cursor = 0;
}

void kset_cursor(int row, int col) { cursor = row * COLS + col; }

void knewline(void) {
    cursor += COLS - (cursor % COLS);
    if (cursor >= COLS * ROWS) scroll();
}

static void kbackspace(void) {
    if (cursor % COLS == 0) return;         /* don't cross to the previous row (yet) */
    cursor--;
    VGA[cursor] = ATTR | ' ';
    serial_putc('\b'); serial_putc(' '); serial_putc('\b');
}

void kputc(char c) {
    if (c == '\n') {
        serial_putc('\r');
        serial_putc('\n');
        knewline();
        return;
    }
    if (c == '\b') { kbackspace(); return; }
    serial_putc(c);
    VGA[cursor++] = ATTR | (u8)c;
    if (cursor >= COLS * ROWS) scroll();
}

void kputs(const char *s) {
    while (*s) kputc(*s++);
}