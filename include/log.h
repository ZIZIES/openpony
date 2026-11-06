#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

enum { LOG_OK, LOG_INFO, LOG_WARN, LOG_FAIL };

int  kvprintf(const char *fmt, va_list ap);     /* returns chars written */
void kprintf(const char *fmt, ...);
void klog(int level, const char *fmt, ...);

#endif