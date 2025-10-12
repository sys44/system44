#include <stddef.h>
#include "../drivers/tty.h"
#include "../drivers/fbcon.h"
#include "../mm/lmm.h"
#include "../mm/pmm.h"
#include "../mm/paging.h"
#include "shell.h"
#include "../fs/kfs.h"
void kmain(unsigned char *vbe){
    tty_init(vbe);
    tty_puts("<init>\n");
    tty_puts(" - system44 (2.06 x86 12/10/2025)\n");
    mmp();
    pmm_init();
    tty_puts("kfs: mount\n");
    kfs_mount();
    sh();
}
