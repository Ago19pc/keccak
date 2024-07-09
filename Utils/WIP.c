#include <stdio.h>


#define _MM_SHUFFLE(fp3,fp2,fp1,fp0) \
 (((fp3) << 6) | ((fp2) << 4) | ((fp1) << 2) | (fp0))

int main () {
    // 2103
    printf("2103: %d\n", _MM_SHUFFLE(2, 1, 0, 3));
    // 0003
    printf("0003: %d\n", _MM_SHUFFLE(0, 0, 0, 3));
    // 0321
    printf("0321: %d\n", _MM_SHUFFLE(0, 3, 2, 1));
    // 0130
    printf("0130: %d\n", _MM_SHUFFLE(0, 1, 3, 0));
    // 3000
    printf("3000: %d\n", _MM_SHUFFLE(3, 0, 0, 0));
    // 0231
    printf("0231: %d\n", _MM_SHUFFLE(0, 2, 3, 1));
    // 3333
    printf("3333: %d\n", _MM_SHUFFLE(3, 3, 3, 3));
    // 0030
    printf("0030: %d\n", _MM_SHUFFLE(0, 0, 3, 0));
    // 1302
    printf("1302: %d\n", _MM_SHUFFLE(1, 3, 0, 2));
    // 2013
    printf("2013: %d\n", _MM_SHUFFLE(2, 0, 1, 3));
    // 0300
    printf("0300: %d\n", _MM_SHUFFLE(0, 3, 0, 0));
    // 3021
    printf("3021: %d\n", _MM_SHUFFLE(3, 0, 2, 1));
    // 0003
    printf("0003: %d\n", _MM_SHUFFLE(0, 0, 0, 3));

    return 0;
}