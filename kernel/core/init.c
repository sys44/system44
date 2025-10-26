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
#include "../lib/io.h"
#include "hwi.h"
void tirq0(void) {
    /* Reduced to 10 for faster boot */
    klog("running IRQ0 (timer) test.\n");
    uint32_t last_tick = ticks;
    while (ticks < 10) {
        if (ticks != last_tick) {
            last_tick = ticks;
            klog("*\n");
        }
    }
    klog("10 ticks elapsed\n");
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
    tty_putc('\n');
    klog(" - system44 v");
    puts(infoKernelVersion);
    puts(" (");
    puts(buildDate);
    puts(") -\n");
    pitsetfreq(1000);
    int_init();
    asm volatile("sti");
    klog("cpu: ");
    cpuident();
    mmp();
    pmm_init();
    kfs_mount();
    fbcstr(190,240,"STATUS UPDATE", 0xFFFFFFF, FONT_BASIC8X8);
    fbcstr(190,260,"MACHINE ID:         V1", 0xFFFFFF, FONT_BASIC8X8);
    fbcstr(190,270,"LOCATION:           APPROACHING HELL", 0xFFFFFF, FONT_BASIC8X8);
    fbcstr(190,280,"CURRENT OBJECTIVE:  FIND A WEAPON", 0xFFFFFF, FONT_BASIC8X8);
    fbcstr(190,300,"MANKIND IS DEAD", 0xFF0000, FONT_BASIC8X8);
    fbcstr(190,310,"BLOOD IS FUEL", 0xFF0000, FONT_BASIC8X8);
    fbcstr(190,320,"HELL IS FULL", 0xFF0000, FONT_BASIC8X8);
    sh();
}
