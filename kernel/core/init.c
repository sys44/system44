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

void cpuident() {
    char brand[49];
    uint32_t *b = (uint32_t*)brand;
    for (uint32_t i = 0; i < 3; i++) {
        uint32_t eax, ebx, ecx, edx;
        asm volatile(
            "cpuid"
            : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "a"(0x80000002 + i)
        );
        b[i*4 + 0] = eax;
        b[i*4 + 1] = ebx;
        b[i*4 + 2] = ecx;
        b[i*4 + 3] = edx;
    }
    brand[48] = '\0';
    klog("cpu: ");
    tty_puts(brand);
    tty_putc('\n');
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
    klog(" - system44 v");
    puts(infoKernelVersion);
    puts(" (");
    puts(buildDate);
    puts(") -\n");
    pitsetfreq(1000);
    int_init();
    asm volatile("sti");
    cpuident();
    mmp();
    pmm_init();
    kfs_mount();
    sh();
}
