#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../lib/io.h"

#include "lmm.h"

#define HHDM_OFFSET 0xFFFF800000000000ULL

//physical to virtual conversion using HHDM
#define P2V(phys) ((void *)((uintptr_t)(phys) + HHDM_OFFSET))

//virtual to physical conversion using HHDM
#define V2P(virt) ((uintptr_t)(virt) - HHDM_OFFSET)

#define PAGE_SIZE 4096

static uint8_t *bitmap = NULL;
static size_t bitmap_size = 0; //in bytes
static uint64_t max_pages = 0;

#define BITMAP_SET(bit)   (bitmap[(bit) / 8] |= (1 << ((bit) % 8)))
#define BITMAP_CLEAR(bit) (bitmap[(bit) / 8] &= ~(1 << ((bit) % 8)))
#define BITMAP_TEST(bit)  (bitmap[(bit) / 8] & (1 << ((bit) % 8)))

static uint64_t last_free_page = 0;

void pmm_init(void) {
    struct e820_entry *mmap = (struct e820_entry*)MMAPADDR;
    if (!mmap) {
        puts("[pmm] ERROR: no memory map tag found\n");
        return;
    }

    uint64_t max_addr = 0;
    uintptr_t entries_ptr = (uintptr_t)mmap;

    puts("[pmm] memory map:\n");
    for (uint16_t i = 0; i < *(uint16_t*)MMAPCOUNT; i++) {
        struct e820_entry *current = (struct e820_entry *)(entries_ptr + i * sizeof(struct e820_entry));
        uint32_t base = current->base_low + current->base_high;
        uint32_t length = current->length_low + current->length_high;
        if (current->type == 1) {
            printf("  - %x - %x (usable)\n", base, base + length);

            if (base + length > max_addr) {
                max_addr = base + length;
            }
        }
    }

    if (max_addr == 0) {
        puts("[pmm] ERROR: no usable memory regions found in map\n");
        return;
    }

    max_pages = max_addr / PAGE_SIZE;
    bitmap_size = max_pages / 8;
    if (max_pages % 8) bitmap_size++;

    printf("[pmm] max_addr: %x, bitmap_size: %x bytes\n", max_addr, bitmap_size);

    //find a place for the bitmap (avoiding the first 1MB if possible)
    bool found = false;
    for (uint32_t i = 0; i < *(uint16_t*)MMAPCOUNT; i++) {
        struct e820_entry *current = (struct e820_entry *)(entries_ptr + i * sizeof(struct e820_entry));
        if (current->type == 1 && current->length_low >= bitmap_size) {
            //don't put bitmap at address 0 try to keep it above 1MB
            if (current->base_low >= 0x100000) {
                //use HHDM for bitmap address
                bitmap = (uint8_t*)P2V(current->base_low);
                found = true;
                break;
            }
        }
    }

    //fallback if no region above 1MB is large enough
    if (!found) {
        for (uint32_t i = 0; i < *(uint16_t*)MMAPCOUNT; i++) {
            struct e820_entry *current = (struct db_mmap_entry *)(entries_ptr + i * sizeof(struct e820_entry));
            if (current->type == 1 && current->length_low >= bitmap_size && current->base_low > 0) {
                bitmap = (uint8_t*)P2V(current->base_low);
                found = true;
                break;
            }
        }
    }

    if (!found) {
        puts("[pmm] ERROR: could not find safe location for bitmap\n");
        return;
    }

    //initially mark everything as reserved (1)
    for (size_t i = 0; i < bitmap_size; i++) bitmap[i] = 0xFF;

    //mark usable regions as free (0)
    for (uint32_t i = 0; i < *(uint16_t*)MMAPCOUNT; i++) {
        struct e820_entry *current = (struct e820_entry *)(entries_ptr + i * sizeof(struct e820_entry));
        if (current->type == 1) {
            uint64_t start_page = current->base_low / PAGE_SIZE;
            uint64_t page_count = current->length_low / PAGE_SIZE;
            for (uint64_t j = 0; j < page_count; j++) {
                if (start_page + j < max_pages) {
                    BITMAP_CLEAR(start_page + j);
                }
            }
        }
    }
    
    //reserve the bitmap itself
    uintptr_t bitmap_phys = V2P(bitmap);
    uint64_t bitmap_start_page = bitmap_phys / PAGE_SIZE;
    uint64_t bitmap_page_count = bitmap_size / PAGE_SIZE;
    if (bitmap_size % PAGE_SIZE) bitmap_page_count++;
    for (uint64_t i = 0; i < bitmap_page_count; i++) {
        BITMAP_SET(bitmap_start_page + i);
    }

    //reserve page 0
    BITMAP_SET(0);

    printf("[pmm] initialised, bitmap @ %x\n", (uintptr_t)bitmap);
}

void *pmm_alloc(size_t pages) {
    if (pages == 0) return NULL;

    uint64_t consecutive = 0;
    uint64_t start_bit = 0;

    //word skipping optimization
    uint32_t *bitmap_words = (uint32_t*)bitmap;
    uint64_t max_words = max_pages / 32;

    for (uint64_t i = last_free_page; i < max_pages; i++) {
        //if we are at the start of a word and need more than current bit try skipping
        if ((i % 32 == 0) && (consecutive == 0)) {
            while (i / 32 < max_words && bitmap_words[i / 32] == (uint32_t)-1) {
                i += 32;
            }
            if (i >= max_pages) break;
        }

        if (!BITMAP_TEST(i)) {
            if (consecutive == 0) start_bit = i;
            consecutive++;
            if (consecutive == pages) {
                for (uint64_t j = 0; j < pages; j++) {
                    BITMAP_SET(start_bit + j);
                }
                last_free_page = start_bit + pages;
                return (void *)(uintptr_t)(start_bit * PAGE_SIZE);
            }
        } else {
            consecutive = 0;
        }
    }

    //if we reached the end try searching from the beginning once
    if (last_free_page > 0) {
        uint64_t search_limit = last_free_page;
        last_free_page = 0;
        consecutive = 0;
        for (uint64_t i = 0; i < search_limit; i++) {
            if ((i % 32 == 0) && (consecutive == 0)) {
                while (i / 32 < max_words && i + 32 <= search_limit && bitmap_words[i / 32] == (uint32_t)-1) {
                    i += 32;
                }
                if (i >= search_limit) break;
            }

            if (!BITMAP_TEST(i)) {
                if (consecutive == 0) start_bit = i;
                consecutive++;
                if (consecutive == pages) {
                    for (uint64_t j = 0; j < pages; j++) {
                        BITMAP_SET(start_bit + j);
                    }
                    last_free_page = start_bit + pages;
                    return (void *)(uintptr_t)(start_bit * PAGE_SIZE);
                }
            } else {
                consecutive = 0;
            }
        }
    }

    return NULL;
}

void pmm_free(void *ptr, size_t pages) {
    if (!ptr) return;
    uintptr_t addr = (uintptr_t)ptr;
    uint64_t start_bit = addr / PAGE_SIZE;

    for (uint64_t i = 0; i < pages; i++) {
        if (start_bit + i < max_pages) {
            BITMAP_CLEAR(start_bit + i);
        }
    }

    if (start_bit < last_free_page) {
        last_free_page = start_bit;
    }
}