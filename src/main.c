#include "cart.h"
#include "gb.h"

int32_t main(int32_t argc, char * argv[])
{
    if(argc > 1){
        printf("Starting CGBEmu!\n");
        cartOpen(argv[1]);
    }
    gbInit();
    gbStart();
    return 0;
}