#include "../drivers/vga.h"
#include "../drivers/tty.h"
#include "../drivers/keyboard.h"
#include "shell.h"

void kmain(void) {
    eclear();
    eprintf("system44 kernel (Rev. 2.03)\n");
    eprintf("Early VGA text mode demonstration.\n\n");
    tty_dev td;
    l_init(&td);
    sh(&td);

    for (;;) {
        __asm__ __volatile__("hlt");
    }
}
