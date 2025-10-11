#include <stddef.h>
#include "../drivers/tty.h"
#include "../drivers/fbcon.h"
#include "shell.h"
#include "../fs/kfs.h"
void kmain(unsigned char *vbe){
    kfs_mount();
    tty_init(vbe);
    tty_puts("War is peace\n");
    tty_puts("Freedom is slavery\n");
    tty_puts("Ignorance is strength\n");
    tty_puts("\nBig Brother is watching you\n");
    sh();
}
