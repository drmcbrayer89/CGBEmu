#include "timer.h"
#include "cpu.h"

static TIMER timer = {0};

void timerInit(void) {
    // This is apparently just common practice.
    timer.div = 0xAC00;
}

void timerTick(void) {
    uint8_t timer_bit = 0;
    uint16_t div_prev = timer.div;
    timer.div++;

    // if second bit is true, increment tima enabled
    if(timer.tac & (1 << 2)) {
        // tac bottom 2 bits decide the frequency
        // GB timer is 4194304, divide by 4096/262144/65536/16384 to get bit 9, 3, 5, 7
        switch(timer.tac & 0b11) {
            case 0b00: // 256 M-cycles, 4096hz (1024 cycles) -> bit 9
                timer_bit = 9;    
                break;
            case 0b01: // 4 M-cycles, 262144hz
                timer_bit = 3;
                break;
            case 0b10: // 16 M-cycles, 65536hz
                timer_bit = 5;
                break;
            case 0b11: // 64 M-cycles, 16384hz
                timer_bit = 7;
                break;
            default:
                return;
        }
        // Detect if a falling edge occured 
        bool update = (div_prev & (1 << timer_bit)) & (!(timer.div & (1 << timer_bit))); 

        if(update) {
            // look for overflow before incrementing TIMA
            if(timer.tima == 0xFF) {
                timer.tima = timer.tma;
                cpuSetIntFlags(INT_TIMER);
            } 
            else {
                timer.tima++;
            }
        }
    }
}

void timerWrite(uint16_t addr, uint8_t val) {
    switch(addr) {
        case 0xFF04: // div is always cleared if being written to.
            timer.div = 0x00;
            return;
        case 0xFF05:
            timer.tima = val;
            return;
        case 0xFF06:
            timer.tma = val;
            return;
        case 0xFF07:
            timer.tac = val;
            return;
        default:
            printf("Incorrect timer address\n");
            return;
    }
}

uint16_t timerRead(uint16_t addr) {
    switch(addr) {
        case 0xFF04: return timer.div >> 8;
        case 0xFF05: return timer.tima;
        case 0xFF06: return timer.tma;
        case 0xFF07: return timer.tac;
        default: return 0;
    }
}