#include <stdint.h>
#include <stdio.h>

/* 32KiB = 0x8000*/
#define CART_DEFAULT_SIZE 0x8000

/* Cart type defines*/
#define NUM_CART_TYPES                  28
#define ROM_ONLY                        0x00
#define MBC1                            0x01
#define MBC1_RAM                        0x02
#define MBC1_RAM_BATTERY                0x03
#define MBC2                            0x05
#define MBC2_BATTERY                    0x06
#define ROM_RAM                         0x08
#define ROM_RAM_BATTERY                 0x09
#define MMM01                           0x0B
#define MMM01_RAM                       0x0C
#define MMM01_RAM_BATTERY               0x0D
#define MBC3_TIMER_BATTERY              0x0F
#define MBC3_TIMER_RAM_BATTERY          0x10
#define MBC3                            0x11
#define MBC3_RAM                        0x12
#define MBC3_RAM_BATTERY                0x13
#define MBC5                            0x19
#define MBC5_RAM                        0x1A
#define MBC5_RAM_BATTERY                0x1B
#define MBC5_RUMBLE                     0x1C
#define MBC5_RUMBLE_RAM                 0x1D
#define MBC5_RUMBLE_RAM_BATTERY         0x1E
#define MBC6                            0x20 
#define MBC7_SENSOR_RUMBLE_RAM_BATTERY  0x22
#define POCKET_CAMERA                   0xFC
#define BANDAI_TAMA5                    0xFD
#define HuC3                            0xFE
#define HuC1_RAM_BATTERY                0xFF

/* Cartridge memory locations */
#define CART_TITLE          0x134
#define CART_TITLE_BYTES    16
#define CART_ROM_SIZE       0x148


typedef struct {
    uint32_t type;
    char * label;
} ENUM_MAP;

typedef struct {
    union {
        uint8_t title[16];

        struct {
            uint8_t title[15];
            uint8_t cgb_flag;
        } title_cgb;

        struct {
            uint8_t title[11];
            uint8_t mfr_code[4];
            uint8_t cgb_flag;
        } title_mfr_cgb;
    } title;

    uint16_t new_lic_code;
    uint8_t sgb_flag;
    uint8_t cart_type;
    uint8_t rom_size;
    uint8_t ram_size;
    uint8_t dest_code;
    uint8_t old_lic_code;
    uint8_t version_num;
    uint8_t header_checksum;
    uint16_t global_checksum;
} CART_HEADER;

typedef struct {
    CART_HEADER header;
    uint32_t rom_size;
    uint8_t * rom_data;
} CARTRIDGE;

/* Public functions */
CARTRIDGE * cartGetCartridge(void);
uint8_t cartOpen(char * filename);
uint8_t cartReadAddr(uint16_t addr);