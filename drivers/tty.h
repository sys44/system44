#pragma once
#include <stdint.h>
void tty_init(uint8_t *vbe);
void tty_putc(char c);
void tty_puts(const char *s);
void tty_clear();
