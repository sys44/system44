#include <stdint.h>

unsigned int* pagedir = (unsigned int*)0x9C000;
unsigned int* ftb = (unsigned int*)0x9D000;

void paging_init() {
    for (int i=0;i<1024;i++)
        ftb[i]=(i*0x1000)|3;
    pagedir[0]=((uint32_t)ftb)|3;
    for (int i=1;i<1024;i++)
        pagedir[i]=0;
    __asm__ volatile("mov %0, %%cr3"::"r"(pagedir));
    uint32_t cr0;
    __asm__ volatile("mov %%cr0,%0":"=r"(cr0));
    cr0|=0x80000000;
    __asm__ volatile("mov %0,%%cr0"::"r"(cr0));
}
