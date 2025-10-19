#include "pmm.h"
#include "lmm.h"
#include "../kernel/log.h"
#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 4096
#define MAX_PAGES (1024*64)

static uint8_t bitmap[MAX_PAGES / 8];

static inline void setb(uint32_t b){ bitmap[b/8] |= (1 << (b%8)); }
static inline void clrb(uint32_t b){ bitmap[b/8] &= ~(1 << (b%8)); }
static inline uint8_t tstb(uint32_t b){ return bitmap[b/8] & (1 << (b%8)); }

void pmm_init() {
    klog("initalizing PMM\n");
    for (uint32_t i=0; i<sizeof(bitmap); i++) bitmap[i]=0xFF;
    uint16_t count = *(uint16_t*)MMAPCOUNT;
    struct e820_entry* map = (struct e820_entry*)MMAPADDR;
    for (uint16_t i=0; i<count; i++) {
        if (map[i].type == 1) {
            uint32_t base = map[i].base_low;
            uint32_t len  = map[i].length_low;
            uint32_t pages = len / PAGE_SIZE;
            for (uint32_t p = 0; p < pages; p++) {
                uint32_t page = (base / PAGE_SIZE) + p;
                if (page < MAX_PAGES)
                    clrb(page);
            }
        }
    }
}

void* pmm_alloc() {
    for (uint32_t i = 0; i < MAX_PAGES; i++) {
        if (!tstb(i)) {
            setb(i);
            return (void*)(i * PAGE_SIZE);
        }
    }
    return NULL;
}

void pmm_free(void* a) {
    uint32_t p = (uint32_t)a / PAGE_SIZE;
    clrb(p);
}

void* pmm_alloc_pages(uint32_t n) {
    if (n == 0) return NULL;
    for (uint32_t i = 0; i <= MAX_PAGES - n; i++) {
        uint32_t j;
        for (j = 0; j < n; j++) {
            if (tstb(i + j)) break;
        }
        if (j == n) {
            for (uint32_t k = 0; k < n; k++)
                setb(i + k);
            return (void*)(i * PAGE_SIZE);
        }
        i += j;
    }
    return NULL;
}

void pmm_free_pages(void* a, uint32_t n) {
    uint32_t p = (uint32_t)a / PAGE_SIZE;
    for (uint32_t i = 0; i < n; i++)
        clrb(p + i);
}
