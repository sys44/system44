// init.c
/* Comments added by VeryEpicKebap
This is the first part of the kernel ran when the kernel actually starts executing
*/
#include <stddef.h>
#include "../drivers/tty.h"
#include "../drivers/fbcon.h"
#include "../mm/lmm.h"
#include "../mm/pmm.h"
#include "../mm/paging.h"
#include "shell.h"
#include "../fs/kfs.h"
#include "version.h"
#include "../int/interrupts.h"

void tirq0(void) {
    tty_puts("\n[ 0.0000 ] running IRQ0 (timer) test.\n");
    uint32_t last_tick = ticks;
    while (ticks < 100) {
        if (ticks != last_tick) {
            last_tick = ticks;
            tty_puts("*");
        }
    }
    tty_puts("\n[ 0.0000 ] 100 ticks elapsed\n");
}



void kmain(unsigned char *vbe){
    // the timestamps are there only for the "dmesg feel". they do not serve a purpose (for now atleast)  -veryepickebap
    tty_init(vbe);
    tty_puts("[ 0.0000 ] <init>\n");
    tty_puts("[ 0.0000 ] reported LOADER version is ");
    tty_puts(infoLoaderVersion);
    tty_puts("\n[ 0.0000 ] - system44 (");
    tty_puts(infoKernelVersion);
    tty_puts(" x86 12/10/2025)\n");
    mmp();
    pmm_init();
    int_init();
    asm volatile("sti");
    tirq0();
    tty_puts("interrupts enabled");
    kfs_mount();
    tty_puts("\n[ 0.0000 ] kfs: mounted the first ATA device found\n-- end of init, dropping into temporary shell --\n");
    sh();
}
