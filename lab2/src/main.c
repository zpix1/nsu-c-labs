#include <stdio.h>
#include <string.h>

void swap(char* a, char* b) {
    char t = *a;
    *a = *b;
    *b = t;
}


// Return 0 if there is no next permutation
// Otherwise return 1
int next_permutation(char* perm, int n) {
    int j = -1;
    for (int i = 0; i < n - 1; i++) {
        if (perm[i] < perm[i+1]) {
            j = i;
        }
    }

    if (j == -1) return 0;

    int max_l = j;
    for (int l = j + 1; l < n; l++) {
        if (perm[l] > perm[j]) {
            max_l = l;
        }
    }

    swap(&perm[max_l], &perm[j]);

    for (int i = 0; i < (n - j)/2; i++) {
        swap(&perm[j + i + 1], &perm[n - i - 1]);
    }

    return 1;
}

// Return 1 if it is a permutation
// Otherwise return 0
int check_permutation(const char* perm, int n) {
    int check_arr[10];
    for (int i = 0; i < 10; i++) {
        check_arr[i] = 0;
    }
    for (int i = 0; i < n; i++) {
        int el = perm[i] - '0';
        if (el >= 10 || el < 0) {
            return 0;
        }
        check_arr[el]++;
        if (check_arr[el] > 1) {
            return 0;
        }
    }
    return 1;
}

int main() {
    char current_perm[11];
    int n;

    if (scanf("%10s %d", current_perm, &n) != 2) {
        return 1;
    }

    if (!check_permutation(current_perm, strlen(current_perm))) {
        printf("bad input\n");
        return 0;
    }

    for (int i = 0; i < n && next_permutation(current_perm, strlen(current_perm)); i++) {
        printf("%s\n", current_perm);
    }
}
