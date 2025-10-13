#include <stddef.h>
#include "../drivers/tty.h"
#include "../drivers/fbcon.h"
#include "../mm/lmm.h"
#include "../mm/pmm.h"
#include "../mm/paging.h"
#include "shell.h"
#include "../fs/kfs.h"
#include "version.h"
void kmain(unsigned char *vbe){
    tty_init(vbe);
    tty_puts("[ 0.0000 ] <init>\n");
    tty_puts("[ 0.0000 ] reported LOADER version is ");
    tty_puts(infoLoaderVersion);
    tty_puts("\n[ 0.0000 ] - system44 (");
    tty_puts(infoKernelVersion);
    tty_puts(" x86 12/10/2025)\n");
    mmp();
    pmm_init();
    kfs_mount();
    tty_puts("[ 0.0000 ] kfs: mounted the first ATA device found\n-- end of init, dropping into temporary shell --\n");
    sh();
}
