#include "console.h"
#include "keyboard.h"
#include "log.h"
#include "basic.h"

int printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int n = kvprintf(fmt, ap);
    va_end(ap);
    return n;
}

int putchar(int c)       { kputc((char)c); return c; }
int puts(const char *s)  { kputs(s); kputc('\n'); return 0; }
int getchar(void)        { return kgetc(); }
void clear_screen(void)  { kclear(); }
void set_color(int c)    { ksetcolor((u8)c); }