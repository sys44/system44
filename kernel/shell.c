#include "shell.h"
#include "../drivers/tty.h"
#include "../drivers/keyboard.h"
#include "../lib/string.h"
#include "../lib/memory.h"
#include "../fs/kfs.h"
#include "../uex/uex.h"
#include "../mm/pmm.h"
#include "version.h"
#include "log.h"
extern struct kfs_superblock superblock;

void sh(void) {
    klog("sh: scheduler and elfs are not properly implemented yet. dropping into temporary shell.\n");

    char chars[128];
    int i;

    for (;;) {
        tty_puts("// ");
        memset(chars, 0, sizeof(chars));
        i = 0;
        while (1) {
            char c;
            if (!get_key(&c)) {
                asm volatile("hlt");
                continue;
            }
            if (c == '\b') {
                if (i > 0) {
                    i--;
                    tty_puts("\b \b");
                }
            } else {
                tty_putc(c);
                chars[i++] = c;
                if (c == '\n' || (unsigned int)i >= sizeof(chars) - 1) {
                    chars[i - 1] = 0;
                    break;
                }
            }
        }

        if (strcmp(strtok(chars, " "), "version") == 0) {
            tty_puts(infoKernelVersion);
            tty_putc('\n');
        }
        else if (strcmp(strtok(chars, " "), "ls") == 0) {
            for (unsigned int j = 0; j < superblock.file_count; j++) {
                struct kfs_file* f = &superblock.files[j];
                tty_puts(f->name);
                tty_putc('\n');
            }
        }
        else if (strcmp(strtok(chars, " "), "clear") == 0) {
            tty_clear();
        }
        else if (strcmp(strtok(chars, " "), "cat") == 0) {
            struct kfs_file* f = kfs_find(chars + 4);
            if (!f) {
                tty_puts("kfs: not found\n");
            } else {
                uint8_t data[(f->size + 511) & ~511];
                if (kfs_read(f->name, data) < 0) {
                    tty_puts("kfs: ioerr\n");
                } else {
                    for (uint32_t j = 0; j < f->size; j++)
                        tty_putc(data[j]);
                    tty_putc('\n');
                }
            }
        }
        else if (strcmp(strtok(chars, " "), "exec") == 0) {
            void* entry;
            struct uexAlloc alloc;
            if (uexExec(chars + 5, &entry, &alloc) == 0) {
                void (*prog)() = (void (*)())entry;
                prog();
                if (alloc.base) {
                    pmm_free_pages(alloc.base, alloc.pages);
                }
            } else {
                tty_puts("UEX format error\n");
            }
        }
        else if (strcmp(strtok(chars, " "), "help") == 0) {
            tty_puts("Available commands:\n");
            tty_puts(" version - show kernel version\n");
            tty_puts(" ls      - list files in kfs\n");
            tty_puts(" cat <file>  - display file contents\n");
            tty_puts(" exec <file> - execute UEX file\n");
            tty_puts(" clear   - clear the screen\n");
            tty_puts(" help    - show this help message\n");
        }
        else if (chars[0] != 0) {
            tty_puts(chars);
            tty_puts(": command not found\n");
        }
    }
}
