#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <x86intrin.h>

//REFERENCES - 1) https://www.rfc-editor.org/info/rfc8439/#section-2
             //2) "ChaCha, a variant of Salsa20", by Daniel J. Bernstein
             //3) Code written with the help of Co-Pilot

// Rotate left 32-bit value
#define ROTL32(v, n) ((v << n) | (v >> (32 - n))) //This is the rotation function 

// ChaCha quarter round (scalar)
#define QUARTERROUND(a, b, c, d) do { \
    a += b; d ^= a; d = ROTL32(d, 16); \
    c += d; b ^= c; b = ROTL32(b, 12); \
    a += b; d ^= a; d = ROTL32(d, 8);  \
    c += d; b ^= c; b = ROTL32(b, 7);  \
} while (0) //This is what happens in each of the 20 rounds


// ChaCha20 block function
void chacha20_block(const uint32_t key[8], const uint32_t nonce[3],
                    uint32_t counter, uint8_t output[64])
{
    // Initialize state (4x4 matrix) (static)
    uint32_t state[16] = {
        0x61707865, 0x3320646e, 0x79622d32, 0x6b206574, // this is the constant "expand 32-byte k"
        key[0], key[1], key[2], key[3],
        key[4], key[5], key[6], key[7],
        counter, nonce[0], nonce[1], nonce[2]
    };


    // Copy original state for final addition
    uint32_t working[16];
    for (int i = 0; i < 16; i++) working[i] = state[i];

    // 20 rounds (10 column + 10 diagonal)
    for (int i = 0; i < 10; i++) {
        // Column rounds
        QUARTERROUND(working[0], working[4], working[8],  working[12]);
        QUARTERROUND(working[1], working[5], working[9],  working[13]);
        QUARTERROUND(working[2], working[6], working[10], working[14]);
        QUARTERROUND(working[3], working[7], working[11], working[15]);
        // Diagonal rounds
        QUARTERROUND(working[0], working[5], working[10], working[15]);
        QUARTERROUND(working[1], working[6], working[11], working[12]);
        QUARTERROUND(working[2], working[7], working[8],  working[13]);
        QUARTERROUND(working[3], working[4], working[9],  working[14]);
    }

    // Add original state
    for (int i = 0; i < 16; i++) working[i] += state[i];

    // Serialize to bytes (little-endian)
    for (int i = 0; i < 16; i++) {
        output[4*i + 0] = (working[i] >> 0) & 0xFF;
        output[4*i + 1] = (working[i] >> 8) & 0xFF;
        output[4*i + 2] = (working[i] >> 16) & 0xFF;
        output[4*i + 3] = (working[i] >> 24) & 0xFF;
    }
}

void chacha20_encrypt(const uint32_t key[8], const uint32_t nonce[3],
                      uint32_t counter, const uint8_t *plaintext,
                      uint8_t *ciphertext, size_t length)
{
    size_t num_blocks = length / 64;      // full 64-byte blocks
    size_t leftover   = length % 64;      // remainder

    // Process full blocks
    for (size_t j = 0; j < num_blocks; j++) {
        uint8_t keystream[64];
        chacha20_block(key, nonce, counter + j, keystream);

        const uint8_t *block_in  = plaintext  + j * 64;
        uint8_t       *block_out = ciphertext + j * 64;

        for (size_t i = 0; i < 64; i++) {
            block_out[i] = block_in[i] ^ keystream[i];
        }
    }

    // Process leftover bytes (if any)
    if (leftover > 0) {
        uint8_t keystream[64];
        size_t j = num_blocks;
        chacha20_block(key, nonce, counter + j, keystream);

        const uint8_t *block_in  = plaintext  + j * 64;
        uint8_t       *block_out = ciphertext + j * 64;

        for (size_t i = 0; i < leftover; i++) {
            block_out[i] = block_in[i] ^ keystream[i];
        }
    }
}

static inline uint64_t rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

int main() {
    uint32_t key[8] = {0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c,
                       0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c};
    uint32_t nonce[3] = {0x00000000, 0x4a000000, 0x00000000};
    uint8_t keystream[64];
    uint32_t counter = 1;

    static const uint8_t pt[] = {
    0x4c, 0x61, 0x64, 0x69, 0x65, 0x73, 0x20, 0x61, 0x6e, 0x64, 0x20, 0x47,
    0x65, 0x6e, 0x74, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20,
    0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x61, 0x73, 0x73, 0x20, 0x6f, 0x66,
    0x20, 0x27, 0x39, 0x39, 0x3a, 0x20, 0x49, 0x66, 0x20, 0x49, 0x20, 0x63,
    0x6f, 0x75, 0x6c, 0x64, 0x20, 0x6f, 0x66, 0x66, 0x65, 0x72, 0x20, 0x79,
    0x6f, 0x75, 0x20, 0x6f, 0x6e, 0x6c, 0x79, 0x20, 0x6f, 0x6e, 0x65, 0x20,
    0x74, 0x69, 0x70, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x74, 0x68, 0x65, 0x20,
    0x66, 0x75, 0x74, 0x75, 0x72, 0x65, 0x2c, 0x20, 0x73, 0x75, 0x6e, 0x73,
    0x63, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x77, 0x6f, 0x75, 0x6c, 0x64, 0x20,
    0x62, 0x65, 0x20, 0x69, 0x74, 0x2e, 0x00};

    const uint8_t expected_ct[] = {
    0x6e, 0x2e, 0x35, 0x9a, 0x25, 0x68, 0xf9, 0x80, 0x41, 0xba, 0x07, 0x28,
    0xdd, 0x0d, 0x69, 0x81, 0xe9, 0x7e, 0x7a, 0xec, 0x1d, 0x43, 0x60, 0xc2,
    0x0a, 0x27, 0xaf, 0xcc, 0xfd, 0x9f, 0xae, 0x0b, 0xf9, 0x1b, 0x65, 0xc5,
    0x52, 0x47, 0x33, 0xab, 0x8f, 0x59, 0x3d, 0xab, 0xcd, 0x62, 0xb3, 0x57,
    0x16, 0x39, 0xd6, 0x24, 0xe6, 0x51, 0x52, 0xab, 0x8f, 0x53, 0x0c, 0x35,
    0x9f, 0x08, 0x61, 0xd8, 0x07, 0xca, 0x0d, 0xbf, 0x50, 0x0d, 0x6a, 0x61,
    0x56, 0xa3, 0x8e, 0x08, 0x8a, 0x22, 0xb6, 0x5e, 0x52, 0xbc, 0x51, 0x4d,
    0x16, 0xcc, 0xf8, 0x06, 0x81, 0x8c, 0xe9, 0x1a, 0xb7, 0x79, 0x37, 0x36,
    0x5a, 0xf9, 0x0b, 0xbf, 0x74, 0xa3, 0x5b, 0xe6, 0xb4, 0x0b, 0x8e, 0xed,
    0xf2, 0x78, 0x5e, 0x42, 0x87, 0x4d};

    size_t len = sizeof(expected_ct);
    uint8_t *actual_ct = malloc(len);

    // Run encryption
    chacha20_encrypt(key, nonce, counter, pt, actual_ct, len);

    if (memcmp(actual_ct, expected_ct, len) == 0) {
        printf("Ciphertext matches expected test vector!\n");
    
    }

    int iterations = 1000;
    unsigned long long total_cycles = 0;

    for (int i = 0; i < iterations; i++) {

        unsigned long long start = __rdtsc();
        chacha20_encrypt(key, nonce, counter, pt, actual_ct, len);
        unsigned long long end = __rdtsc();

        total_cycles += (end - start);
    }

    double avg_cycles = (double)total_cycles / iterations;
    double cpb = avg_cycles / (double)len;

    printf("Average cycles: %.0f\n", avg_cycles);
    printf("Bytes processed: %zu\n", len);
    printf("Cycles per byte: %.2f\n", cpb);

    free(actual_ct);

    return 0;

 }

//When run with -O3
//Ciphertext matches expected test vector!
//Average cycles: 1035
//Bytes processed: 114
//Cycles per byte: 9.08