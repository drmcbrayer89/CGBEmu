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

    }
    else if(addr <= EXT_RAM_END) {

    }
    else if(addr <= WRAM_END) {

    }
    else if(addr <= WRAM_CGB_END) {

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

    }
    else if(addr <= HRAM_END) {

    }
    else {
        printf("\t\t\tInvalid Memory Address\n");
    }
}

void busWriteAddr16(uint16_t addr, uint16_t val) {
    printf("\t\t\tNot implemented.\n");
}

void busWriteAddr(uint16_t addr, uint8_t val) {
    /* Cartridge */
    if(addr <= ROM_BANK01_END) {
        /* read only? */
    }
    else if(addr <= VRAM_END) {

    }
    else if(addr <= EXT_RAM_END) {

    }
    else if(addr <= WRAM_END) {

    }
    else if(addr <= WRAM_CGB_END) {

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

    }
    else if(addr <= HRAM_END) {

    }
    else {
        printf("\t\t\tInvalid Memory Address\n");
    }

}