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

void tirq0(void) {
    /* Reduced to 10 for faster boot */
    tty_puts("\n[ 0.0000 ] running IRQ0 (timer) test.\n");
    uint32_t last_tick = ticks;
    while (ticks < 10) {
        if (ticks != last_tick) {
            last_tick = ticks;
            tty_puts("*");
        }
    }
    tty_puts("\n[ 0.0000 ] 10 ticks elapsed\n");
}

static inline uint32_t syscall3(uint32_t num, uint32_t arg1, uint32_t arg2, uint32_t arg3) {
    uint32_t ret;
    asm volatile(
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "b"(arg1), "c"(arg2), "d"(arg3)
        : "memory"
    );
    return ret;
}

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
    tirq0();
    tty_puts("[ 0.0000 ] testing syscalls\n");
    syscall0(0);  // test syscall with no args
    uint32_t ret = syscall3(1, 0x1234, 0, 0);  // test with args
    kfs_mount();
    sh();
}
