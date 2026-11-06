#ifndef LOG_H
#define LOG_H

enum { LOG_OK, LOG_INFO, LOG_WARN, LOG_FAIL };

void kprintf(const char *fmt, ...);
void klog(int level, const char *fmt, ...);

#endif