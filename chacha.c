#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//Broad description - There is a 4x4 matrix, each entry a 32 bit word, with one row initialised to constants. (Why?)
//The last row contains the counter and nonce values, if the message is longer than 64 bytes, the counter is incremented 
//and the next block is generated. The other two rows are initialised with the key. 
//The quarter round function is applied 20 times to the matrix, and the result is added to the original matrix. 
//The message is then XORed with the resulting matrix to produce the ciphertext.

//The quarter round function uses the operations ARX, which is very fast as these operations are native to the CPU. (i.e. the 
//processor already has inbuilt circuits for these operations.)


//What we want to see -
// 1) Cha cha implementation with static allotment of state (4x4 matrix)
// 2) Cha cha implementation with dynamic allotment of state (4x4 matrix)
// 3) Cha cha optimised implementation (sequential vs parallel) in sequential, each state/matrix is processed one at a time.

// Rotate left 32-bit value
#define ROTL32(v, n) ((v << n) | (v >> (32 - n))) //This is the rotation function 

// ChaCha quarter round (scalar)
#define QUARTERROUND(a, b, c, d) do { \
    a += b; d ^= a; d = ROTL32(d, 16); \
    c += d; b ^= c; b = ROTL32(b, 12); \
    a += b; d ^= a; d = ROTL32(d, 8);  \
    c += d; b ^= c; b = ROTL32(b, 7);  \
} while (0) //This is what happens in each of the 20 rounds

// ChaCha20 block function (static)
void chacha20_block_stat(const uint32_t key[8], const uint32_t nonce[3],
                    uint32_t counter, uint8_t output[64])
{
    // Initialize state (4x4 matrix) (static)
    uint32_t state[16] = {
        0x61707865, 0x3320646e, 0x7920622d, 0x6b206574, // this is the constant "expand 32-byte k"
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

void chacha20_block_dynamic(const uint32_t key[8], const uint32_t nonce[3],
                            uint32_t counter, uint8_t output[64])
{
    // Dynamically allocate state
    uint32_t *state = malloc(16 * sizeof(uint32_t));
    uint32_t *working = malloc(16 * sizeof(uint32_t));

    // Initialize state (constants, key, counter, nonce)
    state[0] = 0x61707865; state[1] = 0x3320646e;
    state[2] = 0x7920622d; state[3] = 0x6b206574;
    for (int i = 0; i < 8; i++) state[4+i] = key[i];
    state[12] = counter; state[13] = nonce[0];
    state[14] = nonce[1]; state[15] = nonce[2];

    // Copy original state
    for (int i = 0; i < 16; i++) working[i] = state[i];

    // 20 rounds
    for (int i = 0; i < 10; i++) {
        QUARTERROUND(working[0], working[4], working[8],  working[12]);
        QUARTERROUND(working[1], working[5], working[9],  working[13]);
        QUARTERROUND(working[2], working[6], working[10], working[14]);
        QUARTERROUND(working[3], working[7], working[11], working[15]);
        QUARTERROUND(working[0], working[5], working[10], working[15]);
        QUARTERROUND(working[1], working[6], working[11], working[12]);
        QUARTERROUND(working[2], working[7], working[8],  working[13]);
        QUARTERROUND(working[3], working[4], working[9],  working[14]);
    }

    // Add original state
    for (int i = 0; i < 16; i++) working[i] += state[i];

    // Serialize to bytes
    for (int i = 0; i < 16; i++) {
        output[4*i + 0] = (working[i] >> 0) & 0xFF;
        output[4*i + 1] = (working[i] >> 8) & 0xFF;
        output[4*i + 2] = (working[i] >> 16) & 0xFF;
        output[4*i + 3] = (working[i] >> 24) & 0xFF;
    }

    free(state);
    free(working);
}


int main() {
    uint32_t key[8] = {0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c,
                       0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c};
    uint32_t nonce[3] = {0x00000000, 0x4a000000, 0x00000000};
    uint32_t counter = 1;
    uint8_t keystream[64];

    int iterations = 100000; // run many times for timing

    clock_t start, end;

    // Benchmark static
    start = clock();
    for (int i = 0; i < iterations; i++) {
        chacha20_block_stat(key, nonce, counter, keystream);
    }
    end = clock();
    double static_time = (double)(end - start) / CLOCKS_PER_SEC;

    // Benchmark dynamic
    start = clock();
    for (int i = 0; i < iterations; i++) {
        chacha20_block_dynamic(key, nonce, counter, keystream);
    }
    end = clock();
    double dynamic_time = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Static allocation time:  %.6f seconds\n", static_time);
    printf("Dynamic allocation time: %.6f seconds\n", dynamic_time);

    return 0;
}
