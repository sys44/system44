#include <stddef.h>
#include "../drivers/tty.h"
#include "../drivers/fbcon.h"
#include "shell.h"
#include "../fs/kfs.h"
void kmain(unsigned char *vbe){
    kfs_mount();
    tty_init(vbe);
    tty_puts("WAR IS PEACE\n");
    tty_puts("FREEDOM IS SLAVERY\n");
    tty_puts("IGNORANCE IS STRENGTH\n");
    tty_puts("\nBIG BROTHER IS WATCHING YOU\n");
    sh();
}
