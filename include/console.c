#include "types.h"
#include "console.h"

#define VGA  ((volatile u16 *)0xB8000)
#define COLS 80

static int cursor;

void kset_cursor(int row, int col) { cursor = row * COLS + col; }

void knewline(void) { cursor += COLS - (cursor % COLS); }

void kputc(char c) {
    if (c == '\n') { knewline(); return; }
    VGA[cursor++] = 0x0F00 | (u8)c;
}

void kputs(const char *s) {
    while (*s) kputc(*s++);
}