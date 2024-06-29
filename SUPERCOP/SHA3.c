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

#include "KeccakSponge.c"

int crypto_hash512( unsigned char *out, const unsigned char *in, unsigned long long inlen )
{
    return Sponge(576, 1024, in, inlen, 0x06, out, 64);
}
int crypto_hash384( unsigned char *out, const unsigned char *in, unsigned long long inlen )
{
    return Sponge(832, 768, in, inlen, 0x06, out, 48);
}
int crypto_hash256( unsigned char *out, const unsigned char *in, unsigned long long inlen )
{
    return Sponge(1088, 512, in, inlen, 0x06, out, 32);
}
int crypto_hash_shake256( unsigned char *out, const unsigned char *in, unsigned long long inlen)
{
    return Sponge(1088, 512, in, inlen, 0x1F, out, 136);
}
int crypto_hash_shake128( unsigned char *out, const unsigned char *in, unsigned long long inlen)
{
    return Sponge(1344, 256, in, inlen, 0x1F, out, 168);
}

