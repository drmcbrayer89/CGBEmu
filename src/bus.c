#include "bus.h"
#include "cart.h"

/* Pulled the memory map from the pan docs website */
uint8_t busReadAddr(uint16_t addr) {
    /* Cartridge */
    if(addr < 0x8000) {
        return cartReadAddr(addr);
    }
    
    if(addr >= 0x8000 && addr <= 0x9FFF) {

    }
}

void busWriteAddr(uint16_t addr, uint8_t val) {
    /* Cartridge */
    if(addr < 0x8000) {
        /* read only? */
    }
}