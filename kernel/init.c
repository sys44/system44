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
#include "log.h"

void kmain(unsigned char *vbe){
    tty_init(vbe);
    tty_puts("\n - system44 v");
    tty_puts(infoKernelVersion);
    tty_putc(' ');
    tty_puts(buildDate);
    tty_puts(") -\n");
    pitsetfreq(1000);
    int_init();
    asm volatile("sti");
    mmp();
    pmm_init();
    kfs_mount();
    sh();
}
