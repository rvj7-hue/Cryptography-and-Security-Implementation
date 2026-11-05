#ifndef POLY_H
#define POLY_H

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "fips202.h" 
// Constants (define actual values in poly.c or globally)
#define N 256   // example size
#define Q 3329  // example modulus
#define K 3
#define ETA 2   // example small error bound

// Polynomial structure
typedef struct {
    int16_t c[N]; // This represents one polynomial in Rq = Zq[x]/(x^256 + 1)
} Poly;

typedef struct {
    Poly v[K]; // This is a vector of polynomials, which we want for s and e
} PolyVec;

typedef struct {
    Poly a[K][K]; // This a matrix of polynomials, which we want for A
} Matrix;

void bits_to_bytes(const int *bits, size_t nbits, uint8_t *bytes) {
    size_t nbytes = nbits / 8;

    for (size_t i = 0; i < nbytes; i++) {
        bytes[i] = 0;
    }

    for (size_t i = 0; i < nbits; i++) {
        bytes[i / 8] |= (uint8_t)(bits[i] << (i % 8));
    }
}

void bytes_to_bits(uint8_t *bytes, size_t nbytes, int *bits) {
    uint8_t c[nbytes] ;
    for (int i=0; i<nbytes; i++) {
        c[i]=bytes[i];
    }

    for (int i=0; i<nbytes; i++) {
        for (int j=0; j<8; j++) {

            bits[i*8+j]=c[i]&1;
            c[i]>>=1;
        }
    }

}


void byte_encode(int *F, int d, uint8_t *bytes) { // F[256] is an array of int from Z_m,
//where m is 2^d if d<12, and m=3329 if d=12.


    int m = (d < 12) ? (1 << d) : 3329;
    int temp_bits[256 * 12];

    for (int i = 0; i < 256; i++) {
        int a = F[i];

        // Convert to a canonical representative modulo q
        if (d == 12) {
            a %= 3329;
            if (a < 0) {
                a += 3329;
            }
        }

        for (int j = 0; j < d; j++) {
            temp_bits[i * d + j] = a & 1;
            a = (a - temp_bits[i * d + j]) / 2;
        }
    }

    bits_to_bytes(temp_bits, (size_t)(256 * d), bytes);
}


void byte_decode(uint8_t *bytes, int d, int *bits) {
    int temp_bits[d*256];
    bytes_to_bits(bytes, d*32, temp_bits);

    int m;

    if (d<12) {
        m=pow(2,d);
    } else {
        m=Q;
    }

    for (int i=0; i<256; i++) {
        int a=0;
        for (int j=0; j<d; j++) {
            a = (a + temp_bits[i*d+j] * (1 << j)) % m;
        }
        bits[i]=a;
    }
}




// Function declarations (prototypes)
int mod_q(int16_t x);
void poly_zero(Poly *p);
void poly_add(Poly *r, const Poly *a, const Poly *b);
void poly_sub(Poly *r, const Poly *a, const Poly *b);
//void poly_mul(Poly *r, const Poly *a, const Poly *b);

void matrix_vector_mul(PolyVec *result, const Matrix *A, const PolyVec *v);

void poly_mul_ntt(Poly *r, const Poly *a, const Poly *b);
void matrix_vector_mul(
    PolyVec *result,
    const Matrix *A,
    const PolyVec *v
);
void matrix_transpose_vector_mul(
    PolyVec *result,
    const Matrix *A,
    const PolyVec *v);

void vector_dot(
    Poly *result,
    const PolyVec *a,
    const PolyVec *b);
#endif // POLY_H
