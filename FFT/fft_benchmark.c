#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "fft.h"
#include <x86intrin.h>
#include<emmintrin.h>
#include<math.h>

static inline unsigned long long rdtsc_start(void) {
    __builtin_ia32_lfence();
    unsigned long long cycles = __rdtsc();
    __builtin_ia32_lfence();
    return cycles;
}

static inline unsigned long long rdtsc_end(void) {
    unsigned int dummy;
    unsigned long long cycles = __rdtscp(&dummy);
    __builtin_ia32_lfence();
    return cycles;
}

static inline void do_not_optimize(void *p) {
    __asm__ volatile("" : : "g"(p) : "memory");
}


int main () {

    int iterations = 1000;

    int poly_size[7]={4,8,16,32,64,126,256};

    for (int i=0; i<7; i++) {
        int n = poly_size[i];
        long long total_cycles = 0;

        for (int it = 0; it < iterations; it++) {

            double complex *a = calloc(n, sizeof(double complex));
            double complex *b = calloc(n, sizeof(double complex));
            double complex *result = malloc(n*sizeof(double complex));

            for (int j=0; j<n/2; j++) a[j] = rand() % 100;
            for (int j=0; j<n/2; j++) b[j] = rand() % 100;

            double complex *resulta;
            double complex *resultb; 
            double complex *final_result;

            fft(a, n); //warm up
            fft(b, n);
            
            unsigned long long start = rdtsc_start();
            
            resulta = fft(a, n);
            resultb = fft(b, n);

            for (int j=0; j<n; j++) {
                result[j] = resulta[j] * resultb[j]; //pointwise multiplication
            }

            final_result = ifft(result, n);
            do_not_optimize(final_result);

            unsigned long long end = rdtsc_end();
            total_cycles += (end - start);

            free(a);
            free(b);
            free(result);
            free(final_result);
        }

        double n_log_n = (double)n * log2((double)n);

        double cycles_per_nlogn = (double)total_cycles / (n_log_n*iterations);

        // --- PRINT METRICS ---
        printf("N                       : %d\n", n);
        printf("Average Cycles per iteration          : %llu\n", total_cycles/iterations);
        printf("Cycles / (N * log2(N))  : %.4f\n", cycles_per_nlogn);
        }

    return 0;

}