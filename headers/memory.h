#include "common.h"

#define KBYTE 1024

#define ROM_BANK00_END 0x3FFF
#define ROM_BANK01_END 0x7FFF
#define VRAM_END 0x9FFF
#define CART_RAM_END 0xBFFF
#define WRAM_END 0xDFFF
#define ECHO_RAM_END 0xFDFF
#define OAM_END 0xFE9F
#define NO_ACCESS_END 0xFEFF
#define IO_END 0xFF7F
#define HRAM_END 0xFFFE

typedef char MEMORY;

uint8_t memReadWRam(uint16_t addr);
void memWriteWRam(uint16_t addr, uint8_t val);
uint8_t memReadHRam(uint16_t addr);
void memWriteHRam(uint16_t addr, uint8_t val);
