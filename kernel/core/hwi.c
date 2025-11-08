#include <stdint.h>
#include "hwi.h"
#include "../drivers/tty.h"

void cpuident() {
    char brand[49];
    uint32_t *b = (uint32_t*)brand;
    for (uint32_t i = 0; i < 3; i++) {
        uint32_t eax, ebx, ecx, edx;
        asm volatile(
            "cpuid"
            : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "a"(0x80000002 + i)
        );
        b[i*4 + 0] = eax;
        b[i*4 + 1] = ebx;
        b[i*4 + 2] = ecx;
        b[i*4 + 3] = edx;
    }
    brand[48] = '\0';
    tty_puts(brand);
    tty_putc('\n');
}
