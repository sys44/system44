#include "pmm.h"
#include "lmm.h"
#include "../core/log.h"
#include <stdint.h>
#include <stddef.h>
#include "../lib/memory.h"
#include "../lib/io.h"
#define PAGE_SIZE 4096
#define MAX_PAGES (1024*64)
#define KMALLOC_SLAB_COUNT 128
#define KMALLOC_MAX_SIZE 4096

static uint8_t bitmap[MAX_PAGES / 8];
static uint32_t used_pages = 0;

typedef struct kmalloc_slab {
    struct kmalloc_slab *next;
    size_t size;
    uint8_t free;
    uint8_t data[];
} kmalloc_slab_t;

static kmalloc_slab_t *slab_head = NULL;

static inline void setb(uint32_t b) { bitmap[b/8] |= (1 << (b%8)); }
static inline void clrb(uint32_t b) { bitmap[b/8] &= ~(1 << (b%8)); }
static inline uint8_t tstb(uint32_t b) { return bitmap[b/8] & (1 << (b%8)); }

void pmm_init(void) {
    memset(bitmap, 0xFF, sizeof(bitmap));
    used_pages = MAX_PAGES;
    uint16_t count = *(uint16_t*)MMAPCOUNT;
    struct e820_entry* map = (struct e820_entry*)MMAPADDR;
    for (uint16_t i = 0; i < count; i++) {
        if (map[i].type != 1) continue;
        uint32_t base = map[i].base_low;
        uint32_t len  = map[i].length_low;
        uint32_t pages = len / PAGE_SIZE;
        for (uint32_t p = 0; p < pages; p++) {
            uint32_t page = (base / PAGE_SIZE) + p;
            if (page >= MAX_PAGES) continue;
            clrb(page);
            used_pages--;
        }
    }
}

void* pmm_alloc(void) {
    for (uint32_t i = 0; i < MAX_PAGES; i++) {
        if (!tstb(i)) {
            setb(i);
            used_pages--;
            return (void*)(i * PAGE_SIZE);
        }
    }
    return NULL;
}

void pmm_free(void* addr) {
    if (!addr) return;
    uint32_t page = ((uintptr_t)addr) / PAGE_SIZE;
    if (page >= MAX_PAGES) return;
    if (!tstb(page)) return;
    clrb(page);
    used_pages++;
}

void* pmm_alloc_pages(uint32_t n) {
    if (n == 0 || n > MAX_PAGES) return NULL;
    for (uint32_t i = 0; i <= MAX_PAGES - n; i++) {
        uint32_t j;
        for (j = 0; j < n; j++)
            if (tstb(i + j)) break;
        if (j == n) {
            for (uint32_t k = 0; k < n; k++) setb(i + k);
            used_pages -= n;
            return (void*)(i * PAGE_SIZE);
        }
        i += j;
    }
    return NULL;
}

void pmm_free_pages(void* addr, uint32_t n) {
    if (!addr || n == 0) return;
    uint32_t page = ((uintptr_t)addr) / PAGE_SIZE;
    if (page + n > MAX_PAGES) return;
    for (uint32_t i = 0; i < n; i++) {
        if (!tstb(page + i)) {
            clrb(page + i);
            used_pages++;
        }
    }
}

void* kmalloc(size_t size) {
    if (size == 0 || size > KMALLOC_MAX_SIZE) return NULL;
    kmalloc_slab_t *s = slab_head;
    while (s) {
        if (s->free && s->size >= size) {
            s->free = 0;
            return s->data;
        }
        s = s->next;
    }
    void *page = pmm_alloc_pages(1);
    if (!page) return NULL;
    s = (kmalloc_slab_t*)page;
    s->next = slab_head;
    s->size = size;
    s->free = 0;
    slab_head = s;
    return s->data;
}

void kfree(void* ptr) {
    if (!ptr) return;
    kmalloc_slab_t *s = slab_head;
    while (s) {
        if (s->data == ptr) {
            s->free = 1;
            return;
        }
        s = s->next;
    }
}
