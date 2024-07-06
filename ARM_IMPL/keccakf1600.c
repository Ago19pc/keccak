#include "keccakf1600.h"
#include <stdio.h>
#include <arm_neon.h>

#define ROL(a, offset) ((a << offset) ^ (a >> (64-offset)))

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

void KeccakF1600_StatePermute(uint64_t * state){
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
    int round;

    uint64_t Aba, Abe, Abi, Abo, Abu;
    uint64_t Aga, Age, Agi, Ago, Agu;
    uint64_t Aka, Ake, Aki, Ako, Aku;
    uint64_t Ama, Ame, Ami, Amo, Amu;
    uint64_t Asa, Ase, Asi, Aso, Asu;
    uint64_t BCa, BCe, BCi, BCo, BCu;
    uint64_t Da, De, Di, Do, Du;
    uint64_t Eba, Ebe, Ebi, Ebo, Ebu;
    uint64_t Ega, Ege, Egi, Ego, Egu;
    uint64_t Eka, Eke, Eki, Eko, Eku;
    uint64_t Ema, Eme, Emi, Emo, Emu;
    uint64_t Esa, Ese, Esi, Eso, Esu;

    //copyFromState(A, state)
    Aba = state[ 0];
    Abe = state[ 2];
    Abi = state[ 4];
    Abo = state[ 6];
    Abu = state[ 1];
    Aga = state[ 3];
    Age = state[ 5];
    Agi = state[ 7];
    Ago = state[ 8];
    Agu = state[10];
    Aka = state[12];
    Ake = state[14];
    Aki = state[ 9];
    Ako = state[11];
    Aku = state[13];
    Ama = state[15];
    Ame = state[16];
    Ami = state[18];
    Amo = state[20];
    Amu = state[22];
    Asa = state[17];
    Ase = state[19];
    Asi = state[21];
    Aso = state[23];
    Asu = state[24];

    for( round = 0; round < 24; round += 2 ) {
        //    prepareTheta
        BCa = Aba^Aga^Aka^Ama^Asa;
        BCe = Abe^Age^Ake^Ame^Ase;
        BCi = Abi^Agi^Aki^Ami^Asi;
        BCo = Abo^Ago^Ako^Amo^Aso;
        BCu = Abu^Agu^Aku^Amu^Asu;

        //thetaRhoPiChiIotaPrepareTheta(round  , A, E)
        Da = BCu^ROL(BCe, 1);
        De = BCa^ROL(BCi, 1);
        Di = BCe^ROL(BCo, 1);
        Do = BCi^ROL(BCu, 1);
        Du = BCo^ROL(BCa, 1);

        Aba ^= Da;
        BCa = Aba;
        Age ^= De;
        BCe = ROL(Age, 44);
        Aki ^= Di;
        BCi = ROL(Aki, 43);
        Amo ^= Do;
        BCo = ROL(Amo, 21);
        Asu ^= Du;
        BCu = ROL(Asu, 14);
        Eba =   BCa ^((~BCe)&  BCi );
        Eba ^= (uint64_t)round_constants[round];
        Ebe =   BCe ^((~BCi)&  BCo );
        Ebi =   BCi ^((~BCo)&  BCu );
        Ebo =   BCo ^((~BCu)&  BCa );
        Ebu =   BCu ^((~BCa)&  BCe );

        Abo ^= Do;
        BCa = ROL(Abo, 28);
        Agu ^= Du;
        BCe = ROL(Agu, 20);
        Aka ^= Da;
        BCi = ROL(Aka,  3);
        Ame ^= De;
        BCo = ROL(Ame, 45);
        Asi ^= Di;
        BCu = ROL(Asi, 61);
        Ega =   BCa ^((~BCe)&  BCi );
        Ege =   BCe ^((~BCi)&  BCo );
        Egi =   BCi ^((~BCo)&  BCu );
        Ego =   BCo ^((~BCu)&  BCa );
        Egu =   BCu ^((~BCa)&  BCe );

        Abe ^= De;
        BCa = ROL(Abe,  1);
        Agi ^= Di;
        BCe = ROL(Agi,  6);
        Ako ^= Do;
        BCi = ROL(Ako, 25);
        Amu ^= Du;
        BCo = ROL(Amu,  8);
        Asa ^= Da;
        BCu = ROL(Asa, 18);
        Eka =   BCa ^((~BCe)&  BCi );
        Eke =   BCe ^((~BCi)&  BCo );
        Eki =   BCi ^((~BCo)&  BCu );
        Eko =   BCo ^((~BCu)&  BCa );
        Eku =   BCu ^((~BCa)&  BCe );

        Abu ^= Du;
        BCa = ROL(Abu, 27);
        Aga ^= Da;
        BCe = ROL(Aga, 36);
        Ake ^= De;
        BCi = ROL(Ake, 10);
        Ami ^= Di;
        BCo = ROL(Ami, 15);
        Aso ^= Do;
        BCu = ROL(Aso, 56);
        Ema =   BCa ^((~BCe)&  BCi );
        Eme =   BCe ^((~BCi)&  BCo );
        Emi =   BCi ^((~BCo)&  BCu );
        Emo =   BCo ^((~BCu)&  BCa );
        Emu =   BCu ^((~BCa)&  BCe );

        Abi ^= Di;
        BCa = ROL(Abi, 62);
        Ago ^= Do;
        BCe = ROL(Ago, 55);
        Aku ^= Du;
        BCi = ROL(Aku, 39);
        Ama ^= Da;
        BCo = ROL(Ama, 41);
        Ase ^= De;
        BCu = ROL(Ase,  2);
        Esa =   BCa ^((~BCe)&  BCi );
        Ese =   BCe ^((~BCi)&  BCo );
        Esi =   BCi ^((~BCo)&  BCu );
        Eso =   BCo ^((~BCu)&  BCa );
        Esu =   BCu ^((~BCa)&  BCe );

        //    prepareTheta
        BCa = Eba^Ega^Eka^Ema^Esa;
        BCe = Ebe^Ege^Eke^Eme^Ese;
        BCi = Ebi^Egi^Eki^Emi^Esi;
        BCo = Ebo^Ego^Eko^Emo^Eso;
        BCu = Ebu^Egu^Eku^Emu^Esu;

        //thetaRhoPiChiIotaPrepareTheta(round+1, E, A)
        Da = BCu^ROL(BCe, 1);
        De = BCa^ROL(BCi, 1);
        Di = BCe^ROL(BCo, 1);
        Do = BCi^ROL(BCu, 1);
        Du = BCo^ROL(BCa, 1);

        Eba ^= Da;
        BCa = Eba;
        Ege ^= De;
        BCe = ROL(Ege, 44);
        Eki ^= Di;
        BCi = ROL(Eki, 43);
        Emo ^= Do;
        BCo = ROL(Emo, 21);
        Esu ^= Du;
        BCu = ROL(Esu, 14);
        Aba =   BCa ^((~BCe)&  BCi );
        Aba ^= (uint64_t)round_constants[round+1];
        Abe =   BCe ^((~BCi)&  BCo );
        Abi =   BCi ^((~BCo)&  BCu );
        Abo =   BCo ^((~BCu)&  BCa );
        Abu =   BCu ^((~BCa)&  BCe );

        Ebo ^= Do;
        BCa = ROL(Ebo, 28);
        Egu ^= Du;
        BCe = ROL(Egu, 20);
        Eka ^= Da;
        BCi = ROL(Eka, 3);
        Eme ^= De;
        BCo = ROL(Eme, 45);
        Esi ^= Di;
        BCu = ROL(Esi, 61);
        Aga =   BCa ^((~BCe)&  BCi );
        Age =   BCe ^((~BCi)&  BCo );
        Agi =   BCi ^((~BCo)&  BCu );
        Ago =   BCo ^((~BCu)&  BCa );
        Agu =   BCu ^((~BCa)&  BCe );

        Ebe ^= De;
        BCa = ROL(Ebe, 1);
        Egi ^= Di;
        BCe = ROL(Egi, 6);
        Eko ^= Do;
        BCi = ROL(Eko, 25);
        Emu ^= Du;
        BCo = ROL(Emu, 8);
        Esa ^= Da;
        BCu = ROL(Esa, 18);
        Aka =   BCa ^((~BCe)&  BCi );
        Ake =   BCe ^((~BCi)&  BCo );
        Aki =   BCi ^((~BCo)&  BCu );
        Ako =   BCo ^((~BCu)&  BCa );
        Aku =   BCu ^((~BCa)&  BCe );

        Ebu ^= Du;
        BCa = ROL(Ebu, 27);
        Ega ^= Da;
        BCe = ROL(Ega, 36);
        Eke ^= De;
        BCi = ROL(Eke, 10);
        Emi ^= Di;
        BCo = ROL(Emi, 15);
        Eso ^= Do;
        BCu = ROL(Eso, 56);
        Ama =   BCa ^((~BCe)&  BCi );
        Ame =   BCe ^((~BCi)&  BCo );
        Ami =   BCi ^((~BCo)&  BCu );
        Amo =   BCo ^((~BCu)&  BCa );
        Amu =   BCu ^((~BCa)&  BCe );

        Ebi ^= Di;
        BCa = ROL(Ebi, 62);
        Ego ^= Do;
        BCe = ROL(Ego, 55);
        Eku ^= Du;
        BCi = ROL(Eku, 39);
        Ema ^= Da;
        BCo = ROL(Ema, 41);
        Ese ^= De;
        BCu = ROL(Ese, 2);
        Asa =   BCa ^((~BCe)&  BCi );
        Ase =   BCe ^((~BCi)&  BCo );
        Asi =   BCi ^((~BCo)&  BCu );
        Aso =   BCo ^((~BCu)&  BCa );
        Asu =   BCu ^((~BCa)&  BCe );
    }

    //copyToState(state, A)
    state[ 0] = Aba;
    state[ 2] = Abe;
    state[ 4] = Abi;
    state[ 6] = Abo;
    state[ 1] = Abu;
    state[ 3] = Aga;
    state[ 5] = Age;
    state[ 7] = Agi;
    state[ 8] = Ago;
    state[10] = Agu;
    state[12] = Aka;
    state[14] = Ake;
    state[ 9] = Aki;
    state[11] = Ako;
    state[13] = Aku;
    state[15] = Ama;
    state[16] = Ame;
    state[18] = Ami;
    state[20] = Amo;
    state[22] = Amu;
    state[17] = Asa;
    state[19] = Ase;
    state[21] = Asi;
    state[23] = Aso;
    state[24] = Asu;
}
