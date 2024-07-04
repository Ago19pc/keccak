#include "keccakf1600.h"
#include <stdio.h>
#include <immintrin.h>


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

/**
 * non modifica lo stato. Vuole stato iniziale interlacciato. Data (l'output) diventa lo stato NON INTERLACCIATO
 */
void KeccakF1600_StateExtractBytes(keccak_state_t *state, unsigned char *data,
                                   unsigned int offset, unsigned int length)
{
    uint64_t  zero[25] = {0};
    uint64_t  *extractedData;
    if(length == 200 && !offset){ //chiamata dalla XOR
        extractedData = (uint64_t*)data;
    } else { //chiamata dalla Squeeze
        extractedData = zero;
    }
    uint64x2x4_t s1 = {state->a04, state->a15, state->a26, state->a37};
    vst4q_u64(extractedData, s1);
    uint64x2x4_t s2 = {state->a812, state->a913, state->a1014, state->a1115};
    vst4q_u64(extractedData + 8, s2);
    uint64x2x4_t s3 = {state->a1620, state->a1721, state->a1822, state->a1923};
    vst4q_u64(extractedData + 16, s3);
    extractedData[24] = state->a24;
    if (extractedData == zero) //modifico data solo se sto venendo chiamato dalla squeeze (altrimenti è ridondante)
        memcpy(data, (uint8_t *)zero + offset, length);
}
/**
 * Vuole lo stato iniziale interlacciato. Ritorna il nuovo stato interlacciato. Vuole data NON interlacciato
 */
void KeccakF1600_StateXORBytes(keccak_state_t *state, const unsigned char *data,
                               unsigned int offset, unsigned int length)
{
    uint64_t*     dataWords = (uint64_t *)data;
    uint64_t      stateWords[25];
    uint64_t*     firstStateWord = (uint64_t *)((uint8_t *)stateWords + offset);
    KeccakF1600_StateExtractBytes(state, (uint8_t *)stateWords, 0, 200);
    while (length > 16) {
        uint64x2_t stateVec = vld1q_u64(firstStateWord);
        uint64x2_t dataVec = vld1q_u64(dataWords);
        stateVec = veorq_u64(stateVec, dataVec);
        vst1q_u64(firstStateWord, stateVec);
        firstStateWord += 2;
        dataWords += 2;
        length -= 16;
    }
    if (length > 8) {
        *firstStateWord ^= *dataWords;
        firstStateWord++;
        dataWords++;
        length -= 8;
    }
    for (int i = 0; i < length; i++) {
      *((uint8_t *) firstStateWord + i) ^= *((uint8_t *) dataWords + i);
    }
    uint64x2_t s04 = {stateWords[0], stateWords[4]};
    vst1q_u64((uint64_t*)&(state->a04), s04);
    uint64x2_t s15 = {stateWords[1], stateWords[5]};
    vst1q_u64((uint64_t*)&(state->a15), s15);
    uint64x2_t s26 = {stateWords[2], stateWords[6]};
    vst1q_u64((uint64_t*)&(state->a26), s26);
    uint64x2_t s37 = {stateWords[3], stateWords[7]};
    vst1q_u64((uint64_t*)&(state->a37), s37);
    uint64x2_t s812 = {stateWords[8], stateWords[12]};
    vst1q_u64((uint64_t*)&(state->a812), s812);
    uint64x2_t s913 = {stateWords[9], stateWords[13]};
    vst1q_u64((uint64_t*)&(state->a913), s913);
    uint64x2_t s1014 = {stateWords[10], stateWords[14]};
    vst1q_u64((uint64_t*)&(state->a1014), s1014);
    uint64x2_t s1115 = {stateWords[11], stateWords[15]};
    vst1q_u64((uint64_t*)&(state->a1115), s1115);
    uint64x2_t s1620 = {stateWords[16], stateWords[20]};
    vst1q_u64((uint64_t*)&(state->a1620), s1620);
    uint64x2_t s1721 = {stateWords[17], stateWords[21]};
    vst1q_u64((uint64_t*)&(state->a1721), s1721);
    uint64x2_t s1822 = {stateWords[18], stateWords[22]};
    vst1q_u64((uint64_t*)&(state->a1822), s1822);
    uint64x2_t s1923 = {stateWords[19], stateWords[23]};
    vst1q_u64((uint64_t*)&(state->a1923), s1923);
    state->a24 = stateWords[24];
}
