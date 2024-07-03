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
    // al contrario perché l'assembly inizia dall'ultima keccak_rc e decrementa il contatore delle rc, quindi la prima deve essere keccak_rc[23] e l'ultima keccak_rc[0]
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
    volatile __m256i left_rotation_constant_a0 = _mm256_setr_epi64x( 0,  1, 62, 28);
    volatile __m256i right_rotation_constant_a0 = _mm256_setr_epi64x(64, 63, 2, 36);
    volatile __m256i left_rotation_constant_a1 = _mm256_setr_epi64x(36, 44,  6, 55);
    volatile __m256i right_rotation_constant_a1 = _mm256_setr_epi64x(28, 20, 58, 9);
    volatile __m256i left_rotation_constant_a2 = _mm256_setr_epi64x( 3, 10, 43, 25);
    volatile __m256i right_rotation_constant_a2 = _mm256_setr_epi64x(61, 54, 21, 39);
    volatile __m256i left_rotation_constant_a3 = _mm256_setr_epi64x(41, 45, 15, 21);
    volatile __m256i right_rotation_constant_a3 = _mm256_setr_epi64x(23, 19, 49, 43);
    volatile __m256i left_rotation_constant_a4 = _mm256_setr_epi64x(18, 2, 61, 56);
    volatile __m256i right_rotation_constant_a4 = _mm256_setr_epi64x(46, 62, 3, 8);
    volatile __m256i left_rotation_constant_c4 = _mm256_setr_epi64x(27, 20, 39,  8);
    volatile __m256i right_rotation_constant_c4 = _mm256_setr_epi64x(37, 44, 25, 56);
    __m256i a0, a1, a2, a3, a4, c4, a44;
    a0 = _mm256_loadu_si256(&state->a0);
    a1 = _mm256_loadu_si256(&state->a1);
    a2 = _mm256_loadu_si256(&state->a2);
    a3 = _mm256_loadu_si256(&state->a3);
    a4 = _mm256_loadu_si256(&state->a4);
    c4 = _mm256_loadu_si256(&state->c4);
    a44 = _mm256_loadu_si256(&state->a44);
    __asm volatile(
        "movq           %7, %%rax\n"                    //carica il numero di round
    "1:\n"                                              //inizio nuovo round
        //theta
        "vpxor          %1, %0, %%ymm9\n"               //ymm9 = a0^a1
        "vpxor          %2, %%ymm9, %%ymm9\n"           //ymm9 ^= a2
        "vpxor          %3, %%ymm9, %%ymm9\n"           //ymm9 ^= a3
        "vpxor          %4, %%ymm9, %%ymm9\n"           //ymm9 ^= a4
        "vpermq         $147, %%ymm9, %%ymm8\n"         //ymm8 = perm(ymm9, 147)
        "vpsrlq         $63, %%ymm9, %%ymm7\n"          //ymm7 = ymm9 >> 63
        "vpsllq         $1, %%ymm9, %%ymm9\n"           //ymm9 = ymm9 << 1
        "vperm2i128     $17, %5, %5, %%ymm0\n"          //ymm0 = perm(c4, c4, 17)
        "vpxor          %%ymm0, %5, %%ymm0\n"           //ymm0 ^= c4
        "vpunpckhqdq    %%ymm0, %%ymm0, %%ymm6\n"       //ymm6 = ymm0H ymm0H
        "vpxor          %%ymm6, %%ymm0, %%ymm6\n"       //ymm6 ^= ymm0
        "vpxor          %6, %%ymm6, %%ymm6\n"           //ymm6 ^= a44
        "vpxor          %%ymm7, %%ymm9, %%ymm7\n"       //ymm7 ^= ymm9
        "vpblendd       $3, %%ymm6, %%ymm8, %%ymm0\n"   //ymm0 = blend(ymm6, ymm8, 3)
        "vpsrlq         $63, %%ymm6, %%ymm9\n"          //ymm9 = ymm6 >> 63
        "vpsllq         $1, %%ymm6, %%ymm6\n"           //ymm6 = ymm6 << 1
        "vpxor          %%ymm9, %%ymm6, %%ymm6\n"       //ymm6 ^= ymm9
        "vpblendd       $3, %%ymm6, %%ymm7, %%ymm6\n"   //ymm6 = blend(ymm6, ymm7, 3)
        "vpxor          %%ymm7, %%ymm8, %%ymm7\n"       //ymm7 ^= ymm8
        "vpxor          %%ymm7, %6, %%ymm9\n"           //ymm9 = ymm7 ^ a44
        "vpermq         $57, %%ymm6, %%ymm6\n"
        "vpxor          %%ymm6, %%ymm0, %%ymm0\n"
        "vpxor          %%ymm0, %0, %0\n"
        "vpxor          %%ymm0, %1, %1\n"
        "vpbroadcastq   %%xmm7, %%ymm7\n"
        "vpxor          %%ymm7, %5, %5\n"
        "vpxor          %%ymm0, %2, %2\n"
        "vpxor          %%ymm0, %3, %3\n"
        "vpxor          %%ymm0, %4, %4\n"
        //ro + pi
        "vmovdqa        %[right_rotation_constant_a0], %%ymm6\n" //ro a0
        "vmovdqa        %[left_rotation_constant_a0], %6\n" //ro a0
        "vmovdqa        %[right_rotation_constant_a1], %%ymm7\n" //ro a1
        "vpsrlvq        %%ymm6, %0, %%ymm6\n" // ro a0
        "vpsllvq        %6, %0, %0\n" //ro a0
        "vmovdqa        %[left_rotation_constant_a1], %6\n" //ro a1
        "vmovdqa        %[right_rotation_constant_a2], %%ymm0\n" //ro a2
        "vpsrlvq        %%ymm7, %1, %%ymm7\n" // ro a1
        "vpsllvq        %6, %1, %1\n" //ro a1
        "vmovdqa        %[left_rotation_constant_a2], %6\n" // ro a2
        "vmovdqa        %[right_rotation_constant_a3], %%ymm10\n" //ro a3
        "vpsrlvq        %%ymm0, %2, %%ymm0\n" // ro a2
        "vpsllvq        %6, %2, %2\n" //ro a2
        "vpxor          %%ymm7, %1, %%ymm7\n" //ro a1
        "vpxor          %%ymm6, %0, %%ymm6\n" //ro a0
        "vpermq         $28, %%ymm6, %1\n" // pi
        "vperm2i128     $17, %%ymm6, %%ymm6, %%ymm6\n" //pi
        "vpxor          %%ymm0, %2, %2\n" // ro a2
        "vpsrlvq        %%ymm10, %3, %%ymm0\n" //ro a3
        "vpermq         $114, %2, %2\n" // pi
        "vmovdqa        %[left_rotation_constant_a3], %%ymm10\n" //ro a3
        "vpsllvq        %%ymm10, %3, %3\n" //ro a3
        "vpxor          %%ymm0, %3, %%ymm10\n" // ro a3
        "vpermq         $135, %%ymm10, %3\n" // pi
        "vmovdqa        %[right_rotation_constant_a4], %%ymm0\n" //ro a4
        "vpsrlvq        %%ymm0, %4, %6\n" //ro a4
        "vmovdqa        %[left_rotation_constant_a4], %%ymm0\n" //ro a4
        "vpsllvq        %%ymm0, %4, %4\n" // ro a4
        "vpxor          %6, %4, %%ymm0\n" //ro a4
        "vpermq         $201, %%ymm0, %0\n" // pi
        "vpunpckhqdq    %%ymm0, %%ymm0, %%ymm0\n" //pi
        "vmovdqa        %[right_rotation_constant_c4], %4\n" // ro c4
        "vpsrlvq        %4, %5, %6\n" //ro c4
        "vmovdqa        %[left_rotation_constant_c4], %4\n" //ro c4
        "vpsllvq        %4, %5, %5\n" //ro c4
        "vpxor          %6, %5, %4\n" //ro c4
        "vpsrlq         $50, %%ymm9, %5\n" //ro a44
        "vpsllq         $14, %%ymm9, %6\n" //ro a44
        "vperm2i128     $17, %4, %4, %%ymm8\n" //pi
        "vpxor          %5, %6, %%ymm9\n" //ro a44
        "vmovdqa        %x4, %x6\n" // pi
        "vpunpckhqdq    %4, %4, %5\n" // pi
        "vpbroadcastq   %%xmm9, %%ymm9\n" // pi
        "vpbroadcastq   %x6, %6\n" // pi
        "vpermq         $255, %4, %4\n" // pi
        "vpblendd       $48, %4, %3, %4\n" // pi
        "vpblendd       $3, %%ymm9, %0, %3\n" // pi
        "vpblendd       $192, %6, %1, %1\n" // pi
        "vpermq         $45, %%ymm7, %6\n" // pi
        "vpblendd       $12, %5, %6, %5\n" // pi
        "vpermq         $255, %%ymm7, %%ymm7\n" // pi
        //chi + iota
        "vpandn         %2, %5, %%ymm9\n"
        "subq           $32, %%rax\n"
        "vpxor          %1, %%ymm9, %%ymm9\n"
        "vpandn         %4, %2, %6\n"
        "vpandn         %3, %4, %0\n"
        "vpxor          (%%rdx, %%rax), %%ymm9, %%ymm9\n"
        "vpxor          %0, %2, %0\n"
        "vpxor          %5, %6, %6\n"
        "vpandn         %1, %3, %2\n"
        "vpandn         %5, %1, %5\n"
        "vpxor          %4, %2, %4\n"
        "vpxor          %3, %5, %5\n"
        "vpunpcklqdq    %6, %%ymm9, %2\n"
        "vpunpckhqdq    %6, %%ymm9, %6\n"
        "vpunpcklqdq    %4, %0, %%ymm9\n"
        "vpunpckhqdq    %4, %0, %4\n"
        "vperm2i128     $32, %%ymm9, %2, %0\n"
        "vperm2i128     $32, %4, %6, %1\n"
        "vperm2i128     $19, %6, %4, %3\n"
        "vperm2i128     $19, %2, %%ymm9, %2\n"
        "vpandn         %%ymm10, %%ymm8, %4\n"
        "vpandn         %%ymm0, %%ymm10, %6\n"
        "vpandn         %%ymm8, %%ymm7, %%ymm9\n"
        "vpxor          %4, %%ymm7, %4\n"
        "vpxor          %6, %%ymm8, %%ymm8\n"
        "vpxor          %%ymm9, %%ymm6, %%ymm9\n"
        "vpandn         %%ymm6, %%ymm0, %6\n"
        "vpxor          %6, %%ymm10, %%ymm10\n"
        "vpandn         %%ymm7, %%ymm6, %6\n"
        "vpunpcklqdq    %4, %%ymm9, %%ymm9\n"
        "vpunpcklqdq    %%ymm10, %%ymm8, %%ymm8\n"
        "vperm2i128     $32, %%ymm8, %%ymm9, %4\n"
        "vpxor          %6, %%ymm0, %6\n"
        "jnz            1b\n"
        : "+x"(a0), "+x"(a1), "+x"(a2), "+x"(a3), "+x"(a4), "+x"(c4), "+x"(a44)
        : "i"(768), "d"(round_constants),
            [left_rotation_constant_a0] "m"(left_rotation_constant_a0), [right_rotation_constant_a0] "m"(right_rotation_constant_a0),
            [left_rotation_constant_a1] "m"(left_rotation_constant_a1), [right_rotation_constant_a1] "m"(right_rotation_constant_a1),
            [left_rotation_constant_a2] "m"(left_rotation_constant_a2), [right_rotation_constant_a2] "m"(right_rotation_constant_a2),
            [left_rotation_constant_a3] "m"(left_rotation_constant_a3), [right_rotation_constant_a3] "m"(right_rotation_constant_a3),
            [left_rotation_constant_a4] "m"(left_rotation_constant_a4), [right_rotation_constant_a4] "m"(right_rotation_constant_a4),
            [left_rotation_constant_c4] "m"(left_rotation_constant_c4), [right_rotation_constant_c4] "m"(right_rotation_constant_c4)
        : "rax", "xmm0", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10"
    );

    _mm256_storeu_si256(&state->a0, a0);
    _mm256_storeu_si256(&state->a1, a1);
    _mm256_storeu_si256(&state->a2, a2);
    _mm256_storeu_si256(&state->a3, a3);
    _mm256_storeu_si256(&state->a4, a4);
    _mm256_storeu_si256(&state->c4, c4);
    _mm256_storeu_si256(&state->a44, a44);
}
