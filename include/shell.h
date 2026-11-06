#ifndef SHELL_H
#define SHELL_H

struct command {
    const char *name;
    int (*fn)(int argc, char **argv);   /* function pointer: same shape as main() */
    const char *help;
};

void shell_run(void);

#endif