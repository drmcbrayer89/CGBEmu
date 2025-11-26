#include "common.h"

typedef struct {
    uint16_t div;
    uint8_t tima;
    uint8_t tma;
    uint8_t tac;
} TIMER;

void timerInit(void);
void timerWrite(uint16_t addr, uint8_t val);
uint16_t timerRead(uint16_t addr);