#include "basic.h"
#include "logo.h"

void print_logo(void) {
    static const char *name = "openpony";
    static const int colors[] = {
        COLOR_RED, COLOR_YELLOW, COLOR_GREEN, COLOR_CYAN,
        COLOR_PINK, COLOR_RED, COLOR_YELLOW, COLOR_GREEN
    };

    printf("  ");
    for (int i = 0; name[i]; i++) {
        set_color(colors[i]);
        putchar(name[i]);
    }
    set_color(COLOR_GREY);
    printf(" 0.0.1\n");
}