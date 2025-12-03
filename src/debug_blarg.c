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
        printf("debugUpdate\n");
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

void debugGbDocOut(CPU * p_cpu) {
    const char * out_msg = "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X PC:%04X PCMEM:%02X,%02X,%02X,%02X\n";
    uint16_t pc = cpuReadReg(R_PC);
    pc--;
    // Gameboy doctor logging
    printf(out_msg, p_cpu->regs.a, p_cpu->regs.f, p_cpu->regs.b, 
            p_cpu->regs.c, p_cpu->regs.d, p_cpu->regs.e, 
            p_cpu->regs.h, p_cpu->regs.l, p_cpu->regs.sp, 
            pc, busReadAddr(pc), busReadAddr(pc+1), busReadAddr(pc+2), busReadAddr(pc+3));
        
}
