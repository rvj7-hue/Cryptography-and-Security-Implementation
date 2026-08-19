#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <x86intrin.h> // For __rdtsc() and _mm_lfence()
#include "aes.h"

// Hardware Key Expansion Helper for AES-128
static inline __m128i aes_128_key_expansion(__m128i key, __m128i keygened) {
    keygened = _mm_shuffle_epi32(keygened, _MM_SHUFFLE(3,3,3,3));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    key = _mm_xor_si128(key, _mm_slli_si128(key, 4));
    return _mm_xor_si128(key, keygened);
}

void aes128_key_expansion_hw(const uint8_t *key, __m128i *roundKeys) {
    roundKeys[0] = _mm_loadu_si128((const __m128i*)key);
    roundKeys[1]  = aes_128_key_expansion(roundKeys[0],  _mm_aeskeygenassist_si128(roundKeys[0],  0x01));
    roundKeys[2]  = aes_128_key_expansion(roundKeys[1],  _mm_aeskeygenassist_si128(roundKeys[1],  0x02));
    roundKeys[3]  = aes_128_key_expansion(roundKeys[2],  _mm_aeskeygenassist_si128(roundKeys[2],  0x04));
    roundKeys[4]  = aes_128_key_expansion(roundKeys[3],  _mm_aeskeygenassist_si128(roundKeys[3],  0x08));
    roundKeys[5]  = aes_128_key_expansion(roundKeys[4],  _mm_aeskeygenassist_si128(roundKeys[4],  0x10));
    roundKeys[6]  = aes_128_key_expansion(roundKeys[5],  _mm_aeskeygenassist_si128(roundKeys[5],  0x20));
    roundKeys[7]  = aes_128_key_expansion(roundKeys[6],  _mm_aeskeygenassist_si128(roundKeys[6],  0x40));
    roundKeys[8]  = aes_128_key_expansion(roundKeys[7],  _mm_aeskeygenassist_si128(roundKeys[7],  0x80));
    roundKeys[9]  = aes_128_key_expansion(roundKeys[8],  _mm_aeskeygenassist_si128(roundKeys[8],  0x1B));
    roundKeys[10] = aes_128_key_expansion(roundKeys[9],  _mm_aeskeygenassist_si128(roundKeys[9],  0x36));
}

// Generate random bytes for message inputs
void generate_random_bytes(uint8_t *buf, size_t size) {
    for (size_t i = 0; i < size; i++) {
        buf[i] = (uint8_t)(rand() % 256);
    }
}

// Serialized timer helpers to prevent instruction reordering
static inline uint64_t rdtsc_start(void) {
    _mm_lfence();
    return __rdtsc();
}

static inline uint64_t rdtsc_end(void) {
    uint64_t cycles = __rdtsc();
    _mm_lfence();
    return cycles;
}

int main() {
    srand((unsigned int)time(NULL));

    // Define Key and buffers
    const uint8_t key[16] = {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
    };

    uint8_t plaintext[16];
    uint8_t ciphertext_sw[16];
    uint8_t ciphertext_hw[16];

    // Generate random input plaintext
    generate_random_bytes(plaintext, 16);

    printf("Random Plaintext: ");
    for (int i = 0; i < 16; i++) printf("%02x", plaintext[i]);
    printf("\n\n");

    // Pre-calculate Hardware and Software round keys outside benchmark loops
    __m128i hw_roundKeys[11];
    aes128_key_expansion_hw(key, hw_roundKeys);

    int warmup_iterations = 10;
    int benchmark_iterations = 10;

    // --- Benchmark 1: Software Implementation ---
    // Warm-up cache
    for (int i = 0; i < warmup_iterations; i++) {
        AES128_Encrypt(plaintext, ciphertext_sw, key);
    }

    uint64_t start_cycles = rdtsc_start();
    for (int i = 0; i < benchmark_iterations; i++) {
        AES128_Encrypt(plaintext, ciphertext_sw, key);
    }
    uint64_t sw_total_cycles = rdtsc_end() - start_cycles;

    double sw_avg_cycles = (double)sw_total_cycles / benchmark_iterations;
    double sw_cpb = sw_avg_cycles / 16.0;

    // --- Benchmark 2: Hardware AES-NI Implementation ---
    // Warm-up cache
    for (int i = 0; i < warmup_iterations; i++) {
        aes128_encrypt_block(plaintext, ciphertext_hw, hw_roundKeys);
    }

    start_cycles = rdtsc_start();
    for (int i = 0; i < benchmark_iterations; i++) {
        aes128_encrypt_block(plaintext, ciphertext_hw, hw_roundKeys);
    }
    uint64_t hw_total_cycles = rdtsc_end() - start_cycles;

    double hw_avg_cycles = (double)hw_total_cycles / benchmark_iterations;
    double hw_cpb = hw_avg_cycles / 16.0;

    // --- Print Results ---
    printf("--- Execution Output ---\n");
    printf("Software Ciphertext: ");
    for (int i = 0; i < 16; i++) printf("%02x", ciphertext_sw[i]);
    printf("\n");

    printf("Hardware Ciphertext: ");
    for (int i = 0; i < 16; i++) printf("%02x", ciphertext_hw[i]);
    printf("\n\n");

    printf("--- Benchmark Results (%d iterations) ---\n", benchmark_iterations);
    printf("Software AES-128 : %.2f cycles/block | %.2f cycles/byte (CPB)\n", sw_avg_cycles, sw_cpb);
    printf("Hardware AES-NI  : %.2f cycles/block | %.2f cycles/byte (CPB)\n", hw_avg_cycles, hw_cpb);
    printf("Speedup Factor   : %.2fx faster with AES-NI\n", sw_avg_cycles / hw_avg_cycles);

    return 0;
}