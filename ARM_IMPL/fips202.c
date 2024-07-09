#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "fips202.h"
#include "keccakf1600.c"

static void keccak_init(keccak_state_t *state)
{
   memset(state, 0, sizeof(keccak_state_t));
}

static size_t keccak_absorb(keccak_state_t *state, uint32_t rate, const uint8_t *input, size_t inlen)
{
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

static void keccak_squeeze(uint8_t *out, size_t outlen, keccak_state_t *state, uint32_t rate)
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
   ALIGN (64) keccak_state_t state;
   keccak_init(&state);

   inlen = keccak_absorb(&state, SHAKE128_RATE, input, inlen);
   keccak_finalize(&state, SHAKE128_RATE, 0x1F, inlen);

   keccak_squeeze(output, outlen, &state, SHAKE128_RATE);
}

void shake256(uint8_t *output, size_t outlen,
              const uint8_t *input, size_t inlen)
{
   ALIGN(64) keccak_state_t state;

   keccak_init(&state);

   inlen = keccak_absorb(&state, SHAKE256_RATE, input, inlen);
   keccak_finalize(&state, SHAKE256_RATE, 0x1F, inlen);

   keccak_squeeze(output, outlen, &state, SHAKE256_RATE);
}
void sha3_256(uint8_t *output, const uint8_t *input, size_t inlen)
{
   ALIGN(64) keccak_state_t state;
   keccak_init(&state);
   inlen = keccak_absorb(&state, SHA3_256_RATE, input, inlen);
   keccak_finalize(&state, SHA3_256_RATE, 0x06, inlen);
   keccak_squeeze(output, 32, &state, SHA3_256_RATE);
}
void sha3_384(uint8_t *output, const uint8_t *input, size_t inlen)
{
   ALIGN(64) keccak_state_t state;
   keccak_init(&state);

   inlen = keccak_absorb(&state, SHA3_384_RATE, input, inlen);
   keccak_finalize(&state, SHA3_384_RATE, 0x06, inlen);

   keccak_squeeze(output, 48, &state, SHA3_384_RATE);
}

void sha3_512(uint8_t *output, const uint8_t *input, size_t inlen)
{
   ALIGN(64) keccak_state_t state;
   keccak_init(&state);
   inlen = keccak_absorb(&state, SHA3_512_RATE, input, inlen);
   keccak_finalize(&state, SHA3_512_RATE, 0x06, inlen);
   keccak_squeeze(output, 64, &state, SHA3_512_RATE);
}
