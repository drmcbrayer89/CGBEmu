#include "io.h"
#include "serial.h"
#include "bus.h"

void ioWrite(uint16_t addr, uint8_t val) {
    switch(addr) {
        case 0xFF01:
            printf("hi\n");
            serialWrite(0, val);      
            return;
        case 0xFF02:
            serialWrite(1, val);
            return;
        case 0xFF0F:
            return;
        default:
            printf("incorrect addr\n");
            return;
    }
}

uint8_t ioRead(uint16_t addr) {
    switch(addr) {
        case 0xFF01:
            return serialRead(0);
        case 0xFF02:
            return serialRead(1);
        case 0xFF0F:
            return 1;
        default:
            printf("Incorrect addr\n");
            return 0;
    }
}