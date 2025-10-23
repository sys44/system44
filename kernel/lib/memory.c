#include "memory.h"

void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    for (size_t i = 0; i < n; i++) d[i] = s[i];
    return dest;
}

void *memset(void *dest, int val, size_t n) {
    unsigned char *d = dest;
    for (size_t i = 0; i < n; i++) d[i] = val;
    return dest;
}