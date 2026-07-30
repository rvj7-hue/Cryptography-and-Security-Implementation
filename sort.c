#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//Mergesort

void merge(int arr[], int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    int L[n1], R[n2];
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(int arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

//---------------------------------------------------------------------------------------

//Quicksort

int partition(int arr[], int low, int high) {
    int pivot = arr[high], i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            int tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
        }
    }
    int tmp = arr[i + 1]; arr[i + 1] = arr[high]; arr[high] = tmp;
    return i + 1;
}

void quickSort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

//------------------------------------------------------------------------------------------

void fill_random(int arr[], int n) {
    for (int i = 0; i < n; i++) arr[i] = rand();
}

void cpy(int arr[], int cpr[], int n ) {
    for (int i = 0; i < n; i++) cpr[i]=arr[i];
}

int main() {

    srand(time(NULL));

    double merget;
    double quickt;
    double tot_m;
    double tot_q;

    for (int n=100; n<=1000; n+=100) {
        tot_m=0;
        tot_q=0;
        int *arr = malloc(n * sizeof(int));
        int *cpr = malloc(n * sizeof(int));
        for (int i=0; i<1000; i++) {


            fill_random(arr, n);
            cpy(arr, cpr, n);

            clock_t startm = clock();
            mergeSort(arr, 0, n-1);
            clock_t endm = clock();

            merget=(double)(endm - startm) / CLOCKS_PER_SEC;
            tot_m=tot_m+merget;

            clock_t startq = clock();
            quickSort(arr, 0, n-1);
            clock_t endq = clock();
            

            quickt=(double)(endq - startq) / CLOCKS_PER_SEC;
            tot_q=tot_q+quickt;

            
            

        }
        free(arr);
        free(cpr);
        printf("%d,%.6f,%.6f\n", n, tot_m/1000, tot_q/1000);


    }

    return 0;
}

//Results -
//#iter, mergetime, quicktime
//100,0.000019,0.000010
//200,0.000016,0.000057
//300,0.000021,0.000116
//400,0.000071,0.000175
//500,0.000037,0.000310
//600,0.000045,0.000453
//700,0.000046,0.000509
//800,0.000092,0.000573
//900,0.000107,0.000749
//1000,0.000102,0.000910