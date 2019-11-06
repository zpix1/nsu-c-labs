#include <stdio.h>
#include <assert.h>
#include "pq.h"

int cmpfunc(const void * a, const void * b) {
    return -(*(int*)a - *(int*)b);
}

int main() {
    int a[] = {2, 3, 5, 6, 7};

    struct priority_queue* pq = create_pq(sizeof(int), cmpfunc);

    pq_add(pq, &a[2]);
    assert(a[2] == *((int*)pq_pop(pq)));

    pq_add(pq, &a[2]);
    pq_add(pq, &a[4]);
    pq_add(pq, &a[3]);
    assert(a[4] == *((int*)pq_pop(pq)));
    assert(a[3] == *((int*)pq_pop(pq)));
    assert(a[2] == *((int*)pq_pop(pq)));

    assert(NULL == pq_pop(pq));

    delete_pq(pq);

    int n;
    if (scanf("%d", &n) != 1) {
        fprintf(stderr, "Scanf error\n");
        return 1;
    }
    int arr[n];
    for (int i = 0; i < n; i++) {
        if (scanf("%d", &arr[i]) != 1) {
            fprintf(stderr, "Scanf error\n");
            return 1;
        }
    }

    myheapsort(arr, n, sizeof(int), cmpfunc);

    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}
