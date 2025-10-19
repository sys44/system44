#include "shell.h"
#include "../drivers/tty.h"
#include "../drivers/keyboard.h"
#include "../lib/string.h"
#include "../lib/memory.h"
#include "../lib/io.h"
#include "../lib/exec.h"
#include "../fs/kfs.h"
#include "../uex/uex.h"
#include "version.h"
#include "log.h"

#define CMD_COMP(name) (strcmp(strtok(chars, " "), name) == 0)

extern struct kfs_superblock superblock;

void sh(void) {
    klog("sh: scheduler and elfs are not properly implemented yet. dropping into temporary shell.\n");

    char chars[128];
    int i;

    for (;;) {
        puts("// ");
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
                    puts("\b \b");
                }
            } else {
                putc(c);
                chars[i++] = c;
                if (c == '\n' || (unsigned int)i >= sizeof(chars) - 1) {
                    chars[i - 1] = 0;
                    break;
                }
            }
        }

        if (CMD_COMP("version")) {
            puts(infoKernelVersion);
            putc('\n');
        }
        else if (CMD_COMP("ls")) {
            for (unsigned int j = 0; j < superblock.file_count; j++) {
                struct kfs_file* f = &superblock.files[j];
                puts(f->name);
                putc('\n');
            }
        }
        else if (CMD_COMP("clear")) {
            clear();
        }
        else if (CMD_COMP("touch")) {
            kfs_write(chars + 6, "", 0);
        }
        else if (CMD_COMP("cat")) {
            struct kfs_file* f = kfs_find(chars + 4);
            if (!f) {
                printf("cat: %s: not found\n", chars + 4);
            } else {
                uint8_t data[(f->size + 511) & ~511];
                if (kfs_read(f->name, data) < 0) {
                    puts("kfs: ioerr\n");
                } else {
                    for (uint32_t j = 0; j < f->size; j++)
                        putc(data[j]);
                    putc('\n');
                }
            }
        }
        else if (CMD_COMP("exec")) {
            exec(chars + 5);
        }
        else if (CMD_COMP("help")) {
            puts("Available commands:\n"
                 " version      - show kernel version\n"
                 " ls           - list files in kfs\n"
                 " cat <file>   - display file contents\n"
                 " exec <file>  - execute UEX file\n"
                 " clear        - clear the screen\n"
                 " help         - show this help message\n"
                 " touch <file> - create an empty file\n");
        } else if (chars[0] != 0) {
            printf("%s: command not found\n", chars);
        }
    }
}
