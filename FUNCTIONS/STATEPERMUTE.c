
#include <stdint.h>
#include <immintrin.h>
#include "../Utils/intrinUtils.h"

#define NROUNDS 24

static const uint64_t RoundCostants[NROUNDS] = {
    (uint64_t)0x0000000000000001ULL,
   (uint64_t)0x0000000000008082ULL,
   (uint64_t)0x800000000000808aULL,
   (uint64_t)0x8000000080008000ULL,
   (uint64_t)0x000000000000808bULL,
   (uint64_t)0x0000000080000001ULL,
   (uint64_t)0x8000000080008081ULL,
   (uint64_t)0x8000000000008009ULL,
   (uint64_t)0x000000000000008aULL,
   (uint64_t)0x0000000000000088ULL,
   (uint64_t)0x0000000080008009ULL,
   (uint64_t)0x000000008000000aULL,
   (uint64_t)0x000000008000808bULL,
   (uint64_t)0x800000000000008bULL,
   (uint64_t)0x8000000000008089ULL,
   (uint64_t)0x8000000000008003ULL,
   (uint64_t)0x8000000000008002ULL,
   (uint64_t)0x8000000000000080ULL,
   (uint64_t)0x000000000000800aULL,
   (uint64_t)0x800000008000000aULL,
   (uint64_t)0x8000000080008081ULL,
   (uint64_t)0x8000000000008080ULL,
   (uint64_t)0x0000000080000001ULL,
   (uint64_t)0x8000000080008008ULL
};




__m256i ROL1 (__m256i a) {
    __m256i b = _mm256_slli_epi64(a, 1);
    __m256i c = _mm256_srli_epi64(a, 63);
    b = _mm256_or_si256(b, c);
    return b;

}



void StatePermute(uint64_t* state){

    //printf("Implementation Permutation\n");
    // Ogni paROLa dello stato è da 64 bit, 
    // in un vettore ce ne vanno 4 ma ne servono 5 quindi ne servono 2 di cui uno con 4 elementi e uno con 1 gli altri 3 sono 0
    __m256i a[5][2];
                    // Aba Abe Abi Abo | Abu 
                    // Aga Age Agi Ago | Agu
                    // Aka Ake Aki Ako | Aku
                    // Ama Ame Ami Amo | Amu
                    // Asa Ase Asi Aso | Asu

                    
    #define Aba_Index 0
    #define Aba_Where a[0][0]
    #define Aba getElement_64(Aba_Where, Aba_Index)
    #define Abe_Index 1
    #define Abe_Where a[0][0]
    #define Abe getElement_64(Abe_Where, Abe_Index)
    #define Abi_Index 2
    #define Abi_Where a[0][0]
    #define Abi getElement_64(Abi_Where, Abi_Index)
    #define Abo_Index 3
    #define Abo_Where a[0][0]
    #define Abo getElement_64(Abo_Where, Abo_Index)
    #define Abu_Index 0
    #define Abu_Where a[0][1]
    #define Abu getElement_64(Abu_Where, Abu_Index)

    #define Aga_Index 0
    #define Aga_Where a[1][0]
    #define Aga getElement_64(Aga_Where, Aga_Index)
    #define Age_Index 1
    #define Age_Where a[1][0]
    #define Age getElement_64(Age_Where, Age_Index)
    #define Agi_Index 2
    #define Agi_Where a[1][0]
    #define Agi getElement_64(Agi_Where, Agi_Index)
    #define Ago_Index 3
    #define Ago_Where a[1][0]
    #define Ago getElement_64(Ago_Where, Ago_Index)
    #define Agu_Index 0
    #define Agu_Where a[1][1]
    #define Agu getElement_64(Agu_Where, Agu_Index)

    #define Aka_Index 0
    #define Aka_Where a[2][0]
    #define Aka getElement_64(Aka_Where, Aka_Index)
    #define Ake_Index 1
    #define Ake_Where a[2][0]
    #define Ake getElement_64(Ake_Where, Ake_Index)
    #define Aki_Index 2
    #define Aki_Where a[2][0]
    #define Aki getElement_64(Aki_Where, Aki_Index)
    #define Ako_Index 3
    #define Ako_Where a[2][0]
    #define Ako getElement_64(Ako_Where, Ako_Index)
    #define Aku_Index 0
    #define Aku_Where a[2][1]
    #define Aku getElement_64(Aku_Where, Aku_Index)

    #define Ama_Index 0
    #define Ama_Where a[3][0]
    #define Ama getElement_64(Ama_Where, Ama_Index)
    #define Ame_Index 1
    #define Ame_Where a[3][0]
    #define Ame getElement_64(Ame_Where, Ame_Index)
    #define Ami_Index 2
    #define Ami_Where a[3][0]
    #define Ami getElement_64(Ami_Where, Ami_Index)
    #define Amo_Index 3
    #define Amo_Where a[3][0]
    #define Amo getElement_64(Amo_Where, Amo_Index)
    #define Amu_Index 0
    #define Amu_Where a[3][1]
    #define Amu getElement_64(Amu_Where, Amu_Index)
    
    #define Asa_Index 0
    #define Asa_Where a[4][0]
    #define Asa getElement_64(Asa_Where, Asa_Index)
    #define Ase_Index 1
    #define Ase_Where a[4][0]
    #define Ase getElement_64(Ase_Where, Ase_Index)
    #define Asi_Index 2
    #define Asi_Where a[4][0]
    #define Asi getElement_64(Asi_Where, Asi_Index)
    #define Aso_Index 3
    #define Aso_Where a[4][0]
    #define Aso getElement_64(Aso_Where, Aso_Index)
    #define Asu_Index 0
    #define Asu_Where a[4][1]
    #define Asu getElement_64(Asu_Where, Asu_Index)


    __m256i b[2];   // BCa BCe BCi BCo | BCu

    #define BCa_Index 0
    #define BCa_Where b[0]
    #define BCa getElement_64(BCa_Where, BCa_Index)
    #define BCe_Index 1
    #define BCe_Where b[0]
    #define BCe getElement_64(BCe_Where, BCe_Index)
    #define BCi_Index 2
    #define BCi_Where b[0]
    #define BCi getElement_64(BCi_Where, BCi_Index)
    #define BCo_Index 3
    #define BCo_Where b[0]
    #define BCo getElement_64(BCo_Where, BCo_Index)
    #define BCu_Index 0
    #define BCu_Where b[1]
    #define BCu getElement_64(BCu_Where, BCu_Index)


    __m256i d[2];   // Da De Di Do | Du 0 0 0
                    // BCu^ROL(BCe, 1) BCa^ROL(BCi, 1) BCe^ROL(BCo, 1) BCi^ROL(BCu, 1) | BCo^ROL(BCa, 1) 0 0 0
                    // Da notare che nel primo elemento va BCu^ROL(BCe, 1)!
    #define Da_Index 0
    #define Da_Where d[0]
    #define Da getElement_64(Da_Where, Da_Index)
    #define De_Index 1
    #define De_Where d[0]
    #define De getElement_64(De_Where, De_Index)
    #define Di_Index 2
    #define Di_Where d[0]
    #define Di getElement_64(Di_Where, Di_Index)
    #define Do_Index 3
    #define Do_Where d[0]
    #define Do getElement_64(Do_Where, Do_Index)
    #define Du_Index 0
    #define Du_Where d[1]
    #define Du getElement_64(Du_Where, Du_Index)
    

    __m256i e[5][2];    // Eba Ebe Ebi Ebo | Ebu
                        // Ega Ege Egi Ego | Egu
                        // Eka Eke Eki Eko | Eku
                        // Ema Eme Emi Emo | Emu
                        // Esa Ese Esi Eso | Esu
    #define Eba_Index 0
    #define Eba_Where e[0][0]
    #define Eba getElement_64(Eba_Where, Eba_Index)
    #define Ebe_Index 1
    #define Ebe_Where e[0][0]
    #define Ebe getElement_64(Ebe_Where, Ebe_Index)
    #define Ebi_Index 2
    #define Ebi_Where e[0][0]
    #define Ebi getElement_64(Ebi_Where, Ebi_Index)
    #define Ebo_Index 3
    #define Ebo_Where e[0][0]
    #define Ebo getElement_64(Ebo_Where, Ebo_Index)
    #define Ebu_Index 0
    #define Ebu_Where e[0][1]
    #define Ebu getElement_64(Ebu_Where, Ebu_Index)

    #define Ega_Index 0
    #define Ega_Where e[1][0]
    #define Ega getElement_64(Ega_Where, Ega_Index)
    #define Ege_Index 1
    #define Ege_Where e[1][0]
    #define Ege getElement_64(Ege_Where, Ege_Index)
    #define Egi_Index 2
    #define Egi_Where e[1][0]
    #define Egi getElement_64(Egi_Where, Egi_Index)
    #define Ego_Index 3
    #define Ego_Where e[1][0]
    #define Ego getElement_64(Ego_Where, Ego_Index)
    #define Egu_Index 0
    #define Egu_Where e[1][1]
    #define Egu getElement_64(Egu_Where, Egu_Index)

    #define Eka_Index 0
    #define Eka_Where e[2][0]
    #define Eka getElement_64(Eka_Where, Eka_Index)
    #define Eke_Index 1
    #define Eke_Where e[2][0]
    #define Eke getElement_64(Eke_Where, Eke_Index)
    #define Eki_Index 2
    #define Eki_Where e[2][0]
    #define Eki getElement_64(Eki_Where, Eki_Index)
    #define Eko_Index 3
    #define Eko_Where e[2][0]
    #define Eko getElement_64(Eko_Where, Eko_Index)
    #define Eku_Index 0
    #define Eku_Where e[2][1]
    #define Eku getElement_64(Eku_Where, Eku_Index)

    #define Ema_Index 0
    #define Ema_Where e[3][0]
    #define Ema getElement_64(Ema_Where, Ema_Index)
    #define Eme_Index 1
    #define Eme_Where e[3][0]
    #define Eme getElement_64(Eme_Where, Eme_Index)
    #define Emi_Index 2
    #define Emi_Where e[3][0]
    #define Emi getElement_64(Emi_Where, Emi_Index)
    #define Emo_Index 3
    #define Emo_Where e[3][0]
    #define Emo getElement_64(Emo_Where, Emo_Index)
    #define Emu_Index 0
    #define Emu_Where e[3][1]
    #define Emu getElement_64(Emu_Where, Emu_Index)

    #define Esa_Index 0
    #define Esa_Where e[4][0]
    #define Esa getElement_64(Esa_Where, Esa_Index)
    #define Ese_Index 1
    #define Ese_Where e[4][0]
    #define Ese getElement_64(Ese_Where, Ese_Index)
    #define Esi_Index 2
    #define Esi_Where e[4][0]
    #define Esi getElement_64(Esi_Where, Esi_Index)
    #define Eso_Index 3
    #define Eso_Where e[4][0]
    #define Eso getElement_64(Eso_Where, Eso_Index)
    #define Esu_Index 0
    #define Esu_Where e[4][1]
    #define Esu getElement_64(Esu_Where, Esu_Index)


    // Copy from state
    // a1 = 0  1  2  3  | 4
    // a2 = 5  6  7  8  | 9
    // a3 = 10 11 12 13 |14
    // a4 = 15 16 17 18 | 19
    // a5 = 20 21 22 23 | 24


    for (int i = 0; i < 5; i++){
        a[i][0] = _mm256_set_epi64x(state[i*5+3], state[i*5+2], state[i*5+1], state[i*5]);
        a[i][1] = _mm256_set_epi64x(0, 0, 0, state[i*5+4]);
    }

    for (int round = 0; round < NROUNDS; round += 2) {
        // b = a1 ^ a2 ^ a3 ^ a4 ^ a5
        // (Aba ^ Aga ^ Aka ^ Ama ^ Asa) (Abe ^ Age ^ Ake ^ Ame ^ Ase) (Abi ^ Agi ^ Aki ^ Ami ^ Asi) (Abo ^ Ago ^ Ako ^ Amo ^ Aso) | (Abu ^ Agu ^ Aku ^ Amu ^ Asu)
        b[0] = _mm256_xor_si256(a[0][0], a[1][0]);
        b[1] = _mm256_xor_si256(a[0][1], a[1][1]);

        b[0] = _mm256_xor_si256(b[0], a[2][0]);
        b[1] = _mm256_xor_si256(b[1], a[2][1]);

        b[0] = _mm256_xor_si256(b[0], a[3][0]);
        b[1] = _mm256_xor_si256(b[1], a[3][1]);

        b[0] = _mm256_xor_si256(b[0], a[4][0]);
        b[1] = _mm256_xor_si256(b[1], a[4][1]);
        //print_vector_64(b[0]);
        //print_vector_64(b[1]);

        // Creo una copia di B per il ROL riordinandolo da BCa BCe BCi BCo | BCu 
                                                    //   a BCe BCi BCo BCu | BCa
        __m256i temp[2];
        temp[0] = _setElement_64(b[0], 0, BCe);    // Metti Bce
        temp[0] = _setElement_64(temp[0], 1, BCi); // Metti Bci
        temp[0] = _setElement_64(temp[0], 2, BCo); // Metti Bco
        temp[0] = _setElement_64(temp[0], 3, BCu); // Metti Bcu

        temp[1] = _mm256_setzero_si256();                             // Inizializzo a 0
        temp[1] = _setElement_64(temp[1], 0, BCa); // Metti Bca 

        // Esegui vettorialmente il ROL1
        temp[0] = ROL1(temp[0]);
        temp[1] = ROL1(temp[1]);

        // Genera il vettore da XOR con temp 
        // BCu BCa BCe BCi | BCo
        d[0] = _setElement_64(b[0], 0, BCu);     // BCu
        d[0] = _setElement_64(d[0], 1, BCa);     // BCa
        d[0] = _setElement_64(d[0], 2, BCe);     // BCe
        d[0] = _setElement_64(d[0], 3, BCi);     // BCi

        d[1] = _mm256_setzero_si256();                              // Inizializzo a 0
        d[1] = _setElement_64(d[1], 0, BCo);     // BCo

        // XOR con il vettore temp
        d[0] = _mm256_xor_si256(d[0], temp[0]);
        d[1] = _mm256_xor_si256(d[1], temp[1]);

        //print_vector_64(d[0]);
        //print_vector_64(d[1]);



        // INSTRUCTION                                      ORDER OF EVENT

        // Aba ^= Da;                                       i
        // BCa = Aba;                                       ii

        // Age ^= De;                                       i
        // BCe = ROL(Age, 44);                              ii

        // Aki ^= Di;                                       i
        // BCi = ROL(Aki, 43);                              ii

        // Amo ^= Do;                                       i
        // BCo = ROL(Amo, 21);                              ii

        // Asu ^= Du;                                       i
        // BCu = ROL(Asu, 14);                              ii

        // Eba =   BCa ^((~BCe)&  BCi );                    iii
        // Eba ^= (uint64_t)KeccakF_RoundConstants[round];  iv

        // Ebe =   BCe ^((~BCi)&  BCo );                    iii

        // Ebi =   BCi ^((~BCo)&  BCu );                    iii

        // Ebo =   BCo ^((~BCu)&  BCa );                    iii

        // Ebu =   BCu ^((~BCa)&  BCe );                    iii


        // EVENT i
        // Aba ^= Da; Age ^= De; Aki ^= Di; Amo ^= Do; Asu ^= Du;

        // Aba Age Aki Amo Asu sono la diagonale principale
        // Estrai elementi da a[][]
        __m256i step1[2];
        step1[0] = _to_256_Vector_64 (Aba, Age, Aki, Amo);
        step1[1] = _to_256_Vector_64 (Asu, 0, 0, 0);
        // Calcola
        step1[0] = _mm256_xor_si256(step1[0], d[0]);
        step1[1] = _mm256_xor_si256(step1[1], d[1]);
        // Rimetti gli elementi in a[][]
        Aba_Where = _setElement_64(Aba_Where, Aba_Index, getElement_64(step1[0], 0));
        Age_Where = _setElement_64(Age_Where, Age_Index, getElement_64(step1[0], 1));
        Aki_Where = _setElement_64(Aki_Where, Aki_Index, getElement_64(step1[0], 2));
        Amo_Where = _setElement_64(Amo_Where, Amo_Index, getElement_64(step1[0], 3));
        Asu_Where = _setElement_64(Asu_Where, Asu_Index, getElement_64(step1[1], 0));

        // EVENT ii
        // BCa = Aba; BCe = ROL(Age, 44); BCi = ROL(Aki, 43); BCo = ROL(Amo, 21); BCu = ROL(Asu, 14);
        b[0] = _setElement_64(b[0], 0, Aba);
        b[0] = _setElement_64(b[0], 1, ROL(Age, 44));
        b[0] = _setElement_64(b[0], 2, ROL(Aki, 43));
        b[0] = _setElement_64(b[0], 3, ROL(Amo, 21));
        b[1] = _setElement_64(b[1], 0, ROL(Asu, 14));

        // EVENT iii
        // Eba = BCa ^((~BCe)&  BCi ); 
        // Ebe = BCe ^((~BCi)&  BCo ); 
        // Ebi = BCi ^((~BCo)&  BCu ); 
        // Ebo = BCo ^((~BCu)&  BCa ); 
        // Ebu = BCu ^((~BCa)&  BCe );

        // VEC1 ^ ((not VEC2) & VEC3)
        __m256i VEC1[2];
        VEC1[0] = _to_256_Vector_64(BCa, BCe, BCi, BCo);
        VEC1[1] = _to_256_Vector_64(BCu, 0, 0, 0);

        __m256i VEC2[2];
        VEC2[0] = _to_256_Vector_64(BCe, BCi, BCo, BCu);
        VEC2[1] = _to_256_Vector_64(BCa, 0, 0, 0);

        // Negate VEC2
        VEC2[0] = negate(VEC2[0]);
        VEC2[1] = negate(VEC2[1]);
        __m256i VEC3[2];
        VEC3[0] = _to_256_Vector_64(BCi, BCo, BCu, BCa);
        VEC3[1] = _to_256_Vector_64(BCe, 0, 0, 0);


        // Calculate AND
        VEC2[0] = _mm256_and_si256(VEC2[0], VEC3[0]);
        VEC2[1] = _mm256_and_si256(VEC2[1], VEC3[1]);

        // Calculate XOR
        e[0][0] = _mm256_xor_si256(VEC1[0], VEC2[0]);
        Ebu_Where = _setElement_64(Ebu_Where, Ebu_Index, getElement_64(VEC1[1], 0) ^ getElement_64(VEC2[1], 0));


        // EVENT iv
        // Eba ^= (uint64_t)KeccakF_RoundConstants[round];
        Eba_Where = _setElement_64(Eba_Where, Eba_Index, getElement_64(e[0][0], 0) ^ RoundCostants[round]);



        /*
        ISTRUZIONE                          EVENTO
        Abo ^= Do;                          i
        BCa = ROL(Abo, 28);                 ii
        Agu ^= Du;                          i
        BCe = ROL(Agu, 20);                 ii
        Aka ^= Da;                          i
        BCi = ROL(Aka,  3);                 ii
        Ame ^= De;                          i
        BCo = ROL(Ame, 45);                 ii
        Asi ^= Di;                          i
        BCu = ROL(Asi, 61);                 ii
        Ega =   BCa ^((~BCe)&  BCi );       iii
        Ege =   BCe ^((~BCi)&  BCo );       iii
        Egi =   BCi ^((~BCo)&  BCu );       iii
        Ego =   BCo ^((~BCu)&  BCa );       iii
        Egu =   BCu ^((~BCa)&  BCe );       iii
        */

        // EVENT i
        // Abo ^= Do; Agu ^= Du; Aka ^= Da; Ame ^= De; Asi ^= Di;
        __m256i LEFTXOR[2];
        LEFTXOR[0] = _to_256_Vector_64(Abo, Agu, Aka, Ame);
        LEFTXOR[1] = _to_256_Vector_64(Asi, 0, 0, 0);

        __m256i XORRIGHT[2];
        XORRIGHT[0] = _to_256_Vector_64(Do, Du, Da, De);
        XORRIGHT[1] = _to_256_Vector_64(Di, 0, 0, 0);

        LEFTXOR[0] = _mm256_xor_si256(LEFTXOR[0], XORRIGHT[0]);
        LEFTXOR[1] = _mm256_xor_si256(LEFTXOR[1], XORRIGHT[1]);

        // Rimetti in memoria i valori calcolati
        // Abo Agu Aka Ame Asi

        // Abo a[0][0] index 3
        // Agu a[1][1] index 0
        // Aka a[2][0] index 0
        // Ame a[3][0] index 1
        // Asi a[4][0] index 2
        Abo_Where = _setElement_64(Abo_Where, Abo_Index, getElement_64(LEFTXOR[0], 0));
        Agu_Where = _setElement_64(Agu_Where, Agu_Index, getElement_64(LEFTXOR[0], 1));
        Aka_Where = _setElement_64(Aka_Where, Aka_Index, getElement_64(LEFTXOR[0], 2));
        Ame_Where = _setElement_64(Ame_Where, Ame_Index, getElement_64(LEFTXOR[0], 3));
        Asi_Where = _setElement_64(Asi_Where, Asi_Index, getElement_64(LEFTXOR[1], 0));

        // EVENT ii
        // BCa = ROL(Abo, 28); BCe = ROL(Agu, 20); BCi = ROL(Aka,  3); BCo = ROL(Ame, 45); BCu = ROL(Asi, 61);
        BCa_Where = _setElement_64(BCa_Where, BCa_Index, ROL(Abo, 28));
        BCe_Where = _setElement_64(BCe_Where, BCe_Index, ROL(Agu, 20));
        BCi_Where = _setElement_64(BCi_Where, BCi_Index, ROL(Aka, 3));
        BCo_Where = _setElement_64(BCo_Where, BCo_Index, ROL(Ame, 45));
        BCu_Where = _setElement_64(BCu_Where, BCu_Index, ROL(Asi, 61));

        // EVENT iii
        // Ega =   BCa ^((~BCe)&  BCi );
        // Ege =   BCe ^((~BCi)&  BCo );
        // Egi =   BCi ^((~BCo)&  BCu );
        // Ego =   BCo ^((~BCu)&  BCa );
        // Egu =   BCu ^((~BCa)&  BCe );

        // e[1][0] = VEC1[0] ^ ((not VEC2[0]) & VEC3[0]) Dove e[1][0] = Ega Ege Egi Ego
        // e[1][1] = VEC1[1] ^ ((not VEC2[1]) & VEC3[1]) Dove e[1][1] = Egu 0 0 0
        VEC1[0] = _to_256_Vector_64(BCa, BCe, BCi, BCo);
        VEC1[1] = _to_256_Vector_64(BCu, 0, 0, 0);

        VEC2[0] = _to_256_Vector_64(BCe, BCi, BCo, BCu);
        VEC2[1] = _to_256_Vector_64(BCa, 0, 0, 0);

        VEC2[0] = negate(VEC2[0]);
        VEC2[1] = negate(VEC2[1]);

        VEC3[0] = _to_256_Vector_64(BCi, BCo, BCu, BCa);
        VEC3[1] = _to_256_Vector_64(BCe, 0, 0, 0);

        VEC2[0] = _mm256_and_si256(VEC2[0], VEC3[0]);
        VEC2[1] = _mm256_and_si256(VEC2[1], VEC3[1]);

        e[1][0] = _mm256_xor_si256(VEC1[0], VEC2[0]);
        Egu_Where = _setElement_64(Egu_Where, Egu_Index, getElement_64(VEC2[1], 0) ^ getElement_64(VEC1[1], 0));

 


        /*
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
        */

        // EVENT i
        LEFTXOR[0] = _to_256_Vector_64(Abe, Agi, Ako, Amu);
        LEFTXOR[1] = _to_256_Vector_64(Asa, 0, 0, 0);

        XORRIGHT[0] = _to_256_Vector_64(De, Di, Do, Du);
        XORRIGHT[1] = _to_256_Vector_64(Da, 0, 0, 0);

        LEFTXOR[0] = _mm256_xor_si256(LEFTXOR[0], XORRIGHT[0]);
        LEFTXOR[1] = _mm256_xor_si256(LEFTXOR[1], XORRIGHT[1]);

        // Rimetti in memoria i valori calcolati
        // Abe Agi Ako Amu Asa
        Abe_Where = _setElement_64(Abe_Where, Abe_Index, getElement_64(LEFTXOR[0], 0));
        Agi_Where = _setElement_64(Agi_Where, Agi_Index, getElement_64(LEFTXOR[0], 1));
        Ako_Where = _setElement_64(Ako_Where, Ako_Index, getElement_64(LEFTXOR[0], 2));
        Amu_Where = _setElement_64(Amu_Where, Amu_Index, getElement_64(LEFTXOR[0], 3));
        Asa_Where = _setElement_64(Asa_Where, Asa_Index, getElement_64(LEFTXOR[1], 0));

        // EVENT ii
        // BCa = ROL(Abe,  1); BCe = ROL(Agi,  6); BCi = ROL(Ako, 25); BCo = ROL(Amu,  8); BCu = ROL(Asa, 18);
        BCa_Where = _setElement_64(BCa_Where, BCa_Index, ROL(Abe, 1));
        BCe_Where = _setElement_64(BCe_Where, BCe_Index, ROL(Agi, 6));
        BCi_Where = _setElement_64(BCi_Where, BCi_Index, ROL(Ako, 25));
        BCo_Where = _setElement_64(BCo_Where, BCo_Index, ROL(Amu, 8));
        BCu_Where = _setElement_64(BCu_Where, BCu_Index, ROL(Asa, 18));

        // EVENT iii
        // Eka =   BCa ^((~BCe)&  BCi );
        // Eke =   BCe ^((~BCi)&  BCo );
        // Eki =   BCi ^((~BCo)&  BCu );
        // Eko =   BCo ^((~BCu)&  BCa );
        // Eku =   BCu ^((~BCa)&  BCe );

        VEC1[0] = _to_256_Vector_64(BCa, BCe, BCi, BCo);
        VEC1[1] = _to_256_Vector_64(BCu, 0, 0, 0);

        VEC2[0] = _to_256_Vector_64(BCe, BCi, BCo, BCu);
        VEC2[1] = _to_256_Vector_64(BCa, 0, 0, 0);

        VEC2[0] = negate(VEC2[0]);
        VEC2[1] = negate(VEC2[1]);

        VEC3[0] = _to_256_Vector_64(BCi, BCo, BCu, BCa);
        VEC3[1] = _to_256_Vector_64(BCe, 0, 0, 0);

        VEC2[0] = _mm256_and_si256(VEC2[0], VEC3[0]);
        VEC2[1] = _mm256_and_si256(VEC2[1], VEC3[1]);

        e[2][0] = _mm256_xor_si256(VEC1[0], VEC2[0]);
        Eku_Where = _setElement_64(Eku_Where, Eku_Index, getElement_64(VEC2[1], 0) ^ getElement_64(VEC1[1], 0));

        
        /*
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
        */

        // EVENT i
        LEFTXOR[0] = _to_256_Vector_64(Abu, Aga, Ake, Ami);
        LEFTXOR[1] = _to_256_Vector_64(Aso, 0, 0, 0);

        XORRIGHT[0] = _to_256_Vector_64(Du, Da, De, Di);
        XORRIGHT[1] = _to_256_Vector_64(Do, 0, 0, 0);

        LEFTXOR[0] = _mm256_xor_si256(LEFTXOR[0], XORRIGHT[0]);
        LEFTXOR[1] = _mm256_xor_si256(LEFTXOR[1], XORRIGHT[1]);

        // Rimetti in memoria i valori calcolati
        // Abu Aga Ake Ami Aso
        Abu_Where = _setElement_64(Abu_Where, Abu_Index, getElement_64(LEFTXOR[0], 0));
        Aga_Where = _setElement_64(Aga_Where, Aga_Index, getElement_64(LEFTXOR[0], 1));
        Ake_Where = _setElement_64(Ake_Where, Ake_Index, getElement_64(LEFTXOR[0], 2));
        Ami_Where = _setElement_64(Ami_Where, Ami_Index, getElement_64(LEFTXOR[0], 3));
        Aso_Where = _setElement_64(Aso_Where, Aso_Index, getElement_64(LEFTXOR[1], 0));

        // EVENT ii
        // BCa = ROL(Abu, 27); BCe = ROL(Aga, 36); BCi = ROL(Ake, 10); BCo = ROL(Ami, 15); BCu = ROL(Aso, 56);
        BCa_Where = _setElement_64(BCa_Where, BCa_Index, ROL(Abu, 27));
        BCe_Where = _setElement_64(BCe_Where, BCe_Index, ROL(Aga, 36));
        BCi_Where = _setElement_64(BCi_Where, BCi_Index, ROL(Ake, 10));
        BCo_Where = _setElement_64(BCo_Where, BCo_Index, ROL(Ami, 15));
        BCu_Where = _setElement_64(BCu_Where, BCu_Index, ROL(Aso, 56));

        // EVENT iii
        // Ema =   BCa ^((~BCe)&  BCi );
        // Eme =   BCe ^((~BCi)&  BCo );
        // Emi =   BCi ^((~BCo)&  BCu );
        // Emo =   BCo ^((~BCu)&  BCa );
        // Emu =   BCu ^((~BCa)&  BCe );

        VEC1[0] = _to_256_Vector_64(BCa, BCe, BCi, BCo);
        VEC1[1] = _to_256_Vector_64(BCu, 0, 0, 0);

        VEC2[0] = _to_256_Vector_64(BCe, BCi, BCo, BCu);
        VEC2[1] = _to_256_Vector_64(BCa, 0, 0, 0);

        VEC2[0] = negate(VEC2[0]);
        VEC2[1] = negate(VEC2[1]);

        VEC3[0] = _to_256_Vector_64(BCi, BCo, BCu, BCa);
        VEC3[1] = _to_256_Vector_64(BCe, 0, 0, 0);

        VEC2[0] = _mm256_and_si256(VEC2[0], VEC3[0]);
        VEC2[1] = _mm256_and_si256(VEC2[1], VEC3[1]);

        e[3][0] = _mm256_xor_si256(VEC1[0], VEC2[0]);
        Emu_Where = _setElement_64(Emu_Where, Emu_Index, getElement_64(VEC2[1], 0) ^ getElement_64(VEC1[1], 0));



        /*
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
        */

        // EVENT i
        LEFTXOR[0] = _to_256_Vector_64(Abi, Ago, Aku, Ama);
        LEFTXOR[1] = _to_256_Vector_64(Ase, 0, 0, 0);

        XORRIGHT[0] = _to_256_Vector_64(Di, Do, Du, Da);
        XORRIGHT[1] = _to_256_Vector_64(De, 0, 0, 0);

        LEFTXOR[0] = _mm256_xor_si256(LEFTXOR[0], XORRIGHT[0]);
        LEFTXOR[1] = _mm256_xor_si256(LEFTXOR[1], XORRIGHT[1]);

        // Rimetti in memoria i valori calcolati
        // Abi Ago Aku Ama Ase
        Abi_Where = _setElement_64(Abi_Where, Abi_Index, getElement_64(LEFTXOR[0], 0));
        Ago_Where = _setElement_64(Ago_Where, Ago_Index, getElement_64(LEFTXOR[0], 1));
        Aku_Where = _setElement_64(Aku_Where, Aku_Index, getElement_64(LEFTXOR[0], 2));
        Ama_Where = _setElement_64(Ama_Where, Ama_Index, getElement_64(LEFTXOR[0], 3));
        Ase_Where = _setElement_64(Ase_Where, Ase_Index, getElement_64(LEFTXOR[1], 0));

        // EVENT ii
        // BCa = ROL(Abi, 62); BCe = ROL(Ago, 55); BCi = ROL(Aku, 39); BCo = ROL(Ama, 41); BCu = ROL(Ase, 2);
        BCa_Where = _setElement_64(BCa_Where, BCa_Index, ROL(Abi, 62));
        BCe_Where = _setElement_64(BCe_Where, BCe_Index, ROL(Ago, 55));
        BCi_Where = _setElement_64(BCi_Where, BCi_Index, ROL(Aku, 39));
        BCo_Where = _setElement_64(BCo_Where, BCo_Index, ROL(Ama, 41));
        BCu_Where = _setElement_64(BCu_Where, BCu_Index, ROL(Ase, 2));

        // EVENT iii
        // Esa =   BCa ^((~BCe)&  BCi );
        // Ese =   BCe ^((~BCi)&  BCo );
        // Esi =   BCi ^((~BCo)&  BCu );
        // Eso =   BCo ^((~BCu)&  BCa );
        // Esu =   BCu ^((~BCa)&  BCe );

        VEC1[0] = _to_256_Vector_64(BCa, BCe, BCi, BCo);
        VEC1[1] = _to_256_Vector_64(BCu, 0, 0, 0);

        VEC2[0] = _to_256_Vector_64(BCe, BCi, BCo, BCu);
        VEC2[1] = _to_256_Vector_64(BCa, 0, 0, 0);

        VEC2[0] = negate(VEC2[0]);
        VEC2[1] = negate(VEC2[1]);

        VEC3[0] = _to_256_Vector_64(BCi, BCo, BCu, BCa);
        VEC3[1] = _to_256_Vector_64(BCe, 0, 0, 0);

        VEC2[0] = _mm256_and_si256(VEC2[0], VEC3[0]);
        VEC2[1] = _mm256_and_si256(VEC2[1], VEC3[1]);

        e[4][0] = _mm256_xor_si256(VEC1[0], VEC2[0]);
        Esu_Where = _setElement_64(Esu_Where, Esu_Index, getElement_64(VEC2[1], 0) ^ getElement_64(VEC1[1], 0));



        /*
            prepareTheta
        BCa = Eba^Ega^Eka^Ema^Esa;
        BCe = Ebe^Ege^Eke^Eme^Ese;
        BCi = Ebi^Egi^Eki^Emi^Esi;
        BCo = Ebo^Ego^Eko^Emo^Eso;
        BCu = Ebu^Egu^Eku^Emu^Esu;
        */

        b[0] = _mm256_xor_epi64(e[0][0], e[1][0]);
        b[0] = _mm256_xor_epi64(b[0], e[2][0]);
        b[0] = _mm256_xor_epi64(b[0], e[3][0]);
        b[0] = _mm256_xor_epi64(b[0], e[4][0]);

        b[1] = _mm256_xor_epi64(e[0][1], e[1][1]);
        b[1] = _mm256_xor_epi64(b[1], e[2][1]);
        b[1] = _mm256_xor_epi64(b[1], e[3][1]);
        b[1] = _mm256_xor_epi64(b[1], e[4][1]);


        /*
        thetaRhoPiChiIotaPrepareTheta(round+1, E, A)
        Da = BCu^ROL(BCe, 1);
        De = BCa^ROL(BCi, 1);
        Di = BCe^ROL(BCo, 1);
        Do = BCi^ROL(BCu, 1);
        Du = BCo^ROL(BCa, 1);
        */

        LEFTXOR[0] = _to_256_Vector_64(BCu, BCa, BCe, BCi);
        LEFTXOR[1] = _to_256_Vector_64(BCo, 0, 0, 0);

        XORRIGHT[0] = _to_256_Vector_64(ROL(BCe, 1), ROL(BCi, 1), ROL(BCo, 1), ROL(BCu, 1));
        XORRIGHT[1] = _to_256_Vector_64(ROL(BCa, 1), 0, 0, 0);

        LEFTXOR[0] = _mm256_xor_si256(LEFTXOR[0], XORRIGHT[0]);
        LEFTXOR[1] = _mm256_xor_si256(LEFTXOR[1], XORRIGHT[1]);

        // Rimetti in memoria i valori calcolati
        d[0] = LEFTXOR[0];
        d[1] = LEFTXOR[1];
  

        /*
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
        Aba ^= (uint64_t)KeccakF_RoundConstants[round+1];
        Abe =   BCe ^((~BCi)&  BCo );
        Abi =   BCi ^((~BCo)&  BCu );
        Abo =   BCo ^((~BCu)&  BCa );
        Abu =   BCu ^((~BCa)&  BCe );
        */

       // EVENT i
        LEFTXOR[0] = _to_256_Vector_64(Eba, Ege, Eki, Emo);
        LEFTXOR[1] = _to_256_Vector_64(Esu, 0, 0, 0);

        XORRIGHT[0] = _to_256_Vector_64(Da, De, Di, Do); // XORRIGHT[0] = d[0]
        XORRIGHT[1] = _to_256_Vector_64(Du, 0, 0, 0); // XORRIGHT[1] = d[1]

        LEFTXOR[0] = _mm256_xor_si256(LEFTXOR[0], XORRIGHT[0]);
        LEFTXOR[1] = _mm256_xor_si256(LEFTXOR[1], XORRIGHT[1]);

        // Rimetti in memoria i valori calcolati
        // Eba Ege Eki Emo Esu
        Eba_Where = _setElement_64(Eba_Where, Eba_Index, getElement_64(LEFTXOR[0], 0));
        Ege_Where = _setElement_64(Ege_Where, Ege_Index, getElement_64(LEFTXOR[0], 1));
        Eki_Where = _setElement_64(Eki_Where, Eki_Index, getElement_64(LEFTXOR[0], 2));
        Emo_Where = _setElement_64(Emo_Where, Emo_Index, getElement_64(LEFTXOR[0], 3));
        Esu_Where = _setElement_64(Esu_Where, Esu_Index, getElement_64(LEFTXOR[1], 0));

        // EVENT ii
        // BCa = Eba; BCe = ROL(Ege, 44); BCi = ROL(Eki, 43); BCo = ROL(Emo, 21); BCu = ROL(Esu, 14);
        BCa_Where = _setElement_64(BCa_Where, BCa_Index, Eba);
        BCe_Where = _setElement_64(BCe_Where, BCe_Index, ROL(Ege, 44));
        BCi_Where = _setElement_64(BCi_Where, BCi_Index, ROL(Eki, 43));
        BCo_Where = _setElement_64(BCo_Where, BCo_Index, ROL(Emo, 21));
        BCu_Where = _setElement_64(BCu_Where, BCu_Index, ROL(Esu, 14));

        // EVENT iii
        // Aba =   BCa ^((~BCe)&  BCi );
        // Aba ^= (uint64_t)KeccakF_RoundConstants[round+1];
        // Abe =   BCe ^((~BCi)&  BCo );
        // Abi =   BCi ^((~BCo)&  BCu );
        // Abo =   BCo ^((~BCu)&  BCa );
        // Abu =   BCu ^((~BCa)&  BCe );

        VEC1[0] = _to_256_Vector_64(BCa, BCe, BCi, BCo);
        VEC1[1] = _to_256_Vector_64(BCu, 0, 0, 0);

        VEC2[0] = _to_256_Vector_64(BCe, BCi, BCo, BCu);
        VEC2[1] = _to_256_Vector_64(BCa, 0, 0, 0);

        VEC2[0] = negate(VEC2[0]);
        VEC2[1] = negate(VEC2[1]);

        VEC3[0] = _to_256_Vector_64(BCi, BCo, BCu, BCa);
        VEC3[1] = _to_256_Vector_64(BCe, 0, 0, 0);

        VEC2[0] = _mm256_and_si256(VEC2[0], VEC3[0]);
        VEC2[1] = _mm256_and_si256(VEC2[1], VEC3[1]);

        a[0][0] = _mm256_xor_si256(VEC1[0], VEC2[0]);
        Abu_Where = _setElement_64(Abu_Where, Abu_Index, getElement_64(VEC2[1], 0) ^ getElement_64(VEC1[1], 0));
        Aba_Where = _setElement_64(Aba_Where, Aba_Index, Aba ^ (uint64_t)RoundCostants[round+1]);


        /*
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
        */

        // EVENT i
        LEFTXOR[0] = _to_256_Vector_64(Ebo, Egu, Eka, Eme);
        LEFTXOR[1] = _to_256_Vector_64(Esi, 0, 0, 0);

        XORRIGHT[0] = _to_256_Vector_64(Do, Du, Da, De);
        XORRIGHT[1] = _to_256_Vector_64(Di, 0, 0, 0);

        LEFTXOR[0] = _mm256_xor_si256(LEFTXOR[0], XORRIGHT[0]);
        LEFTXOR[1] = _mm256_xor_si256(LEFTXOR[1], XORRIGHT[1]);

        // Rimetti in memoria i valori calcolati
        // Ebo Egu Eka Eme Esi
        Ebo_Where = _setElement_64(Ebo_Where, Ebo_Index, getElement_64(LEFTXOR[0], 0));
        Egu_Where = _setElement_64(Egu_Where, Egu_Index, getElement_64(LEFTXOR[0], 1));
        Eka_Where = _setElement_64(Eka_Where, Eka_Index, getElement_64(LEFTXOR[0], 2));
        Eme_Where = _setElement_64(Eme_Where, Eme_Index, getElement_64(LEFTXOR[0], 3));
        Esi_Where = _setElement_64(Esi_Where, Esi_Index, getElement_64(LEFTXOR[1], 0));

        // EVENT ii
        // BCa = ROL(Ebo, 28); BCe = ROL(Egu, 20); BCi = ROL(Eka, 3); BCo = ROL(Eme, 45); BCu = ROL(Esi, 61);
        BCa_Where = _setElement_64(BCa_Where, BCa_Index, ROL(Ebo, 28));
        BCe_Where = _setElement_64(BCe_Where, BCe_Index, ROL(Egu, 20));
        BCi_Where = _setElement_64(BCi_Where, BCi_Index, ROL(Eka, 3));
        BCo_Where = _setElement_64(BCo_Where, BCo_Index, ROL(Eme, 45));
        BCu_Where = _setElement_64(BCu_Where, BCu_Index, ROL(Esi, 61));

        // EVENT iii
        // Aga =   BCa ^((~BCe)&  BCi );
        // Age =   BCe ^((~BCi)&  BCo );
        // Agi =   BCi ^((~BCo)&  BCu );
        // Ago =   BCo ^((~BCu)&  BCa );
        // Agu =   BCu ^((~BCa)&  BCe );

        VEC1[0] = b[0];
        VEC1[1] = _to_256_Vector_64(BCu, 0, 0, 0);

        VEC2[0] = _to_256_Vector_64(BCe, BCi, BCo, BCu);
        VEC2[1] = _to_256_Vector_64(BCa, 0, 0, 0);

        VEC2[0] = negate(VEC2[0]);
        VEC2[1] = negate(VEC2[1]);

        VEC3[0] = _to_256_Vector_64(BCi, BCo, BCu, BCa);
        VEC3[1] = _to_256_Vector_64(BCe, 0, 0, 0);

        VEC2[0] = _mm256_and_si256(VEC2[0], VEC3[0]);
        VEC2[1] = _mm256_and_si256(VEC2[1], VEC3[1]);

        a[1][0] = _mm256_xor_si256(VEC1[0], VEC2[0]);
        Agu_Where = _setElement_64(Agu_Where, Agu_Index, getElement_64(VEC2[1], 0) ^ getElement_64(VEC1[1], 0));

        /*
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
        */

        // EVENT i
        LEFTXOR[0] = _to_256_Vector_64(Ebe, Egi, Eko, Emu);
        LEFTXOR[1] = _to_256_Vector_64(Esa, 0, 0, 0);

        XORRIGHT[0] = _to_256_Vector_64(De, Di, Do, Du);
        XORRIGHT[1] = _to_256_Vector_64(Da, 0, 0, 0);

        LEFTXOR[0] = _mm256_xor_si256(LEFTXOR[0], XORRIGHT[0]);
        LEFTXOR[1] = _mm256_xor_si256(LEFTXOR[1], XORRIGHT[1]);

        // Rimetti in memoria i valori calcolati
        // Ebe Egi Eko Emu Esa
        Ebe_Where = _setElement_64(Ebe_Where, Ebe_Index, getElement_64(LEFTXOR[0], 0));
        Egi_Where = _setElement_64(Egi_Where, Egi_Index, getElement_64(LEFTXOR[0], 1));
        Eko_Where = _setElement_64(Eko_Where, Eko_Index, getElement_64(LEFTXOR[0], 2));
        Emu_Where = _setElement_64(Emu_Where, Emu_Index, getElement_64(LEFTXOR[0], 3));
        Esa_Where = _setElement_64(Esa_Where, Esa_Index, getElement_64(LEFTXOR[1], 0));

        // EVENT ii
        // BCa = ROL(Ebe, 1); BCe = ROL(Egi, 6); BCi = ROL(Eko, 25); BCo = ROL(Emu, 8); BCu = ROL(Esa, 18);
        b[0] = _to_256_Vector_64(ROL(Ebe, 1), ROL(Egi, 6), ROL(Eko, 25), ROL(Emu, 8));
        b[1] = _to_256_Vector_64(ROL(Esa, 18), 0, 0, 0);

        // EVENT iii
        // Aka =   BCa ^((~BCe)&  BCi );
        // Ake =   BCe ^((~BCi)&  BCo );
        // Aki =   BCi ^((~BCo)&  BCu );
        // Ako =   BCo ^((~BCu)&  BCa );
        // Aku =   BCu ^((~BCa)&  BCe );

        VEC1[0] = b[0];
        VEC1[1] = b[1];

        VEC2[0] = _to_256_Vector_64(BCe, BCi, BCo, BCu);
        VEC2[1] = _to_256_Vector_64(BCa, 0, 0, 0);

        VEC2[0] = negate(VEC2[0]);
        VEC2[1] = negate(VEC2[1]);

        VEC3[0] = _to_256_Vector_64(BCi, BCo, BCu, BCa);
        VEC3[1] = _to_256_Vector_64(BCe, 0, 0, 0);

        VEC2[0] = _mm256_and_si256(VEC2[0], VEC3[0]);
        VEC2[1] = _mm256_and_si256(VEC2[1], VEC3[1]);

        a[2][0] = _mm256_xor_si256(VEC1[0], VEC2[0]);
        Aku_Where = _setElement_64(Aku_Where, Aku_Index, getElement_64(VEC2[1], 0) ^ getElement_64(VEC1[1], 0));

        /*
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
        */

        // EVENT i
        LEFTXOR[0] = _to_256_Vector_64(Ebu, Ega, Eke, Emi);
        LEFTXOR[1] = _to_256_Vector_64(Eso, 0, 0, 0);

        XORRIGHT[0] = _to_256_Vector_64(Du, Da, De, Di);
        XORRIGHT[1] = _to_256_Vector_64(Do, 0, 0, 0);

        LEFTXOR[0] = _mm256_xor_si256(LEFTXOR[0], XORRIGHT[0]);
        LEFTXOR[1] = _mm256_xor_si256(LEFTXOR[1], XORRIGHT[1]);

        // Rimetti in memoria i valori calcolati
        // Ebu Ega Eke Emi Eso
        Ebu_Where = _setElement_64(Ebu_Where, Ebu_Index, getElement_64(LEFTXOR[0], 0));
        Ega_Where = _setElement_64(Ega_Where, Ega_Index, getElement_64(LEFTXOR[0], 1));
        Eke_Where = _setElement_64(Eke_Where, Eke_Index, getElement_64(LEFTXOR[0], 2));
        Emi_Where = _setElement_64(Emi_Where, Emi_Index, getElement_64(LEFTXOR[0], 3));
        Eso_Where = _setElement_64(Eso_Where, Eso_Index, getElement_64(LEFTXOR[1], 0));

        // EVENT ii
        // BCa = ROL(Ebu, 27); BCe = ROL(Ega, 36); BCi = ROL(Eke, 10); BCo = ROL(Emi, 15); BCu = ROL(Eso, 56);
        b[0] = _to_256_Vector_64(ROL(Ebu, 27), ROL(Ega, 36), ROL(Eke, 10), ROL(Emi, 15));
        b[1] = _to_256_Vector_64(ROL(Eso, 56), 0, 0, 0);

        // EVENT iii
        // Ama =   BCa ^((~BCe)&  BCi );
        // Ame =   BCe ^((~BCi)&  BCo );
        // Ami =   BCi ^((~BCo)&  BCu );
        // Amo =   BCo ^((~BCu)&  BCa );
        // Amu =   BCu ^((~BCa)&  BCe );

        VEC1[0] = b[0];
        VEC1[1] = b[1];

        VEC2[0] = _to_256_Vector_64(BCe, BCi, BCo, BCu);
        VEC2[1] = _to_256_Vector_64(BCa, 0, 0, 0);

        VEC2[0] = negate(VEC2[0]);
        VEC2[1] = negate(VEC2[1]);

        VEC3[0] = _to_256_Vector_64(BCi, BCo, BCu, BCa);
        VEC3[1] = _to_256_Vector_64(BCe, 0, 0, 0);

        VEC2[0] = _mm256_and_si256(VEC2[0], VEC3[0]);
        VEC2[1] = _mm256_and_si256(VEC2[1], VEC3[1]);

        a[3][0] = _mm256_xor_si256(VEC1[0], VEC2[0]);
        Amu_Where = _setElement_64(Amu_Where, Amu_Index, getElement_64(VEC2[1], 0) ^ getElement_64(VEC1[1], 0));

        /*
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
        */

        // EVENT i
        LEFTXOR[0] = _to_256_Vector_64(Ebi, Ego, Eku, Ema);
        LEFTXOR[1] = _to_256_Vector_64(Ese, 0, 0, 0);

        XORRIGHT[0] = _to_256_Vector_64(Di, Do, Du, Da);
        XORRIGHT[1] = _to_256_Vector_64(De, 0, 0, 0);

        LEFTXOR[0] = _mm256_xor_si256(LEFTXOR[0], XORRIGHT[0]);
        LEFTXOR[1] = _mm256_xor_si256(LEFTXOR[1], XORRIGHT[1]);

        // Rimetti in memoria i valori calcolati
        // Ebi Ego Eku Ema Ese
        Ebi_Where = _setElement_64(Ebi_Where, Ebi_Index, getElement_64(LEFTXOR[0], 0));
        Ego_Where = _setElement_64(Ego_Where, Ego_Index, getElement_64(LEFTXOR[0], 1));
        Eku_Where = _setElement_64(Eku_Where, Eku_Index, getElement_64(LEFTXOR[0], 2));
        Ema_Where = _setElement_64(Ema_Where, Ema_Index, getElement_64(LEFTXOR[0], 3));
        Ese_Where = _setElement_64(Ese_Where, Ese_Index, getElement_64(LEFTXOR[1], 0));

        // EVENT ii
        // BCa = ROL(Ebi, 62); BCe = ROL(Ego, 55); BCi = ROL(Eku, 39); BCo = ROL(Ema, 41); BCu = ROL(Ese, 2);
        b[0] = _to_256_Vector_64(ROL(Ebi, 62), ROL(Ego, 55), ROL(Eku, 39), ROL(Ema, 41));
        b[1] = _to_256_Vector_64(ROL(Ese, 2), 0, 0, 0);

        // EVENT iii
        // Asa =   BCa ^((~BCe)&  BCi );
        // Ase =   BCe ^((~BCi)&  BCo );
        // Asi =   BCi ^((~BCo)&  BCu );
        // Aso =   BCo ^((~BCu)&  BCa );
        // Asu =   BCu ^((~BCa)&  BCe );

        VEC1[0] = b[0];
        VEC1[1] = b[1];

        VEC2[0] = _to_256_Vector_64(BCe, BCi, BCo, BCu);
        VEC2[1] = _to_256_Vector_64(BCa, 0, 0, 0);

        VEC2[0] = negate(VEC2[0]);
        VEC2[1] = negate(VEC2[1]);

        VEC3[0] = _to_256_Vector_64(BCi, BCo, BCu, BCa);
        VEC3[1] = _to_256_Vector_64(BCe, 0, 0, 0);

        VEC2[0] = _mm256_and_si256(VEC2[0], VEC3[0]);
        VEC2[1] = _mm256_and_si256(VEC2[1], VEC3[1]);

        a[4][0] = _mm256_xor_si256(VEC1[0], VEC2[0]);
        Asu_Where = _setElement_64(Asu_Where, Asu_Index, getElement_64(VEC2[1], 0) ^ getElement_64(VEC1[1], 0));



    }

    
    //copyToState(state, A)
    state[ 0] = Aba;
    state[ 1] = Abe;
    state[ 2] = Abi;
    state[ 3] = Abo;
    state[ 4] = Abu;
    state[ 5] = Aga;
    state[ 6] = Age;
    state[ 7] = Agi;
    state[ 8] = Ago;
    state[ 9] = Agu;
    state[10] = Aka;
    state[11] = Ake;
    state[12] = Aki;
    state[13] = Ako;
    state[14] = Aku;
    state[15] = Ama;
    state[16] = Ame;
    state[17] = Ami;
    state[18] = Amo;
    state[19] = Amu;
    state[20] = Asa;
    state[21] = Ase;
    state[22] = Asi;
    state[23] = Aso;
    state[24] = Asu;


}