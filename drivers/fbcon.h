#pragma once
#include <stdint.h>
void fbcputchar(uint16_t x, uint16_t y, char c, uint32_t color);
void fbcstr(uint16_t x, uint16_t y, const char *s, uint32_t color);
