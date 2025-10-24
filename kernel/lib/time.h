#pragma once

#include <stdint.h>

extern volatile uint32_t ticks;

void usleep(uint32_t time);
void sleep(uint32_t time);