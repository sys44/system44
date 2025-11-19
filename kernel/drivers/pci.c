// https://wiki.osdev.org/PCI
#include "pci.h"
#include <stddef.h>

#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC

static inline uint32_t inl(uint16_t port) {
    uint32_t result;
    __asm__ volatile("inl %1, %0" : "=a"(result) : "d"(port));
    return result;
}

static inline void outl(uint16_t port, uint32_t data) {
    __asm__ volatile("outl %0, %1" : : "a"(data), "d"(port));
}

uint32_t pciConfigReadDword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint32_t tmp = 0;
  
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
  
    // Write out the address
    outl(CONFIG_ADDRESS, address);
    // Read in the data
    tmp = inl(CONFIG_DATA);
    return tmp;
}

void pciConfigWriteDword(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t value) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint32_t tmp = 0;
  
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
  
    // Write out the address
    outl(CONFIG_ADDRESS, address);
    // Read in the data
    outl(CONFIG_DATA, value);
}

#define get_byte(dword, offset) ((dword >> ((offset & 3) * 8)) & 0xFF) 
#define get_word(dword, offset) ((dword >> ((offset & 2) * 8)) & 0xFFFF) 

int pciProbeDevice(uint8_t bus, uint8_t dev, uint8_t func, pci_Device* out) {
    pci_Device pci = {0};

    pci.vendor = get_word(pciConfigReadDword(bus, dev, func, 0x00), 0);
    pci.device = get_word(pciConfigReadDword(bus, dev, func, 0x00), 2);
    if (pci.vendor == 0xFFFF) return -1;

    
    pci.status      = get_word(pciConfigReadDword(bus, dev, func, 0x04), 2);
    pci.command     = get_word(pciConfigReadDword(bus, dev, func, 0x04), 0);
    
    pci.class       = get_byte(pciConfigReadDword(bus, dev, func, 0x08), 3);
    pci.subclass    = get_byte(pciConfigReadDword(bus, dev, func, 0x08), 2);
    pci.prog_if     = get_byte(pciConfigReadDword(bus, dev, func, 0x08), 1);
    
    pci.header_type = get_byte(pciConfigReadDword(bus, dev, func, 0x0C), 2);

    if (pci.header_type == 0x0) {

        pci.bar[0].addr        = pciConfigReadDword(bus, dev, func, 0x10);
        pci.bar[1].addr        = pciConfigReadDword(bus, dev, func, 0x14);
        pci.bar[2].addr        = pciConfigReadDword(bus, dev, func, 0x18);
        pci.bar[3].addr        = pciConfigReadDword(bus, dev, func, 0x1C);
        pci.bar[4].addr        = pciConfigReadDword(bus, dev, func, 0x20);
        pci.bar[5].addr        = pciConfigReadDword(bus, dev, func, 0x24);
        
        pci.int_line    = get_byte(pciConfigReadDword(bus, dev, func, 0x3C), 1);
    }

    *out = pci;

    return 0;
}

int pciProbeBars(uint8_t bus, uint8_t dev, uint8_t func, pci_Device* out) {
    for (int i = 0; i < 6; i++) {
        uint8_t offset = 0x10 + (i * 4);
        uint32_t orig = pciConfigReadDword(bus, dev, func, offset);

        // Skip if there is nothing to read lmao
        if (orig == 0) {
            out->bar[i].size = 0;
            out->bar[i].is_io = 0;
            continue;
        }

        // apparently this determines size idk ty wiki
        pciConfigWriteDword(bus, dev, func, offset, 0xFFFFFFFF);
        uint32_t probe = pciConfigReadDword(bus, dev, func, offset);

        pciConfigWriteDword(bus, dev, func, offset, orig);

        // Skip if size is 0
        if (probe == 0) {
            out->bar[i].size = 0;
            out->bar[i].is_io = 0;
            continue;
        }

        if (probe & 0x1) {  // I/O space BAR
            out->bar[i].is_io = true;
            out->bar[i].size = ~(probe & 0xFFFFFFFC) + 1;   // sorcery
            out->bar[i].addr = orig & 0xFFFFFFFC;
        } else {
            // Memory space BAR
            uint32_t type = (probe >> 1) & 0x3;
            if (type == 0x2) {
                // 64-bit BAR: combine with next BAR
                if (i + 1 < 6) {
                    uint8_t next_offset = 0x10 + ((i + 1) * 4);
                    uint32_t orig_hi = pciConfigReadDword(bus, dev, func, next_offset);

                    // same magic as earlier
                    pciConfigWriteDword(bus, dev, func, next_offset, 0xFFFFFFFF);
                    uint32_t probe_hi = pciConfigReadDword(bus, dev, func, next_offset);

                    // restore upper original
                    pciConfigWriteDword(bus, dev, func, next_offset, orig_hi);

                    uint64_t full_probe = ((uint64_t)probe_hi << 32) | (uint64_t)probe;

                    // mask lower 4 bits of low dword per spec
                    uint64_t mask = ~(full_probe & (~(uint64_t)0xFULL));
                    uint64_t size = mask + 1;

                    uint64_t full_orig = ((uint64_t)orig_hi << 32) | (uint64_t)orig;
                    out->bar[i].is_io = false;
                    out->bar[i].size = (uint32_t)size;
                    out->bar[i].addr = (uint32_t)(full_orig & (~(uint64_t)0xFULL));

                    // mark next BAR as consumed
                    out->bar[i+1].size = 0;
                    out->bar[i+1].is_io = 0;
                    out->bar[i+1].addr = 0;

                    i++; // skip next BAR (upper half of 64-bit BAR)
                    continue;
                } else {
                    // malformed device: 64-bit BAR but no next BAR - treat as 32-bit
                }
            }

            // 32-bit memory BAR
            uint32_t mask = ~(probe & 0xFFFFFFF0);
            uint32_t size = mask + 1;
            out->bar[i].is_io = false;
            out->bar[i].size = size;
            out->bar[i].addr = orig & 0xFFFFFFF0;
        }
    }
}

void pciEnumerate(struct pci_Out out[]) {
    pci_Device device;
    uint8_t bus = 0;
    size_t out_pos = 0;

    for (uint8_t dev = 0; dev < 32; dev++) {
        if (pciProbeDevice(bus, dev, 0, &device) != 0) continue;
        out[out_pos++] = (struct pci_Out){ .pci = device, .bus = bus, .dev = dev, .func = 0 };

        if ((device.header_type & 0x80) != 0) {
            for (uint8_t f = 1; f <= 7; f++) {
                if (pciProbeDevice(bus, dev, f, &device) != 0) continue;
                out[out_pos++] = (struct pci_Out){ .pci = device, .bus = bus, .dev = dev, .func = f };
            }
        }
    }
}