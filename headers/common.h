#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef char byte;
#define BIT_CLEAR(a,n) (a &= ~(1<< n))
// stole this one :P
#define BIT_SET(a, n, on) { if(on) a |= (1 << n); else a &= ~(1 << n);}
#define BIT_CHECK(a,n) ((a & (1 << n)) ? 1 : 0)
