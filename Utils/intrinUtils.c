#pragma once

#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>


__m256i _setElement_64 (__m256i src, uint8_t index, uint64_t value) {
    // Quando generi il vettore a = set(4, 3, 2, 1)
    // _setElement_64(src, index = 0, value) prende imposta value al posto di 1
    // _setElement_64(src, index = 1, value) prende imposta value al posto di 2
    // _setElement_64(src, index = 2, value) prende imposta value al posto di 3
    // _setElement_64(src, index = 3, value) prende imposta value al posto di 4


    __mmask8 mask;
    switch (index) {
        case 0:
            mask = 0b00000001;
            break;
        case 1:
            mask = 0b00000010;
            break;
        case 2:
            mask = 0b00000100;
            break;
        case 3:
            mask = 0b00001000;
    }

    
    __m256i temp = _mm256_set1_epi64x(value);

    return _mm256_mask_blend_epi64(mask, src, temp);

}

uint64_t getElement_64 (__m256i src, uint8_t index) {
    uint64_t* temp = (uint64_t*) &src;
    return temp[index];
}

uint64_t getElement_64_2 (__m256i src, uint8_t index) {
    switch (index)
    {
    case 0:
        return _mm256_extract_epi64(src, 0);
    case 1:
        return _mm256_extract_epi64(src, 1);
    case 2:
        return _mm256_extract_epi64(src, 2);
    case 3:
        return _mm256_extract_epi64(src, 3);
    default:
        return 0;
    }
    
}

uint8_t getElement_8 (__m256i src, uint8_t index) {
    uint8_t* temp = (uint8_t*) &src;
    return temp[index];
}

uint8_t getElement_8_2 (__m256i src, uint8_t index) {
    switch (index)
    {
    case 0:
        return _mm256_extract_epi8(src, 24);
    case 1:
        return _mm256_extract_epi8(src, 25);
    case 2:
        return _mm256_extract_epi8(src, 26);
    case 3:
        return _mm256_extract_epi8(src, 27);
    case 4:
        return _mm256_extract_epi8(src, 28);
    case 5:
        return _mm256_extract_epi8(src, 29);
    case 6:
        return _mm256_extract_epi8(src, 30);
    case 7:
        return _mm256_extract_epi8(src, 31);
    case 8:
        return _mm256_extract_epi8(src, 16);
    case 9:
        return _mm256_extract_epi8(src, 17);
    case 10:
        return _mm256_extract_epi8(src, 18);
    case 11:
        return _mm256_extract_epi8(src, 19);
    case 12:
        return _mm256_extract_epi8(src, 20);
    case 13:
        return _mm256_extract_epi8(src, 21);
    case 14:    
        return _mm256_extract_epi8(src, 22);
    case 15:
        return _mm256_extract_epi8(src, 23);
    case 16:
        return _mm256_extract_epi8(src, 8);
    case 17:
        return _mm256_extract_epi8(src, 9);
    case 18:
        return _mm256_extract_epi8(src, 10);
    case 19:
        return _mm256_extract_epi8(src, 11);
    case 20:
        return _mm256_extract_epi8(src, 12);
    case 21:
        return _mm256_extract_epi8(src, 13);
    case 22:
        return _mm256_extract_epi8(src, 14);
    case 23:
        return _mm256_extract_epi8(src, 15);
    case 24:
        return _mm256_extract_epi8(src, 0);
    case 25:
        return _mm256_extract_epi8(src, 1);
    case 26:
        return _mm256_extract_epi8(src, 2);
    case 27:
        return _mm256_extract_epi8(src, 3);
    case 28:
        return _mm256_extract_epi8(src, 4);
    case 29:
        return _mm256_extract_epi8(src, 5);
    case 30:
        return _mm256_extract_epi8(src, 6);
    case 31:
        return _mm256_extract_epi8(src, 7);
    
    default:
        return 0;
    }
}

__m256i _to_256_Vector (void* src) {
    return _mm256_loadu_si256((__m256i*)src);
}

__m256i _to_256_Vector_64 (uint64_t a, uint64_t b, uint64_t c, uint64_t d) {
    return _mm256_set_epi64x(d, c, b, a);
}

void print_vector_64 (__m256i src) {
    uint64_t* temp = (uint64_t*) &src;
    printf("long long int:\t %llu, %llu, %llu, %llu\n", temp[0], temp[1], temp[2], temp[3]);

}

__m256i negate(__m256i src) {
    return _mm256_andnot_si256(src, _mm256_set1_epi8(255));
}