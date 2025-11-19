#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    struct pci_BAR {
        bool is_io;
        uint8_t size;
        uint32_t addr;
    } bar[6];
    uint16_t vendor, device, status, command;
    uint8_t header_type, class, subclass, prog_if, int_line;
} pci_Device;

struct pci_Out {
    pci_Device pci;
    uint8_t bus, dev, func;
};

void pciEnumerate(struct pci_Out out[]);