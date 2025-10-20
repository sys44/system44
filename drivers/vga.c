#include "vga.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
static volatile unsigned char* video = (unsigned char*)0xb8000;
static int cursor_x = 0;
static int cursor_y = 0;

void ecursorsync() {
    unsigned short pos = cursor_y * VGA_WIDTH + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

void eclear() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2) {
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
        int pos = (cursor_y * VGA_WIDTH + cursor_x) * 2;
        video[pos] = c;
        video[pos + 1] = 0x07;
        cursor_x++;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }
    if (cursor_y >= VGA_HEIGHT) {
        eclear();
    }
    ecursorsync();
}

void eprintf(const char *s) {
    for (int i = 0; s[i]; i++) {
        eputchar(s[i]);
    }
}