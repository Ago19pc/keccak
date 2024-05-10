#pragma once

#include "intrinUtils.c"

#define ROL(a, offset) (((a) << (offset)) ^ ((a) >> (64 - (offset))))

__m256i _to_256_Vector (void* src);

uint8_t getElement_8_2 (__m256i src, uint8_t index);                    // Uses specific AVX2 function

uint8_t getElement_8 (__m256i src, uint8_t index);                      // Uses casting

uint64_t getElement_64_2 (__m256i src, uint8_t index);                  // Uses specific AVX2 function

uint64_t getElement_64 (__m256i src, uint8_t index);                    // Uses casting

__m256i _setElement_64 (__m256i src, uint8_t index, uint64_t value);    

__m256i _to_256_Vector_64 (uint64_t a, uint64_t b, uint64_t c, uint64_t d);

void print_vector_64 (__m256i src);

__m256i negate(__m256i src); // Renamed the function from 'not' to 'negate'


