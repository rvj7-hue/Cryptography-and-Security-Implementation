#ifndef KEYGEN_ENC_DEC_H
#define KEYGEN_ENC_DEC_H
#include "poly.h"
#include <stdint.h>
#include <stdlib.h>
// Constants (define actual values in poly.c or globally)
#define N 256   // example size
#define Q 3329  // example modulus
#define K 3
#define ETA 2   // example small error bound

void random_poly(Poly *p);

int small_random(void);
void random_small_poly(Poly *p);

void sample_ntt(uint8_t b[34], int16_t a[256]);
void spcbd(uint8_t b[64*ETA], uint16_t f[256]);

void keygen(
    Matrix *A,
    PolyVec *s,
    PolyVec *t
);

void encode_message(
    Poly *m,
    const uint8_t message[32]);


void decode_message(
    uint8_t message[32],
    const Poly *m);


void encrypt(
    const Matrix *A,
    const PolyVec *t,
    const uint8_t message[32],
    PolyVec *u,
    Poly *v
);

void decrypt(
    const PolyVec *s,
    const PolyVec *u,
    const Poly *v,
    uint8_t message[32]);

//static inline uint64_t start_cycles();

//static inline uint64_t end_cycles(unsigned int *aux);

int messages_equal(
    const uint8_t *a,
    const uint8_t *b);


#endif // KEYGEN_ENC_DEC_H