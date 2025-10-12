#include "lmm.h"
#include "../drivers/tty.h"

void mmp() {
    uint16_t count = *(uint16_t*)MMAPCOUNT;
    struct e820_entry* map = (struct e820_entry*)MMAPADDR;
    tty_puts("kmm: initalizing memory management\n");
    for (int i = 0; i < count; i++) {
        if (map[i].type == 1) {
            uint32_t start = map[i].base_low;
            uint32_t length = map[i].length_low;
            tty_puts("usable: ");
            tty_puts(": base=0x");
            for (int s = 28; s >= 0; s -= 4)
                tty_putc("0123456789ABCDEF"[(start >> s) & 0xF]);
            tty_puts(" len=0x");
            for (int s = 28; s >= 0; s -= 4)
                tty_putc("0123456789ABCDEF"[(length >> s) & 0xF]);
            tty_putc('\n');
        }
    }
}
