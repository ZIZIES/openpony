#include "io.h"
#include "basic.h"

/* acpi-less poweroff: works on emulators. real hardware needs the acpi tables parsed. */
void poweroff(void) {
    outw(0x604,  0x2000);       /* qemu (piix4 / q35): PM1a_CNT with SLP_EN + S5 */
    outw(0xB004, 0x2000);       /* older qemu / bochs */
    outw(0x4004, 0x3400);       /* virtualbox */
}