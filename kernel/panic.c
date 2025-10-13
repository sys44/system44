#include "../drivers/tty.h"
void panic(const char *msg) {
    tty_puts("\n\npanic: ");
    tty_puts(msg);
    tty_puts("\n");
    asm volatile ("cli");
    for (;;) {
        asm volatile ("hlt");
    }
}
