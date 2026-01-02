#pragma once
#include <stddef.h>
#include <stdint.h>

void pmm_init(void);
void* pmm_alloc(size_t pages);
void pmm_free(void *ptr, size_t pages);
