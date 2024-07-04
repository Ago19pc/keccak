#pragma once

#include <stdint.h>
#include <arm_neon.h>

typedef struct keccak_state_t
{
    uint64x2_t a04, a15, a26, a37, a812, a913, a1014, a1115, a1620, a1721, a1822, a1923; //2 elementi alla volta
    uint64_t a24; //ultima parola
} keccak_state_t;

void KeccakF1600_StateExtractBytes(keccak_state_t *state, unsigned char *data,
                                   unsigned int offset, unsigned int length);
void KeccakF1600_StateXORBytes(keccak_state_t *state, const unsigned char *data,
                               unsigned int offset, unsigned int length);
void keccakF1600_StatePermute(keccak_state_t* state);