#ifndef PMM_H
#define PMM_H
#define PAGE_SIZE 4096
#include <stdint.h>

void pmm_init();
void* pmm_alloc();
void pmm_free(void* a);
void* pmm_alloc_pages(uint32_t n);
void pmm_free_pages(void* a, uint32_t n);

#endif
