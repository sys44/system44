#include "vga.h"

static char *video = (char*)0xb8000;
static int cursor_x = 0;
static int cursor_y = 0;

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
    ecursorsync();
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