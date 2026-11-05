#include <stdio.h>
#include <gmp.h>
#include <time.h>
#include <x86intrin.h>
#include <stdint.h>

//mpz_t is an array type, i.e. it is mpz_struct[1], where mpz_struct is a struct type, consisting of limbs (the actual number), 
//size (the number of limbs), and alloc (the allocated size). 
//If we want 1024 bits, we need 1024/64 = 16 limbs, since each limb is 64 bits.
//So, when you pass an mpz_t variable to a function, you are actually passing a pointer to the first element of the array,
// which is equivalent to passing a pointer to the struct. Therefore, you can modify the value of the mpz_t variable inside 
//the function, and the changes will be reflected outside the function as well.


void gen_primes(mpz_t p, mpz_t q, gmp_randstate_t state) {
    do {
        mpz_urandomb(p, state, 2048);
        mpz_setbit(p, 2047);  // force 2048-bit i.e. 2047 th bit is set to 1
        mpz_setbit(p, 0);     // the 1st bit is set to 1, so that the number is odd
    } while (mpz_probab_prime_p(p, 25) == 0);

    mpz_nextprime(q, p);  // q is the next prime after p

}

void phi(mpz_t result, mpz_t p, mpz_t q) {
    mpz_t p_minus_1, q_minus_1;
    mpz_inits(p_minus_1, q_minus_1, NULL);

    mpz_sub_ui(p_minus_1, p, 1);
    mpz_sub_ui(q_minus_1, q, 1);
    mpz_mul(result, p_minus_1, q_minus_1);

    mpz_clears(p_minus_1, q_minus_1, NULL);
}

void encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {
    mpz_powm(c, m, e, n);
}

void decrypt(mpz_t c, mpz_t n, mpz_t d, mpz_t m) {
    mpz_powm(m, c, d, n);
}

static inline uint64_t start_cycles()
{
    _mm_lfence();
    return __rdtsc();
}

static inline uint64_t end_cycles(unsigned int *aux)
{
    uint64_t t = __rdtscp(aux);
    _mm_lfence();
    return t;
}



int main() {
    mpz_t p, q, n, e, d;
    mpz_inits(p, q, n, e, d, NULL);

    gmp_randstate_t state;
    gmp_randinit_default(state);
    gmp_randseed_ui(state, time(NULL));

    //generating p and q
    gen_primes(p, q, state);
    mpz_mul(n, p, q); //n=pq

    mpz_t phi_n; //generating phi
    mpz_init(phi_n);
    phi(phi_n, p, q); //phi(n) = (p-1)(q-1)

    mpz_set_ui(e, 65537);  // common choice for e
    mpz_invert(d, e, phi_n); // d=e^-1 mod phi(n)

    mpz_t m,c, result;
    mpz_inits(m, c, result, NULL);
    uint64_t start, end;
    unsigned int aux;
    uint64_t total_cycles = 0;
    for (int i=0; i<100000; i++) {
        
        mpz_urandomm(m, state, n);
        
        start = start_cycles();
        encrypt(c, m, e, n);
        end = end_cycles(&aux);
        total_cycles += end - start;

    }
    
   
    printf("Average cycles for encryption: %lu\n", total_cycles / 100000);
    mpz_clears(p, q, n, e, d,m,c, result, NULL);
    gmp_randclear(state);
    return 0;
}

//  gmp_printf("Original message: %Zd\n", m);
//     gmp_printf("Encrypted message: %Zd\n", c);

//     decrypt(c, n, d, result);
//     gmp_printf("Decrypted message: %Zd\n", result);

//Benchmark

//For n=1024 bits
//Average cycles for encryption: 18328 (-O1)
//Average cycles for encryption: 21491 (-O2)
//Average cycles for encryption: 18538 (-O3)

//For n=2048 bits
//Average cycles for encryption: 61553 (-O1)
//Average cycles for encryption: 64269 (-O2)
//Average cycles for encryption: 64095 (-O3)

//For n=4096 bits
//Average cycles for encryption: 234200 (-O1)
//Average cycles for encryption: 219764 (-O2)
//Average cycles for encryption: 188455 (-O3)

