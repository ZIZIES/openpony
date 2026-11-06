#include "console.h"
#include "keyboard.h"
#include "log.h"
#include "shell.h"
#include "sysinfo.h"

void kmain(void) {
    serial_init();

    /* continue the log right where canterlot's bios text stopped:
       the bios keeps the cursor (col, row) in its data area at 0x450 */
    volatile u8 *bda = (volatile u8 *)0x400;
    kset_cursor(bda[0x51], bda[0x50]);

    klog(LOG_OK,   "kernel: entered long mode (paging on, 1 GiB identity map)");
    klog(LOG_OK,   "console: vga text 80x50 + serial com1");

    sysinfo_init();
    klog(LOG_INFO, "cpu: %s (%s)", sysinfo_cpu_brand(), sysinfo_cpu_vendor());
    klog(LOG_INFO, "memory: %lu MiB usable", sysinfo_ram_kib() / 1024);
    sysinfo_log_memmap();

    klog(LOG_OK,   "keyboard: ps/2, polled");
    klog(LOG_OK,   "shell: pid 0 on tty0");
    klog(LOG_INFO, "welcome to openpony!");
    kputc('\n');

    shell_run();
}