#pragma once
#include <stdint.h>
extern uint32_t *pagedir;
extern uint32_t *ftb;
void paging_init(void);
