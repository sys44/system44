#include "lmm.h"
#include "../drivers/vga.h"

void mmp() {
    uint16_t count = *(uint16_t*)MMAPCOUNT;
    struct e820_entry* map = (struct e820_entry*)MMAPADDR;
    eprintf("kmm: initalizing memory management\n");
    eprintf("regions found: ");
    eputchar('0' + count);
    eputchar('\n');
    for (int i = 0; i < count; i++) {
        if (map[i].type == 1) {
            uint32_t start = map[i].base_low;
            uint32_t length = map[i].length_low;
            eprintf("usable: ");
            eputchar('0' + (i % 10));
            eprintf(": base=0x");
            for (int s = 28; s >= 0; s -= 4)
                eputchar("0123456789ABCDEF"[(start >> s) & 0xF]);
            eprintf(" len=0x");
            for (int s = 28; s >= 0; s -= 4)
                eputchar("0123456789ABCDEF"[(length >> s) & 0xF]);
            eputchar('\n');
        }
    }
}
