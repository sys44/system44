#include "shell.h"
#include "../drivers/tty.h"
#include "../drivers/keyboard.h"
#include "../lib/string.h"
#include "../lib/memory.h"
#include "../lib/io.h"
#include "../lib/error.h"
#include "../lib/exec.h"
#include "../fs/kfs.h"
#include "../exe/uex.h"
#include "version.h"
#include "log.h"
#include "../int/interrupts.h"
#include "../lib/math.h"
#include "panic.h"
#include "power.h"
#include "../sched/sched.h"
#define CMD_COMP(name) (strcmp(strtok(chars, " "), name) == 0)
#define MAX_CAT_SIZE 8192  // Max file size for cat (8KB for now)

extern struct kfs_superblock superblock;

void sh(void) {
    char chars[128];
    int i;
    static uint8_t cat_buffer[MAX_CAT_SIZE];  // Static buffer to avoid stack overflow

    for (;;) {
        puts("> ");
        memset(chars, 0, sizeof(chars));
        i = 0;
        while (1) {
            char c;
            if (!get_key(&c)) {
                yield();
                continue;
            }
            if (c == '\b') {
                if (i > 0) {
                    i--;
                    putc('\b');
                }
            } else {
                if ((unsigned int)i >= sizeof(chars) - 1) {
                    chars[i] = 0;
                    break;
                }
                putc(c);
                chars[i++] = c;
                if (c == '\n') {
                    chars[i - 1] = 0;
                    break;
                }
            }
            yield();
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
            } else if (f->size > MAX_CAT_SIZE) {
                printf("cat: file too large (max %d bytes)\n", MAX_CAT_SIZE);
            } else {
                uint32_t rounded_size = (f->size + 511) & ~511;
                if (kfs_read(f->name, cat_buffer) < 0) {
                    puts("kfs: ioerr\n");
                } else {
                    for (uint32_t j = 0; j < f->size; j++)
                        putc(cat_buffer[j]);
                    putc('\n');
                }
            }
        }
        else if (CMD_COMP("reboot")) {
            reboot();
        }
        else if (CMD_COMP("shutdown")) {
            shutdown();
        }
        else if (CMD_COMP("exec")) {
            exec(chars + 5) == ERR_FORMAT ? puts("exec: format error\n") : 0;
        }

        // Placeholder for now
        // Don't remove these comments until there is a proper implementation for elf.
        // AHEM *gingrspacecadet*
        // - Bryson

        /*else if (CMD_COMP("elf")) {
            extern int elfExec(const char* name, void** entry, struct uexAlloc* out_alloc);
            elfExec(chars + 4, NULL, NULL);
        } 
        */
        else if (CMD_COMP("help")) {
            puts("Available commands:\n"
                 " version      - show kernel version\n"
                 " ls           - list files in kfs\n"
                 " cat <file>   - display file contents\n"
                 " exec <file>  - execute UEX file\n"
                 " clear        - clear the screen\n"
                 " help         - show this help message\n"
                 " touch <file> - create an empty file\n"
                 " reboot       - reboot the system\n"
                 " shutdown     - power off the system\n");
        } else if (chars[0] != 0) {
            printf("%s: command not found\n", chars);
        }
    }
}
