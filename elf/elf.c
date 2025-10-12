#include "../fs/kfs.h"
#include "../mm/pmm.h"
#include "../drivers/ata.h"
#include <stdint.h>
#include "../lib/memory.h"
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

int elfexec(const char* name, void** entry) {
    uint8_t buf[512];
    if (kfs_read(name, buf) < 0) return -1;
    struct elf32_hdr* eh = (struct elf32_hdr*)buf;
    if (*(uint32_t*)eh != ELF_MAGIC) return -2;
    for (int i = 0; i < eh->phnum; i++) {
        struct elf32_phdr ph;
        uint32_t off = eh->phoff + i * eh->phentsize;
        uint32_t sector = off / 512;
        uint32_t insec = off % 512;
        if (ata_read_sectors(1 + sector, 1, buf) < 0) return -3;
        memcpy(&ph, buf + insec, sizeof(ph));
        if (ph.type != PT_LOAD) continue;
        uint8_t* seg = pmm_alloc();
        if (!seg) return -4;
        if (kfs_read(name, seg) < 0) return -5;
        memset(seg + ph.filesz, 0, ph.memsz - ph.filesz);
    }
    *entry = (void*)eh->entry;
    return 0;
}
