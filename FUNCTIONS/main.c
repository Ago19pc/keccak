#include "STATEPERMUTE.c"
#include "STATEXORDATA.c"
#include "STATEEXTRACT.c"
#include "FIPS.c"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


uint8_t hexToInt(char hex){
    switch(hex){
        case '0':
            return 0;
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':   
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'a':
            return 10;
        case 'b':
            return 11;
        case 'c':
            return 12;
        case 'd':
            return 13;
        case 'e':
            return 14;
        case 'f':
            return 15;
        default:
            return 0;
    }
}

char* byteToHex(uint8_t byte){
    char* hex = malloc(2);
    uint8_t first = byte/16;
    uint8_t second = byte%16;
    printf("converting %d", byte);
    switch(first){
        case 0:
            hex[0] = '0';
            break;
        case 1:
            hex[0] = '1';
            break;
        case 2:
            hex[0] = '2';
            break;
        case 3:
            hex[0] = '3';
            break;
        case 4:
            hex[0] = '4';
            break;
        case 5:
            hex[0] = '5';
            break;
        case 6:
            hex[0] = '6';
            break;
        case 7:
            hex[0] = '7';
            break;
        case 8:
            hex[0] = '8';
            break;
        case 9:
            hex[0] = '9';
            break;
        case 10:
            hex[0] = 'a';
            break;
        case 11:
            hex[0] = 'b';
            break;
        case 12:
            hex[0] = 'c';
            break;
        case 13:
            hex[0] = 'd';
            break;
        case 14:
            hex[0] = 'e';
            break;
        case 15:
            hex[0] = 'f';
            break;
    }
    switch(second){
        case 0:
            hex[1] = '0';
            break;
        case 1:
            hex[1] = '1';
            break;
        case 2:
            hex[1] = '2';
            break;
        case 3:
            hex[1] = '3';
            break;
        case 4:
            hex[1] = '4';
            break;
        case 5:
            hex[1] = '5';
            break;
        case 6:
            hex[1] = '6';
            break;
        case 7:
            hex[1] = '7';
            break;
        case 8:
            hex[1] = '8';
            break;
        case 9:
            hex[1] = '9';
            break;
        case 10:
            hex[1] = 'a';
            break;
    }
    printf("converted %d to %c%c", byte, hex[0], hex[1]);
    return hex;
}


int main () {
    size_t len = 2184;
    uint8_t input[len/8];
    uint8_t output[32];
    char* inputString = "b1caa396771a09a1db9bc20543e988e359d47c2a616417bbca1b62cb02796a888fc6eeff5c0b5c3d5062fcb4256f6ae1782f492c1cf03610b4a1fb7b814c057878e1190b9835425c7a4a0e182ad1f91535ed2a35033a5d8c670e21c575ff43c194a58a82d4a1a44881dd61f9f8161fc6b998860cbe4975780be93b6f87980bad0a99aa2cb7556b478ca35d1f3746c33e2bb7c47af426641cc7bbb3425e2144820345e1d0ea5b7da2c3236a52906acdc3b4d34e474dd714c0c40bf006a3a1d889a632983814bbc4a14fe5f159aa89249e7c738b3b73666bac2a615a83fd21ae0a1ce7352ade7b278b587158fd2fabb217aa1fe31d0bda53272045598015a8ae4d8cec226fefa58daa05500906c4d85e7567";
    for(int i = 0; i < len/8; i++){
        char first = inputString[i*2];
        char second = inputString[i*2+1];
        uint8_t correspondingByte = 0;
        correspondingByte += hexToInt(first)*16;
        correspondingByte += hexToInt(second);
        input[i] = correspondingByte;
    }
    for (int i = 0; i < len/8; i++){
        printf("%x", input[i]);
    }
    printf("input read\n");
    sha3_256(output, input, len/8);

    for(int i = 0; i < 32; i++){
        printf("%x", output[i]);
    }
    printf("\n");

    printf("sha done\n");
    for(int i = 0; i < 32; i++){
        printf("%x", output[i]);
    }

    printf("big input");
    system("pause");
    

    return 0;

}