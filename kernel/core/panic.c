/* Do CPUs dream of eternal halting? */

#include "../drivers/tty.h"
#include <stdint.h>
#include "log.h"
#include "hwi.h"
#include "version.h"
typedef struct {
    uint32_t eax, ebx, ecx, edx;
    uint32_t esi, edi, ebp, esp;
    uint32_t eip, cs, eflags;
} cpu_regs_t;

static void puthex(uint32_t val) {
    const char hex[] = "0123456789ABCDEF";
    char buf[9];
    for (int i = 0; i < 8; i++) {
        buf[7-i] = hex[val & 0xF];
        val >>= 4;
    }
    buf[8] = 0;
    tty_puts(buf);
}

void panic(const char *msg, cpu_regs_t *r) {
    tty_puts("\n ----- oops! -----\n\n Kernel: ");
    tty_puts(infoKernelVersion);
    tty_puts("\n CPU: ");
    cpuident();
    tty_puts("\n EAX="); puthex(r->eax);tty_putc(' ');
    tty_puts("EBX="); puthex(r->ebx);tty_putc(' ');
    tty_puts("ECX="); puthex(r->ecx);tty_putc(' ');
    tty_putc('\n');
    tty_puts(" EDX="); puthex(r->edx);tty_putc(' ');
    tty_puts("ESI="); puthex(r->esi);tty_putc(' ');
    tty_puts("EDI="); puthex(r->edi);tty_putc(' ');
    tty_putc('\n');
    tty_puts(" EBP="); puthex(r->ebp);tty_putc(' ');
    tty_puts("ESP="); puthex(r->esp);tty_putc(' ');
    tty_puts("EIP="); puthex(r->eip);tty_putc(' ');
    tty_putc('\n');
    tty_puts(" CS=");  puthex(r->cs);tty_putc(' ');
    tty_puts("EFLAGS="); puthex(r->eflags);
    tty_puts("\n\n panic: ");
    tty_puts(msg);
    asm volatile("cli");
    for (;;) asm volatile("hlt");
}
