#ifndef AES_H
#define AES_H

#include <stdint.h>
#include <wmmintrin.h>

#define Nb 4   // block size in 32-bit words
#define Nk 4   // key size in 32-bit words
#define Nr 10  // number of rounds for AES-128

// Software AES-128
void KeyExpansion(const uint8_t *key, uint8_t roundKey[176]);
void AES128_Encrypt(const uint8_t input[16], uint8_t output[16], const uint8_t key[16]);

// Hardware AES-NI
void aes128_encrypt_block(const uint8_t *in, uint8_t *out, const __m128i *roundKeys);

#endif // AES_H