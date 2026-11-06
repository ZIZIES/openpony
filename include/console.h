#ifndef CONSOLE_H
#define CONSOLE_H

#include "types.h"

/* vga text attribute colors (foreground, black background) */
#define C_GREY    0x07
#define C_GREEN   0x0A
#define C_CYAN    0x0B
#define C_RED     0x0C
#define C_PINK    0x0D
#define C_YELLOW  0x0E
#define C_WHITE   0x0F

void serial_init(void);
void kclear(void);
void kset_cursor(int row, int col);
void ksetcolor(u8 color);
void kputc(char c);
void kputs(const char *s);
void knewline(void);

#endif