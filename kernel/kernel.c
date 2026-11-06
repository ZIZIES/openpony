#include "console.h"

void kmain(void) {
    kset_cursor(22, 0);
    kputs("hello from openpony\n");
    kputs("hello from zion also\n");
    kputs("dont forget mason!");
    for (;;) __asm__ volatile ("hlt");
}