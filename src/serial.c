#include "serial.h"

static byte data[2];

void serialWrite(uint16_t index, uint8_t val) {
    data[index] = val;
}

uint8_t serialRead(uint16_t index) {
    return data[index];
}