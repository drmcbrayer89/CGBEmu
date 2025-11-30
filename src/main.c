#include "cart.h"
#include "gb.h"

int32_t main(int32_t argc, char * argv[])
{
    gbInit();
    if(argc > 1){
        printf("Starting CGBEmu!\n");
        cartOpen(argv[1]);
        gbStart();
    }
    /* Remove this after done... */
    else {
        if(cartOpen("01-special.gb")){
            gbStart();
        }
    }

    return 0;
}