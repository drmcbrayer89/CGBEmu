#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define BIT_CLEAR(a,n) (a &= ~(1<< n))
// stole this one :P
#define BIT_SET(a, n, on) { if(on) a |= (1 << n); else a &= ~(1 << n);}
#define BIT_CHECK(a,n) ((a & (1 << n)) ? 1 : 0)
#define BYTE_SWAP(n) ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);

#define CLEAR 0
#define SET   1
#define STAY  -1

typedef char byte;

