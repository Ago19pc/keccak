#include "keccakf1600.h"
#include <stdio.h>
#include <immintrin.h>


#ifdef ALIGN
#undef ALIGN
#endif

#if defined(__GNUC__)
#define ALIGN(x) __attribute__((aligned(x)))
#elif defined(_MSC_VER)
#define ALIGN(x) __declspec(align(x))
#elif defined(__ARMCC_VERSION)
#define ALIGN(x) __align(x)
#else
#define ALIGN(x)
#endif


ALIGN(32) __m256i round_constants[24] = {
               {0x8000000080008008ull},    //round 23
               {0x0000000080000001ull},
               {0x8000000000008080ull},
               {0x8000000080008081ull},
               {0x800000008000000Aull},
               {0x000000000000800Aull},
               {0x8000000000000080ull},
               {0x8000000000008002ull},
               {0x8000000000008003ull},
               {0x8000000000008089ull},
               {0x800000000000008Bull},
               {0x000000008000808Bull},
               {0x000000008000000Aull},
               {0x0000000080008009ull},
               {0x0000000000000088ull},
               {0x000000000000008Aull},
               {0x8000000000008009ull},
               {0x8000000080008081ull},
               {0x0000000080000001ull},
               {0x000000000000808Bull},
               {0x8000000080008000ull},
               {0x800000000000808Aull},
               {0x0000000000008082ull},
               {0x0000000000000001ull},    //round 0
            };

      __m256i left_rotation_constant_a0;
      __m256i right_rotation_constant_a0;
      __m256i left_rotation_constant_a1;
      __m256i right_rotation_constant_a1;
      __m256i left_rotation_constant_a2;
      __m256i right_rotation_constant_a2;
      __m256i left_rotation_constant_a3;
      __m256i right_rotation_constant_a3;
      __m256i left_rotation_constant_a4;
      __m256i right_rotation_constant_a4;
      __m256i left_rotation_constant_c4;
      __m256i right_rotation_constant_c4;


void KeccakF1600_StateExtractBytes(keccak_state_t *state, unsigned char *data,
                                   unsigned int offset, unsigned int length)
{
    uint64_t  zero[25] = {0};
    uint64_t  *extractedData;
    if(length == 200 && !offset){ //chiamata dalla XOR
        extractedData = (uint64_t*)data;
    } else { //chiamata dalla Squeeze
        extractedData = zero;
    }
    //Estrazione righe (tranne ultima colonna)
    _mm256_storeu_si256((__m256i *)(extractedData), state->a0);
    _mm256_storeu_si256((__m256i *)(extractedData + 5), state->a1);
    _mm256_storeu_si256((__m256i *)(extractedData + 10), state->a2);
    _mm256_storeu_si256((__m256i *)(extractedData + 15), state->a3);
    _mm256_storeu_si256((__m256i *)(extractedData + 20), state->a4);
    //Estrazione ultima colonna.
    uint64_t *lastCol = (uint64_t *)&state->c4;
    for(int i = 0; i < 5; i++) {
        extractedData[4 + i*5] = lastCol[i];
    }
    if (extractedData == zero) //modifico data solo se sto venendo chiamato dalla squeeze (altrimenti è ridondante)
        memcpy(data, (uint8_t *)zero + offset, length);
}

void KeccakF1600_StateXORBytes(keccak_state_t *state, const unsigned char *data,
                               unsigned int offset, unsigned int length)
{
    uint64_t*     dataWords = (uint64_t *)data;
    uint64_t      stateWords[25];
    uint64_t*     firstStateWord = (uint64_t *)((uint8_t *)stateWords + offset);

    KeccakF1600_StateExtractBytes(state, (uint8_t *)stateWords, 0, 200);

    while (length > 32) {
        __m256i stateVec = _mm256_loadu_si256((__m256i *) firstStateWord);
        __m256i dataVec = _mm256_loadu_si256((__m256i *) dataWords);
        stateVec = _mm256_xor_si256(stateVec, dataVec);
        _mm256_storeu_si256((__m256i *) firstStateWord, stateVec);
        firstStateWord += 4;
        dataWords += 4;
        length -= 32;
    }
    while (length > 8) {
        *firstStateWord ^= *dataWords;
        firstStateWord++;
        dataWords++;
        length -= 8;
    }
    for (int i = 0; i < length; i++) {
      *((uint8_t *) firstStateWord + i) ^= *((uint8_t *) dataWords + i);
    }

    state->a0 = _mm256_loadu_si256((const __m256i *)(stateWords));
    state->a1 = _mm256_loadu_si256((const __m256i *)(stateWords + 5));
    state->a2 = _mm256_loadu_si256((const __m256i *)(stateWords + 10));
    state->a3 = _mm256_loadu_si256((const __m256i *)(stateWords + 15));
    state->a4 = _mm256_loadu_si256((const __m256i *)(stateWords + 20));
    state->c4 = _mm256_setr_epi64x(stateWords[4], stateWords[9], stateWords[14], stateWords[19]);
    state->a44 = _mm256_set1_epi64x(stateWords[24]);
}

void KeccakF1600_StatePermute(keccak_state_t *state)
{
    ptrdiff_t       round_i;
    __m256i a0, a1, a2, a3, a4, c4;
    __m256i a04, a14, a24, a34, a44;
    __m256i b0, b1, b2, b3, b4;
    __m256i b04, b14, b24, b34, b44;
    __m256i r0, r1, r2, r3;
    __m128i t0;
    a0 = _mm256_loadu_si256(&state->a0);
    a1 = _mm256_loadu_si256(&state->a1);
    a2 = _mm256_loadu_si256(&state->a2);
    a3 = _mm256_loadu_si256(&state->a3);
    a4 = _mm256_loadu_si256(&state->a4);
    c4 = _mm256_loadu_si256(&state->c4);
    a44 = _mm256_loadu_si256(&state->a44);
    for (round_i = 23; round_i >= 0; round_i--)
    {
        //theta
        r0 = _mm256_xor_si256(a0, a1);
        r1 = _mm256_permute2x128_si256(c4, c4, 0x11);
        r0 = _mm256_xor_si256(r0, a2);
        r1 = _mm256_xor_si256(c4, r1);
        r0 = _mm256_xor_si256(r0, a3);
        r2 = _mm256_unpackhi_epi64(r1, r1);
        r0 = _mm256_xor_si256(r0, a4);
        r1 = _mm256_xor_si256(r1, r2);
        b0 = _mm256_permute4x64_epi64(r0, 147);
        r1 = _mm256_xor_si256(r1, a44);
        b04 = b0;
        r2 = _mm256_slli_epi64(r0, 1);
        b0 = _mm256_blend_epi32(b0, r1, 3);
        r3 = _mm256_srli_epi64(r0, 63);
        b1 = _mm256_slli_epi64(r1, 1);
        r0 = _mm256_xor_si256(r2, r3);
        r2 = _mm256_srli_epi64(r1, 63);
        b04 = _mm256_xor_si256(b04, r0);
        r1 = _mm256_xor_si256(b1, r2);
        a44 = _mm256_xor_si256(a44, b04);
        r1 = _mm256_blend_epi32(r0, r1, 3);
        t0 = _mm256_castsi256_si128(b04);
        r1 = _mm256_permute4x64_epi64(r1, 57);
        r2 = _mm256_broadcastq_epi64(t0);
        b0 = _mm256_xor_si256(b0, r1);
        c4 = _mm256_xor_si256(c4, r2);
        a0 = _mm256_xor_si256(a0, b0);
        a1 = _mm256_xor_si256(a1, b0);
        a2 = _mm256_xor_si256(a2, b0);
        a3 = _mm256_xor_si256(a3, b0);
        a4 = _mm256_xor_si256(a4, b0);
        //rho
        r0 = _mm256_sllv_epi64(a0, left_rotation_constant_a0);
        r1 = _mm256_srlv_epi64(a0, right_rotation_constant_a0);
        r2 = _mm256_sllv_epi64(a1, left_rotation_constant_a1);
        r3 = _mm256_srlv_epi64(a1, right_rotation_constant_a1);
        b0 = _mm256_xor_si256(r0, r1);
        r0 = _mm256_sllv_epi64(a2, left_rotation_constant_a2);
        r1 = _mm256_srlv_epi64(a2, right_rotation_constant_a2);
        b1 = _mm256_xor_si256(r2, r3);
        r2 = _mm256_sllv_epi64(a3, left_rotation_constant_a3);
        r3 = _mm256_srlv_epi64(a3, right_rotation_constant_a3);
        b2 = _mm256_xor_si256(r0, r1);
        r0 = _mm256_sllv_epi64(a4, left_rotation_constant_a4);
        r1 = _mm256_srlv_epi64(a4, right_rotation_constant_a4);
        b3 = _mm256_xor_si256(r2, r3);
        r2 = _mm256_sllv_epi64(c4, left_rotation_constant_c4);
        r3 = _mm256_srlv_epi64(c4, right_rotation_constant_c4);
        b4 = _mm256_xor_si256(r0, r1);
        r0 = _mm256_slli_epi64(a44, 14);
        r1 = _mm256_srli_epi64(a44, 50);
        c4 = _mm256_xor_si256(r2, r3);
        a44 = _mm256_xor_si256(r0,r1);
        //pi. Traspone anche lo stato per velocizzare chi
        t0 = _mm256_castsi256_si128(c4);
        r0 = _mm256_permute4x64_epi64(b0, 28);
        r1 = _mm256_broadcastq_epi64(t0);
        b04 = _mm256_permute2x128_si256(b0, b0, 0x11);
        b0 = _mm256_blend_epi32(r0, r1, 192);
        r0 = _mm256_permute4x64_epi64(b1, 45);
        r1 = _mm256_unpackhi_epi64(c4, c4);
        b14 = _mm256_permute4x64_epi64(b1, 255);
        b1 = _mm256_blend_epi32(r0, r1, 12);
        b2 = _mm256_permute4x64_epi64(b2, 114);
        b24 = _mm256_permute2x128_si256(c4, c4, 0x11);
        r0 = _mm256_permute4x64_epi64(b3, 135);
        r1 = _mm256_permute4x64_epi64(c4, 255);
        b34 = b3;
        b3 = _mm256_blend_epi32(r0, r1, 48);
        t0 = _mm256_castsi256_si128(a44);
        r0 = _mm256_permute4x64_epi64(b4, 201);
        r1 = _mm256_broadcastq_epi64(t0);
        b44 = _mm256_unpackhi_epi64(b4, b4);
        b4 = _mm256_blend_epi32(r0, r1, 3);
        //chi
        r0 = _mm256_andnot_si256(b1, b2);
        r1 = _mm256_andnot_si256(b2, b3);
        r2 = _mm256_andnot_si256(b3, b4);
        a0 = _mm256_xor_si256(b0, r0);
        a0 = _mm256_xor_si256(a0, *(__m256i *)(round_constants + round_i)); //iota
        a1 = _mm256_xor_si256(b1, r1);
        a2 = _mm256_xor_si256(b2, r2);
        r0 = _mm256_andnot_si256(b4, b0);
        r1 = _mm256_andnot_si256(b0, b1);
        a3 = _mm256_xor_si256(b3, r0);
        c4 = _mm256_xor_si256(b4, r1);
        //ritrasposizione dello stato dopo chi (e chi per l'ultima colonna)
        r0 = _mm256_unpacklo_epi64(a0, a1);
        r1 = _mm256_unpackhi_epi64(a0, a1);
        r2 = _mm256_unpacklo_epi64(a2, a3);
        r3 = _mm256_unpackhi_epi64(a2, a3);
        a0 = _mm256_permute2x128_si256(r0, r2, 0x20);
        a1 = _mm256_permute2x128_si256(r1, r3, 0x20);
        a2 = _mm256_permute2x128_si256(r2, r0, 0x13);
        a3 = _mm256_permute2x128_si256(r3, r1, 0x13);
        r0 = _mm256_andnot_si256(b14, b24);
        r1 = _mm256_andnot_si256(b24, b34);
        r2 = _mm256_andnot_si256(b34, b44);
        a04 = _mm256_xor_si256(b04, r0);
        a14 = _mm256_xor_si256(b14, r1);
        a24 = _mm256_xor_si256(b24, r2);
        r0 = _mm256_andnot_si256(b44, b04);
        r1 = _mm256_andnot_si256(b04, b14);
        a34 = _mm256_xor_si256(b34, r0);
        a44 = _mm256_xor_si256(b44, r1);
        r0 = _mm256_unpacklo_epi64(a04, a14);
        r1 = _mm256_unpacklo_epi64(a24, a34);
        a4 = _mm256_permute2x128_si256(r0, r1, 0x20);
    }
    _mm256_storeu_si256(&state->a0, a0);
    _mm256_storeu_si256(&state->a1, a1);
    _mm256_storeu_si256(&state->a2, a2);
    _mm256_storeu_si256(&state->a3, a3);
    _mm256_storeu_si256(&state->a4, a4);
    _mm256_storeu_si256(&state->c4, c4);
    _mm256_storeu_si256(&state->a44, a44);
}


