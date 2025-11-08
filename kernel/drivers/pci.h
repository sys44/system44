#pragma once

#include <stdint.h>

typedef struct {
    uint32_t bar0, bar1, bar2, bar3, bar4, bar5;
    uint16_t vendor, device, status, command;
    uint8_t header_type, class, subclass, prog_if, int_line;
} pci_Device;

struct pci_Out {
    pci_Device pci;
    uint8_t bus, dev, func;
};

void pciEnumerate(struct pci_Out* out[]);