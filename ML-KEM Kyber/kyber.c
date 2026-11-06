#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <x86intrin.h>
#include <stdint.h>
#include "keygen_enc_dec.h"
#include "fips202.h" // Includes shake128 routines. This fips202 file is taken from the kyber reference implementation. 

#include "Poly.h"

#define N 256
#define Q 3329
#define K 3
#define ETA 2

//We want to implement Kyber ML-KEM
//First we need to implement the polynomial arithmetic and the NTT (Number Theoretic Transform) for efficient polynomial multiplication.
//The idea is that the messages are polynomials and we use the hardness of the LWE problem to guarantee security.
//So, public key is (A,t) where t=As+e
//For Alice to encrypt m in {0,1}^n, she samples a random polynomial r and computes c1 = Ar + e1 and c2 = t*r + e2 + m(q/2), where e1 and e2 are small error polynomials. 
//She then sends (c1,c2) to Bob. Bob can then decrypt by computing c2 - s*c1 = e2 + m(q/2) - s*e1, and then uses the round function to recover m.


//polynomial structure 


//Polynomial arithmetic 

//generate random beta



  


int main() {

    Matrix A;
    PolyVec s;
    PolyVec t;

    keygen(&A, &s, &t);

   // uint8_t message[32] = {0xDE, 0xAD, 0xBE, 0xEF}; // Example message
    //each 0xab requires 8 bits, i.e. 1 byte.

    uint8_t message[32];
    Poly v;
    Poly w;
    PolyVec u;
    Poly result;
    srand((unsigned)time(NULL));

    for (int i = 0; i < 32; i++){
    message[i] = rand() & 0xFF;}

    encrypt(&A, &t, message, &u, &v);

    uint8_t decrypted_message[32];

    decrypt(&s, &u, &v, decrypted_message);

    int equal = messages_equal(message, decrypted_message);

    printf("Original:  ");

    for (int i = 0; i < 32; i++)
        printf("%02X ", message[i]);

    printf("\nDecrypted: ");

    for (int i = 0; i < 32; i++)
        printf("%02X ", decrypted_message[i]);

    printf("\nEqual: %d\n", equal);
   

    

    return 0;
}

//Benchmark - Average cycles: 4340053
//With -O1 -  1967663
//With -O2 -  1535988
//With -O3 -  532314 (Huge improvement)

//Multiplying with and without NTT  -

//Average cycles: 259499
//Average cycles with NTT: 96970


//------------------------------------------------------------------------------------------------


    // //Example multiplication of two polynomials
    // Poly p3;
    // Poly p1={0};
    // p1.c[255]=1; //x^255
    // Poly p2={0};
    // p2.c[1]=1; //x^1
    // poly_mul_ntt(&p3,&p1,&p2); //should give x^256 = -1 mod q, so p3 should be -1 mod q

    // printf("Polynomial 1: ");
    // for (int i = 255; i > 251; i--) {
    //     printf("%d ", p1.c[i]);
    // }
    // printf("\nPolynomial 2: ");
    // for (int i = 0; i < 4; i++) {
    //     printf("%d ", p2.c[i]);
    // }
    // printf("\n");

    // printf("Polynomial 3 (result of multiplication): ");
    // for (int i = 0; i < 4; i++) {
    //     printf("%d ", p3.c[i]);
    // }



//--------------------------------------------------------------------------------------------


    // for (int i = 0; i < 32; i++){
    // message[i] = rand() & 0xFF;}

    // encrypt(&A, &t, message, &u, &v);

    // uint8_t decrypted_message[32];

    // decrypt(&s, &u, &v, decrypted_message);

    // int equal = messages_equal(message, decrypted_message);

    // printf("Original:  ");

    // for (int i = 0; i < 32; i++)
    //     printf("%02X ", message[i]);

    // printf("\nDecrypted: ");

    // for (int i = 0; i < 32; i++)
    //     printf("%02X ", decrypted_message[i]);

    // printf("\nEqual: %d\n", equal);

//---------------------------------------------------------------------------------

//  uint64_t start, end;
//     unsigned int aux;
//     uint64_t total_cycles = 0;
//     uint64_t total_cycles_ntt = 0;

    
//     for (int i=0; i<100000; i++) {
//             random_poly(&v);
//             random_poly(&w);

//             start= start_cycles();
//             poly_mul(&result, &v, &w);
//             end = end_cycles(&aux);
//             total_cycles += end - start;

//             start= start_cycles();
//             poly_mul_ntt(&result, &v, &w);
//             end = end_cycles(&aux);
//             total_cycles_ntt += end - start;

//         }

//     printf("Average cycles: %lu\n", total_cycles / 100000);
//     printf("Average cycles with NTT: %lu\n", total_cycles_ntt / 100000);


//------------------------------------------------------------------------------------------------

//Checking if poly_mul and poly_mul_ntt give the same result

 //Example multiplication of two polynomials
    // random_poly(&v);
    // random_poly(&w);
    // poly_mul_ntt(&result, &v, &w); 

    // printf("Polynomial 1: ");
    // for (int i = 255; i > 251; i--) {
    //     printf("%d ", v.c[i]);
    // }
    // printf("\nPolynomial 2: ");
    // for (int i = 0; i < 4; i++) {
    //     printf("%d ", w.c[i]);
    // }
    // printf("\n");

    // printf("Polynomial 3 (result of ntt multiplication): ");
    // for (int i = 0; i < 4; i++) {
    //     printf("%d ", result.c[i]);
    // }

    // poly_mul(&result, &v, &w);
    // printf("\n");

    // printf("Polynomial 3 (result of multiplication): ");
    // for (int i = 0; i < 4; i++) {
    //     printf("%d ", result.c[i]);
    // }
