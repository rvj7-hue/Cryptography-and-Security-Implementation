#include <stdio.h>
#include <x86intrin.h>
#include <time.h>
#include <stdlib.h>

unsigned int gcd(unsigned int a, unsigned int b) {
    unsigned int c;
    if (a==0) {
        return 0;
    }

    while (a!=0 ) {
        c=a;
        a=b%a;
        b=c;
    }

    return b;
}

//--------------------------------------------------------------------


int main() {

    //checking avg number of clock cycles reqd for addition

    unsigned int l=-1;
    volatile unsigned int r=0; //to prevent optimisation, volatile indicates the variable can change value 
    //at any time.
    unsigned int aux;
    unsigned long long start, end; //__rdtscp() returns a 64‑bit timestamp counter
    printf("l: %u\n", l);
    start = __rdtscp(&aux);
    for (unsigned int i=0; i<l; i++) {
        r=r+1;
    } 
    end = __rdtscp(&aux);

    double cycles = (double)(end - start);
    printf("avg no. of cycles for addition: %f\n", cycles/l);

    //-----------------------------------------------------------------------------------
    
    //checking avg number of clock cycles reqd for gcd

    unsigned int a;
    unsigned int b;

    printf("a,b: ");
    scanf("%d %d", &a, &b);

    unsigned int c;
    unsigned int aux1;
    unsigned long long start1, end1;
 
    start1 = __rdtscp(&aux1);
    for (unsigned int i=0; i<l; i++) {
        c=gcd(a,b);
    } 
    end1 = __rdtscp(&aux1);

    printf("gcd: %u\n", c);
    double cycles1 = (double)(end1 - start1);
    printf("avg no. of cycles for gcd: %f\n", cycles1/l);

    //------------------------------------------------------------------------------------

    // number of clock cycles required for gcd in the average case

    const unsigned int N = 100000; // number of random trials
    unsigned long long total_cycles = 0;

    srand((unsigned int)time(NULL));

    for (unsigned int i = 0; i < N; i++) {
        unsigned int a = rand();
        unsigned int b = rand();

        start = __rdtscp(&aux);
        gcd(a, b);
        end = __rdtscp(&aux);

        total_cycles += (end - start);
    }

    double avg_cycles = (double)total_cycles / N;
    printf("Average clock cycles per gcd: %f\n", avg_cycles);

    return 0;
}

//avg no. of cycles for addition: 1.637297
//a,b: 45 33
//gcd: 3
//avg no. of cycles for gcd: 21.101754

//avg no. of cycles for addition: 2.165103
//a,b: 36 20
//gcd: 4
//avg no. of cycles for gcd: 59.693226
//Average clock cycles per gcd: 711.681990


//unsigned int k=-1;
//    volatile unsigned int p=0;
//    clock_t startc = clock();
//    for (unsigned int i = 0; i < k; i++) {
//        p=p+1;
//    }
//    clock_t endc = clock();
//    double time_taken = ((double)(endc - start)) / CLOCKS_PER_SEC;
//    printf("clock() time: %f seconds\n", time_taken);
