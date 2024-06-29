#pragma once

#include <stddef.h>
#include <stdint.h>

#define SHAKE128_RATE (168)
#define SHAKE256_RATE (136)
#define SHA3_256_RATE (136)
#define SHA3_384_RATE (104)
#define SHA3_512_RATE ( 72)


// Context for incremental API
typedef struct {
   uint64_t ctx[26];
} shake128incctx;

// Context for incremental API
typedef struct {
   uint64_t ctx[26];
} shake256incctx;

// Context for incremental API
typedef struct {
   uint64_t ctx[26];
} sha3_256incctx;

// Context for incremental API
typedef struct {
   uint64_t ctx[26];
} sha3_384incctx;

// Context for incremental API
typedef struct {
   uint64_t ctx[26];
} sha3_512incctx;

/* One-stop SHAKE128 call */
void shake128(uint8_t *output, size_t outlen, const uint8_t *input,
              size_t inlen);
/* One-stop SHAKE256 call */
void shake256(uint8_t *output, size_t outlen, const uint8_t *input,
              size_t inlen);
/* One-stop SHA3-256 shop */
void sha3_256(uint8_t *output, const uint8_t *input, size_t inlen);
/* One-stop SHA3-384 shop */
void sha3_384(uint8_t *output, const uint8_t *input, size_t inlen);
/* One-stop SHA3-512 shop */
void sha3_512(uint8_t *output, const uint8_t *input, size_t inlen);
