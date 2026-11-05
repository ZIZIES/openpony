typedef unsigned char  u8;
typedef unsigned short u16;

#define VGA ((volatile u16 *)0xB8000)

static int cursor;

static void putc(char c) { VGA[cursor++] = 0x0F00 | (u8)c; }

static void puts(const char *s) {
    while (*s) putc(*s++);
}

void kmain(void) {
    cursor = 80 * 22;               /* row 22, under the canterlot boot log */
    puts("hello from openpony");
    for (;;) __asm__ volatile ("hlt");
    puts("hello from zion also");
    for (;;) __asm__ volatile ("hlt");
    puts("dont forget mason!");
    for (;;) __asm__ volatile ("hlt");
}