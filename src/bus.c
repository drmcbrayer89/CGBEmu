#include "bus.h"
#include "cart.h"
#include "memory.h"

/* Pulled the memory map from the pan docs website */
uint8_t busReadAddr(uint16_t addr) {
    /* Cartridge */
    if(addr <= ROM_BANK01_END) {
        /* read only? */
    }
    else if(addr <= VRAM_END) {
        // TODO
    }
    else if(addr <= CART_RAM_END) {
        return cartReadAddr(addr);
    }
    else if(addr <= WRAM_END) {
        return memReadWRam(addr);
    }
    else if(addr <= ECHO_RAM_END) {
        printf("\t\t\tNintendo says nintenDONT do anything here\n");
        return 0;
    }
    else if(addr <= OAM_END) {
        // TODO Do this when working on the PPU
    }
    else if(addr <= NO_ACCESS_END) {
        printf("\t\t\tNintendo says nintenDONT do anything here\n");
        return 0;
    }
    else if(addr <= IO_END) {
        // TODO 
        return 0;
    }
    else if(addr <= HRAM_END) {
        return memReadHRam(addr);
    }
    else if(addr == 0xFFFF) {
        // TODO this is the interrupt stuff
        return 0;
    }
    else {
        printf("\t\t\tInvalid Memory Address\n");
        return 0;
    }

}

void busWriteAddr16(uint16_t addr, uint16_t val) {
    printf("\t\t\tNot implemented.\n");
}

void busWriteAddr(uint16_t addr, uint8_t val) {
    /* Cartridge */
    if(addr <= ROM_BANK01_END) {
        cartWriteAddr(addr, val);
    }
    else if(addr <= VRAM_END) {

    }
    else if(addr <= CART_RAM_END) {
        cartWriteAddr(addr, val);
    }
    else if(addr <= WRAM_END) {
        memWriteWRam(addr, val);
    }
    else if(addr <= ECHO_RAM_END) {
        printf("\t\t\tNintendo says nintenDONT do anything here\n");
    }
    else if(addr <= OAM_END) {

    }
    else if(addr <= NO_ACCESS_END) {
        printf("\t\t\tNintendo says nintenDONT do anything here\n");
    }
    else if(addr <= IO_END) {
        printf("\t\t\tTODO\n");
    }
    else {
        memWriteHRam(addr, val);
    }

}