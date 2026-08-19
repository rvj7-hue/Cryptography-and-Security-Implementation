#ifndef FFT_H
#define FFT_H

#include <complex.h>


double complex *fft(double complex *p, int n);
double complex *ifft(double complex *p, int n);

#endif // FFT_H