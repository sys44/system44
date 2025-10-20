#pragma once
#include <stdint.h>

typedef enum {
    FONT_BASIC8X8,
} font_t;

void fbcputchar(uint16_t px, uint16_t py, char c, uint32_t color, font_t font);
void fbcstr(uint16_t x, uint16_t y, const char *s, uint32_t color, font_t font);