#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int bitrev(int x, int logn) {
    int rev=0;
    for (int i=0; i<logn;i++) {
       rev= rev <<1 | (x & 1);
       x >>=1;
   }
   return rev;
}

int modpow(int base, int exp, int q) { //square and multiply method for modular exponentiation
    int result = 1;
    base %= q;

    while (exp > 0) {
        if (exp & 1)
            result = (result * base) % q;

        base = (base * base) % q;
        exp >>= 1;
    }

    return result;
}


void ntt(int *a, int n, int q, int zeta) {
    
    int k=1;
    int twiddle; 
    for (int len = n/2; len >1; len/=2) {

        for (int j=0; j<(n/len); j+=2) {
            twiddle=modpow(zeta, bitrev(k,7), q);
            k=k+1;

            for (int i=0; i<len; i++) {
                int t=twiddle*a[j*len+i+len]%q; 
                a[j*len+i+len]=(a[j*len+i]-t+q)%q;
                a[j*len+i]=(a[j*len+i]+t)%q;
            }
        }
    }
}

void intt(int *a, int n, int q, int zeta, int inv_n) {
    
    int k=127;
    int twiddle; 
   
    for (int len = 2; len <n; len*=2) {
        for (int j=0; j<(n/len); j+=2) {
            twiddle=modpow(zeta, bitrev(k,7), q);
            k=k-1;

            for (int i=0; i<len; i++) {
                int t=a[j*len+i];
                a[j*len+i]=((a[j*len+i+len]+t))%q;
                a[j*len+i+len]=(twiddle*(a[j*len+i+len]-t+q))%q;
                
            }
        }
    }

    for (int i=0; i<n; i++) {
        a[i]=(a[i]*inv_n)%q;
    }
}


void bcm(int c[2], int a0, int a1,
         int b0, int b1, int q, int twiddle)
{
    int64_t t0 = (int64_t)a0 * b0;
    int64_t t1 = (int64_t)a1 * b1 * twiddle;
    int64_t t2 = (int64_t)a0 * b1;
    int64_t t3 = (int64_t)a1 * b0;

    c[0] = (int)((t0 + t1) % q);
    c[1] = (int)((t2 + t3) % q);
}

void ntt_mul(int *c, int *a, int *b, int n, int q, int zeta) {
    
    int temp[2];
    for (int i=0; i<n/2; i++) {
        int twiddle=modpow(zeta, 2*bitrev(i,7)+1, q);
        bcm(temp, a[2*i], a[2*i+1], b[2*i], b[2*i+1], q, twiddle);
        c[2*i]=temp[0];
        c[2*i+1]=temp[1];
    }
   
}



// int main() {

//     int a[8]={99,52,0,7,0,29,33,100};
//     int b[8]={6,0,45,12,14,78,65,112};
//     ntt(a,8,137,10);
//     ntt(b,8,137,10);
//     int c[8];
//     ntt_mul(c, a, b, 8, 137, 10);
//     for (int i=0; i<8; i++) {
//         printf("%d ",c[i]);
//     }
    
//     printf("\n");
//     intt(c,8,137,10,103);
//     for (int i=0; i<8; i++) {
//         printf("%d ",c[i]);
//     }
//     return 0;
// }




