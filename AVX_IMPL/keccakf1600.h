#pragma once

#include <stdint.h>
#include <immintrin.h>

typedef struct keccak_state_t
{
    __m256i a0, a1, a2, a3, a4; //righe (solo i primi 4 elementi per riga): a0=S[0, 0...3], a1=S[1, 0...3], a2=S[2, 0...3], a3=S[3, 0...3], a4=S[4, 0...3]
    __m256i c4;                 //ultima colonna (solo i primi 4 elementi) = S[0...4, 4]
    __m256i a44;                //ultimo elemento = S[4,4]
} keccak_state_t;

void KeccakF1600_StateExtractBytes(keccak_state_t *state, unsigned char *data,
                                   unsigned int offset, unsigned int length);
void KeccakF1600_StateXORBytes(keccak_state_t *state, const unsigned char *data,
                               unsigned int offset, unsigned int length);
void KeccakF1600_StatePermute(keccak_state_t *state);

void KeccakF1600_StatePermute_Light(keccak_state_t *state, __m256i *a0, __m256i *a1, __m256i *a2, __m256i *a3, __m256i *a4, __m256i *c4, __m256i *a44);