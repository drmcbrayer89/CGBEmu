#include "bus.h"
#include "cart.h"
#include "memory.h"

/* Pulled the memory map from the pan docs website */
uint8_t busReadAddr(uint16_t addr) {
    /* Cartridge */
    if(addr < 0x8000) {
        return cartReadAddr(addr);
    } 
    // char/map
    else if(addr < 0xA000) {
        exit(-1);
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
    }
    else if(addr == 0xFFFF) {
        //TODO
        return 0;
    }
    else {
        return memReadHRam(addr);
    }
}

void busWriteAddr(uint16_t addr, uint8_t val) {
    /* Cartridge */
}

void busWriteAddr16(uint16_t addr, uint16_t val) {
    busWriteAddr(addr + 1, (val >> 8) & 0xFF);
    busWriteAddr(addr, val & 0xFF);   
}