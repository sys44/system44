#include "lmm.h"
#include "../drivers/tty.h"
#include "../core/log.h"
void mmp() {
    uint16_t count = *(uint16_t*)MMAPCOUNT;
    struct e820_entry* map = (struct e820_entry*)MMAPADDR;
    const char* types[] = { "Unknown", "RAM", "Reserved", "ACPI Reclaim", "ACPI NVS", "Bad RAM" };
    klog("E820 Memory Map:\n");
    for (int i = 0; i < count; i++) {
        char num[2] = { '0' + (i % 10), 0 };
        klog(num);
        tty_puts(": base=0x");
        uint32_t start = map[i].base_low;
        uint32_t len   = map[i].length_low;
        uint8_t type   = map[i].type;
        for (int s = 28; s >= 0; s -= 4)
            tty_putc("0123456789ABCDEF"[(start >> s) & 0xF]);
        tty_puts(" len=0x");
        for (int s = 28; s >= 0; s -= 4)
            tty_putc("0123456789ABCDEF"[(len >> s) & 0xF]);
        tty_puts(" type=");
        tty_puts(type < 6 ? types[type] : "Unknown");
        tty_putc('\n');
    }
}
