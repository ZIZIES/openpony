#include <stdarg.h>
#include "types.h"
#include "console.h"
#include "log.h"

static int emitted;                     /* chars written by the current kvprintf */

static void out(char c) { kputc(c); emitted++; }

static void out_str(const char *s) { while (*s) out(*s++); }

static void put_num(u64 v, unsigned base, int width, char pad) {
    char tmp[24];
    int n = 0;
    do {
        tmp[n++] = "0123456789abcdef"[v % base];
        v /= base;
    } while (v);
    for (int i = n; i < width; i++) out(pad);
    while (n) out(tmp[--n]);
}

/* supports: %s %c %d %u %x %p %%, optional 0 flag + width, optional l / ll */
int kvprintf(const char *fmt, va_list ap) {
    emitted = 0;
    for (; *fmt; fmt++) {
        if (*fmt != '%') { out(*fmt); continue; }
        fmt++;

        char pad = ' ';
        int width = 0, lng = 0;
        if (*fmt == '0') { pad = '0'; fmt++; }
        while (*fmt >= '0' && *fmt <= '9') width = width * 10 + (*fmt++ - '0');
        while (*fmt == 'l') { lng++; fmt++; }

        switch (*fmt) {
        case 's': {
            const char *s = va_arg(ap, const char *);
            out_str(s ? s : "(null)");
            break;
        }
        case 'c':
            out((char)va_arg(ap, int));
            break;
        case 'd': {
            long v = lng ? va_arg(ap, long) : va_arg(ap, int);
            u64 u = (u64)v;
            if (v < 0) { out('-'); u = -u; if (width) width--; }
            put_num(u, 10, width, pad);
            break;
        }
        case 'u':
            put_num(lng ? va_arg(ap, unsigned long) : va_arg(ap, unsigned int), 10, width, pad);
            break;
        case 'x':
            put_num(lng ? va_arg(ap, unsigned long) : va_arg(ap, unsigned int), 16, width, pad);
            break;
        case 'p':
            out_str("0x");
            put_num((u64)va_arg(ap, void *), 16, 16, '0');
            break;
        case '%':
            out('%');
            break;
        case 0:
            return emitted;
        default:
            out('%');
            out(*fmt);
            break;
        }
    }
    return emitted;
}

void kprintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    kvprintf(fmt, ap);
    va_end(ap);
}

void klog(int level, const char *fmt, ...) {
    static const char *tag[]   = { " ok ", "info", "warn", "fail" };
    static const u8    color[] = { C_GREEN, C_CYAN, C_YELLOW, C_RED };

    kputs("[ ");
    ksetcolor(color[level]);
    kputs(tag[level]);
    ksetcolor(C_GREY);
    kputs(" ] ");

    va_list ap;
    va_start(ap, fmt);
    kvprintf(fmt, ap);
    va_end(ap);
    kputc('\n');
}