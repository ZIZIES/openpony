#include <stdarg.h>
#include "types.h"
#include "console.h"
#include "log.h"

static void put_num(u64 v, unsigned base, int width, char pad) {
    char tmp[24];
    int n = 0;
    do {
        tmp[n++] = "0123456789abcdef"[v % base];
        v /= base;
    } while (v);
    for (int i = n; i < width; i++) kputc(pad);
    while (n) kputc(tmp[--n]);
}

/* supports: %s %c %d %u %x %p %%, optional 0 flag + width, optional l / ll */
static void kvprintf(const char *fmt, va_list ap) {
    for (; *fmt; fmt++) {
        if (*fmt != '%') { kputc(*fmt); continue; }
        fmt++;

        char pad = ' ';
        int width = 0, lng = 0;
        if (*fmt == '0') { pad = '0'; fmt++; }
        while (*fmt >= '0' && *fmt <= '9') width = width * 10 + (*fmt++ - '0');
        while (*fmt == 'l') { lng++; fmt++; }

        switch (*fmt) {
        case 's': {
            const char *s = va_arg(ap, const char *);
            kputs(s ? s : "(null)");
            break;
        }
        case 'c':
            kputc((char)va_arg(ap, int));
            break;
        case 'd': {
            long v = lng ? va_arg(ap, long) : va_arg(ap, int);
            u64 u = (u64)v;
            if (v < 0) { kputc('-'); u = -u; if (width) width--; }
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
            kputs("0x");
            put_num((u64)va_arg(ap, void *), 16, 16, '0');
            break;
        case '%':
            kputc('%');
            break;
        case 0:
            return;
        default:
            kputc('%');
            kputc(*fmt);
            break;
        }
    }
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