#include <stdio.h>
#include <complex.h>
#include <math.h>
#include <stdlib.h>


//we want to evaluate the FFT of a polynomial of degree n-1, where n is a power of 2
//the input is an array of n complex numbers, representing the coefficients of the polynomial
//the output is an array of n complex numbers, representing the values of the polynomial at the n-th roots of unity
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//big problem with this fft code is that malloc gets called recursively and is not freed. This takes up way too many cycles as 
//when malloc is called repeatedly, the 32 KB cache gets filled quickly and so the stored numbers have to go to RAM in order to make room
//for the new malloc. This slows down the hardware.

//The CPU doesn't store single bytes into empty 64-byte slots. Instead, the CPU always reads and writes data in fixed 64-byte blocks.

double complex *fft(double complex *p, int n) { //n is the number of coefficients, which is a power of 2
  
    if (n==1) return p; //constant polynomial

    double complex *even = malloc(n/2 * sizeof(double complex));//to store the even indexed coefficients
    double complex *odd = malloc(n/2 * sizeof(double complex));// to store the odd indexed coefficients
    
    for (int i=0; i<n/2 ; i++) {
        even[i] = p[2*i]; //extracting even indexed coefficients
        odd[i] = p[2*i + 1]; //extracting odd indexed coefficients
    }

    double complex *even_fft = fft(even, n/2); //this is the recursive step
    double complex *odd_fft = fft(odd, n/2);

    double angle = 2 * M_PI / n;
    double complex w = cos(angle) + sin(angle) * I;//n^th primitive root of unity

    double complex *result = malloc(n * sizeof(double complex));

    for (int k=0; k<n/2; k++) {
        result[k] = even_fft[k] + cpow(w, k) * odd_fft[k];
        result[k + n/2] = even_fft[k] - cpow(w, k) * odd_fft[k];
    }

    return result; //this contains p evaluated at the n-th roots of unity
    
}


double complex *ifft_helper(double complex *p, int n) { //this is where we are given the evaluation of the polynomial at the n-th roots of unity, and we want to recover the coefficients
    if (n==1) return p;

    double complex *even = malloc(n/2 * sizeof(double complex));
    double complex *odd = malloc(n/2 * sizeof(double complex));
    
    for (int i=0; i<n/2 ; i++) {
        even[i] = p[2*i]; //extracting even indexed coefficients
        odd[i] = p[2*i + 1]; //extracting odd indexed coefficients
    }

    double complex *even_ifft = ifft_helper(even, n/2);
    double complex *odd_ifft = ifft_helper(odd, n/2);

    double angle = -2 * M_PI / n; //this is because of the vandermonde matrix, we need to take the conjugate of the roots of unity
    double complex w = cos(angle) + sin(angle) * I;

    double complex *result = malloc(n * sizeof(double complex));

    for (int k=0; k<n/2; k++) {
        result[k] = even_ifft[k] + cpow(w, k) * odd_ifft[k];
        result[k + n/2] = even_ifft[k] - cpow(w, k) * odd_ifft[k];
    }

    return result;
    
}

double complex *ifft(double complex *p, int n) {
    double complex *result = ifft_helper(p, n);
    for (int i=0; i<n; i++) {
        result[i] /= n; //this is because of the inverse matrix 
    }
    return result;

}


// int main() {
    
//     double complex a[8] = {1, 2, 0,1, 0, 0, 0, 0}; //example polynomial coefficients (1 + 2x + 0x^2 + 1x^3)
//     double complex b[8] = {0, 1, 0,0, 0, 0, 0, 0}; //example polynomial coefficients (0 + 1x + 0x^2 + 0x^3)
//     double complex *resulta = fft(a, 8);
//     double complex *resultb = fft(b, 8);

//     double complex result[8];

//     for (int i=0; i<8; i++) {
//         result[i] = resulta[i] * resultb[i]; //pointwise multiplication
//     }

//     double complex *final_result = ifft(result, 8);

//     printf("Final result (coefficients of the product polynomial):\n");
//     for (int i=0; i<8; i++) {
//         printf("final_result[%d] = %f + %fi\n", i, creal(final_result[i]), cimag(final_result[i]));
//     }
    
    
//     return 0;
// }