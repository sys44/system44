#include "keyboard.h"
#include <stdint.h>
#define KBDBUF 256
static char kbd_buffer[KBDBUF];
static volatile int kbd_head = 0;
static volatile int kbd_tail = 0;

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %%dx, %%al" : "=a"(ret) : "d"(port));
    return ret;
}
static const char kbdus[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0,
};

static const char kbdus_shift[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0,
};
static int shift = 0;
void keyboardirqh(void) {
    uint8_t status = inb(0x64);
    if (!(status & 1)) return;
    uint8_t sc = inb(0x60);
    if (sc == 0x2A || sc == 0x36) { shift = 1; return; }
    if (sc == 0xAA || sc == 0xB6) { shift = 0; return; }
    if (sc & 0x80) return;
    char c = shift ? kbdus_shift[sc] : kbdus[sc];
    if (c) {
        int next = (kbd_head + 1) % KBDBUF;
        if (next != kbd_tail) {
            kbd_buffer[kbd_head] = c;
            kbd_head = next;
        }
    }
}

int get_key(char *c) {
    if (kbd_head == kbd_tail) return 0;
    *c = kbd_buffer[kbd_tail];
    kbd_tail = (kbd_tail + 1) % KBDBUF;
    return 1;
}
