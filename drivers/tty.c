#include "tty.h"
#include "fbcon.h"
#include "fb.h"
#define CHAR_W 6
#define CHAR_H 8
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define BGC 0x000000090
static uint16_t cx = 0, cy = 0;
void tty_init(u8 *vbe) {
    fb_init(vbe);
    fb_fill(BGC);
    cx = cy = 0;
}
void tty_putc(char c) {
    if (c == '\n') {
        cx = 0;
        cy += CHAR_H;
        if (cy >= SCREEN_HEIGHT) cy = 0;
        return;
    }

    if (c == '\b') {
        if (cx >= CHAR_W) {
            cx -= CHAR_W;
        } else if (cy >= CHAR_H) {
            cy -= CHAR_H;
            cx = SCREEN_WIDTH - CHAR_W;
        }
        fbcputchar(cx, cy, ' ', 0xFFFFFFFF);
        return;
    }

    fbcputchar(cx, cy, c, 0xFFFFFFFF);
    cx += CHAR_W;

    if (cx >= SCREEN_WIDTH) {
        cx = 0;
        cy += CHAR_H;
        if (cy >= SCREEN_HEIGHT) cy = 0;
    }
}
void tty_puts(const char *s) {
    while (*s) tty_putc(*s++);
}
void tty_clear(void) {
    fb_fill(BGC);
    cx = cy = 0;
}
