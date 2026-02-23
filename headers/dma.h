#include "common.h"

typedef struct {
    bool active;
    uint8_t val;
    uint8_t byte;
    uint8_t delay;
} DMA;

void dmaWrite(uint8_t val);
void dmaTick(void);