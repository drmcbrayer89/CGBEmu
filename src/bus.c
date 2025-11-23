#include "bus.h"
#include "cart.h"
#include "memory.h"
#include "cpu.h"
#include "io.h"

/* Pulled the memory map from the pan docs website */
uint8_t busReadAddr(uint16_t addr) {
    /* Cartridge */
    if(addr < 0x8000) {
        return cartReadAddr(addr);
    } 
    // char/map
    else if(addr < 0xA000) {
    }
    else if(addr < 0xC0000) {
        return cartReadAddr(addr);
    }
    else if(addr < 0xE0000) {
        return memReadWRam(addr);
    }
    else if(addr < 0xFE00) {
        return 0;
    }
    else if(addr < 0xFEA0) {
        //TODO
        return 0;
    } // do not enter
    else if(addr < 0xFF00) {
        return 0;
    }
    else if(addr < 0xFF80) {
        return ioRead(addr);
    }
    else if(addr == 0xFFFF) {
        //TODO
        return cpuGetIE();
    }
    else {
        return memReadHRam(addr);
    }
}

void busWriteAddr(uint16_t addr, uint8_t val) {
        /* Cartridge */
    if(addr < 0x8000) {
        return cartWriteAddr(addr, val);
    } 
    // char/map
    else if(addr < 0xA000) {
    } // cartridge ram
    else if(addr < 0xC000) {
        return cartWriteAddr(addr,val);
    } // working ram
    else if(addr < 0xE000) {
        //printf("WRITING (0x%04X): 0x%04X\n", addr, val);
        return memWriteWRam(addr, val);
    } // reserved 
    else if(addr < 0xFE00) {
    } // do not enter
    else if(addr < 0xFEA0) {
        //TODO
    } // do not enter
    else if(addr < 0xFF00) {

    } // i/o
    else if(addr < 0xFF80) {
        ioWrite(addr, val);
    } // enable interrupt register
    else if(addr == 0xFFFF) {
        //TODO
        return cpuSetIE(val);
    } // anything else goes to hram
    else {
        memWriteHRam(addr, val);
    }
}

void busWriteAddr16(uint16_t addr, uint16_t val) {
    busWriteAddr(addr + 1, (val >> 8) & 0xFF);
    busWriteAddr(addr, val & 0xFF);   
}