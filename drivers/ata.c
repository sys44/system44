// ata.c ATA driver written by Comrade PS2 03/10/2025

#include "ata.h"

static inline uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "d"(port));
    return result;
}

static inline void outb(uint16_t port, uint8_t data) {
    __asm__ volatile("outb %0, %1" : : "a"(data), "d"(port));
}

static inline void insw(uint16_t port, void* buffer, uint32_t count) {
    __asm__ volatile("rep insw" : : "d"(port), "D"(buffer), "c"(count));
}

static inline void outsw(uint16_t port, void* buffer, uint32_t count) {
    __asm__ volatile("rep outsw" : : "d"(port), "S"(buffer), "c"(count));
}

static void ata_wait_busy(void) {
    while (inb(ATA_PRIMARY_STATUS) & ATA_STATUS_BSY);
}

static void ata_wait_drq(void) {
    while (!(inb(ATA_PRIMARY_STATUS) & ATA_STATUS_DRQ));
}

void ata_init(void) {
    outb(ATA_PRIMARY_DRIVE, 0xA0);
    
    for (int i = 0; i < 4; i++) {
        inb(ATA_PRIMARY_STATUS);
    }
}

int ata_read_sectors(uint32_t lba, uint8_t count, void* buffer) {
    if (count == 0) return -1;
    
    ata_wait_busy();
    
    outb(ATA_PRIMARY_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECCOUNT, count);
    outb(ATA_PRIMARY_LBA_LO, lba & 0xFF);
    outb(ATA_PRIMARY_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_PRIMARY_LBA_HI, (lba >> 16) & 0xFF);
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_READ_SECTORS);
    
    uint16_t* buf = (uint16_t*)buffer;
    
    for (int i = 0; i < count; i++) {
        ata_wait_busy();
        ata_wait_drq();
        
        if (inb(ATA_PRIMARY_STATUS) & ATA_STATUS_ERR) {
            return -1;
        }
        
        insw(ATA_PRIMARY_DATA, buf, 256);
        buf += 256;
    }
    
    return count;
}

int ata_write_sectors(uint32_t lba, uint8_t count, const void* buffer) {
    if (count == 0) return -1;
    
    ata_wait_busy();
    
    outb(ATA_PRIMARY_DRIVE, 0xE0 | ((lba >> 24) & 0x0F));
    outb(ATA_PRIMARY_SECCOUNT, count);
    outb(ATA_PRIMARY_LBA_LO, lba & 0xFF);
    outb(ATA_PRIMARY_LBA_MID, (lba >> 8) & 0xFF);
    outb(ATA_PRIMARY_LBA_HI, (lba >> 16) & 0xFF);
    outb(ATA_PRIMARY_COMMAND, ATA_CMD_WRITE_SECTORS);
    
    uint16_t* buf = (uint16_t*)buffer;

    for (int i = 0; i < count; i++) {
        ata_wait_busy();
        ata_wait_drq();
        
        if (inb(ATA_PRIMARY_STATUS) & ATA_STATUS_ERR) {
            return -1;
        }
        
        outsw(ATA_PRIMARY_DATA, buf, 256);
        buf += 256;
    }
    
    return count;
}
