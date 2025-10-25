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

#define CMD_COMP(name) (strcmp(strtok(chars, " "), name) == 0)
#define MAX_CAT_SIZE 8192  // Max file size for cat (8KB for now)

extern struct kfs_superblock superblock;

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

void reboot(void) {
    klog("reboot triggered");
    asm volatile (
        "cli\n"
        "mov $0xFE, %al\n"
        "out %al, $0x64\n"
    );
}

// NOTE: This probably won't work for the current qemu run command for testing.
// If not, try: qemu-system-i386 -fda floppy.img -hda rootfs/kfs.img -device isa-debug-exit,iobase=0x501,iosize=0x1
void shutdown(void) {
    klog("shutdown triggered\n");
    
    // QEMU ISA debug exit 
    outb(0x501, 0x00);  // Exit with code 0
    
    // Try ACPI shutdown 
    outb(0xB004, 0x2000);  // Bochs/QEMU 
    outb(0x604, 0x2000);   // QEMU (but newer)
    outb(0x4004, 0x3400);  // VirtualBox
    
    // Try writing to ACPI PM1a control register
    outb(0x600, 0x34);
    outb(0xb004, 0x2000);
    
    // If ACPI didn't work, try APM
    asm volatile(
        "mov $0x5301, %%ax\n"
        "xor %%bx, %%bx\n"
        "int $0x15\n"
        "jc apm_fail\n"
        
        "mov $0x5308, %%ax\n"
        "mov $1, %%bx\n"
        "mov $1, %%cx\n"
        "int $0x15\n"
        
        "mov $0x5307, %%ax\n"
        "mov $1, %%bx\n"
        "mov $3, %%cx\n"
        "int $0x15\n"
        
        "apm_fail:\n"
        ::: "ax", "bx", "cx"
    );
    
    klog("shutdown failed - halting\n");
    asm volatile("cli");
    for (;;) {
        asm volatile("hlt");
    }
}

void sh(void) {
    klog("sh: scheduler and elfs are not properly implemented yet. dropping into temporary shell.\n");

    char chars[128];
    int i;
    static uint8_t cat_buffer[MAX_CAT_SIZE];  // Static buffer to avoid stack overflow

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
