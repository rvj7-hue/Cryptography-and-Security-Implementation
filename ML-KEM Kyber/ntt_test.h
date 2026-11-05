#ifndef NTT_H
#define NTT_H

int bitrev(int x, int logn);
int modpow(int base, int exp, int q);
void ntt(int *a, int n, int q, int zeta);
void intt(int *a, int n, int q, int zeta, int inv_n);
void bcm(int c[2], int a0, int a1, int b0, int b1, int q, int twiddle);
void ntt_mul(int *c, int *a, int *b, int n, int q, int zeta);

#endif
