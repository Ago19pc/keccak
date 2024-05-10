#pragma once

#include "../Utils/intrinUtils.h"
#include <stdint.h>
#include <stdio.h>
#include <immintrin.h>


void KeccakF1600_StateExtractBytes(uint64_t *state, unsigned char *data,
                                   unsigned int offset, unsigned int length)
{
    

   unsigned int i;
   for(i=0; i<length; i++) {
      data[i] = state[(offset + i) >> 3] >> (8*((offset + i) & 0x07));
   }
}

/*
offset = 0
data[0] = state[0] >> 0
data[8] = state[1] >> 0
data[16] = state[2] >> 0
data[24] = state[3] >> 0 
data[32] = state[4] >> 0
data[40] = state[5] >> 0
data[48] = state[6] >> 0
data[56] = state[7] >> 0

data[1] = state[0] >> 8
data[9] = state[1] >> 8
data[17] = state[2] >> 8
data[25] = state[3] >> 8

data[2] = state[0] >> 16
data[10] = state[1] >> 16
data[18] = state[2] >> 16
data[26] = state[3] >> 16

etc etc

offset = 1
data[0] = state[0] >> 1
data[8] = state[1] >> 1
data[16] = state[2] >> 1
data[24] = state[3] >> 1

data[1] = state[0] >> 9
data[9] = state[1] >> 9
data[17] = state[2] >> 9
data[25] = state[3] >> 9

offset = 8
data[0] = state[1] >> 0
data[8] = state[2] >> 0
data[16] = state[3] >> 0
data[24] = state[4] >> 0

data[1] = state[1] >> 8
data[9] = state[2] >> 8
data[17] = state[3] >> 8
data[25] = state[4] >> 8

etc etc
*/