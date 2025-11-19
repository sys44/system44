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
#include "panic.h"
#include <stdint.h>
#include <stdint.h>
#include "../lib/memory.h"
#include <stdint.h>
#include "../sched/sched.h"



/*
void task1() {
  for(;;) {
    tty_puts("Task1");
    usleep(60);
    yield();
  }
}

void task2() {
  for(;;) {
    tty_puts("    Task2\n");
    usleep(60);
    yield();
  }
}
*/


static void task1(void) {
    for(int n = 0; ; n = ++n % 10) {
        fbcstr(SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, "\b", 0x000000, FONT_BASIC8X8);
        fbcstr(SCREEN_WIDTH / 2 - 16 * CHAR_W, SCREEN_WIDTH / 2, "TASK 1 Counter:", 0xFFFFFF, FONT_BASIC8X8);
        char str[2] = {'0' + n, 0};
        fbcstr(SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, str, 0xFFFFFF, FONT_BASIC8X8);
        usleep(100);
        yield();
    }
}

static void task2(void) {
    for(int n = 0; ; n = ++n % 10) {
        fbcstr(SCREEN_WIDTH / 2, SCREEN_WIDTH / 2, "\b", 0x000000, FONT_BASIC8X8);
        fbcstr(SCREEN_WIDTH / 2 - 16 * CHAR_W, SCREEN_WIDTH / 2, "TASK 2 Counter:", 0xFFFFFF, FONT_BASIC8X8);
        char str[2] = {'0' + n, 0};
        fbcstr(SCREEN_WIDTH / 2,  SCREEN_WIDTH / 2, str, 0xFFFFFF, FONT_BASIC8X8);
        usleep(100);
        yield();
    }
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
    kfs_mount();
    sched_init();
    task_l_spawn(task1);
    //create_task(task2); slows the kernel dwwn beacuse of coopearative multitasking.
    task_l_spawn(sh);
    m_scheduler();
    panic("scheduler has given up");
}


