#include "console.h"
#include "keyboard.h"
#include "kstring.h"
#include "shell.h"
#include "log.h"
#include "sysinfo.h"

#define LINE_MAX 128
#define ARGV_MAX 16

static int cmd_help(int argc, char **argv);

static int cmd_echo(int argc, char **argv) {
    (void)argv;
    for (int i = 1; i < argc; i++) {
        if (i > 1) kputc(' ');
        kputs(argv[i]);
    }
    kputc('\n');
    return 0;
}

static int cmd_clear(int argc, char **argv) {
    (void)argc; (void)argv;
    kclear();
    return 0;
}

static int cmd_fetch(int argc, char **argv) {
    (void)argc; (void)argv;
    kprintf("  openpony 0.0.1\n");
    kprintf("  ---------------\n");
    kprintf("  arch:        x86-64 (long mode)\n");
    kprintf("  bootstrap:   canterlot\n");
    kprintf("  cpu:         %s\n", sysinfo_cpu_brand());
    kprintf("  memory:      %lu MiB usable\n", sysinfo_ram_kib() / 1024);
    kprintf("  display:     vga text 80x50 + serial\n");
    kprintf("  shell:       pid 0 tty\n");
    return 0;
}

static const struct command commands[] = {
    { "help",  cmd_help,  "list commands" },
    { "echo",  cmd_echo,  "print arguments" },
    { "clear", cmd_clear, "clear the screen" },
    { "fetch", cmd_fetch, "system info" },
    { 0, 0, 0 }
};

static int cmd_help(int argc, char **argv) {
    (void)argc; (void)argv;
    for (const struct command *c = commands; c->name; c++) {
        kputs("  ");
        kputs(c->name);
        kputs(" - ");
        kputs(c->help);
        kputc('\n');
    }
    return 0;
}

static void readline(char *buf, int max) {
    int len = 0;
    for (;;) {
        char c = kgetc();
        if (c == '\n') {
            kputc('\n');
            buf[len] = 0;
            return;
        }
        if (c == '\b') {
            if (len > 0) { len--; kputc('\b'); }
            continue;
        }
        if (c < 32 || c > 126) continue;
        if (len < max - 1) {
            buf[len++] = c;
            kputc(c);
        }
    }
}

/* splits line in place: spaces become '\0', argv[] points into line */
static int tokenize(char *line, char **argv, int max) {
    int argc = 0;
    char *p = line;
    while (*p) {
        while (*p == ' ') *p++ = 0;
        if (!*p) break;
        if (argc < max) argv[argc++] = p;
        while (*p && *p != ' ') p++;
    }
    return argc;
}

static const struct command *find(const char *name) {
    for (const struct command *c = commands; c->name; c++)
        if (kstrcmp(c->name, name) == 0) return c;
    return 0;
}

void shell_run(void) {
    char  line[LINE_MAX];
    char *argv[ARGV_MAX];

    for (;;) {
        kputs("> ");
        readline(line, LINE_MAX);

        int argc = tokenize(line, argv, ARGV_MAX);
        if (argc == 0) continue;

        const struct command *cmd = find(argv[0]);
        if (cmd) {
            cmd->fn(argc, argv);                /* call through the pointer */
        } else {
            kputs(argv[0]);
            kputs(": command not found\n");
        }
    }
}