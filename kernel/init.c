#include "../drivers/vga.h"
#include "../drivers/tty.h"
#include "../drivers/keyboard.h"
#include "../mm/lmm.h"
#include "../mm/pmm.h"
#include "../mm/paging.h"
#include "shell.h"

void kmain(void) {
    eclear();
    eprintf("<init>\n - system44 (2.06 x86_32 7/10/2025) -\n");
    eprintf("\n");
    mmp();
    pmm_init();
    paging_init();
    tty_dev td;
    l_init(&td);
    sh(&td);

    for (;;) {
        __asm__ __volatile__("hlt");
    }
}
