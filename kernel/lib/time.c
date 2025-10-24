#include "../int/interrupts.h"

void usleep(uint32_t time) {
    uint32_t init = ticks;
    while (ticks - init < time) {
    }
}

void sleep(uint32_t time) {
    uint32_t init = ticks;
    while (ticks - init < time * 1000) {
    }
}