#ifndef SYSINFO_H
#define SYSINFO_H

#include "types.h"

void sysinfo_init(void);                /* reads cpuid + the e820 map canterlot saved */
const char *sysinfo_cpu_vendor(void);
const char *sysinfo_cpu_brand(void);
u64  sysinfo_ram_kib(void);             /* total usable ram per e820 */
void sysinfo_log_memmap(void);

#endif