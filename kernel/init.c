static int cursor_x = 0;
static int cursor_y = 0;
static char *video = (char*)0xb8000;






static inline void outb(unsigned short port, unsigned char val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ __volatile__("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void beepboop() {
    unsigned int div = 1193180 / 1000;
    outb(0x43, 0xb6);
    outb(0x42, div & 0xFF);
    outb(0x42, div >> 8);
    unsigned char tmp = inb(0x61);
    outb(0x61, tmp | 3);
    outb(0x43, 0x36);
    outb(0x40, 0);
    outb(0x40, 0);
    for (volatile int i = 0; i < 10000000; i++);
    outb(0x61, tmp & 0xFC);
}


void ecursorsync() {
    unsigned short pos = cursor_y * 80 + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}


void eclear() {
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video[i] = ' ';
        video[i + 1] = 0x07;
    }
    cursor_x = 0;
    cursor_y = 0;
}

void eputchar(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else {
        int pos = (cursor_y * 80 + cursor_x) * 2;
        video[pos] = c;
        video[pos + 1] = 0x07;
        cursor_x++;
        if (cursor_x >= 80) {
            cursor_x = 0;
            cursor_y++;
        }
    }
    if (cursor_y >= 25) {
        eclear();
    }
    ecursorsync();
}

void eprintf(const char *s) {
    for (int i = 0; s[i]; i++) {
        eputchar(s[i]);
    }
}

void kmain(void) {
    eclear();
    eprintf("System44 Kernel (Rev. 2.02)\n");
    eprintf("Early VGA text mode demonstration.\n");
    beepboop();
    for (;;) {
        __asm__ __volatile__("hlt");
    }
}