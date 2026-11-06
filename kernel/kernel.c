#include "console.h"
#include "keyboard.h"
#include "shell.h"

void kmain(void) {
    serial_init();
    kset_cursor(22, 0);
    kputs("openpony booted sucessfully.\n");
    kputs("press enter to continue with the boot text, otherwise press any key.\n\n");

    if (kgetc() != 10) {
        kclear();
    }

    kputs("openpony\n");
    shell_run();
}