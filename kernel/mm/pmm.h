#pragma once
#include <stddef.h>
#include <stdint.h>

void pmm_init(void);
void* pmm_alloc(void);
void pmm_free(void* addr);
void* pmm_alloc_pages(uint32_t n);
void pmm_free_pages(void* addr, uint32_t n);
void* kmalloc(size_t size);
void kfree(void* ptr);

