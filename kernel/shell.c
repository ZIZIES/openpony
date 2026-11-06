#include "console.h"
#include "keyboard.h"
#include "basic.h"
#include "shell.h"

#define LINE_MAX 128
#define ARGV_MAX 16

/* ---- the one command that needs to see the table itself ---- */

static int cmd_help(int argc, char **argv);
static const struct command help_cmd = { "help", cmd_help, "list commands" };

static void print_cmd(const struct command *c) {
    printf("  %s", c->name);
    for (size_t n = strlen(c->name); n < 10; n++) putchar(' ');
    printf("%s\n", c->help);
}

static int cmd_help(int argc, char **argv) {
    (void)argc; (void)argv;
    print_cmd(&help_cmd);
    for (const struct command *c = builtins; c->name; c++)
        print_cmd(c);
    return 0;
}

/* ---- shell ---- */

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
        if (c < 32 || c > 126) continue;        /* ignore tab, esc, etc. */
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
    if (strcmp(name, "help") == 0) return &help_cmd;
    for (const struct command *c = builtins; c->name; c++)
        if (strcmp(c->name, name) == 0) return c;
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