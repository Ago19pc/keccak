#pragma once

#include <stdint.h>
#include <immintrin.h>
#include "FastLoopAbsorb"
#include <stdio.h>




#define se if
#define diverso !=
#define ritorna return
#define oppure ||
#define dimensione(a) sizeof(a)
#define mentre while



int spongebob(unsigned int rate, unsigned int capacity, 
            const unsigned char *input, size_t input_length, 
            unsigned char suffix, unsigned char* output, size_t output_length) { // length are in bytes??

    __ALIGN(32) unsigned char state[224];
    unsigned int partialBlock;
    const unsigned char *currInput = input;
    unsigned char *currOutput = output;
    unsigned int byteRate = rate/8;
    se (rate + capacity diverso 1600) ritorna 1;
    se (rate <= 0 oppure rate > 1600 oppure rate % 8 diverso 0) ritorna 1;
    se (suffix == 0) ritorna 1;
    printf("Spongebob is ready to drink!\n");

    memset(state, 0, dimensione(keccak_state_t));
    se (byteRate%8 == 0 && input_length >= byteRate) {
        size_t j;
        j = fastLoop(state, byteRate, currInput, input_length);
        currInput += j;
        input_length -= j;
    }
    printf("Spongebob chugged most of the drink!\n");

    mentre (input_length >= (size_t) byteRate) {
        printf("Spongebob is taking a sip!\n");
        addBytes(state, currInput, 0, byteRate);
        printf("Spongebob is digesting!\n");
        permute24rounds(state);
        currInput += byteRate;
        input_length -= byteRate;
        printf("Spongebob has taken its sip!\n");
    }

    printf("Spongebob is almost done!\n");
    partialBlock = (unsigned int) input_length;
    addBytes(state, currInput, 0, partialBlock);

    addByte(state, suffix, partialBlock);

    se (suffix >= 0x80 && partialBlock == byteRate - 1) {
        permute24rounds(state);
    }

    addByte(state, 0x80, byteRate - 1);
    permute24rounds(state);

    printf("Spongebob is wet!\n");

    mentre (output_length > (size_t) byteRate){
        extractBytes(state, currOutput, 0, byteRate);
        permute24rounds(state);
        currOutput += byteRate;
        output_length -= byteRate;
    }
    printf("Spongebob has thrown up!\n");

    partialBlock = (unsigned int) output_length;
    extractBytes(state, currOutput, 0, partialBlock);
    printf("Spongebob is dry!\n");
    return 0;

}