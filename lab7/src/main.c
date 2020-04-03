#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define INT_MAX 2147483647

// The fastest way to solve the problem
char* G = NULL;

void DIE() {
    if (G != NULL) {
        free(G);
    }
    exit(0);
}

void scanf_int(int* a) {
    if (scanf("%d", a) != 1) {
        printf("bad number of lines\n");
        DIE();
    }
}

void my_assert(int bool, char* error_str) {
    if (!bool) {
        printf("%s\n", error_str);
        DIE();
    }
}

// color[v] = 0 - white, 1 - gray, 2 - black
int dfs(int v, int n, int* color, int* nitems_ptr, int* sorted_vertices) {
    if (color[v] == 2) {
        return 0;
    }

    if (color[v] == 1) {
        // there is a cycle, can't sort
        return 1;
    }
    
    color[v] = 1;

    for (int new_v = 0; new_v < n; new_v++) {
        if (G[v * n + new_v] == 1) {
            if (dfs(new_v, n, color, nitems_ptr, sorted_vertices) != 0) {
                return 1;
            }
        }
    }

    color[v] = 2;

    sorted_vertices[*nitems_ptr] = v;
    (*nitems_ptr)++;

    return 0;
}

// 0 - sorted, != 0 - can't sort, sorted_vertices is reversed 
int topological_sort(int n, int** sorted_vertices_ptr) {
    int color[n];
    for (int i = 0; i < n; i++) {
        color[i] = 0;
    }

    int* sorted_vertices = malloc(sizeof(int) * n);
    int nitems = 0;

    for (int v = 0; v < n; v++) {
        if (dfs(v, n, color, &nitems, sorted_vertices) != 0) {
            free(sorted_vertices);
            return 1;
        }
    }
    
    *sorted_vertices_ptr = sorted_vertices;

    return 0;
}

int main() {
    int n, m;
    scanf_int(&n);
    my_assert(0 <= n && n <= 1000, "bad number of vertices");
    const int MAXS = n*n;
    G = malloc(MAXS * sizeof(double));
    memset(G, 0, MAXS);

    scanf_int(&m);
    my_assert(0 <= m && m <= (n*n+n)/2, "bad number of edges");
    
    for (int i = 0; i < m; i++) {
        int v1, v2;
        scanf_int(&v1); scanf_int(&v2);
        my_assert(1 <= v1 && v1 <= n, "bad vertex");
        my_assert(1 <= v2 && v2 <= n, "bad vertex");
        v1--; v2--;
        G[v1 * n + v2] = 1;
    }
    
    int* sorted;
    int status = topological_sort(n, &sorted);

    if (status != 0) {
        printf("impossible to sort\n");
    } else {
        for (int i = n - 1; i >= 0; i--) {
            printf("%d ", sorted[i]+1);
        }
        printf("\n");
        free(sorted);
    }
    free(G);
    return 0;
}
