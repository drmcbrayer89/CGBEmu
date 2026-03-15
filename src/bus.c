#include "bus.h"
#include "cart.h"
#include "memory.h"
#include "cpu.h"
#include "io.h"
#include "ppu.h"
#include "dma.h"

// This is used to allow "god mode" access to the bus for DMA.
// Could also pass a bool into busReadAddr, but would be annoying to refactor
uint8_t busReadDmaAddr(uint16_t addr) {
    switch(addr) {
        case 0x0000 ... 0x7FFF:
            return cartReadAddr(addr);  
        case 0x8000 ... 0x9FFF:
            return ppuReadVram(addr);
        case 0xA000 ... 0xBFFF:
            return cartReadAddr(addr);
        case 0xC000 ... 0xDFFF:
            return memReadWRam(addr);
        case 0xE000 ... 0xFDFF:
            return 0x0;
        case 0xFE00 ... 0xFE9F:
            return ppuReadOam(addr);
        case 0xFEA0 ... 0xFEFF:
            return 0x0;
        case 0xFF00 ... 0xFF7F:
            if(addr == 0xFF44) {
                return 0x90;
            }
            else {
                return ioRead(addr);
            }
        case 0xFF80 ... 0xFFFE:
            return memReadHRam(addr);
        case 0xFFFF:
            return cpuGetIE();
        default:
            printf("\t\tInvalid memory address %04X\n", addr);
            return 0x0;
    }
}

/* Pulled the memory map from the pan docs website */
uint8_t busReadAddr(uint16_t addr) {
    if(dmaIsActive) {
        if(addr >= 0xFF80 && addr <= 0xFFFE) {
            return memReadHRam(addr);
        }
    }

    switch(addr) {
        case 0x0000 ... 0x7FFF:
            return cartReadAddr(addr);  
        case 0x8000 ... 0x9FFF:
            return ppuReadVram(addr);
        case 0xA000 ... 0xBFFF:
            return cartReadAddr(addr);
        case 0xC000 ... 0xDFFF:
            return memReadWRam(addr);
        case 0xE000 ... 0xFDFF:
            return 0x0;
        case 0xFE00 ... 0xFE9F:
            return ppuReadOam(addr);
        case 0xFEA0 ... 0xFEFF:
            return 0x0;
        case 0xFF00 ... 0xFF7F:
            if(addr == 0xFF44) {
                return 0x90;
            }
            else {
                return ioRead(addr);
            }
        case 0xFF80 ... 0xFFFE:
            return memReadHRam(addr);
        case 0xFFFF:
            return cpuGetIE();
        default:
            printf("\t\tInvalid memory address %04X\n", addr);
            return 0x0;
    }
}

void busWriteAddr(uint16_t addr, uint8_t val) {
    /* When DMA is active, CPU only has access to HRAM */
    if(dmaIsActive()) {
        if((addr >= 0xFF80) && (addr <= 0xFFFE)) {
            memWriteHRam(addr, val);
        }
    }

    switch(addr) {
        case 0x0000 ... 0x7FFF:
            cartWriteAddr(addr, val);
            return;
        case 0x8000 ... 0x9FFF:
            ppuWriteVram(addr, val);
            return;
        case 0xA000 ... 0xBFFF:
            cartWriteAddr(addr, val);
            return;
        case 0xC000 ... 0xDFFF:
            memWriteWRam(addr, val);
            return;
        case 0xE000 ... 0xFDFF:
            return;
        case 0xFE00 ... 0xFE9F:
            ppuWriteOam(addr, val);
            return;
        case 0xFEA0 ... 0xFEFF:
            return;
        case 0xFF00 ... 0xFF7F:
            ioWrite(addr, val);
            return;
        case 0xFF80 ... 0xFFFE:
            memWriteHRam(addr, val);
            return;
        case 0xFFFF:
            cpuSetIE(val);
            return;
        default:
            printf("\t\tInvalid memory address %04X\n", addr);
            return;
    }
}

void busWriteAddr16(uint16_t addr, uint16_t val) {
    busWriteAddr(addr + 1, (val >> 8) & 0xFF);
    busWriteAddr(addr, val & 0xFF);   
}