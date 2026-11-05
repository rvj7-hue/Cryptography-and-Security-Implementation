//Gen pubkey
#include "poly.h"
#include <stdint.h>
#include <stdlib.h>
#include "keygen_enc_dec.h"
#include <time.h>
#include <x86intrin.h>
#include "fips202.h"
#include <string.h>
// Constants (define actual values in poly.c or globally)
#define N 256   // example size
#define Q 3329  // example modulus
#define K 3
#define ETA 2   // example small error bound


//sample NTT

void sample_ntt(uint8_t b[34], int16_t a[256]) {

    keccak_state ctx;

    shake128_init(&ctx);  
    shake128_absorb(&ctx, b, sizeof(b));
    shake128_finalize(&ctx);
    int j=0;
    uint8_t c[3];
    uint16_t d1, d2;
    int k;
    while (j<256) {
        shake128_squeeze(c, 3, &ctx);
        d1=c[0]+256*(c[1]%16);
        k=c[1]/16;
        d2=k+16*c[2];

        if (d1<Q) {
            a[j]=d1;
            j=j+1;
        }
        
        if (d2<Q && j<256) {
            a[j]=d2;
            j=j+1;
        }
    }

}

//SamplePolyCBD

void spcbd(uint8_t b[64*ETA], uint16_t f[256]) {
    int bits[64*ETA*8];
    bytes_to_bits(b, 64*ETA, bits);
    uint16_t x,y;
    for (int i=0; i<256; i++) {
        for (int j=0; j<ETA; j++) {
            x=x+b[2*i*ETA + j];
            y=y+b[2*i*ETA + j+ETA];

        }

        f[i] = (x-y)%Q;
    }
}

//Key generation

void kpke_gen(uint8_t d[32], uint8_t ek[384*K + 32], uint8_t dk[384*K]) {
    uint8_t input[33]; //d||k
    uint8_t rs[64];

    // Construct d || k
    memcpy(input, d, 32);
    input[32] = K;

    // Compute SHA3-512(d || k)
    sha3_512(rs, input, sizeof(input));

    // Split the output into rho and sigma
    uint8_t rho[32];
    uint8_t sigma[32];

    memcpy(rho, rs, 32);
    memcpy(sigma, rs + 32, 32);

    int n=0;

    Matrix A;

    uint8_t b[34];

    memcpy(b, rho, 32);

    for (int i=0; i<K; i++) {
        for (int j=0; j<K; j++) {
            b[32]=j;
            b[33]=i;
            sample_ntt(b, A.a[i][j].c);
        }
    }

    PolyVec s;
    PolyVec e;
    uint8_t out[64*ETA];

    memcpy(input, sigma, 32);
    
    for (int i=0; i<K; i++) {
        input[32]=n;
        keccak_state ctx;
        shake256_init(&ctx);  
        shake256_absorb(&ctx, input, sizeof(input));
        shake256_finalize(&ctx);
        shake256_squeeze(out, 64*ETA, &ctx);
        spcbd(out, s.v[i].c);
        n=n+1;

    }

    for (int i=0; i<K; i++) {
        input[32]=n;
        keccak_state ctx;
        shake256_init(&ctx);  
        shake256_absorb(&ctx, input, sizeof(input));
        shake256_finalize(&ctx);
        shake256_squeeze(out, 64*ETA, &ctx);
        spcbd(out, e.v[i].c);
        n=n+1;

    }

    for (int i=0; i<K; i++) {
        ntt(e.v[i].c, N, Q, 17);
        ntt(s.v[i].c, N, Q, 17);
    }

    PolyVec t;
    PolyVec p;
    Poly sum;
    for (int i=0; i<K; i++) {
        for (int j=0; j<K; j++) {
            ntt_mul(p.v[j].c, A.a[i][j].c, s.v[j].c, N, Q, 17);
        }
        poly_add(&sum, &p.v[0], &p.v[1]);
        poly_add(&sum, &sum, &p.v[2]);
        poly_add(&t.v[i], &sum, &e.v[i]);

    }

    uint8_t rest[384];
    uint8_t ress[384];
    for (int i=0; i<K; i++) {
        byte_encode((t.v[i]).c, 12, rest);
        memcpy(ek+384*i, rest, 384);

        byte_encode(s.v[i].c, 12, ress);
        memcpy(dk+384*i, ress, 384);

    }

    memcpy(ek+384*K, rho, 32);


}

//Random  polynomial 

void random_poly(Poly *p) {
    for (int i = 0; i < N; i++) {
        p->c[i] = rand() % Q;
    }
}

int small_random(void)
{
    return (rand() % (2 * ETA + 1)) - ETA;
}

//generate random small error polynomial

void random_small_poly(Poly *p) {
    for (int i = 0; i < N; i++) {
        p->c[i] = small_random();
    }
}




void keygen(
    Matrix *A,
    PolyVec *s,
    PolyVec *t
) {
    for (int i=0; i<K; i++) {
        for (int j=0; j<K; j++) {
            random_poly(&A->a[i][j]);
        }
    }

    for (int i=0; i<K; i++) {
        random_small_poly(&s->v[i]);
    }

    matrix_vector_mul(t, A, s);

    PolyVec e;

    for (int i = 0; i < K; i++) {

        random_small_poly(&e.v[i]);
    }

    for (int i = 0; i < K; i++) {

        poly_add(
            &t->v[i],
            &t->v[i],
            &e.v[i]);
    }



}

//Enc(m)

void encode_message(
    Poly *m,
    const uint8_t message[32]) //the message is 32 bytes = 256 bits, which is the same as the degree of the polynomial
{
    for (int i = 0; i < N; i++) {

        int bit =
            (message[i / 8] >> //from the i/8th byte, we are take the (i%8)th bit, and check if it is 1 or 0
             (i % 8)) & 1;

        if (bit)
            m->c[i] = Q / 2;
        else
            m->c[i] = 0;
    }

}

//Decode func

void decode_message(
    uint8_t message[32],
    const Poly *m)
{
    for (int i = 0; i < 32; i++)
        message[i] = 0;


    for (int i = 0; i < N; i++) {

        int x = mod_q(m->c[i]);

        /*
         * Values close to Q/2 represent 1.
         * Values close to 0 represent 0.
         */
       // Convert 0..Q-1 to approximately -Q/2..Q/2
        if (x > Q / 2)
            x -= Q;

        /*
         * x is now the noisy version of either:
         *
         * 0       -> bit 0
         * Q/2     -> bit 1
         *
         * For this toy scheme, values near 0 are 0.
         */
        if (abs(x) > Q / 4) {
            message[i / 8] |=
                (uint8_t)(1 << (i % 8));
        }
    }
}

//Encrypt func

void encrypt(
    const Matrix *A,
    const PolyVec *t,
    const uint8_t message[32],
    PolyVec *u,
    Poly *v
) {
    PolyVec r;
    PolyVec e1;
    Poly e2;
    Poly m;

    for (int i = 0; i < K; i++) {
        random_small_poly(&r.v[i]);
    }

    for (int i = 0; i < K; i++) {
        random_small_poly(&e1.v[i]);
    }

    random_small_poly(&e2);

    encode_message(&m, message);

    matrix_transpose_vector_mul(u, A, &r);

    for (int i = 0; i < K; i++) {

        poly_add(
            &u->v[i],
            &u->v[i],
            &e1.v[i]);
    }

    Poly tr;

    vector_dot(&tr, t, &r);

    poly_add(v, &tr, &e2);

    poly_add(v, v, &m);



}

//Dec(c1,c2)

void decrypt(
    const PolyVec *s,
    const PolyVec *u,
    const Poly *v,
    uint8_t message[32])
{
    Poly su;

    /*
     * s^T * u
     */
    vector_dot(
        &su,
        s,
        u);


    /*
     * m' = v - s^T*u
     */
    Poly m;

    poly_sub(
    &m,
    v,
    &su);


/*
    * Decode the noisy message.
    */
    decode_message(
        message,
        &m);
}

//compare two messages

// static inline uint64_t start_cycles()
// {
//     _mm_lfence();
//     return __rdtsc();
// }

// static inline uint64_t end_cycles(unsigned int *aux)
// {
//     uint64_t t = __rdtscp(aux);
//     _mm_lfence();
//     return t;
// }

int messages_equal(
    const uint8_t *a,
    const uint8_t *b)
{
    for (int i = 0; i < 32; i++) {

        if (a[i] != b[i])
            return 0;
    }

    return 1;
}