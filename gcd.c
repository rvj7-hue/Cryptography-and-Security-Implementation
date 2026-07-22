#include <stdio.h>
#include <time.h>

unsigned int gcd(unsigned int x, unsigned int y) {//we assume x>=y
    if (x==0) {
        return 0;
    }
    while (y != 0) {
        unsigned int temp = y;
        y = x % y; 
        x = temp;
    }
    return x;
}

int main() {
    unsigned int x, y;
    printf("Enter two unsigned integers: ");
    scanf("%u %u", &x, &y);

    printf("GCD of %u and %u is %u\n", x, y, gcd(x, y));
    
    float a;
    int repcount=2000000;
    clock_t t1, t2;
    t1 = clock();
    float timeTaken;
    for (int i = 0; i < repcount; i++) {
        a = gcd(x,y);
    }
    t2=clock();
    timeTaken = ( (float)(t2 - t1) / (float)CLOCKS_PER_SEC ) / (float)repcount;
printf("Time taken = %f seconds\n", timeTaken);

    return 0;
}