#ifndef BASIC_H
#define BASIC_H

/* the one header for programs in builtins/ : our own tiny libc */

#include <stddef.h>
#include <stdarg.h>
#include "types.h"
#include "sysinfo.h"            /* sysinfo_cpu_brand(), sysinfo_ram_kib() ... */

#define COLOR_GREY    0x07
#define COLOR_GREEN   0x0A
#define COLOR_CYAN    0x0B
#define COLOR_RED     0x0C
#define COLOR_PINK    0x0D
#define COLOR_YELLOW  0x0E
#define COLOR_WHITE   0x0F

/* ---- stdio ---- */
int  printf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
int  putchar(int c);
int  puts(const char *s);               /* writes s and a newline, like libc */
int  getchar(void);                     /* blocks until a key is pressed */
void clear_screen(void);
void set_color(int color);

/* ---- power ---- */
void poweroff(void);                    /* returns only if the machine ignored us */

/* ---- string.h ---- */
size_t strlen(const char *s);
int    strcmp(const char *a, const char *b);
int    strncmp(const char *a, const char *b, size_t n);
char  *strcpy(char *dst, const char *src);
char  *strchr(const char *s, int c);
void  *memset(void *dst, int c, size_t n);
void  *memcpy(void *dst, const void *src, size_t n);
void  *memmove(void *dst, const void *src, size_t n);
int    memcmp(const void *a, const void *b, size_t n);

/* ---- stdlib.h ---- */
int atoi(const char *s);
int abs(int x);

#endif