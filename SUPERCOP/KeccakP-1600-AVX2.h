#include <stddef.h>

typedef unsigned char UINT8;

#ifdef __cplusplus
extern "C" {
#endif

void KeccakP1600_StaticInitialize       (void);
void KeccakP1600_Initialize             (void *state);
void KeccakP1600_AddByte                (void *state, UINT8 byte, size_t offset);
void KeccakP1600_AddBytes               (void *state, const UINT8 *data, size_t offset, size_t length);
void KeccakP1600_Permute_24rounds       (void *state);
void KeccakP1600_ExtractBytes           (const void *state, UINT8 *data, size_t offset, size_t length);


size_t KeccakF1600_FastLoop_Absorb      (void *state, size_t laneCount, const UINT8 *data, size_t dataByteLen);

#ifdef __cplusplus
}
#endif

