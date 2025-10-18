#pragma once
#include <stdint.h>
#define uexMagic 0x31555845
struct uexHdr {
    uint32_t magic;
    uint32_t entry;
    uint32_t loadAddr;
    uint32_t fileSize;
    uint32_t memSize;
    uint32_t flags;
} __attribute__((packed));

struct uexAlloc {
    void* base;
    uint32_t pages;
};

int uexExec(const char* name, void** entry, struct uexAlloc* alloc);
