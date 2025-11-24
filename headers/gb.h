#include "common.h"
#define CYCLES_PER_BYTE 4

typedef struct {
    bool halted;
    bool running;
    bool stop_emu;
    uint64_t ticks;
} GB;

void gbTick(uint8_t cycles);
void gbInit(void);
void gbStart(void);
void gbGetGbPtr(GB * gb);
void gbStopEmu(void);