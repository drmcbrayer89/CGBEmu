#include "io.h"
#include "serial.h"
#include "bus.h"
#include "cpu.h"
#include "timer.h"
#include "lcd.h"

void ioWrite(uint16_t addr, uint8_t val) {
    //printf("0x%04X 0x%02X\n", addr, val);
    switch(addr) {
        case 0xFF01:
            //printf("WRITE: 0xFF01\n");
            serialWrite(0, val);      
            return;
        case 0xFF02:
            //printf("WRITE: 0xFF02\n");
            serialWrite(1, val);
            return;
        case 0xFF04:
        case 0xFF05:
        case 0XFF06:
        case 0xFF07:
            timerWrite(addr, val);
        case 0xFF0F:
            cpuSetIntFlags(val);
            return;
        case 0xFF40 ... 0xFF45:
            lcdWrite(addr, val);
            return;
        default:
            //printf("incorrect addr\n");
            return;
    }
}

uint8_t ioRead(uint16_t addr) {
    switch(addr) {
        case 0xFF01:
            //printf("READ 0xFF01\n");
            return serialRead(0);
        case 0xFF02:
            //printf("READ 0xFF02\n");
            return serialRead(1);
        case 0xFF04:
        case 0xFF05:
        case 0XFF06:
        case 0xFF07:
            return timerRead(addr);
        case 0xFF0F:
            return cpuGetIntFlags();
        case 0xFF40 ... 0xFF45:
            return lcdRead(addr);
        default:
            //printf("Incorrect addr\n");
            return 0;
    }
}