#include "panic.h"
#include "log.h"
#include <stdint.h>

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

void reboot(void) {
    klog("reboot triggered");
    asm volatile (
        "cli\n"
        "mov $0xFE, %al\n"
        "out %al, $0x64\n"
    );
}

void shutdown(void) {
    klog("shutdown triggered\n");
    outb(0x501, 0x00);
    outw(0xB004, 0x2000);
    outw(0x604,  0x2000);
    outw(0x4004, 0x3400);
    outw(0x600,  0x34);
    outw(0xB004, 0x2000);
    asm volatile(
        "mov $0x5301, %%ax\n"
        "xor %%bx, %%bx\n"
        "int $0x15\n"
        "jc apm_fail\n"

        "mov $0x5308, %%ax\n"
        "mov $1, %%bx\n"
        "mov $1, %%cx\n"
        "int $0x15\n"

        "mov $0x5307, %%ax\n"
        "mov $1, %%bx\n"
        "mov $3, %%cx\n"
        "int $0x15\n"

        "apm_fail:\n"
        ::: "ax", "bx", "cx"
    );
    panic("shutdown failed\n");
}
