typedef unsigned char  u8;
typedef unsigned short u16;

#define VGA  ((volatile u16 *)0xB8000)
#define COLS 80

static int cursor;

static void knewline(void) { cursor += COLS - (cursor % COLS); }

static void kputc(char c) {
    if (c == '\n') { knewline(); return; }
    VGA[cursor++] = 0x0F00 | (u8)c;
}

static void kputs(const char *s) {
    while (*s) kputc(*s++);
}

void kmain(void) {
    cursor = COLS * 22;                 /* row 22, under the canterlot boot log */
    kputs("hello from openpony\n");
    kputs("hello from zion also\n");
    kputs("dont forget mason!");        /* no trailing \n: this is row 24, the last row */
    for (;;) __asm__ volatile ("hlt");
}