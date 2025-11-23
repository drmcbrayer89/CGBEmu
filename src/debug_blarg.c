#include "debug_blarg.h"
#include "bus.h"

static byte msg[1024] = {0};
static int32_t msg_size = 0;

void debugUpdate(void) {
    /* 0xFF02 -- SC Serial Transfer Control 
       BIT 7: 1 = transfer enable
       BIT 0: 1 = internal clock 
       
       this is where 0x81 comes from 
    */
    if(busReadAddr(0xFF02) == 0x81) {
        char c = busReadAddr(0xFF01);
        msg[msg_size++] = c;
        busWriteAddr(0xFF02, 0);
    }
}
void debugShow(void) {
    if(msg[0]) {
        printf("Debug: %s\n", msg);
    }
}