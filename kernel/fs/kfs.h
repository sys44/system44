#pragma once
#include <stdint.h>
#define KFS_MAGIC      0x30534653
#define KFS_MAX_FILES  8
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
int kfs_mount();
int kfs_format();
struct kfs_file* kfs_find(const char* name);
int kfs_read(const char* name, void* buf);
int kfs_write(const char* name, const void* data, uint32_t size);
