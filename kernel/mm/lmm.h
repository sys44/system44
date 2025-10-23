#pragma once
#include <stdint.h>
struct e820_entry {
    uint32_t base_low;
    uint32_t base_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
};
#define MMAPADDR  0x8000
#define MMAPCOUNT 0x7DFE
void mmp();
