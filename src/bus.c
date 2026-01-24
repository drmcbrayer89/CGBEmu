#include "bus.h"
#include "cart.h"
#include "memory.h"
#include "cpu.h"
#include "io.h"
#include "ppu.h"

/* Pulled the memory map from the pan docs website */
uint8_t busReadAddr(uint16_t addr) {
    // cartridge
    if(addr < 0x8000) {
        return cartReadAddr(addr);
    } 
    else if(addr < 0xA000) {
        return ppuReadVram(addr);
    }
    else if(addr < 0xC000) {
        return cartReadAddr(addr);
    }
    else if(addr < 0xE000) {
        return memReadWRam(addr);
    }
    else if(addr < 0xFE00) {
        return 0;
    }
    else if(addr < 0xFEA0) {
        return ppuReadOam(addr);
    } // do not enter
    else if(addr < 0xFF00) {
        return 0;
    }
    else if(addr < 0xFF80) {
        if(addr == 0xFF44) {
            return 0x90;
        }
        else {
            return ioRead(addr);
        }
    }
    else if(addr == 0xFFFF) {
        return cpuGetIE();
    }

    return memReadHRam(addr);
}

void busWriteAddr(uint16_t addr, uint8_t val) {
    // cartridge
    if(addr < 0x8000) {
        cartWriteAddr(addr, val);
        return;
    } // vram
    else if(addr < 0xA000) {
        ppuWriteVram(addr, val);
    } // cartridge ram
    else if(addr < 0xC000) {
        cartWriteAddr(addr,val);
        return;
    } // working ram
    else if(addr < 0xE000) {
        memWriteWRam(addr, val);
        return;
    } // reserved 
    else if(addr < 0xFE00) {
    } // do not enter
    else if(addr < 0xFEA0) {
        ppuWriteOam(addr, val);
    } // do not enter
    else if(addr < 0xFF00) {
        return;
    } // i/o
    else if(addr < 0xFF80) {
        ioWrite(addr, val);
    } // enable interrupt register
    else if(addr == 0xFFFF) {
        cpuSetIE(val);
        return;
    } // anything else goes to hram
    else {
        memWriteHRam(addr, val);
    }
}

void busWriteAddr16(uint16_t addr, uint16_t val) {
    busWriteAddr(addr + 1, (val >> 8) & 0xFF);
    busWriteAddr(addr, val & 0xFF);   
}