
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "fips202.h"
#include "keccakf1600.c"

#define NROUNDS 24


/*************************************************
 * Name:        keccak_absorb
 *
 * Description: Absorb step of Keccak;
 *              non-incremental, starts by zeroeing the state.
 *
 * Arguments:   - uint64_t *s:       pointer to (uninitialized) output Keccak state
 *              - uint32_t r:        rate in bytes (e.g., 168 for SHAKE128)
 *              - const uint8_t *m:  pointer to input to be absorbed into s
 *              - size_t mlen:       length of input in bytes
 *              - uint8_t p:         domain-separation byte for different Keccak-derived functions
 **************************************************/
static void keccak_absorb(uint64_t *s,
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


/*************************************************
 * Name:        keccak_inc_init
 *
 * Description: Initializes the incremental Keccak state to zero.
 *
 * Arguments:   - uint64_t *s_inc: pointer to input/output incremental state
 *                First 25 values represent Keccak state.
 *                26th value represents either the number of absorbed bytes
 *                that have not been permuted, or not-yet-squeezed bytes.
 **************************************************/
static void keccak_inc_init(uint64_t *s_inc)
{
   memset(s_inc, 0, sizeof(keccak_state_t));
}
/*************************************************
 * Name:        keccak_inc_absorb
 *
 * Description: Incremental keccak absorb
 *              Preceded by keccak_inc_init, succeeded by keccak_inc_finalize
 *
 * Arguments:   - uint64_t *s_inc: pointer to input/output incremental state
 *                First 25 values represent Keccak state.
 *                26th value represents either the number of absorbed bytes
 *                that have not been permuted, or not-yet-squeezed bytes.
 *              - uint32_t r: rate in bytes (e.g., 168 for SHAKE128)
 *              - const uint8_t *m: pointer to input to be absorbed into s_inc
 *              - size_t mlen: length of input in bytes
 **************************************************/
static void keccak_inc_absorb(uint64_t *s_inc, uint32_t r, const uint8_t *m,
                              size_t mlen)
{
   //printf("-Absorbing\n");
   /* Recall that s_inc[25] is the non-absorbed bytes xored into the state */
   while (mlen + s_inc[25] >= r) {
      KeccakF1600_StateXORBytes(s_inc, m, s_inc[25], r-s_inc[25]);
      
      s_inc[25] = 0;

      mlen -= (size_t)(r - s_inc[25]);
      m += r - s_inc[25];
      s_inc[25] = 0;
      KeccakF1600_StatePermute(s_inc);
      s_inc[25] = 0;
   }
   KeccakF1600_StateXORBytes(s_inc, m, s_inc[25], mlen);
   s_inc[25] = mlen;
}

/*************************************************
 * Name:        keccak_inc_finalize
 *
 * Description: Finalizes Keccak absorb phase, prepares for squeezing
 *
 * Arguments:   - uint64_t *s_inc: pointer to input/output incremental state
 *                First 25 values represent Keccak state.
 *                26th value represents either the number of absorbed bytes
 *                that have not been permuted, or not-yet-squeezed bytes.
 *              - uint32_t r: rate in bytes (e.g., 168 for SHAKE128)
 *              - uint8_t p: domain-separation byte for different
 *                                 Keccak-derived functions
 **************************************************/
static void keccak_inc_finalize(uint64_t *s_inc, uint32_t r, uint8_t p)
{
   /* After keccak_inc_absorb, we are guaranteed that s_inc[25] < r,
      so we can always use one more byte for p in the current state. */
   if(s_inc[25] == r-1) {
      
      p |= 128;
      KeccakF1600_StateXORBytes(s_inc, &p, s_inc[25], 1);
   } else {
      uint64_t tmp = s_inc[25];
      KeccakF1600_StateXORBytes(s_inc, &p, s_inc[25], 1);
      p = 128;
      s_inc[25] = tmp;
      KeccakF1600_StateXORBytes(s_inc, &p, r-1, 1);
   }
   s_inc[25] = 0;
}

/*************************************************
 * Name:        keccak_inc_squeeze
 *
 * Description: Incremental Keccak squeeze; can be called on byte-level
 *
 * Arguments:   - uint8_t *h: pointer to output bytes
 *              - size_t outlen: number of bytes to be squeezed
 *              - uint64_t *s_inc: pointer to input/output incremental state
 *                First 25 values represent Keccak state.
 *                26th value represents either the number of absorbed bytes
 *                that have not been permuted, or not-yet-squeezed bytes.
 *              - uint32_t r: rate in bytes (e.g., 168 for SHAKE128)
 **************************************************/
static void keccak_inc_squeeze(uint8_t *h, size_t outlen,
                               uint64_t *s_inc, uint32_t r)
{
   size_t len;
   if(outlen < s_inc[25]) {
      len = outlen;
   } else {
      len = s_inc[25];
   }

   KeccakF1600_StateExtractBytes(s_inc, h, r-s_inc[25], len);
   h += len;
   outlen -= len;
   s_inc[25] -= len;

   /* Then squeeze the remaining necessary blocks */
   while (outlen > 0) {
       uint64_t tmp = s_inc[25];
       KeccakF1600_StatePermute(s_inc);
       s_inc[25] = tmp;


       if (outlen < r) {
           len = outlen;
       } else {
           len = r;
       }
       KeccakF1600_StateExtractBytes(s_inc, h, 0, len);
       h += len;
       outlen -= len;
       s_inc[25] = r - len;
   }
}

void shake128(uint8_t *output, size_t outlen, const uint8_t *input,
              size_t inlen)
{
   ALIGN (32) shake128incctx state;
   keccak_inc_init(state.ctx);

   /* Absorb input */
   keccak_inc_absorb(state.ctx, SHAKE128_RATE, input, inlen);
   keccak_inc_finalize(state.ctx, SHAKE128_RATE, 0x1F);

   /* Squeeze output */
   keccak_inc_squeeze(output, outlen, state.ctx, SHAKE128_RATE);
}

/*************************************************
 * Name:        shake256
 *
 * Description: SHAKE256 XOF with non-incremental API
 *
 * Arguments:   - uint8_t *output:      pointer to output
 *              - size_t outlen:        requested output length in bytes
 *              - const uint8_t *input: pointer to input
 *              - size_t inlen:         length of input in bytes
 **************************************************/
void shake256(uint8_t *output, size_t outlen,
              const uint8_t *input, size_t inlen)
{
   ALIGN(32) shake256incctx state;

   keccak_inc_init(state.ctx);

   /* Absorb input */
   keccak_inc_absorb(state.ctx, SHAKE256_RATE, input, inlen);
   keccak_inc_finalize(state.ctx, SHAKE256_RATE, 0x1F);

   /* Squeeze output */
   keccak_inc_squeeze(output, outlen, state.ctx, SHAKE256_RATE);
}
/*************************************************
 * Name:        sha3_256
 *
 * Description: SHA3-256 with non-incremental API
 *
 * Arguments:   - uint8_t *output:      pointer to output
 *              - const uint8_t *input: pointer to input
 *              - size_t inlen:         length of input in bytes
 **************************************************/
void sha3_256(uint8_t *output, const uint8_t *input, size_t inlen)
{
   ALIGN(32) sha3_256incctx state;
   keccak_inc_init(state.ctx);
   keccak_inc_absorb(state.ctx, SHA3_256_RATE, input, inlen);
   keccak_inc_finalize(state.ctx, SHA3_256_RATE, 0x06);
   keccak_inc_squeeze(output, 32, state.ctx, SHA3_256_RATE);
}
/*************************************************
 * Name:        sha3_384
 *
 * Description: SHA3-256 with non-incremental API
 *
 * Arguments:   - uint8_t *output:      pointer to output
 *              - const uint8_t *input: pointer to input
 *              - size_t inlen:   length of input in bytes
 **************************************************/
void sha3_384(uint8_t *output, const uint8_t *input, size_t inlen)
{
   ALIGN(32) sha3_384incctx state;
   keccak_inc_init(state.ctx);

   /* Absorb input */
   keccak_inc_absorb(state.ctx, SHA3_384_RATE, input, inlen);
   keccak_inc_finalize(state.ctx, SHA3_384_RATE, 0x06);

   /* Squeeze output */
   keccak_inc_squeeze(output, 48, state.ctx, SHA3_384_RATE);
}

/*************************************************
 * Name:        sha3_512
 *
 * Description: SHA3-512 with non-incremental API
 *
 * Arguments:   - uint8_t *output:      pointer to output
 *              - const uint8_t *input: pointer to input
 *              - size_t inlen:         length of input in bytes
 **************************************************/
void sha3_512(uint8_t *output, const uint8_t *input, size_t inlen)
{
   ALIGN(32) sha3_512incctx state;
   keccak_inc_init(state.ctx);
   keccak_inc_absorb(state.ctx, SHA3_512_RATE, input, inlen);
   keccak_inc_finalize(state.ctx, SHA3_512_RATE, 0x06);
   keccak_inc_squeeze(output, 64, state.ctx, SHA3_512_RATE);
}
