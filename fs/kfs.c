#include "../drivers/ata.h"
#include <stdint.h>
#include "../lib/memory.h"
#include "../lib/string.h"
#include "../kernel/panic.h"
#define KFS_MAGIC 0x3053464B
#define KFS_MAX_FILES 8
#define KFS_SECTOR_SIZE 512
struct kfs_file {
    char name[32];
    uint32_t start_lba;
    uint32_t size;
};
struct kfs_superblock {
    uint32_t magic;
    uint32_t file_count;
    struct kfs_file files[KFS_MAX_FILES];
};
struct kfs_superblock superblock;

int kfs_mount() {
    if (ata_read_sectors(0, 1, &superblock) < 0)
        panic("i have no root and i must scream. (disk read failure)");
    if (superblock.magic != KFS_MAGIC)
        panic("i have no root and i must scream. (invalid magic, not a KFS volume)");
    return 0;
}

int kfs_mkfs() {
    memset(&superblock, 0, sizeof(superblock));
    superblock.magic = KFS_MAGIC;
    return ata_write_sectors(0, 1, &superblock);
}

struct kfs_file* kfs_find(const char* name) {
    for (uint32_t i = 0; i < superblock.file_count; i++)
        if (!strcmp(superblock.files[i].name, name))
            return &superblock.files[i];
    return 0;
}

int kfs_read(const char* name, void* buf) {
    struct kfs_file* f = kfs_find(name);
    if (!f) return -1;
    uint32_t sectors = (f->size + 511) / 512;
    return ata_read_sectors(f->start_lba, sectors, buf);
}

int kfs_write(const char* name, const void* data, uint32_t size) {
    if (superblock.file_count >= KFS_MAX_FILES) return -1;
    struct kfs_file* f = &superblock.files[superblock.file_count++];
    strcpy(f->name, name);
    f->size = size;
    uint32_t lba = 1;
    for (uint32_t i = 0; i < superblock.file_count - 1; i++)
        lba += (superblock.files[i].size + 511) / 512;
    f->start_lba = lba;
    uint32_t sectors = (size + 511) / 512;
    ata_write_sectors(f->start_lba, sectors, data);
    return ata_write_sectors(0, 1, &superblock);
}
