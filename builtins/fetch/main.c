// help: system info
#include "basic.h"
#include "logo.h"

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    print_logo();
    printf("  ---------------\n");
    printf("  arch:        x86-64 (long mode)\n");
    printf("  bootstrap:   canterlot\n");
    printf("  cpu:         %s\n", sysinfo_cpu_brand());
    printf("  memory:      %lu MiB usable\n", (unsigned long)(sysinfo_ram_kib() / 1024));
    printf("  display:     vga text 80x28 + serial\n");
    printf("  shell:       pid 0 tty\n");
    return 0;
}