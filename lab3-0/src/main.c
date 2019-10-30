#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 2000000
#define SWAP(x, y) do { int SWAP = x; x = y; y = SWAP; } while (0)

// Sort given array [begin, end);
void my_qsort(const int* begin, const int* end);

// Utility func
void _my_qsort(const int* from, const int* to);

// Return pointer to pivot element in given array
int* get_pivot(const int* from, const int* to);

int main() {
    int n;
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "Scanf error\n");
        return 0;
    }

    if (n > MAX_SIZE) {
        fprintf(stderr, "Max size error\n");
        return 0;
    }

    int* arr = (int*)malloc(sizeof(int) * n);

    for (size_t i = 0; i < (size_t)n; i++) {
        if (scanf("%d", &arr[i]) != 1) {
            fprintf(stderr, "Scanf error\n");
            free(arr);
            return 0;
        }
    }
    my_qsort(arr, arr+n);

    for (size_t i = 0; i < (size_t)n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    free(arr);
    return 0;
}

void my_qsort(const int* begin, const int* end) {
    if (begin == NULL || end == NULL) {
        return;
    }
    _my_qsort(begin, end - 1);
}

void _my_qsort(const int* from, const int* to) {
    int* l = (int*)from;
    int* r = (int*)to;
    int pivot = *get_pivot(from, to);
    while (l <= r) {
        for (; *l < pivot; l++);
        for (; *r > pivot; r--);
        if (l <= r) {
            SWAP(*l, *r);
            l++;
            r--;
        }
    }
    if (from < r) {
        _my_qsort(from, r);
    }
    if (to > l) {
        _my_qsort(l, to);
    }
}

int* get_pivot(const int* from, const int* to) {
    return (int*)(from + (to - from) / 2);
}
