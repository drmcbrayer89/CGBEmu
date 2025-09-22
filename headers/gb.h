#include "common.h"
#define CYCLES_PER_BYTE 4

typedef struct {
    bool halted;
    bool running;
    uint64_t ticks;
} GB;

void gbTick(void);
void gbInit(void);
void gbStart(void);