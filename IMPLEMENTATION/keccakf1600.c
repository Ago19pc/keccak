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

#define logger 1
#include "../Utils/logger.c"

#define NROUNDS 24
#define ROL(a, offset) ((a << offset) ^ (a >> (64-offset)))

#define LOAD(p)                         _mm256_loadu_si256((const __m256i *)(p))
#define SET(i0, i1, i2, i3)             _mm256_setr_epi64x(i0, i1, i2, i3)
#define STORE(p, a)                     _mm256_storeu_si256((__m256i *)(p), a)


#define ROLV_TYPE       __m256i

#ifdef __GNUC__
    #define _ROLV_TYPE  volatile __m256i
#else
    #define _ROLV_TYPE  __m256i
#endif

#define ROLV_CONST(name, i0, i1, i2, i3) \
    ROLV_TYPE  SLLV##name = SET(i0, i1, i2, i3); \
    ROLV_TYPE  SRLV##name = SET(64 - i0, 64 - i1, 64 - i2, 64 - i3);

#define _ROLV_CONST(name, i0, i1, i2, i3) \
    _ROLV_TYPE  SLLV##name = SET(i0, i1, i2, i3); \
    _ROLV_TYPE  SRLV##name = SET(64 - i0, 64 - i1, 64 - i2, 64 - i3);


#ifdef __GNUC__
    #define __ALIGN(x)      __attribute__((aligned(x)))
#else
    #define __ALIGN(x)      __declspec(align(x))
#endif


#define KECCAK_CONSTANTS \
ROLV_CONST(A0,  0,  1, 62, 28) \
ROLV_CONST(A1, 36, 44,  6, 55) \
ROLV_CONST(A2,  3, 10, 43, 25) \
ROLV_CONST(A3, 41, 45, 15, 21) \
ROLV_CONST(A4, 18,  2, 61, 56) \
ROLV_CONST(C4, 27, 20, 39,  8) \
_ROLV_CONST(_A0,  0,  1, 62, 28) \
_ROLV_CONST(_A1, 36, 44,  6, 55) \
_ROLV_CONST(_A2,  3, 10, 43, 25) \
_ROLV_CONST(_A3, 41, 45, 15, 21) \
_ROLV_CONST(_A4, 18,  2, 61, 56) \
_ROLV_CONST(_C4, 27, 20, 39,  8)

/**************************/\
#define KECCAK_PERMUTE_VARS \
/**************************/\
    __m256i a0, a1, a2, a3, a4, c4; \
    __m256i a04, a14, a24, a34, a44; \
    __m256i b0, b1, b2, b3, b4; \
    __m256i b04, b14, b24, b34, b44; \
    __m256i r0, r1, r2, r3; \
\
    keccak_state_t  *s = (keccak_state_t *) state; \
    ptrdiff_t       round_i;


/******************/\
#define KECCAK_LOAD \
/******************/\
    a0 = LOAD(&s->a0); \
    a1 = LOAD(&s->a1); \
    a2 = LOAD(&s->a2); \
    a3 = LOAD(&s->a3); \
    a4 = LOAD(&s->a4); \
    c4 = LOAD(&s->c4); \
    a44 = LOAD(&s->a44);

/*******************/\
#define KECCAK_STORE \
/*******************/\
    STORE(&s->a0, a0); \
    STORE(&s->a1, a1); \
    STORE(&s->a2, a2); \
    STORE(&s->a3, a3); \
    STORE(&s->a4, a4); \
    STORE(&s->c4, c4); \
    STORE(&s->a44, a44);


/*********************************/\
#define KECCAK_PERMUTE_LOOP(const_pref, nrRounds) \
/*********************************/\
__asm volatile \
( \
    "movq           %7, %%rax\n" \
"1:\n" \
    "vpxor          %1, %0, %%ymm9\n" \
    "vpxor          %2, %%ymm9, %%ymm9\n" \
    "vpxor          %3, %%ymm9, %%ymm9\n" \
    "vpxor          %4, %%ymm9, %%ymm9\n" \
    "vpermq         $147, %%ymm9, %%ymm8\n" \
    "vpsrlq         $63, %%ymm9, %%ymm7\n" \
    "vpsllq         $1, %%ymm9, %%ymm9\n" \
    "vperm2i128     $17, %5, %5, %%ymm0\n" \
    "vpxor          %%ymm0, %5, %%ymm0\n" \
    "vpunpckhqdq    %%ymm0, %%ymm0, %%ymm6\n" \
    "vpxor          %%ymm6, %%ymm0, %%ymm6\n" \
    "vpxor          %6, %%ymm6, %%ymm6\n" \
    "vpxor          %%ymm7, %%ymm9, %%ymm7\n" \
    "vpblendd       $3, %%ymm6, %%ymm8, %%ymm0\n" \
    "vpsrlq         $63, %%ymm6, %%ymm9\n" \
    "vpsllq         $1, %%ymm6, %%ymm6\n" \
    "vpxor          %%ymm9, %%ymm6, %%ymm6\n" \
    "vpblendd       $3, %%ymm6, %%ymm7, %%ymm6\n" \
    "vpxor          %%ymm7, %%ymm8, %%ymm7\n" \
    "vpxor          %%ymm7, %6, %%ymm9\n" \
    "vpermq         $57, %%ymm6, %%ymm6\n" \
    "vpxor          %%ymm6, %%ymm0, %%ymm0\n" \
    "vpxor          %%ymm0, %0, %0\n" \
    "vpxor          %%ymm0, %1, %1\n" \
    "vpbroadcastq   %%xmm7, %%ymm7\n" \
    "vpxor          %%ymm7, %5, %5\n" \
    "vpxor          %%ymm0, %2, %2\n" \
    "vpxor          %%ymm0, %3, %3\n" \
    "vpxor          %%ymm0, %4, %4\n" \
    "vmovdqa        %[SRLV_A0], %%ymm6\n" \
    "vmovdqa        %[SLLV_A0], %6\n" \
    "vmovdqa        %[SRLV_A1], %%ymm7\n" \
    "vpsrlvq        %%ymm6, %0, %%ymm6\n" \
    "vpsllvq        %6, %0, %0\n" \
    "vmovdqa        %[SLLV_A1], %6\n" \
    "vmovdqa        %[SRLV_A2], %%ymm0\n" \
    "vpsrlvq        %%ymm7, %1, %%ymm7\n" \
    "vpsllvq        %6, %1, %1\n" \
    "vmovdqa        %[SLLV_A2], %6\n" \
    "vmovdqa        %[SRLV_A3], %%ymm10\n" \
    "vpsrlvq        %%ymm0, %2, %%ymm0\n" \
    "vpsllvq        %6, %2, %2\n" \
    "vpxor          %%ymm7, %1, %%ymm7\n" \
    "vpxor          %%ymm6, %0, %%ymm6\n" \
    "vpermq         $28, %%ymm6, %1\n" \
    "vperm2i128     $17, %%ymm6, %%ymm6, %%ymm6\n" \
    "vpxor          %%ymm0, %2, %2\n" \
    "vpsrlvq        %%ymm10, %3, %%ymm0\n" \
    "vpermq         $114, %2, %2\n" \
    "vmovdqa        %[SLLV_A3], %%ymm10\n" \
    "vpsllvq        %%ymm10, %3, %3\n" \
    "vpxor          %%ymm0, %3, %%ymm10\n" \
    "vpermq         $135, %%ymm10, %3\n" \
    "vmovdqa        %[SRLV_A4], %%ymm0\n" \
    "vpsrlvq        %%ymm0, %4, %6\n" \
    "vmovdqa        %[SLLV_A4], %%ymm0\n" \
    "vpsllvq        %%ymm0, %4, %4\n" \
    "vpxor          %6, %4, %%ymm0\n" \
    "vpermq         $201, %%ymm0, %0\n" \
    "vpunpckhqdq    %%ymm0, %%ymm0, %%ymm0\n" \
    "vmovdqa        %[SRLV_C4], %4\n" \
    "vpsrlvq        %4, %5, %6\n" \
    "vmovdqa        %[SLLV_C4], %4\n" \
    "vpsllvq        %4, %5, %5\n" \
    "vpxor          %6, %5, %4\n" \
    "vpsrlq         $50, %%ymm9, %5\n" \
    "vpsllq         $14, %%ymm9, %6\n" \
    "vperm2i128     $17, %4, %4, %%ymm8\n" \
    "vpxor          %5, %6, %%ymm9\n" \
    "vmovdqa        %x4, %x6\n" \
    "vpunpckhqdq    %4, %4, %5\n" \
    "vpbroadcastq   %%xmm9, %%ymm9\n" \
    "vpbroadcastq   %x6, %6\n" \
    "vpermq         $255, %4, %4\n" \
    "vpblendd       $48, %4, %3, %4\n" \
    "vpblendd       $3, %%ymm9, %0, %3\n" \
    "vpblendd       $192, %6, %1, %1\n" \
    "vpermq         $45, %%ymm7, %6\n" \
    "vpblendd       $12, %5, %6, %5\n" \
    "vpermq         $255, %%ymm7, %%ymm7\n" \
    "vpandn         %2, %5, %%ymm9\n" \
    "subq           $32, %%rax\n" \
    "vpxor          %1, %%ymm9, %%ymm9\n" \
    "vpandn         %4, %2, %6\n" \
    "vpandn         %3, %4, %0\n" \
    "vpxor          (%%rdx, %%rax), %%ymm9, %%ymm9\n" \
    "vpxor          %0, %2, %0\n" \
    "vpxor          %5, %6, %6\n" \
    "vpandn         %1, %3, %2\n" \
    "vpandn         %5, %1, %5\n" \
    "vpxor          %4, %2, %4\n" \
    "vpxor          %3, %5, %5\n" \
    "vpunpcklqdq    %6, %%ymm9, %2\n" \
    "vpunpckhqdq    %6, %%ymm9, %6\n" \
    "vpunpcklqdq    %4, %0, %%ymm9\n" \
    "vpunpckhqdq    %4, %0, %4\n" \
    "vperm2i128     $32, %%ymm9, %2, %0\n" \
    "vperm2i128     $32, %4, %6, %1\n" \
    "vperm2i128     $19, %6, %4, %3\n" \
    "vperm2i128     $19, %2, %%ymm9, %2\n" \
    "vpandn         %%ymm10, %%ymm8, %4\n" \
    "vpandn         %%ymm0, %%ymm10, %6\n" \
    "vpandn         %%ymm8, %%ymm7, %%ymm9\n" \
    "vpxor          %4, %%ymm7, %4\n" \
    "vpxor          %6, %%ymm8, %%ymm8\n" \
    "vpxor          %%ymm9, %%ymm6, %%ymm9\n" \
    "vpandn         %%ymm6, %%ymm0, %6\n" \
    "vpxor          %6, %%ymm10, %%ymm10\n" \
    "vpandn         %%ymm7, %%ymm6, %6\n" \
    "vpunpcklqdq    %4, %%ymm9, %%ymm9\n" \
    "vpunpcklqdq    %%ymm10, %%ymm8, %%ymm8\n" \
    "vperm2i128     $32, %%ymm8, %%ymm9, %4\n" \
    "vpxor          %6, %%ymm0, %6\n" \
    "jnz            1b\n" \
\
    : "+x"(a0), "+x"(a1), "+x"(a2), "+x"(a3), "+x"(a4), "+x"(c4), "+x"(a44) \
    : "i"(8*4*nrRounds), "d"(keccak_rc), \
        [SLLV_A0] "m"(SLLV_A0), [SRLV_A0] "m"(SRLV_A0), \
        [SLLV_A1] "m"(SLLV_A1), [SRLV_A1] "m"(SRLV_A1), \
        [SLLV_A2] "m"(SLLV_A2), [SRLV_A2] "m"(SRLV_A2), \
        [SLLV_A3] "m"(SLLV_A3), [SRLV_A3] "m"(SRLV_A3), \
        [SLLV_A4] "m"(SLLV_A4), [SRLV_A4] "m"(SRLV_A4), \
        [SLLV_C4] "m"(SLLV_C4), [SRLV_C4] "m"(SRLV_C4) \
    : "rax", "xmm0", "xmm6", "xmm7", "xmm8", "xmm9", "xmm10" \
);


typedef uint64_t  keccak_rc_t[4];

// Reverse order.
__ALIGN(32) keccak_rc_t keccak_rc[24] =
{
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


#define KECCAK_PERMUTE(const_pref) KECCAK_PERMUTE_LOOP(const_pref, 24)



//*******************
typedef struct keccak_state_t
//*******************
{
    __m256i a0, a1, a2, a3, a4; //a[row, 0..3] rows
    __m256i c4;                 //a[0..3, 4] column
    __m256i a44;                //a[4, 4]
} keccak_state_t;


static const uint64_t KeccakF_RoundConstants[NROUNDS] = {
   (uint64_t)0x0000000000000001ULL,
   (uint64_t)0x0000000000008082ULL,
   (uint64_t)0x800000000000808aULL,
   (uint64_t)0x8000000080008000ULL,
   (uint64_t)0x000000000000808bULL,
   (uint64_t)0x0000000080000001ULL,
   (uint64_t)0x8000000080008081ULL,
   (uint64_t)0x8000000000008009ULL,
   (uint64_t)0x000000000000008aULL,
   (uint64_t)0x0000000000000088ULL,
   (uint64_t)0x0000000080008009ULL,
   (uint64_t)0x000000008000000aULL,
   (uint64_t)0x000000008000808bULL,
   (uint64_t)0x800000000000008bULL,
   (uint64_t)0x8000000000008089ULL,
   (uint64_t)0x8000000000008003ULL,
   (uint64_t)0x8000000000008002ULL,
   (uint64_t)0x8000000000000080ULL,
   (uint64_t)0x000000000000800aULL,
   (uint64_t)0x800000008000000aULL,
   (uint64_t)0x8000000080008081ULL,
   (uint64_t)0x8000000000008080ULL,
   (uint64_t)0x0000000080000001ULL,
   (uint64_t)0x8000000080008008ULL
};

void KeccakF1600_StateExtractBytes(uint64_t *state, unsigned char *data,
                                   unsigned int offset, unsigned int length)
{
    keccak_state_t  *s = (keccak_state_t *)state;
    uint64_t  t[25] = {0};
    uint64_t  *d = (!offset && (length >= sizeof(t))) ? (uint64_t *)data : t;
    uint64_t  *c4 = (uint64_t *)&s->c4;
    if ((d == t) && (length > sizeof(t)))
        length = sizeof(t);
    STORE(d + 0*5, s->a0);
    STORE(d + 1*5, s->a1);
    STORE(d + 2*5, s->a2);
    STORE(d + 3*5, s->a3);
    STORE(d + 4*5, s->a4);
    d[0*5 + 4] = c4[0];
    d[1*5 + 4] = c4[1];
    d[2*5 + 4] = c4[2];
    d[3*5 + 4] = c4[3];
    d[4*5 + 4] = c4[4]; //s->a44[0]

    if (d == t)
        memcpy(data, (uint8_t *)t + offset, length);
}

void KeccakF1600_StateXORBytes(uint64_t *state, const unsigned char *data,
                               unsigned int offset, unsigned int length)
{
   keccak_state_t  *s = (keccak_state_t *)state;
   uint64_t      *d = (uint64_t *)data;
   uint8_t       *t1, *d1;
   uint64_t      t[25];
   uint64_t      *t0;
   ptrdiff_t   lane_n = length / sizeof(uint64_t);
   ptrdiff_t   byte_n = length % sizeof(uint64_t);
   ptrdiff_t   i;
   KeccakF1600_StateExtractBytes(state, (uint8_t *)t, 0, sizeof(t));
   //LOG(" EXTRACTED\n");

   t0 = (uint64_t *)((uint8_t *)t + offset);
   //LOG("ADDRESS t0: %p      OFFSET: %d\n", t0, offset);
   // QUI SOTTO CE L'ERRORE
   for (i = 0; i < lane_n; i++) {
      //LOG("t0[i] = %d, i = %d\n", t0[i], i);
      //LOG("d[i] = %d, i = %d\n", d[i], i);
      t0[i] ^= d[i];
   }
   // FINE ERRORE

   //LOG(" LANES DONE\n");

   if (byte_n) {
      t1 = (uint8_t *)(t0 + i);
      d1 = (uint8_t *)(d + i);

      for (i = 0; i < byte_n; i++) t1[i] ^= d1[i];
   }
   //LOG(" XORed\n");
   s->a0 = LOAD(t + 0*5);
   s->a1 = LOAD(t + 1*5);
   s->a2 = LOAD(t + 2*5);
   s->a3 = LOAD(t + 3*5);
   s->a4 = LOAD(t + 4*5);
   //LOG(" LOADED\n");
   s->c4 = SET(t[0*5 + 4], t[1*5 + 4], t[2*5 + 4], t[3*5 + 4]);
   //LOG(" SET\n");
   s->a44 = _mm256_set1_epi64x(t[4*5 + 4]);
}

void KeccakF1600_StatePermute(uint64_t *state)
{
   KECCAK_CONSTANTS

   KECCAK_PERMUTE_VARS

   KECCAK_LOAD

   KECCAK_PERMUTE()

   KECCAK_STORE
}