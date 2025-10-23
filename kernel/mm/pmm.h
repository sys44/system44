#ifndef PMM_H
#define PMM_H

#include <stdint.h>

void pmm_init();
void* pmm_alloc();
void pmm_free(void* a);
void* pmm_alloc_pages(uint32_t n);
void pmm_free_pages(void* a, uint32_t n);

#endif
