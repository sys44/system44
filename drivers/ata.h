// ATA driver for system44 written by Comrade PS2 03/10/2025

#ifndef ATA_H
#define ATA_H
#include <stdint.h>

#define ATA_PRIMARY_DATA       0x1F0
#define ATA_PRIMARY_ERROR      0x1F1
#define ATA_PRIMARY_SECCOUNT   0x1F2
#define ATA_PRIMARY_LBA_LO     0x1F3
#define ATA_PRIMARY_LBA_MID    0x1F4
#define ATA_PRIMARY_LBA_HI     0x1F5
#define ATA_PRIMARY_DRIVE      0x1F6
#define ATA_PRIMARY_COMMAND    0x1F7
#define ATA_PRIMARY_STATUS     0x1F7

#define ATA_CMD_READ_SECTORS  0x20
#define ATA_CMD_WRITE_SECTORS 0x30

#define ATA_STATUS_BSY        0x80
#define ATA_STATUS_DRQ        0x08
#define ATA_STATUS_ERR        0x01

void ata_init();
int ata_read_sectors(uint32_t lba, uint8_t sector_count, void* buffer);
int ata_write_sectors(uint32_t lba, uint8_t count, const void* buffer);

#endif // ATA_H


