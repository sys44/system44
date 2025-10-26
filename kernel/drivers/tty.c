#include "tty.h"
#include "fbcon.h"
#include "fb.h"
static uint16_t cx = 0, cy = 0;
void tty_init(u8 *vbe) {
    fb_init(vbe);
    fb_fill(BGC);
    cx = cy = 0;
}

void tty_putc(char c) {
    int colour = 0xFFFFFFFF;
    fbcputchar(cx, cy, '\b', BGC, FONT_BASIC8X8);
    fbcputchar(cx+CHAR_W, cy, '_', colour, FONT_BASIC8X8);
    switch (c) {
    case '\n':
        fbcputchar(cx+CHAR_W, cy, '\b', BGC, FONT_BASIC8X8);
        cx = 0;
        cy += CHAR_H;
        if (cy >= SCREEN_HEIGHT) cy = 0;
        return;
    case '\b':
        fbcputchar(cx+CHAR_W, cy, '\b', BGC, FONT_BASIC8X8);
        if (cx >= CHAR_W) {
            cx -= CHAR_W;
        } else if (cy >= CHAR_H) {
            cy -= CHAR_H;
            cx = SCREEN_WIDTH - CHAR_W;
        }
        fbcputchar(cx, cy, ' ', BGC, FONT_BASIC8X8);
        fbcputchar(cx, cy, '_', colour, FONT_BASIC8X8);
        return;
    case '\t':
        fbcputchar(cx+CHAR_W, cy, ' ', BGC, FONT_BASIC8X8);
        cx += (4 - (cx % 5)) * CHAR_W;
        if (cx >= SCREEN_WIDTH) {
            cx = 0;
            cy += CHAR_H;
            if (cy >= SCREEN_HEIGHT) cy = 0;
        }
        fbcputchar(cx, cy, '_', colour, FONT_BASIC8X8);
        return;
    case '\r':
        cx = 0;
        return;
    case ' ':
        colour = BGC;
        __attribute__((fallthrough));
    default:
        fbcputchar(cx, cy, c, colour, FONT_BASIC8X8);
        cx += CHAR_W;

        if (cx >= SCREEN_WIDTH) {
            cx = 0;
            cy += CHAR_H;
            if (cy >= SCREEN_HEIGHT) cy = 0;
        }
    }
}
void tty_puts(const char *s) {
    while (*s) tty_putc(*s++);
}
void tty_clear(void) {
    fb_fill(BGC);
    cx = cy = 0;
}
