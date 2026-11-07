#include <stdio.h>
#include <x86intrin.h>


//KSA 
void rc4_init(unsigned char s[256], unsigned char k[256]) {
    for (int i = 0; i < 256; i++) { //initialising s
        s[i] = i;
    }

    int j = 0;
    for (int i = 0; i < 256; i++) {
        j = (j + s[i] + k[i])%256;
        
        unsigned char temp = s[i]; //swapping s[i] and s[j]
        s[i] = s[j];
        s[j] = temp;
        
    }
}  

//PRGA
void rc4_prga(unsigned char s[256], int length) {
    int i,j=0;
    volatile unsigned char z; //to prevent compiler optimizations that might remove the computation of z

    for (int t=0; t<length; t++) {
        i=(i+1)%256;
        j=(j+s[i])%256;

        unsigned char temp = s[i];
        s[i] = s[j];
        s[j] = temp;

        z = s[(s[i]+s[j])%256];
        
    }
   
}

int main() {

    unsigned char key[16]= {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10};
    unsigned char k[256];
    unsigned char s[256];

    for (int i=0; i<256; i++) {
        k[i]=key[i%16];
    }

    int iterations =100000;
    int n=10000;//number of bytes to generate in the keystream

    volatile unsigned long long total_cycles=0;//for KSA
    volatile unsigned long long total_cyclesp=0;//for PRGA

    for (unsigned int i = 0; i < iterations; i++) {

        unsigned long long start, end;
        unsigned long long startp, endp;

        unsigned int aux;
        start = __rdtscp(&aux);
        rc4_init(s, k);
        end = __rdtscp(&aux);

        total_cycles += (end - start);

        unsigned int auxp;
        startp = __rdtscp(&auxp);
        rc4_prga(s, n);
        endp = __rdtscp(&auxp);

        total_cyclesp += (endp - startp);
    }

    printf("Cycles for KSA: %llu\n", total_cycles/iterations);

    double cpb = (double)(total_cyclesp/iterations) / (double)n;

    printf("Cycles per byte for PRGA: %f\n", cpb);
   
;
    return 0;
}

//Outputs -

// when compiled with -OO -

// Cycles for KSA: 5416
// Cycles per byte for PRGA: 15.306000

// when compiled with -O3 

// Cycles for KSA: 1652
// Cycles per byte for PRGA: 6.085900






