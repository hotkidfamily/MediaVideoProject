/* 7.8 Format conversion of integer types <inttypes.h> */
#include "stdint.h"

#ifndef INT64_C 
#define INT64_C(c) (c ## LL) 
#define UINT64_C(c) (c ## ULL) 
#endif