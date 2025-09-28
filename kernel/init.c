#include "../drivers/vga.h"

void kmain(void) {
    eclear();
    eprintf("System44 Kernel (Rev. 2.03)\n");
    eprintf("Early VGA text mode demonstration.\n");
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}