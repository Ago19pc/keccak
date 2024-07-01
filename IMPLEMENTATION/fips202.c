#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "fips202.h"
#include "keccakf1600.c"
static void keccak_absorb(keccak_state_t *s,
                          uint32_t r,
                          const uint8_t *m, size_t mlen,
                          uint8_t p)
{
   while (mlen >= r) {
      KeccakF1600_StateXORBytes(s, m, 0, r);
      KeccakF1600_StatePermute(s);
      mlen -= r;
      m += r;
   }

   if(mlen > 0) {
      KeccakF1600_StateXORBytes(s, m, 0, mlen);
   }

   if(mlen == r-1) {
      p |= 128;
      KeccakF1600_StateXORBytes(s, &p, mlen, 1);
   } else {
      KeccakF1600_StateXORBytes(s, &p, mlen, 1);
      p = 128;
      KeccakF1600_StateXORBytes(s, &p, r-1, 1);
   }
}


static void keccak_inc_init(keccak_state_t *state)
{
   memset(state, 0, sizeof(keccak_state_t));
}

static size_t keccak_inc_absorb(keccak_state_t *state, uint32_t r, const uint8_t *m,
                              size_t mlen)
{
   while (mlen >= r) {
      KeccakF1600_StateXORBytes(state, m, 0, r);
      mlen -= (size_t)(r);
      m += r;
      KeccakF1600_StatePermute(state);
   }
   KeccakF1600_StateXORBytes(state, m, 0, mlen);
   return mlen;
}

static void keccak_inc_finalize(keccak_state_t *state, uint32_t r, uint8_t p, size_t remainingLen)
{
   if(remainingLen == r-1) {
      
      p |= 128;
      KeccakF1600_StateXORBytes(state, &p, remainingLen, 1);
   } else {
      KeccakF1600_StateXORBytes(state, &p, remainingLen, 1);
      p = 128;
      KeccakF1600_StateXORBytes(state, &p, r-1, 1);
   }
}

static void keccak_inc_squeeze(uint8_t *h, size_t outlen,
                               keccak_state_t *state, uint32_t r)
{
   size_t len = 0;
   while (outlen > 0) {
       KeccakF1600_StatePermute(state);
       if (outlen < r) {
           len = outlen;
       } else {
           len = r;
       }
       KeccakF1600_StateExtractBytes(state, h, 0, len);
       h += len;
       outlen -= len;
   }
}

void shake128(uint8_t *output, size_t outlen, const uint8_t *input,
              size_t inlen)
{
   ALIGN (32) keccak_state_t state;
   keccak_inc_init(&state);

   inlen = keccak_inc_absorb(&state, SHAKE128_RATE, input, inlen);
   keccak_inc_finalize(&state, SHAKE128_RATE, 0x1F, inlen);

   keccak_inc_squeeze(output, outlen, &state, SHAKE128_RATE);
}

void shake256(uint8_t *output, size_t outlen,
              const uint8_t *input, size_t inlen)
{
   ALIGN(32) keccak_state_t state;

   keccak_inc_init(&state);

   inlen = keccak_inc_absorb(&state, SHAKE256_RATE, input, inlen);
   keccak_inc_finalize(&state, SHAKE256_RATE, 0x1F, inlen);

   keccak_inc_squeeze(output, outlen, &state, SHAKE256_RATE);
}
void sha3_256(uint8_t *output, const uint8_t *input, size_t inlen)
{
   ALIGN(32) keccak_state_t state;
   keccak_inc_init(&state);
   inlen = keccak_inc_absorb(&state, SHA3_256_RATE, input, inlen);
   keccak_inc_finalize(&state, SHA3_256_RATE, 0x06, inlen);
   keccak_inc_squeeze(output, 32, &state, SHA3_256_RATE);
}
void sha3_384(uint8_t *output, const uint8_t *input, size_t inlen)
{
   ALIGN(32) keccak_state_t state;
   keccak_inc_init(&state);

   inlen = keccak_inc_absorb(&state, SHA3_384_RATE, input, inlen);
   keccak_inc_finalize(&state, SHA3_384_RATE, 0x06, inlen);

   keccak_inc_squeeze(output, 48, &state, SHA3_384_RATE);
}

void sha3_512(uint8_t *output, const uint8_t *input, size_t inlen)
{
   ALIGN(32) keccak_state_t state;
   keccak_inc_init(&state);
   inlen = keccak_inc_absorb(&state, SHA3_512_RATE, input, inlen);
   keccak_inc_finalize(&state, SHA3_512_RATE, 0x06, inlen);
   keccak_inc_squeeze(output, 64, &state, SHA3_512_RATE);
}
