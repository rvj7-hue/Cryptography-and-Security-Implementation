#include "poly.h"
#include "ntt_test.h"

int mod_q(int16_t x) {
    x %= Q;
    if (x < 0) {
        x += Q;
    }
    return x;
}

void poly_zero(Poly *p) {
    for (int i = 0; i < N; i++) {
        p->c[i] = 0;
    }
}

void poly_add(Poly *r, const Poly *a, const Poly *b) {
    for (int i = 0; i < N; i++)
        r->c[i] = mod_q(a->c[i] + b->c[i]);
}

void poly_sub(Poly *r, const Poly *a, const Poly *b) {
    for (int i = 0; i < N; i++)
        r->c[i] = mod_q(a->c[i] - b->c[i]);
}



//Polynomial multiplication using NTT

void poly_mul_ntt(Poly *r, const Poly *a, const Poly *b) {
    int a_ntt[N], b_ntt[N], r_ntt[N];
    for (int i = 0; i < N; i++) {
        a_ntt[i] = a->c[i];
        b_ntt[i] = b->c[i];
    }

    ntt(a_ntt, N, Q, 17); // 17 is a primitive root of unity for Q=3329
    ntt(b_ntt, N, Q, 17);

    ntt_mul(r_ntt, a_ntt, b_ntt, N, Q, 17);

    intt(r_ntt, N, Q, 17, 3303); // 3303 is the modular inverse of N=128 mod Q=3329

    for (int i = 0; i < N; i++) {
        r->c[i] = mod_q((int16_t)r_ntt[i]);
    
}


//Matrix vector multiplication

void matrix_vector_mul(
    PolyVec *result,
    const Matrix *A,
    const PolyVec *v
) {
    for (int i = 0; i < K; i++) {

        poly_zero(&result->v[i]);

        for (int j = 0; j < K; j++) {

            Poly tmp;

            poly_mul_ntt(
                &tmp,
                &A->a[i][j],
                &v->v[j]);

            poly_add(
                &result->v[i],
                &result->v[i],
                &tmp);

            }
    }
}

//A^T v multiplication 

void matrix_transpose_vector_mul(
    PolyVec *result,
    const Matrix *A,
    const PolyVec *v)
{
    for (int i = 0; i < K; i++) {

        poly_zero(&result->v[i]);

        for (int j = 0; j < K; j++) {

            Poly tmp;

            poly_mul_ntt(
                &tmp,
                &A->a[j][i],
                &v->v[j]);

            poly_add(
                &result->v[i],
                &result->v[i],
                &tmp);
        }
    }
}

//dot product

void vector_dot(
    Poly *result,
    const PolyVec *a,
    const PolyVec *b)
{
    poly_zero(result);

    for (int i = 0; i < K; i++) {

        Poly tmp;

        poly_mul_ntt(
            &tmp,
            &a->v[i],
            &b->v[i]);

        poly_add(
            result,
            result,
            &tmp);
    }
}


//-----------------------------------------------------------------------------------------

//Normal multiplication 

// void poly_mul(Poly *r, const Poly *a, const Poly *b) {
//     int64_t tmp[2 * N - 1] = {0};//Why use 64 here? The reason is that each product can be as large 
//     //as < 2^24. And at most we have to sum 256 (x^255 -> (0,255), (255,0), (1,254), (254,1),...) 
//     //such products, so the maximum value can be as large as N*(2^24) = 2^32 (approx), which we need signed    //

//     for (int i = 0; i < N; i++) {
//         for (int j = 0; j < N; j++) {
//             tmp[i + j] += (int32_t)a->c[i] * (int32_t)b->c[j]; //x^255* x ->gets stored at tmp[255+1]=tmp[256]
//         }
//     }

//     for (int i = 0; i < N-1; i++) {
//         tmp[i] -= tmp[i + N]; //since x^256=-1
//     }

//     for (int i = 0; i < N; i++) {
//         r->c[i] = mod_q((int16_t)(tmp[i] % Q));//if temp[i] is negative, then so is temp[i]%Q, so mod_q will handle it and make it positive
//     }
// }