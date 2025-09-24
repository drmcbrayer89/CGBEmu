#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define BIT_CLEAR(a,n) (a &= ~(1<< n))
#define BIT_SET(a, n) (a |= (1 << n))
#define BIT_CHECK(a,n) ((a & (1 << n)) ? 1 : 0)
