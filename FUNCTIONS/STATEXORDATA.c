// SHA3 NON INCREMENTAL VERSION

// KECCAKF1600 -> 25 parole da 8 byte (64 bit)
// KECCAKF1600 -> 1600 bit di stato

#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>

#include "../Utils/logger.c"

#include "../Utils/intrinUtils.h"

#define size 64 //bytes



void StateXORBytes(uint64_t *state, const unsigned char *data, unsigned int offset, unsigned int length)
{
    // Carica da state+offset fino a state + offset + lenght in un vettore (stato)
    // Carica data in un altro vettore
    // STATO XOR DATA
    // Lenght puo arrivare fino a 136 Byte ergo max 9 vettori per lo stato
    // Si usano length / 16 (+1 se ce resto) vettori

    // Calcolare quanti vettori servono e l'ultimo vettore fino a che posizione viene usato

    // Carica lo state

    //LOG("--STATEXORBYTE with offset: %d\n", offset);

    uint8_t *new_state = (uint8_t*)state;

    int vec_quantity = length / 32;
    if (length % 32 > 0) vec_quantity++;
    int usage_of_last_vector = length % 32;
    //LOG("Length: %d, vec_quantity: %d\n", length, vec_quantity);
    __m256i _state_window[vec_quantity];
    for (int i = 0; i < vec_quantity; i++){
        uint8_t state_vec[32] = {0};
        if (i < vec_quantity - 1 || usage_of_last_vector == 0 ){
            for(int j = 0; j < 32; j++){
                state_vec[j] = *(new_state + offset + i*32 + j);
            }
            //_state_window[i] = _mm256_loadu_si256((__m256i*)(new_state + offset + i*32));
        } else {
            //uint8_t last_state_vec[32] = {0};
            for (int j = 0; j < usage_of_last_vector; j++){
                //last_state_vec[j] = *(new_state + offset + i*32 + j);
                state_vec[j] = *(new_state + offset + i*32 + j);
            }
            //_state_window[i] = _mm256_loadu_si256((__m256i*)last_state_vec);
        }

        for(int j = 0; j < 4; j++){
            uint8_t tmp = state_vec[j];
            //LOG("tmp: %d\n", tmp);
            state_vec[j] = state_vec[7-j];
            //LOG("new data_vec[j]: %d\n", data_vec[j]);
            state_vec[7-j] = tmp;
            //LOG("new data_vec[7-j]: %d\n", data_vec[7-j]);
        }
        //LOG("8 to 15 swap\n");
        for(int j = 8; j < 12; j++){
            uint8_t tmp = state_vec[j];
            //LOG("tmp: %d\n", tmp);
            state_vec[j] = state_vec[23-j];
            //LOG("new data_vec[j]: %d\n", data_vec[j]);
            state_vec[23-j] = tmp;
            //LOG("new data_vec[23-j]: %d\n", data_vec[23-j]);
        }
        //LOG("16 to 23 swap\n");
        for(int j = 16; j < 20; j++){
            uint8_t tmp = state_vec[j];
            //LOG("tmp: %d\n", tmp);
            
            state_vec[j] = state_vec[39-j];
            //LOG("new data_vec[j]: %d\n", data_vec[j]);
            state_vec[39-j] = tmp;
            //LOG("new data_vec[39-j]: %d\n", data_vec[39-j]);
        }
        //LOG("24 to 31 swap\n");
        for(int j = 24; j < 28; j++){
            uint8_t tmp = state_vec[j];
            //LOG("tmp: %d\n", tmp);
            state_vec[j] = state_vec[55-j];
            //LOG("new data_vec[j]: %d\n", data_vec[j]);
            state_vec[55-j] = tmp;
            //LOG("new data_vec[55-j]: %d\n", data_vec[55-j]);
        }

        _state_window[i] = _mm256_loadu_si256((__m256i*)state_vec);
    } 

    // Carica data
    __m256i _data_window[vec_quantity];
    for (int i = 0; i < vec_quantity; i++){
        uint8_t data_vec[32] = {0};
        if(i < vec_quantity - 1 || usage_of_last_vector == 0){
            for(int j = 0; j < 32; j++){
                data_vec[j] = *(data + i*32 + j);
            }
        } else {
            //LOG("CREATING LAST DATA VEC\n");
            for(int j = 0; j < usage_of_last_vector; j++){
                ////LOG("data[%d]: %d\n", i*32 + j, *(data + i*32 + j));
                data_vec[j] = *(data + i*32 + j);
            }
        }
        //LOG("CREATION DATA VEC: %d/%d\n", i+1, vec_quantity);
        //for(int j = 0; j < 32; j++){
            //LOG("%d ", data_vec[j]);
        //}
        //LOG("\nEND CREATION DATA VEC\n");
        //LOG("0 to 7 swap\n");
        
        for(int j = 0; j < 4; j++){
            uint8_t tmp = data_vec[j];
            //LOG("tmp: %d\n", tmp);
            data_vec[j] = data_vec[7-j];
            //LOG("new data_vec[j]: %d\n", data_vec[j]);
            data_vec[7-j] = tmp;
            //LOG("new data_vec[7-j]: %d\n", data_vec[7-j]);
        }
        //LOG("8 to 15 swap\n");
        for(int j = 8; j < 12; j++){
            uint8_t tmp = data_vec[j];
            //LOG("tmp: %d\n", tmp);
            data_vec[j] = data_vec[23-j];
            //LOG("new data_vec[j]: %d\n", data_vec[j]);
            data_vec[23-j] = tmp;
            //LOG("new data_vec[23-j]: %d\n", data_vec[23-j]);
        }
        //LOG("16 to 23 swap\n");
        for(int j = 16; j < 20; j++){
            uint8_t tmp = data_vec[j];
            //LOG("tmp: %d\n", tmp);
            
            data_vec[j] = data_vec[39-j];
            //LOG("new data_vec[j]: %d\n", data_vec[j]);
            data_vec[39-j] = tmp;
            //LOG("new data_vec[39-j]: %d\n", data_vec[39-j]);
        }
        //LOG("24 to 31 swap\n");
        for(int j = 24; j < 28; j++){
            uint8_t tmp = data_vec[j];
            //LOG("tmp: %d\n", tmp);
            data_vec[j] = data_vec[55-j];
            //LOG("new data_vec[j]: %d\n", data_vec[j]);
            data_vec[55-j] = tmp;
            //LOG("new data_vec[55-j]: %d\n", data_vec[55-j]);
        }
        //LOG("END SWAP\n");
        //LOG("NEW DATA VEC\n");
        for(int j = 0; j < 32; j++){
            //LOG("%d ", data_vec[j]);
        }
        

        _data_window[i] = _mm256_loadu_si256((__m256i*)data_vec);
    }
    
    //LOG("\n----------\n");

    
    //LOG("---STATE PRE\n");
    for(int i = 0; i < 25; i++){
        //LOG("-----IMPL - %d: %llu\n", i, state[i]);
    }
    //LOG("---DATA\n");
    for(int i = 0; i < length; i++){
        //LOG("-----data[%d]: %d\n", i, *(data + i));
    }
    // XOR AND SAVE RESULT
    for(int i = 0; i < vec_quantity; i++){

        // PRINT ALL STATE
        /*//LOG("STATE PRE\n");
        for (int j = 0; j < vec_quantity; j++){
            //LOG("PRE: %llu %llu %llu %llu\n",_mm256_extract_epi64(_state_window[j], 0), _mm256_extract_epi64(_state_window[j], 1), _mm256_extract_epi64(_state_window[j], 2), _mm256_extract_epi64(_state_window[j], 3));
        }
        //LOG("DATA\n");
        for (int j = 0; j < vec_quantity; j++){
            //LOG("%llu %llu %llu %llu\n",_mm256_extract_epi64(_data_window[j], 0), _mm256_extract_epi64(_data_window[j], 1), _mm256_extract_epi64(_data_window[j], 2), _mm256_extract_epi64(_data_window[j], 3));
        }*/
        for(int j = 0; j < 32; j++){
            //LOG("xorring %d with %d\n", getElement_8_2(_state_window[i], 31-j), getElement_8_2(_data_window[i], 31-j));
        }
        _state_window[i] = _mm256_xor_si256(_state_window[i], _data_window[i]);
        /*//LOG("STATE POST\n");
        for (int j = 0; j < vec_quantity; j++){
            //LOG("POST: %llu %llu %llu %llu\n",_mm256_extract_epi64(_state_window[j], 0), _mm256_extract_epi64(_state_window[j], 1), _mm256_extract_epi64(_state_window[j], 2), _mm256_extract_epi64(_state_window[j], 3));
        }*/
        /*for (int j = 0; j < 32; j++){
            *(new_state + offset + i*32 + j) = getElement_8_2(_state_window[i], 31-j);
        }*/
        //if it goes into seg fault use this one instead/
            if (i < vec_quantity - 1 || usage_of_last_vector == 0){
                for (int j = 0; j < 32; j++){
                    //LOG("-----XORRING %d into state[%d], byte[%d]. ", getElement_8_2(_data_window[i], 31-j), (offset + i*32 + j) / 8, (offset + i*32 + j) % 8);
                    *(new_state + offset + i*32 + j) = getElement_8_2(_state_window[i], 31-j);
                    //LOG("-----RESULT: %llu\n", state[(offset + i*32 + j) / 8]);
                }
            } else {
                for (int j = 0; j < usage_of_last_vector; j++){
                    //LOG("-----XORRING %d into state[%d], byte[%d]. ", getElement_8_2(_data_window[i], 31-j), (offset + i*32 + j) / 8, (offset + i*32 + j) % 8);
                    *(new_state + offset + i*32 + j) = getElement_8_2(_state_window[i], 31-j);
                    //LOG("-----RESULT: %llu\n", state[(offset + i*32 + j) / 8]);
                }
            }



    }
    // print state
    //LOG("---STATE POST\n");
    for(int i = 0; i < 25; i++){
        //LOG("-----IMPL - %d: %llu\n", i, state[i]);
    }

}


