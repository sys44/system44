#include "../drivers/vga.h"

void kmain(void) {
    eclear();
    eprintf("system44 kernel (Rev. 2.03)\n");
    eprintf("Early VGA text mode demonstration.\n");
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}