#include "cart.h"
#include <stdlib.h>

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

static void cartSetDestCode(FILE * p_file) {
    fseek(p_file, 0x14A, SEEK_SET);
    fread(&cart.header.dest_code, 1, 1, p_file);
}

static void cartSetVersion(FILE * p_file) {
    fseek(p_file, 0x14C, SEEK_SET);
    fread(&cart.header.version_num, 1, 1, p_file);
}

static void cartSetOldLicCode(FILE * p_file) {
    uint8_t i = 0;
    fseek(p_file, 0x14B, SEEK_SET);
    fread(&cart.header.old_lic_code, 1, 1, p_file);
    for(i = 0; i < 2; i++) {
        if(cart.header.old_lic_code == (uint8_t)cart_old_lic_table[i].type) {
            printf("\t\tOld Lic Code: %i (%s)\n", cart.header.old_lic_code, cart_old_lic_table[i].label);
        }
    }
}

static void cartSetNewLicCode(FILE * p_file) {
    fseek(p_file, 0x144, SEEK_SET);
    fread(&cart.header.new_lic_code, 1, 1, p_file);
    printf("\t\tNew Lic Code: %i\n", cart.header.new_lic_code);
}

static void cartSetCgbFlag(FILE * p_file) {
    fseek(p_file, 0x143, SEEK_SET);
    fread(&cart.header.title.title_mfr_cgb.cgb_flag, 1, 1, p_file);
    printf("\t\tCGB Flag: %i\n", cart.header.title.title_mfr_cgb.cgb_flag);
}
static void cartSetMfrCode(FILE * p_file) {
    fseek(p_file, 0x13F, SEEK_SET);
    fread(&cart.header.title.title_mfr_cgb.mfr_code, 1, 4, p_file);
    printf("\t\tManufacturer Code: %s\n", cart.header.title.title_mfr_cgb.mfr_code);
}

static void cartSetRamSize(FILE * p_file) {
    fseek(p_file, 0x149, SEEK_SET);
    fread(&cart.header.ram_size, 1, 1, p_file);
    printf("\t\tRAM size: %i KiB\n", cart.header.ram_size);
}

static void cartSetCartType(FILE * p_file) {
    uint8_t i;
    fseek(p_file, 0x147, SEEK_SET);
    fread(&cart.header.cart_type, 1, 1, p_file);
    for(i = 0; i < NUM_CART_TYPES - 1; i++) {
        if(cart.header.cart_type == (uint8_t)cart_type_table[i].type){
            printf("\t\tCartridge type: %s\n", cart_type_table[i].label);
        }
    }
}

static void cartSetRomSize(FILE * p_file) {
    fseek(p_file, 0x148, SEEK_SET);
    fread(&cart.header.rom_size, 1, 1, p_file);
    cart.rom_size = CART_DEFAULT_SIZE * (1 << cart.header.rom_size);
    printf("\t\tROM size: 0x%4X (%u KiB)\n", cart.rom_size, (cart.rom_size/1024));
}

static void cartSetRomTitle(FILE * p_file) {
    fseek(p_file, 0x134, SEEK_SET);
    fread(&cart.header.title.title, 1, 16, p_file);

    printf("\t\tTITLE: %s\n", cart.header.title.title);
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

    printf("\tAttempting to open %s\n", filename);
    p_file = fopen(filename, "rw+");
    
    if(p_file == NULL) {
        printf("\tError opening ROM file\n");
        return 0;
    }
    else {
        printf("\tParsing Cartridge Header...\n");
        cartSetRomTitle(p_file);
        cartSetRomSize(p_file);
        cartSetCartType(p_file);
        cartSetRamSize(p_file);
        cartSetMfrCode(p_file);
        cartSetNewLicCode(p_file);
        cartSetOldLicCode(p_file);
        cartSetCgbFlag(p_file);
        cartSetVersion(p_file);
        cartSetDestCode(p_file);

        /* Load rom data into memory */
        rewind(p_file);
        cart.rom_data = malloc(cart.rom_size);
        fread(cart.rom_data, cart.rom_size, 1, p_file);
        fclose(p_file);

        /* Check checksum */
        (cartGetChecksum()) ? printf("\tPassed Checksum!\n") : printf("\tFailed Checksum!\n");
    }

    return 1;
}

/* Read from ROM loaded into RAM */
uint8_t cartReadAddr(uint16_t addr) {
    return cart.rom_data[addr];
}