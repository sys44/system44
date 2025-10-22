#include "../fs/kfs.h"
#include "../drivers/ata.h"
#include "../mm/pmm.h"
#include "../lib/memory.h"
#include "../lib/error.h"
#include <stdint.h>

#define uexMagic 0x31555845

struct uexHdr {
    uint32_t magic;
    uint32_t entry;
    uint32_t loadAddr;
    uint32_t fileSize;
    uint32_t memSize;
    uint32_t flags;
} __attribute__((packed));

struct uexAlloc {
    void* base;
    uint32_t pages;
};

int uexExec(const char* name, void** entry, struct uexAlloc* alloc) {
    struct kfs_file* f = kfs_find(name);
    if (!f) return ERR_NOT_FOUND;

    if (f->size < sizeof(struct uexHdr)) return ERR_FORMAT;

    uint8_t secbuf[512];
    if (ata_read_sectors(f->start_lba, 1, secbuf) < 0) return ERR_IO;

    struct uexHdr hdr;
    memcpy(&hdr, secbuf, sizeof(hdr));
    if (hdr.magic != uexMagic) return ERR_FORMAT;

    uint8_t* dest = (uint8_t*)hdr.loadAddr;
    uint32_t pages = (hdr.memSize + 4095) / 4096;
    if (!dest) {
        dest = pmm_alloc_pages(pages);
        if (!dest) return ERR_OOM;
        if (alloc) {
            alloc->base = dest;
            alloc->pages = pages;
        }
    } else {
        if (alloc) {
            alloc->base = 0;
            alloc->pages = 0;
        }
    }

    uint32_t toCopy = hdr.fileSize;
    uint32_t copied = 0;
    uint32_t firstChunk = 512 - sizeof(hdr);
    if (firstChunk > 0 && toCopy > 0) {
        uint32_t n = (toCopy < firstChunk) ? toCopy : firstChunk;
        memcpy(dest, secbuf + sizeof(hdr), n);
        copied += n;
    }

    if (copied < toCopy) {
        uint32_t remaining = toCopy - copied;
        uint32_t sectors = (remaining + 511) / 512;
        if (ata_read_sectors(f->start_lba + 1, sectors, dest + copied) < 0) return ERR_IO;
    }

    if (hdr.memSize > hdr.fileSize)
        memset(dest + hdr.fileSize, 0, hdr.memSize - hdr.fileSize);
    if (hdr.loadAddr == 0)
        *entry = (void*)((uint32_t)dest + hdr.entry);
    else
        *entry = (void*)hdr.entry;
    
    return ERR_SUCCESS;
}
