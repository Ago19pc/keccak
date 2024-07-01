#pragma once

#include <stddef.h>
#include <stdint.h>

#define SHAKE128_RATE (168)
#define SHAKE256_RATE (136)
#define SHA3_256_RATE (136)
#define SHA3_384_RATE (104)
#define SHA3_512_RATE ( 72)

void shake128(uint8_t *output, size_t outlen, const uint8_t *input,
              size_t inlen);
void shake256(uint8_t *output, size_t outlen, const uint8_t *input,
              size_t inlen);
void sha3_256(uint8_t *output, const uint8_t *input, size_t inlen);
void sha3_384(uint8_t *output, const uint8_t *input, size_t inlen);
void sha3_512(uint8_t *output, const uint8_t *input, size_t inlen);
