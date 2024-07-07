#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "fips202.h"
#include "keccakf1600.c"

static void keccak_init(keccak_state_t *state)
{
   memset(state, 0, sizeof(keccak_state_t));
}

/** 
 * @param rate in bytes
 * @param inlen in bytes
*/
static size_t keccak_absorb(keccak_state_t *state, uint32_t rate, const uint8_t *input,
                              size_t inlen)
{
   // FASTLOOP
   if (rate % 8 == 0 && inlen >= rate) {

      size_t laneCount = rate >> 3;

      __m256i a0, a1, a2, a3, a4, c4, a44;
      
      a0 = _mm256_loadu_si256(&state->a0);
      a1 = _mm256_loadu_si256(&state->a1);
      a2 = _mm256_loadu_si256(&state->a2);
      a3 = _mm256_loadu_si256(&state->a3);
      a4 = _mm256_loadu_si256(&state->a4);
      c4 = _mm256_loadu_si256(&state->c4);
      a44 = _mm256_loadu_si256(&state->a44);

      const uint64_t * d;
      ptrdiff_t di;

      for (di = 0; di <= (ptrdiff_t)(inlen/sizeof(uint64_t) - laneCount); di += laneCount){
         d = (uint64_t*)(input) + di;
         switch (laneCount)
         {
         case 21:
            a0 = _mm256_xor_si256(a0, _mm256_loadu_si256((const __m256i*)d));
            a1 = _mm256_xor_si256(a1, _mm256_loadu_si256((const __m256i*)(d + 5)));
            a2 = _mm256_xor_si256(a2, _mm256_loadu_si256((const __m256i*)(d + 10)));
            a3 = _mm256_xor_si256(a3, _mm256_loadu_si256((const __m256i*)(d + 15)));
            a4 = _mm256_xor_si256(a4, _mm256_maskload_epi64((const int64_t*)(d+20), _mm256_setr_epi64x((uint64_t)1 << 63, (uint64_t)0, (uint64_t)0, (uint64_t)0)));
            c4 = _mm256_xor_si256(c4, _mm256_setr_epi64x(d[4], d[9], d[14], d[19]));
            break;
         case 17:
            a0 = _mm256_xor_si256(a0, _mm256_loadu_si256((const __m256i*)d));
            a1 = _mm256_xor_si256(a1, _mm256_loadu_si256((const __m256i*)(d + 5)));
            a2 = _mm256_xor_si256(a2, _mm256_loadu_si256((const __m256i*)(d + 10)));
            a3 = _mm256_xor_si256(a3, _mm256_maskload_epi64((const int64_t*)(d + 15), _mm256_setr_epi64x((uint64_t)1 << 63, (uint64_t)1 << 63, (uint64_t)0, (uint64_t)0)));
            c4 = _mm256_xor_si256(c4, _mm256_setr_epi64x(d[4], d[9], d[14], 0));
            break;
         case 13:
            a0 = _mm256_xor_si256(a0, _mm256_loadu_si256((const __m256i*)d));
            a1 = _mm256_xor_si256(a1, _mm256_loadu_si256((const __m256i*)(d + 5)));
            a2 = _mm256_xor_si256(a2, _mm256_maskload_epi64((const int64_t*)(d + 10), _mm256_setr_epi64x((uint64_t)1 << 63, (uint64_t)1 << 63, (uint64_t)1 << 63, (uint64_t)0)));
            c4 = _mm256_xor_si256(c4, _mm256_setr_epi64x(d[4], d[9], 0, 0));
            break;
         case 9:
            a0 = _mm256_xor_si256(a0, _mm256_loadu_si256((const __m256i*)d));
            a1 = _mm256_xor_si256(a1, _mm256_loadu_si256((const __m256i*)(d + 5)));
            c4 = _mm256_xor_si256(c4, _mm256_maskload_epi64((const int64_t*)(d + 4), _mm256_setr_epi64x((uint64_t)1 << 63, (uint64_t)0, (uint64_t)0, (uint64_t)0)));
            break;
         default:
            break;
         }
         KeccakF1600_StatePermute_Light(state, &a0, &a1, &a2, &a3, &a4, &c4, &a44);
      }

      _mm256_storeu_si256(&state->a0, a0);
      _mm256_storeu_si256(&state->a1, a1);
      _mm256_storeu_si256(&state->a2, a2);
      _mm256_storeu_si256(&state->a3, a3);
      _mm256_storeu_si256(&state->a4, a4);
      _mm256_storeu_si256(&state->c4, c4);
      _mm256_storeu_si256(&state->a44, a44);

      input += di * sizeof(uint64_t);
      inlen -= di * sizeof(uint64_t);
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
