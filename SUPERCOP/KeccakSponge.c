/*
Implementation by the Keccak, Keyak and Ketje Teams, namely, Guido Bertoni,
Joan Daemen, Michaël Peeters, Gilles Van Assche and Ronny Van Keer, hereby
denoted as "the implementer".

For more information, feedback or questions, please refer to our websites:
http://keccak.noekeon.org/
http://keyak.noekeon.org/
http://ketje.noekeon.org/

To the extent possible under law, the implementer has waived all copyright
and related or neighboring rights to the source code in this file.
http://creativecommons.org/publicdomain/zero/1.0/
*/

#include <stdlib.h>
#include "KeccakP-1600-AVX2.c"

#ifdef ALIGN
#undef ALIGN
#endif

#if defined(__GNUC__)
#define ALIGN(x) __attribute__((aligned(x)))
#elif defined(_MSC_VER)
#define ALIGN(x) __declspec(align(x))
#elif defined(__ARMCC_VERSION)
#define ALIGN(x) __align(x)
#else
#define ALIGN(x)
#endif


int Sponge(unsigned int rate, unsigned int capacity, const unsigned char *input, size_t inputByteLen, unsigned char suffix, unsigned char *output, size_t outputByteLen)
{
    ALIGN(32) unsigned char state[224];
    unsigned int partialBlock;
    const unsigned char *curInput = input;
    unsigned char *curOutput = output;
    unsigned int rateInBytes = rate/8;

    if (rate+capacity != 1600)
        return 1;
    if ((rate <= 0) || (rate > 1600) || ((rate % 8) != 0))
        return 1;
    if (suffix == 0)
        return 1;

    /* Initialize the state */
    KeccakP1600_StaticInitialize();
    KeccakP1600_Initialize(state);

    /* First, absorb whole blocks */

    if (((rateInBytes % (1600/200)) == 0) && (inputByteLen >= rateInBytes)) {
        /* fast lane: whole lane rate */
        size_t j;
        j = KeccakF1600_FastLoop_Absorb(state, rateInBytes/(1600/200), curInput, inputByteLen);
        curInput += j;
        inputByteLen -= j;
    }

    while(inputByteLen >= (size_t)rateInBytes) {
        KeccakP1600_AddBytes(state, curInput, 0, rateInBytes);
        KeccakP1600_Permute_24rounds(state);
        curInput += rateInBytes;
        inputByteLen -= rateInBytes;
    }

    /* Then, absorb what remains */
    partialBlock = (unsigned int)inputByteLen;
    KeccakP1600_AddBytes(state, curInput, 0, partialBlock);


    /* Last few bits, whose delimiter coincides with first bit of padding */
    KeccakP1600_AddByte(state, suffix, partialBlock);
    /* If the first bit of padding is at position rate-1, we need a whole new block for the second bit of padding */
    if ((suffix >= 0x80) && (partialBlock == (rateInBytes-1)))
        KeccakP1600_Permute_24rounds(state);
    /* Second bit of padding */
    KeccakP1600_AddByte(state, 0x80, rateInBytes-1);


    KeccakP1600_Permute_24rounds(state);


    /* First, output whole blocks */
    while(outputByteLen > (size_t)rateInBytes) {
        KeccakP1600_ExtractBytes(state, curOutput, 0, rateInBytes);
        KeccakP1600_Permute_24rounds(state);
        curOutput += rateInBytes;
        outputByteLen -= rateInBytes;
    }

    /* Finally, output what remains */
    partialBlock = (unsigned int)outputByteLen;
    KeccakP1600_ExtractBytes(state, curOutput, 0, partialBlock);

    return 0;
}
