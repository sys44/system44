#pragma once
#include <stdint.h>
void pmm_init();
void* pmm_alloc();
void pmm_free(void* addr);
