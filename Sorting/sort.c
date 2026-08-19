#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <x86intrin.h>
#include <string.h>

// Global counters
long long quick_comparisons, quick_swaps;
long long merge_comparisons, merge_swaps;
long long heap_comparisons, heap_swaps;
long long rquick_comparisons, rquick_swaps;

// ---------------- Utility ----------------
void swap(int *a, int *b, long long *swap_counter) {
    int temp = *a;
    *a = *b;
    *b = temp;
    (*swap_counter)++;
}

// ---------------- QUICKSORT ----------------
int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        quick_comparisons++;
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j], &quick_swaps);
        }
    }
    swap(&arr[i+1], &arr[high], &quick_swaps);
    return i+1;
}

void quicksort(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi-1);
        quicksort(arr, pi+1, high);
    }
}

// ---------------- RANDOMIZED QUICKSORT ----------------

// Randomized partition
int r_partition(int arr[], int low, int high) {
    // Pick a random pivot index between low and high
    int pivotIndex = low + rand() % (high - low + 1);

    // Swap pivot with the last element
    swap(&arr[pivotIndex], &arr[high], &rquick_swaps);

    int pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; j++) {
        rquick_comparisons++;
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j], &rquick_swaps);
        }
    }
    swap(&arr[i+1], &arr[high], &rquick_swaps);
    return i+1;
}

void r_quicksort(int arr[], int low, int high) {
    if (low < high) {
        int pi = r_partition(arr, low, high);
        r_quicksort(arr, low, pi-1);
        r_quicksort(arr, pi+1, high);
    }
}


// ---------------- MERGESORT ----------------
void merge(int arr[], int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m; //l=4, m=7, r=10 , n1=4, n2=3 4,5,6,7 8,9,10
    int L[n1], R[n2];
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        merge_comparisons++;
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else {
            arr[k++] = R[j++];
            merge_swaps++; // treat placing R[j] as a "swap"
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) {
        arr[k++] = R[j++];
        merge_swaps++;
    }
}

void mergeSort(int arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

// ---------------- HEAPSORT ----------------
void heapify(int arr[], int n, int i) {
    int largest = i;
    int left = 2*i + 1;
    int right = 2*i + 2;

    if (left < n) { heap_comparisons++; if (arr[left] > arr[largest]) largest = left; }
    if (right < n) { heap_comparisons++; if (arr[right] > arr[largest]) largest = right; }

    if (largest != i) {
        swap(&arr[i], &arr[largest], &heap_swaps);
        heapify(arr, n, largest);
    }
}

void heapSort(int arr[], int n) {
    for (int i = n/2 - 1; i >= 0; i--) heapify(arr, n, i);
    for (int i = n-1; i > 0; i--) {
        swap(&arr[0], &arr[i], &heap_swaps);
        heapify(arr, i, 0);
    }
}

// ---------------- MAIN ----------------
int main() {
    srand(time(NULL));
    int sizes[] = {100, 200, 400, 600, 800, 1000};
    int num_sizes = 6;
    int iterations = 100; // run each size multiple times

    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];

        // Reset totals
        long long total_rquick_comp=0, total_rquick_swaps=0;
        long long total_quick_comp=0, total_quick_swaps=0;
        long long total_merge_comp=0, total_merge_swaps=0;
        long long total_heap_comp=0, total_heap_swaps=0;
        unsigned long long total_quick_cycles=0, total_merge_cycles=0, total_heap_cycles=0, total_rquick_cycles=0;

        for (int it = 0; it < iterations; it++) {
            // Generate random array
            int *arr = malloc(n * sizeof(int));
            for (int i = 0; i < n; i++) arr[i] = rand() % 10000; //random array

            // QuickSort
            int *copy1 = malloc(n * sizeof(int));//copy array to sort
            memcpy(copy1, arr, n*sizeof(int));
            quick_comparisons = quick_swaps = 0;

            unsigned long long start = __rdtsc();
            quicksort(copy1, 0, n-1); //running quicksort
            unsigned long long end = __rdtsc();
            total_quick_cycles += (end - start);
            total_quick_comp += quick_comparisons;
            total_quick_swaps += quick_swaps;
            free(copy1);

            // MergeSort
            int *copy2 = malloc(n * sizeof(int));
            memcpy(copy2, arr, n*sizeof(int));
            merge_comparisons = merge_swaps = 0;

            start = __rdtsc();
            mergeSort(copy2, 0, n-1);
            end = __rdtsc();

            total_merge_cycles += (end - start);
            total_merge_comp += merge_comparisons;
            total_merge_swaps += merge_swaps;
            free(copy2);

            // HeapSort
            int *copy3 = malloc(n * sizeof(int));
            memcpy(copy3, arr, n*sizeof(int));
            heap_comparisons = heap_swaps = 0;
            start = __rdtsc();
            heapSort(copy3, n);
            end = __rdtsc();
            total_heap_cycles += (end - start);
            total_heap_comp += heap_comparisons;
            total_heap_swaps += heap_swaps;
            free(copy3);

            //randomized quicksort
            int *copy4 = malloc(n * sizeof(int));
            memcpy(copy4, arr, n*sizeof(int));
            rquick_comparisons = rquick_swaps = 0;
            start = __rdtsc();
            r_quicksort(copy4, 0, n-1);
            end = __rdtsc();
            total_rquick_cycles += (end - start);
            total_rquick_comp += rquick_comparisons;
            total_rquick_swaps += rquick_swaps;
            free(copy4);

            free(arr);
        }

        // Print averages
        printf("n=%d (avg over %d runs)\n", n, iterations);

        printf("  QuickSort: comps=%lld, swaps=%lld, cycles=%llu, ratio=%.3f\n",
               total_quick_comp/iterations, total_quick_swaps/iterations,
               total_quick_cycles/iterations,
               (double)(total_quick_comp/iterations)/(n*log2(n)));


        printf("  Randomised QuickSort: comps=%lld, swaps=%lld, cycles=%llu, ratio=%.3f\n",
               total_rquick_comp/iterations, total_rquick_swaps/iterations,
               total_rquick_cycles/iterations,
               (double)(total_rquick_comp/iterations)/(n*log2(n)));


        printf("  MergeSort: comps=%lld, swaps=%lld, cycles=%llu, ratio=%.3f\n",
               total_merge_comp/iterations, total_merge_swaps/iterations,
               total_merge_cycles/iterations,
               (double)(total_merge_comp/iterations)/(n*log2(n)));

               
        printf("  HeapSort:  comps=%lld, swaps=%lld, cycles=%llu, ratio=%.3f\n\n",
               total_heap_comp/iterations, total_heap_swaps/iterations,
               total_heap_cycles/iterations,
               (double)(total_heap_comp/iterations)/(n*log2(n)));
    }
    return 0;
}

// n=100 (avg over 100 runs)
//   QuickSort: comps=639, swaps=392, cycles=15091, ratio=0.962
//   MergeSort: comps=541, swaps=316, cycles=17926, ratio=0.814
//   HeapSort:  comps=1027, swaps=581, cycles=22971, ratio=1.546

// n=200 (avg over 100 runs)
//   QuickSort: comps=1591, swaps=920, cycles=105645, ratio=1.041
//   MergeSort: comps=1279, swaps=732, cycles=105450, ratio=0.837
//   HeapSort:  comps=2453, swaps=1359, cycles=142712, ratio=1.605

// n=400 (avg over 100 runs)
//   QuickSort: comps=3693, swaps=2135, cycles=94650, ratio=1.068
//   MergeSort: comps=2961, swaps=1664, cycles=117290, ratio=0.856
//   HeapSort:  comps=5702, swaps=3114, cycles=160142, ratio=1.649

// n=600 (avg over 100 runs)
//   QuickSort: comps=5958, swaps=3422, cycles=85581, ratio=1.076
//   MergeSort: comps=4796, swaps=2660, cycles=105146, ratio=0.866
//   HeapSort:  comps=9251, swaps=5020, cycles=155425, ratio=1.671

// n=800 (avg over 100 runs)
//   QuickSort: comps=8431, swaps=4706, cycles=118190, ratio=1.093
//   MergeSort: comps=6719, swaps=3728, cycles=143449, ratio=0.871
//   HeapSort:  comps=13008, swaps=7028, cycles=213427, ratio=1.686

// n=1000 (avg over 100 runs)
//   QuickSort: comps=11164, swaps=6326, cycles=155888, ratio=1.120
//   MergeSort: comps=8707, swaps=4932, cycles=185930, ratio=0.874
//   HeapSort:  comps=16851, swaps=9080, cycles=270938, ratio=1.691


//What we see is that the number of comparisons increases in the order - mergesort < quicksort < heapsort. 
//This is because mergesort always divides the array into two halves and merges them back, leading to fewer comparisons. 
//Quicksort's performance depends on the choice of pivot and so the number of comparisons for mergesort is generally lower than quicksort.
//Heapsort, on the other hand, has a higher number of comparisons due to the nature of maintaining the heap property during sorting.

//However, we can see that the number of cycles in mergesort is higher than quicksort, even though it has fewer comparisons. 
//This is because mergesort involves more data movement (swaps) due to the merging process, which can be more costly in terms of CPU cycles.





