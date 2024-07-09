#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "fips202.h"
#include "keccakf1600.c"


static void keccak_init(keccak_state_t *state)
{
   memset(state, 0, sizeof(keccak_state_t));
   left_rotation_constant_a0 = _mm256_setr_epi64x( 0,  1, 62, 28);
   right_rotation_constant_a0 = _mm256_setr_epi64x(64, 63, 2, 36);
   left_rotation_constant_a1 = _mm256_setr_epi64x(36, 44,  6, 55);
   right_rotation_constant_a1 = _mm256_setr_epi64x(28, 20, 58, 9);
   left_rotation_constant_a2 = _mm256_setr_epi64x( 3, 10, 43, 25);
   right_rotation_constant_a2 = _mm256_setr_epi64x(61, 54, 21, 39);
   left_rotation_constant_a3 = _mm256_setr_epi64x(41, 45, 15, 21);
   right_rotation_constant_a3 = _mm256_setr_epi64x(23, 19, 49, 43);
   left_rotation_constant_a4 = _mm256_setr_epi64x(18, 2, 61, 56);
   right_rotation_constant_a4 = _mm256_setr_epi64x(46, 62, 3, 8);
   left_rotation_constant_c4 = _mm256_setr_epi64x(27, 20, 39,  8);
   right_rotation_constant_c4 = _mm256_setr_epi64x(37, 44, 25, 56);
}

/** 
 * @param rate in bytes
 * @param inlen in bytes
*/
static size_t keccak_absorb(keccak_state_t *state, uint32_t rate, const uint8_t *input, size_t inlen)
{
   if (rate % 8 == 0 && inlen >= rate) {
      ptrdiff_t       round_i;
      __m256i a04, a14, a24, a34;
      __m256i b0, b1, b2, b3, b4;
      __m256i b04, b14, b24, b34, b44;
      __m256i r0, r1, r2, r3;
      __m128i t0;
      size_t laneRate = rate/8;
      __m256i a0, a1, a2, a3, a4, c4, a44;
      a0 = _mm256_loadu_si256(&state->a0);
      a1 = _mm256_loadu_si256(&state->a1);
      a2 = _mm256_loadu_si256(&state->a2);
      a3 = _mm256_loadu_si256(&state->a3);
      a4 = _mm256_loadu_si256(&state->a4);
      c4 = _mm256_loadu_si256(&state->c4);
      a44 = _mm256_loadu_si256(&state->a44);
      const uint64_t * currentDataWord;
      ptrdiff_t dataOffset;

      for (dataOffset = 0; dataOffset <= (ptrdiff_t)(inlen/8 - laneRate); dataOffset += laneRate){
         currentDataWord = (uint64_t*)(input) + dataOffset;
         switch (laneRate)
         {
             case 21:
                a0 = _mm256_xor_si256(a0, _mm256_loadu_si256((const __m256i*)currentDataWord));
                a1 = _mm256_xor_si256(a1, _mm256_loadu_si256((const __m256i*)(currentDataWord+ 5)));
                a2 = _mm256_xor_si256(a2, _mm256_loadu_si256((const __m256i*)(currentDataWord+ 10)));
                a3 = _mm256_xor_si256(a3, _mm256_loadu_si256((const __m256i*)(currentDataWord+ 15)));
                a4 = _mm256_xor_si256(a4, _mm256_maskload_epi64((const int64_t*)(currentDataWord+20), _mm256_setr_epi64x((uint64_t)1 << 63, (uint64_t)0, (uint64_t)0, (uint64_t)0)));
                c4 = _mm256_xor_si256(c4, _mm256_setr_epi64x(currentDataWord[4], currentDataWord[9], currentDataWord[14], currentDataWord[19]));
                break;
             case 17:
                a0 = _mm256_xor_si256(a0, _mm256_loadu_si256((const __m256i*)currentDataWord));
                a1 = _mm256_xor_si256(a1, _mm256_loadu_si256((const __m256i*)(currentDataWord+ 5)));
                a2 = _mm256_xor_si256(a2, _mm256_loadu_si256((const __m256i*)(currentDataWord+ 10)));
                a3 = _mm256_xor_si256(a3, _mm256_maskload_epi64((const int64_t*)(currentDataWord+ 15), _mm256_setr_epi64x((uint64_t)1 << 63, (uint64_t)1 << 63, (uint64_t)0, (uint64_t)0)));
                c4 = _mm256_xor_si256(c4, _mm256_setr_epi64x(currentDataWord[4], currentDataWord[9], currentDataWord[14], 0));
                break;
             case 13:
                a0 = _mm256_xor_si256(a0, _mm256_loadu_si256((const __m256i*)currentDataWord));
                a1 = _mm256_xor_si256(a1, _mm256_loadu_si256((const __m256i*)(currentDataWord+ 5)));
                a2 = _mm256_xor_si256(a2, _mm256_maskload_epi64((const int64_t*)(currentDataWord+ 10), _mm256_setr_epi64x((uint64_t)1 << 63, (uint64_t)1 << 63, (uint64_t)1 << 63, (uint64_t)0)));
                c4 = _mm256_xor_si256(c4, _mm256_setr_epi64x(currentDataWord[4], currentDataWord[9], 0, 0));
                break;
             case 9:
                a0 = _mm256_xor_si256(a0, _mm256_loadu_si256((const __m256i*)currentDataWord));
                a1 = _mm256_xor_si256(a1, _mm256_loadu_si256((const __m256i*)(currentDataWord+ 5)));
                c4 = _mm256_xor_si256(c4, _mm256_maskload_epi64((const int64_t*)(currentDataWord+ 4), _mm256_setr_epi64x((uint64_t)1 << 63, (uint64_t)0, (uint64_t)0, (uint64_t)0)));
                break;
             default:
                break;
         }
         // PERMUTE LIGHT (NO LOAD E STORE)
         for (round_i = 23; round_i >= 0; round_i--){
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
      }
      _mm256_storeu_si256(&state->a0, a0);
      _mm256_storeu_si256(&state->a1, a1);
      _mm256_storeu_si256(&state->a2, a2);
      _mm256_storeu_si256(&state->a3, a3);
      _mm256_storeu_si256(&state->a4, a4);
      _mm256_storeu_si256(&state->c4, c4);
      _mm256_storeu_si256(&state->a44, a44);
      input += dataOffset * 8;
      inlen -= dataOffset * 8;
   }
   while (inlen >= rate) {
      KeccakF1600_StateXORBytes(state, input, 0, rate);
      inlen -= (size_t)(rate);
      input += rate;
      KeccakF1600_StatePermute(state);
   }
   KeccakF1600_StateXORBytes(state, input, 0, inlen);
   return inlen;
}

static void keccak_finalize(keccak_state_t *state, uint32_t rate, uint8_t padding, size_t remainingLen)
{
   if(remainingLen == rate-1) {
      padding |= 128;
      KeccakF1600_StateXORBytes(state, &padding, remainingLen, 1);
   } else {
      KeccakF1600_StateXORBytes(state, &padding, remainingLen, 1);
      padding = 128;
      KeccakF1600_StateXORBytes(state, &padding, rate-1, 1);
   }
}

static void keccak_squeeze(uint8_t *out, size_t outlen,
                               keccak_state_t *state, uint32_t rate)
{
   size_t len = 0;
   while (outlen > 0) {
       KeccakF1600_StatePermute(state);
       if (outlen < rate) {
           len = outlen;
       } else {
           len = rate;
       }
       KeccakF1600_StateExtractBytes(state, out, 0, len);
       out += len;
       outlen -= len;
   }
}

void shake128(uint8_t *output, size_t outlen, const uint8_t *input,
              size_t inlen)
{
   ALIGN (32) keccak_state_t state;
   keccak_init(&state);

   inlen = keccak_absorb(&state, SHAKE128_RATE, input, inlen);
   keccak_finalize(&state, SHAKE128_RATE, 0x1F, inlen);

   keccak_squeeze(output, outlen, &state, SHAKE128_RATE);
}

void shake256(uint8_t *output, size_t outlen,
              const uint8_t *input, size_t inlen)
{
   ALIGN(32) keccak_state_t state;

   keccak_init(&state);

   inlen = keccak_absorb(&state, SHAKE256_RATE, input, inlen);
   keccak_finalize(&state, SHAKE256_RATE, 0x1F, inlen);

   keccak_squeeze(output, outlen, &state, SHAKE256_RATE);
}
void sha3_256(uint8_t *output, const uint8_t *input, size_t inlen)
{
   ALIGN(32) keccak_state_t state;
   keccak_init(&state);
   inlen = keccak_absorb(&state, SHA3_256_RATE, input, inlen);
   keccak_finalize(&state, SHA3_256_RATE, 0x06, inlen);
   keccak_squeeze(output, 32, &state, SHA3_256_RATE);
}
void sha3_384(uint8_t *output, const uint8_t *input, size_t inlen)
{
   ALIGN(32) keccak_state_t state;
   keccak_init(&state);

   inlen = keccak_absorb(&state, SHA3_384_RATE, input, inlen);
   keccak_finalize(&state, SHA3_384_RATE, 0x06, inlen);

   keccak_squeeze(output, 48, &state, SHA3_384_RATE);
}

void sha3_512(uint8_t *output, const uint8_t *input, size_t inlen)
{
   ALIGN(32) keccak_state_t state;
   keccak_init(&state);
   inlen = keccak_absorb(&state, SHA3_512_RATE, input, inlen);
   keccak_finalize(&state, SHA3_512_RATE, 0x06, inlen);
   keccak_squeeze(output, 64, &state, SHA3_512_RATE);
}
