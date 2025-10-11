#include "shell.h"
#include "../drivers/tty.h"
#include "../drivers/keyboard.h"
#include "../lib/string.h"
#include "../lib/memory.h"
#include "../fs/kfs.h"

extern struct kfs_superblock superblock;

void sh(void) {
    char buf[128];
    int i;
    for (;;) {
        tty_puts("Q ");
        memset(buf, 0, sizeof(buf));
        i = 0;
        for (;;) {
            char c = get_key();
            if (c == '\b') {
                if (i > 0) {
                    i--;
                    tty_puts("");
                }
            } else {
                tty_putc(c);
                buf[i++] = c;
                if (c == '\n' || i >= sizeof(buf) - 1) {
                    buf[i - 1] = 0;
                    tty_putc('\n');
                    break;
                }
            }
        }
        if (strcmp(buf, "version") == 0) {
            tty_puts("2.07 font is incomplete  cant print this\n");
        }
        else if (strcmp(buf, "ls") == 0) {
            for (i = 0; i < superblock.file_count; i++) {
                struct kfs_file* f = &superblock.files[i];
                tty_puts(f->name);
                tty_putc('\n');
            }
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
        else if (buf[0] != 0) {
            tty_puts("?\n");
        }
    }
}
