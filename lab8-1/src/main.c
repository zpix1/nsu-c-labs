#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define NO_EDGE (double)-1
#define NO_PARENT -1
#define INT_MAX 2147483647
#define TRUE 1
#define FALSE 0
#define SIZE 5000*5000/2+5000

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

// The fastest way to solve the problem
int* G = NULL;

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


void scanf_ll(long long* a) {
    if (scanf("%lld", a) != 1) {
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

double get_weight(int a, int b) {
    int x = MIN(a,b);
    int y = MAX(a,b);
    return G[x + (y*y+y)/2] == -1 ? INFINITY : (double)G[x + (y*y+y)/2];
}

void set_weight(int a, int b, int w) {
    int x = MIN(a,b);
    int y = MAX(a,b);
    G[x + (y*y+y)/2] = w;
}

// Free e_start, e_end after use
int mst(int n, int** e_start_pointer, int** e_end_pointer) {
    int is_used[n];
    for (int i = 0; i < n; i++) {
        is_used[i] = FALSE;
    }

    double* minimal_edges = malloc(n * sizeof(double));
    double* parents = malloc(n * sizeof(double));

    int* e_start = malloc((n - 1) * sizeof(int));
    int* e_end = malloc((n - 1) * sizeof(int));

    for (int i = 0; i < n; i++) {
        minimal_edges[i] = INFINITY;
        parents[i] = INFINITY;
    }
    int edges_saved = 0;

    minimal_edges[0] = 0;
    for (int i = 0; i < n; i++) {
        int v = -1;
        for (int j = 0 ; j < n ; j++) {
            if (!is_used[j] && (v == -1 || minimal_edges[j] < minimal_edges[v])) {
                v = j;
            }
        }
        if (minimal_edges[v] == INFINITY) {
            free(minimal_edges);
            free(parents);
            free(e_start);
            free(e_end);
            return -1;
        }
    
        is_used[v] = TRUE;
        if (parents[v] != INFINITY) {
            e_start[edges_saved] = v;
            e_end[edges_saved] = parents[v];

            edges_saved++;
        }
    
        for (int to = 0; to < n; to++) {
            if (get_weight(v, to) < minimal_edges[to]) {
                minimal_edges[to] = get_weight(v, to);
                parents[to] = v;
            }
        }
    }
    *e_start_pointer = e_start;
    *e_end_pointer = e_end;
    free(minimal_edges);
    free(parents);

    return 0;
}

int main() {
    int n, m;
    scanf_int(&n);
    my_assert(0 <= n && n <= 5000, "bad number of vertices");
    const int MAXS = n*n/2+5000;
    G = malloc(MAXS * sizeof(double));
    for (int i = 0; i < MAXS; i++) {
        G[i] = NO_EDGE;
    }
    scanf_int(&m);
    my_assert(0 <= m && m <= (n*n+n)/2, "bad number of edges");
    if (n == 0) {
        printf("no spanning tree\n");
        free(G);
        return 0;
    }
    if (m == 0 && n == 1) {
        free(G);
        return 0; 
    }


    for (int i = 0; i < m; i++) {
        int v1, v2;
        long long weight;
        scanf_int(&v1); scanf_int(&v2); scanf_ll(&weight);
        my_assert(1 <= v1 && v1 <= n, "bad vertex");
        my_assert(1 <= v2 && v2 <= n, "bad vertex");
        my_assert(0 <= weight && weight <= INT_MAX, "bad length");
        v1--; v2--;
        set_weight(v1, v2, (int)weight);
    }
    int* e_start;
    int* e_end;
    int status = mst(n, &e_start, &e_end);
    if (status == -1) {
        printf("no spanning tree\n");
    } else {
        for (int i = 0; i < n - 1; i++) {
            printf("%d %d\n", e_start[i] + 1, e_end[i] + 1);
        }

        free(e_start);
        free(e_end);
    }

    free(G);

    return 0;
}
