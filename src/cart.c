#include "cart.h"

static CARTRIDGE cart;

static ENUM_MAP cart_type_table[NUM_CART_TYPES] = {
    {ROM_ONLY, "ROM ONLY\0"}, 
    {MBC1, "MBC1\0"}, 
    {MBC1_RAM, "MBC1 + RAM\0"}, 
    {MBC1_RAM_BATTERY, "MBC1 + RAM + BATTERY\0"},
    {MBC2, "MBC2\0"},
    {MBC2_BATTERY, "MBC2 + BATTERY\0"},
    {ROM_RAM, "ROM + RAM\0"},
    {ROM_RAM_BATTERY, "ROM + RAM + BATTERY\0"},
    {MMM01, "MMM01\0"},
    {MMM01_RAM, "MMM01 + RAM\0"},
    {MBC3_TIMER_BATTERY, "MBC3 + TIMER + BATTERY\0"},
    {MBC3_TIMER_RAM_BATTERY, "MBC3 + TIMER + RAM + BATTERY\0"},
    {MBC3, "MBC3\0"},
    {MBC3_RAM, "MBC3 + RAM\0"},
    {MBC3_RAM_BATTERY, "MBC3 + RAM + BATTERY\0"},
    {MBC5, "MBC5\0"},
    {MBC5_RAM, "MBC5 + RAM\0"},
    {MBC5_RAM_BATTERY, "MBC5 + RAM + BATTERY\0"},
    {MBC5_RUMBLE, "MBC5 + RUMBLE\0"},
    {MBC5_RUMBLE_RAM, "MBC5 + RUMBLE + RAM\0"},
    {MBC5_RUMBLE_RAM_BATTERY, "MBC5 + RUMBLE + RAM + BATTERY\0"},
    {MBC6, "MBC6\0"},
    {MBC7_SENSOR_RUMBLE_RAM_BATTERY, "MBC7 + SENSOR + RUMBLE + RAM + BATTERY\0"},
    {POCKET_CAMERA, "POCKET CAMERA\0"},
    {BANDAI_TAMA5, "BANDAI TAMA5\0"},
    {HuC3, "HuC3\0"},
    {HuC1_RAM_BATTERY, "HuC1 + RAM + BATTERY\0"}
};

static ENUM_MAP cart_old_lic_table[2] = {
    {0x00, "None"},
    {0x01, "Nintendo"}
};

CARTRIDGE * cartGetCartridge(void) {
    return &cart;
}

static void cartGetRomTitle(void) {
    char title[CART_TITLE_BYTES] = {0};
    memcpy(title, &cart.rom_data[CART_TITLE], CART_TITLE_BYTES);
    //printf("\tTitle: %s\n", title);
}

static void cartGetHeader(void) {
    memcpy(&cart.header, &cart.rom_data[CART_HEADER_START], CART_HEADER_SIZE);
}

static uint8_t cartGetChecksum(void) {
    uint16_t checksum = 0;

    for(uint16_t i = 0x0134; i <= 0x014C; i++) {
        checksum = checksum - cart.rom_data[i] - 1;
    }
    /* Does checksum make sense? */
    if(checksum & 0xFF) return true;
    else return false;
}

uint8_t cartOpen(char * filename) {
    FILE * p_file = NULL;
    p_file = fopen(filename, "rw+");
    
    if(p_file == NULL) {
        //printf("\tError opening ROM file\n");
        return 0;
    }
    else {
        //printf("Loading ROM into memory...\n");
        /* Load rom data into memory */
        fseek(p_file, 0, SEEK_END);
        cart.rom_size = ftell(p_file);
        rewind(p_file);
        
        cart.rom_data = malloc(cart.rom_size);
        //printf("Allocated %i bytes\n", cart.rom_size);
        fread(cart.rom_data, cart.rom_size, 1, p_file);
        fclose(p_file);

        /* Output some identifying info... */
        cartGetHeader();
        cartGetRomTitle();
        /* Check checksum */
        //(cartGetChecksum()) ? printf("\tPassed Checksum!\n") : printf("\tFailed Checksum!\n");
    }
    return 1;
}

/* Read from ROM loaded into RAM */
uint8_t cartReadAddr(uint16_t addr) {
    return (uint8_t)cart.rom_data[addr];
}

void cartWriteAddr(uint16_t addr, uint8_t val) {
//    printf("cartWriteAddr: 0x%04X\n", addr);
    cart.rom_data[addr] = val;
}