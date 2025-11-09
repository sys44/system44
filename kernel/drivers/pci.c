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

        pci.bar0        = pciConfigReadDword(bus, dev, func, 0x10);
        pci.bar1        = pciConfigReadDword(bus, dev, func, 0x14);
        pci.bar2        = pciConfigReadDword(bus, dev, func, 0x18);
        pci.bar3        = pciConfigReadDword(bus, dev, func, 0x1C);
        pci.bar4        = pciConfigReadDword(bus, dev, func, 0x20);
        pci.bar5        = pciConfigReadDword(bus, dev, func, 0x24);
        
        pci.int_line    = get_byte(pciConfigReadDword(bus, dev, func, 0x3C), 1);
    }

    *out = pci;

    return 0;
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