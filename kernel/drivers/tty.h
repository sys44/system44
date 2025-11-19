#pragma once
#include <stdint.h>

#define CHAR_W 9
#define CHAR_H 16
#define SCREEN_WIDTH (CHAR_W * 80)
#define SCREEN_HEIGHT (CHAR_H * 25)
#define BGC 0x0000000

void tty_init(uint8_t *vbe);
void tty_putc(char c);
void tty_puts(const char *s);
void tty_clear();
