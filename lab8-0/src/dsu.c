#include "dsu.h"

void create_set(int* arr, int v) {
    if (arr == NULL) {
        return;
    }
    arr[v] = v;
}

int get_set(int* arr, int v) {
    if (arr == NULL) {
        return -1;
    }
    if (v == arr[v]) {
        return v;
    } else {
        arr[v] = get_set(arr, arr[v]);
        return arr[v];
    }
}

void connect_sets(int* arr, int v1, int v2) {
    if (arr == NULL) {
        return;
    }
    int s1 = get_set(arr, v1);
    int s2 = get_set(arr, v2);
    if (s1 != s2) {
        arr[s2] = s1;
    }
}
