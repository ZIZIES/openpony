// help: power off the machine
#include "basic.h"

int main(void) {
    printf("shutting down...\n");
    poweroff();

    /* still running: this machine ignored the emulator poweroff ports */
    printf("poweroff not supported here. system halted, you can turn it off now.\n");
    for (;;) __asm__ volatile ("cli; hlt");
}