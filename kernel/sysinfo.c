#include "types.h"
#include "log.h"
#include "sysinfo.h"

/* what canterlot stage 2 left at 0x5000 (see detect_memory) */
struct e820_entry {
    u64 base;
    u64 len;
    u32 type;
    u32 acpi;
} __attribute__((packed));

#define E820_COUNT   (*(volatile u32 *)0x5000)
#define E820_ENTRIES ((const struct e820_entry *)0x5010)

static char vendor[13];
static char brand[49];
static u64  ram_kib;

static void cpuid(u32 leaf, u32 *a, u32 *b, u32 *c, u32 *d) {
    __asm__ volatile ("cpuid"
                      : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                      : "a"(leaf), "c"(0));
}

static const char *e820_type(u32 t) {
    switch (t) {
    case 1:  return "usable";
    case 2:  return "reserved";
    case 3:  return "acpi reclaimable";
    case 4:  return "acpi nvs";
    case 5:  return "bad memory";
    default: return "unknown";
    }
}

void sysinfo_init(void) {
    u32 a, b, c, d;

    cpuid(0, &a, &b, &c, &d);
    __builtin_memcpy(vendor + 0, &b, 4);        /* vendor string is ebx, edx, ecx */
    __builtin_memcpy(vendor + 4, &d, 4);
    __builtin_memcpy(vendor + 8, &c, 4);
    vendor[12] = 0;

    cpuid(0x80000000, &a, &b, &c, &d);
    if (a >= 0x80000004) {
        u32 r[4];
        for (int i = 0; i < 3; i++) {
            cpuid(0x80000002 + i, &r[0], &r[1], &r[2], &r[3]);
            __builtin_memcpy(brand + 16 * i, r, 16);
        }
        brand[48] = 0;
    } else {
        __builtin_memcpy(brand, "unknown", 8);
    }

    ram_kib = 0;
    for (u32 i = 0; i < E820_COUNT; i++)
        if (E820_ENTRIES[i].type == 1)
            ram_kib += E820_ENTRIES[i].len / 1024;
}

const char *sysinfo_cpu_vendor(void) { return vendor; }

const char *sysinfo_cpu_brand(void) {
    const char *p = brand;
    while (*p == ' ') p++;                      /* some cpus pad the front */
    return p;
}

u64 sysinfo_ram_kib(void) { return ram_kib; }

void sysinfo_log_memmap(void) {
    for (u32 i = 0; i < E820_COUNT; i++) {
        const struct e820_entry *e = &E820_ENTRIES[i];
        klog(LOG_INFO, "  mem %016lx - %016lx  %s",
             e->base, e->base + e->len - 1, e820_type(e->type));
    }
}