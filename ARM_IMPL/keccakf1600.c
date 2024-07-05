#include "keccakf1600.h"
#include <stdio.h>
#include <arm_neon.h>


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

void keccakF1600_StatePermute(keccak_state_t * state){
    ALIGN(32) uint64_t round_constants[24] = {
            0x0000000000000001,
            0x0000000000008082,
            0x800000000000808a,
            0x8000000080008000,
            0x000000000000808b,
            0x0000000080000001,
            0x8000000080008081,
            0x8000000000008009,
            0x000000000000008a,
            0x0000000000000088,
            0x0000000080008009,
            0x000000008000000a,
            0x000000008000808b,
            0x800000000000008b,
            0x8000000000008089,
            0x8000000000008003,
            0x8000000000008002,
            0x8000000000000080,
            0x000000000000800a,
            0x800000008000000a,
            0x8000000080008081,
            0x8000000000008080,
            0x0000000080000001,
            0x8000000080008008
    };
    uint64_t *currentStateWord = (uint64_t *)state;
    uint64x2x4_t s0 = vld4q_u64(currentStateWord);
    uint64x2x4_t s1 = vld4q_u64(currentStateWord + 8);
    uint64x2x4_t s2 = vld4q_u64(currentStateWord + 16);
    uint64x2_t s3 = vld1q_u64(currentStateWord + 24); //forse segfault perché s3 è un solo elemento?
    for (uint64_t i = 0; i < 24; i++) {
        //theta
        uint64x2_t v0 = veorq_u64(((uint64x2_t*)(&s0))[0], ((uint64x2_t*)(&s1))[2]);
        uint64x2_t v1 = veorq_u64(((uint64x2_t*)(&s0))[1], ((uint64x2_t*)(&s1))[3]);
        uint64x2_t v2 = veorq_u64(((uint64x2_t*)(&s0))[2], ((uint64x2_t*)(&s2))[1]);
        uint64x2_t v3 = veorq_u64(((uint64x2_t*)(&s0))[3], ((uint64x2_t*)(&s1))[0]);
        uint64x2_t v4 = veorq_u64(((uint64x2_t*)(&s1))[1], ((uint64x2_t*)(&s2))[3]);
        v1 = veorq_u64(v1, ((uint64x2_t*)(&s2))[0]);
        v3 = veorq_u64(v3, ((uint64x2_t*)(&s2))[2]);
        uint64x2_t v5 = vtrn1q_u64(v0, v1);
        uint64x2_t v6 = vtrn2q_u64(v1, v2);
        v1 = veorq_u64(v5, v6);
        v5 = vextq_u64(v4, v2, 1);
        v3 = veorq_u64(v3, v5);
        ((uint64_t*)(&v5))[0] = ((uint64_t*)(&v0))[1];
        v4 = veorq_u64(v4, v5);
        v4 = veorq_u64(v4, s3);
        v2 = vextq_u64(v1, v1, 1);
        ((uint64_t*)(&v4))[1] = ((uint64_t*)(&v3))[0];
        v0 = vtrn2q_u64(v3, v1);
        v5 = vshlq_n_u64(v2, 1);
        v5 = vsriq_n_u64(v5, v2, 63);
        v6 = vshlq_n_u64(v3, 1);
        v6 = vsriq_n_u64(v6, v3, 63);
        uint64x2_t v7 = vshlq_n_u64(v4, 1);
        v7 = vsriq_n_u64(v7, v4, 63);
        uint64x2_t v18 = veorq_u64(v4, v5);
        v2 = veorq_u64(v2, v6);
        v0 = veorq_u64(v0, v7);
        v7 = vextq_u64(v5, v7, 1);
        v7 = veorq_u64(v3, v7);
        v6 = vextq_u64(v6, v5, 1);
        v4 = vtrn1q_u64(v1, v4);
        v6 = veorq_u64(v4, v6);
        ((uint64x2_t*)(&s0))[0] = veorq_u64(((uint64x2_t*)(&s0))[0], v18);
        ((uint64x2_t*)(&s0))[1] = veorq_u64(((uint64x2_t*)(&s0))[1], v6);
        ((uint64x2_t*)(&s0))[2] = veorq_u64(((uint64x2_t*)(&s0))[2], v2);
        ((uint64x2_t*)(&s0))[3] = veorq_u64(((uint64x2_t*)(&s0))[3], v0);
        ((uint64x2_t*)(&s1))[0] = veorq_u64(((uint64x2_t*)(&s1))[0], v0);
        ((uint64x2_t*)(&s1))[1] = veorq_u64(((uint64x2_t*)(&s1))[1], v7);
        ((uint64x2_t*)(&s1))[2] = veorq_u64(((uint64x2_t*)(&s1))[2], v18);
        ((uint64x2_t*)(&s1))[3] = veorq_u64(((uint64x2_t*)(&s1))[3], v6);
        ((uint64x2_t*)(&s2))[0] = veorq_u64(((uint64x2_t*)(&s2))[0], v6);
        ((uint64x2_t*)(&s2))[1] = veorq_u64(((uint64x2_t*)(&s2))[1], v2);
        ((uint64x2_t*)(&s2))[2] = veorq_u64(((uint64x2_t*)(&s2))[2], v0);
        ((uint64x2_t*)(&s2))[3] = veorq_u64(((uint64x2_t*)(&s2))[3], v7);
        s3 = veorq_u64(s3, v7);
        //ro pi
        uint64_t temp1 = ((uint64_t*)(&s1))[4];
        ((uint64_t*)(&s1))[4] = ((uint64_t*)(&s0))[2] << 1 | ((uint64_t*)(&s0))[2] >> 63;
        uint64_t temp2 = ((uint64_t*)(&s0))[7];
        ((uint64_t*)(&s0))[7] = temp1 << 3 | temp1 >> 61;
        temp1 = ((uint64_t*)(&s1))[6];
        ((uint64_t*)(&s1))[6] = temp2 << 6 | temp2 >> 58;
        temp2 = ((uint64_t*)(&s2))[2];
        ((uint64_t*)(&s2))[2] = temp1 << 10 | temp1 >> 54;
        temp1 = ((uint64_t*)(&s2))[4];
        ((uint64_t*)(&s2))[4] = temp2 << 15 | temp2 >> 49;
        temp2 = ((uint64_t*)(&s0))[6];
        ((uint64_t*)(&s0))[6] = temp1 << 21 | temp1 >> 43;
        temp1 = ((uint64_t*)(&s0))[3];
        ((uint64_t*)(&s0))[3] = temp2 << 28 | temp2 >> 36;
        temp2 = ((uint64_t*)(&s2))[0];
        ((uint64_t*)(&s2))[0] = temp1 << 36 | temp1 >> 28;
        temp1 = ((uint64_t*)(&s2))[3];
        ((uint64_t*)(&s2))[3] = temp2 << 45 | temp2 >> 19;
        temp2 = ((uint64_t*)(&s3))[0];
        ((uint64_t*)(&s3))[0] = temp1 << 55 | temp1 >> 9;
        temp1 = ((uint64_t*)(&s0))[1];
        ((uint64_t*)(&s0))[1] = temp2 << 2 | temp2 >> 62;
        temp2 = ((uint64_t*)(&s1))[7];
        ((uint64_t*)(&s1))[7] = temp1 << 14 | temp1 >> 50;
        temp1 = ((uint64_t*)(&s2))[7];
        ((uint64_t*)(&s2))[7] = temp2 << 27 | temp2 >> 37;
        temp2 = ((uint64_t*)(&s2))[6];
        ((uint64_t*)(&s2))[6] = temp1 << 41 | temp1 >> 23;
        temp1 = ((uint64_t*)(&s1))[3];
        ((uint64_t*)(&s1))[3] = temp2 << 56 | temp2 >> 8;
        temp2 = ((uint64_t*)(&s1))[1];
        ((uint64_t*)(&s1))[1] = temp1 << 8 | temp1 >> 56;
        temp1 = ((uint64_t*)(&s0))[4];
        ((uint64_t*)(&s0))[4] = temp2 << 25 | temp2 >> 39;
        temp2 = ((uint64_t*)(&s2))[1];
        ((uint64_t*)(&s2))[1] = temp1 << 43 | temp1 >> 21;
        temp1 = ((uint64_t*)(&s1))[5];
        ((uint64_t*)(&s1))[5] = temp2 << 62 | temp2 >> 2;
        temp2 = ((uint64_t*)(&s2))[5];
        ((uint64_t*)(&s2))[5] = temp1 << 18 | temp1 >> 46;
        temp1 = ((uint64_t*)(&s1))[2];
        ((uint64_t*)(&s1))[2] = temp2 << 39 | temp2 >> 25;
        temp2 = ((uint64_t*)(&s0))[5];
        ((uint64_t*)(&s0))[5] = temp1 << 61 | temp1 >> 3;
        ((uint64_t*)(&s0))[2] = temp2 << 44 | temp2 >> 20;
        //chi
        v18 = vextq_u64(((uint64x2_t*)(&s1))[3], s3, 1);
        v6 = vbicq_u64(((uint64x2_t*)(&s2))[0], v18);
        uint64x2_t v17 = vextq_u64(((uint64x2_t*)(&s1))[3], s3, 1);
        v5 = vbicq_u64(v17, ((uint64x2_t*)(&s2))[3]);
        v3 = vbicq_u64(((uint64x2_t*)(&s2))[3], ((uint64x2_t*)(&s2))[2]);
        ((uint64x2_t*)(&s2))[3] = veorq_u64(((uint64x2_t*)(&s2))[3], v6);
        v18 = vtrn1q_u64(((uint64x2_t*)(&s1))[3], ((uint64x2_t*)(&s1))[2]);
        v17 = vextq_u64(((uint64x2_t*)(&s1))[0], ((uint64x2_t*)(&s1))[3], 1);
        v7 = vbicq_u64(v17, v18);
        v18 = vtrn2q_u64(((uint64x2_t*)(&s0))[1], ((uint64x2_t*)(&s1))[2]);
        v17 = vextq_u64(((uint64x2_t*)(&s0))[2], ((uint64x2_t*)(&s1))[2], 1);
        v6 = vbicq_u64(v17, v18);
        v18 = vtrn1q_u64(((uint64x2_t*)(&s0))[1], ((uint64x2_t*)(&s0))[0]);
        v17 = vtrn1q_u64(((uint64x2_t*)(&s0))[2], ((uint64x2_t*)(&s0))[1]);
        v1 = vbicq_u64(v17, v18);
        v18 = vextq_u64(((uint64x2_t*)(&s0))[0], ((uint64x2_t*)(&s1))[0], 1);
        v17 = vtrn1q_u64(((uint64x2_t*)(&s0))[0], ((uint64x2_t*)(&s1))[1]);
        v0 = vbicq_u64(v17, v18);
        v18 = vextq_u64(((uint64x2_t*)(&s1))[0], ((uint64x2_t*)(&s2))[0], 1);
        v17 = vextq_u64(((uint64x2_t*)(&s1))[1], ((uint64x2_t*)(&s2))[1], 1);
        v4 = vbicq_u64(v17, v18);
        ((uint64_t*)(&v18))[0] = ((uint64_t*)(&s2))[1];
        ((uint64_t*)(&v17))[0] = ((uint64_t*)(&s2))[3];
        v2 = vbicq_u64(v17, v18);
        s3 = veorq_u64(s3, v2);
        v2 = vbicq_u64(((uint64x2_t*)(&s2))[2], ((uint64x2_t*)(&s2))[1]);
        ((uint64x2_t*)(&s2))[0] = veorq_u64(((uint64x2_t*)(&s2))[0], v2);
        v2 = vbicq_u64(((uint64x2_t*)(&s0))[3], ((uint64x2_t*)(&s0))[2]);
        ((uint64x2_t*)(&s2))[1] = veorq_u64(((uint64x2_t*)(&s2))[1], v3);
        ((uint64x2_t*)(&s2))[2] = veorq_u64(((uint64x2_t*)(&s2))[2], v5);
        v17 = vextq_u64(((uint64x2_t*)(&s0))[0], ((uint64x2_t*)(&s1))[0], 1);
        v3 = vbicq_u64(v17, ((uint64x2_t*)(&s0))[3]);
        v17 = vtrn2q_u64(((uint64x2_t*)(&s0))[1], ((uint64x2_t*)(&s1))[2]);
        v5 = vbicq_u64(v17, ((uint64x2_t*)(&s1))[1]);
        ((uint64x2_t*)(&s0))[0] = veorq_u64(((uint64x2_t*)(&s0))[0], v1);
        ((uint64x2_t*)(&s0))[1] = veorq_u64(((uint64x2_t*)(&s0))[1], v2);
        ((uint64x2_t*)(&s0))[2] = veorq_u64(((uint64x2_t*)(&s0))[2], v3);
        ((uint64x2_t*)(&s0))[3] = veorq_u64(((uint64x2_t*)(&s0))[3], v0);
        ((uint64x2_t*)(&s1))[0] = veorq_u64(((uint64x2_t*)(&s1))[0], v5);
        ((uint64x2_t*)(&s1))[1] = veorq_u64(((uint64x2_t*)(&s1))[1], v6);
        ((uint64x2_t*)(&s1))[2] = veorq_u64(((uint64x2_t*)(&s1))[2], v7);
        ((uint64x2_t*)(&s1))[3] = veorq_u64(((uint64x2_t*)(&s1))[3], v4);
        //iota
        uint64x2_t roundConstant = {0, 0};
        ((uint64_t*)(&roundConstant))[0] = round_constants[i]; //forse segfault perché roundConstant va inizializzato prima?
        ((uint64x2_t*)(&s0))[0] = veorq_u64(((uint64x2_t*)(&s0))[0], roundConstant);
    }
    vst4q_u64(currentStateWord, s0);
    vst4q_u64(currentStateWord + 8, s1);
    vst4q_u64(currentStateWord + 16, s2);
    vst1q_u64(currentStateWord + 24, s3);
}
