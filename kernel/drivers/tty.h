#pragma once
#include <stdint.h>

#define CHAR_W 6
#define CHAR_H 8
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define BGC 0x0000000

void tty_init(uint8_t *vbe);
void tty_putc(char c);
void tty_puts(const char *s);
void tty_clear();
