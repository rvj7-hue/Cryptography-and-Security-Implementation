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

//Heapsort

void heapify(int arr[], int n, int i) {
    int largest = i;          // Initialize largest as root
    int left = 2 * i + 1;     // left child
    int right = 2 * i + 2;    // right child

    // If left child is larger than root
    if (left < n && arr[left] > arr[largest])
        largest = left;

    // If right child is larger than largest so far
    if (right < n && arr[right] > arr[largest])
        largest = right;

    // If largest is not root
    if (largest != i) {
        int temp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = temp;

        // Recursively heapify the affected subtree
        heapify(arr, n, largest);
    }
}

void heapSort(int arr[], int n) {
    // Build max heap
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // Extract elements one by one
    for (int i = n - 1; i > 0; i--) {
        // Move current root to end
        int temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        // Call max heapify on the reduced heap
        heapify(arr, i, 0);
    }
}

//--------------------------------------------------------------------------

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
    double heapt;
    double tot_m;
    double tot_q;
    double tot_h;

    for (int n=100; n<=1000; n+=100) {
        tot_m=0;
        tot_q=0;
        tot_h=0;
        int *arr = malloc(n * sizeof(int));
        int *cpr_m = malloc(n * sizeof(int));
        int *cpr_q = malloc(n * sizeof(int));
        int *cpr_h = malloc(n * sizeof(int));

        for (int i=0; i<1000; i++) {


            fill_random(arr, n);
           
            cpy(arr, cpr_m, n);
            cpy(arr, cpr_q, n);
            cpy(arr, cpr_h, n);

            clock_t startm = clock();
            mergeSort(cpr_m, 0, n-1);
            clock_t endm = clock();

            merget=(double)(endm - startm) / CLOCKS_PER_SEC;
            tot_m=tot_m+merget;

            clock_t startq = clock();
            quickSort(cpr_q, 0, n-1);
            clock_t endq = clock();
            

            quickt=(double)(endq - startq) / CLOCKS_PER_SEC;
            tot_q=tot_q+quickt;

            clock_t starth = clock();
            heapSort(cpr_h, n);
            clock_t endh = clock();
            

            heapt=(double)(endh - starth) / CLOCKS_PER_SEC;
            tot_h=tot_h+heapt;

            
            

        }
        
        printf("%d,%.6f,%.6f, %.6f \n", n, tot_m/1000, tot_q/1000, tot_h/1000);
        free(arr);
        free(cpr_m);
        free(cpr_q);
        free(cpr_h);

    }

    return 0;
}

//Results -
//#iter, mergetime, quicktime, heaptime
//100,0.000003,0.000018, 0.000010
//200,0.000017,0.000016, 0.000000
//300,0.000058,0.000000, 0.000027
//400,0.000029,0.000033, 0.000056
//500,0.000043,0.000021, 0.000083
//600,0.000054,0.000024, 0.000100
//700,0.000052,0.000056, 0.000082
//800,0.000052,0.000088, 0.000092
//900,0.000098,0.000054, 0.000106
//1000,0.000112,0.000073, 0.000074