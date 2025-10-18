#include "shell.h"
#include "../drivers/tty.h"
#include "../drivers/keyboard.h"
#include "../lib/string.h"
#include "../lib/memory.h"
#include "../fs/kfs.h"
#include "../uex/uex.h"
#include "version.h"
extern struct kfs_superblock superblock;

void sh(void) {
    char buf[128];
    int i;
    for (;;) {
        tty_puts("// ");
        memset(buf, 0, sizeof(buf));
        i = 0;
        for (;;) {
            char c = get_key();
            if (c == '\b') {
                if (i > 0) {
                    i--;
                    tty_puts("\b");
                }
            } else {
                tty_putc(c);
                buf[i++] = c;
                if (c == '\n' || (unsigned int)i >= sizeof(buf) - 1) {
                    buf[i - 1] = 0;
                    break;
                }
            }
        }

        if (strcmp(buf, "version") == 0) {
            tty_puts(infoKernelVersion);
            tty_putc('\n');
        }
        else if (strcmp(buf, "ls") == 0) {
            for (unsigned int i = 0; i < superblock.file_count; i++) {
                struct kfs_file* f = &superblock.files[i];
                tty_puts(f->name);
                tty_putc('\n');
            }
        }
        else if (strcmp(buf, "clear") == 0) {
            tty_clear();
        }
        else if (buf[0] == 'c' && buf[1] == 'a' && buf[2] == 't' && buf[3] == ' ' && buf[4] != 0) {
            struct kfs_file* f = kfs_find(buf + 4);
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
        else if (buf[0] == 'e' && buf[1] == 'x' && buf[2] == 'e' && buf[3] == 'c' && buf[4] == ' ' && buf[5] != 0) {
            void* entry;
            if (uexExec(buf + 5, &entry) == 0) {
                void (*prog)() = (void (*)())entry;
                prog();
            } else {
                tty_puts("UEX format error\n");
            }
        }
        else if (strcmp(buf, "checkaddr") == 0) {
            char *data = (char*)0x300000;
            tty_puts(data);
        }
        else if (buf[0] != 0) {
            tty_puts("?\n");
        }
    }
}
