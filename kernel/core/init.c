// init.c
/* Comments added by VeryEpicKebap
This is the first part of the kernel ran when the kernel actually starts executing
*/
#include <stddef.h>
#include "../drivers/tty.h"
#include "../drivers/fbcon.h"
#include "../drivers/fb.h"
#include "../drivers/pci.h"
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
#include "../lib/time.h"
#include "../fs/vfs.h"


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

    struct pci_Out pcis[1024];
    pciEnumerate(pcis);

    for (size_t i = 0; i < 1024; i++) {
        if (pcis[i].pci.vendor != NULL) {
            klog("PCI ");
            printf("device %d "
                "  bus: %d dev: %d func: %d "
                "  vendor: %x device: %x\n",
                i, pcis[i].bus, pcis[i].dev, pcis[i].func, pcis[i].pci.vendor, pcis[i].pci.device);
        }
    }
    file_t *f = vfs_l_open("/dev/tty");
    vfs_l_write(f,"\n\nVFS Write test (to /dev/tty\nActually, /dev/tty doesn't exist. It's just a value open() checks for.\n\n", 128);
    kfs_mount();
    sh();

}
