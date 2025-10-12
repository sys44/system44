#pragma once
#include <stdint.h>
#define ELF_MAGIC 0x464C457F
#define PT_LOAD    1
#define PAGE_SIZE  4096
struct elf32_hdr {
    uint32_t magic;
    uint8_t  pad[12];
    uint32_t entry;
    uint32_t phoff;
    uint16_t phentsize;
    uint16_t phnum;
};
struct elf32_phdr {
    uint32_t type;
    uint32_t offset;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesz;
    uint32_t memsz;
    uint32_t flags;
    uint32_t align;
};

int elfexec(const char* name, void** entry);
