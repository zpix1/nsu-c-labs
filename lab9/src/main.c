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

int get_weight(int a, int b) {
    int x = MIN(a,b);
    int y = MAX(a,b);
    return G[x + (y*y+y)/2];
}

void set_weight(int a, int b, int w) {
    int x = MIN(a,b);
    int y = MAX(a,b);
    G[x + (y*y+y)/2] = w;
}

// Return value and parents_pointer should be freed 
double* shortest_path(int n, int s, int** parents_pointer) {
    int is_used[n];
    for (int i = 0; i < n; i++) {
        is_used[i] = FALSE;
    }

    double* paths = malloc(n * sizeof(double));
    int* parents = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        paths[i] = INFINITY;
        parents[i] = NO_PARENT;
    }
    paths[s] = 0; 

    for (int i = 0; i < n; i++) {
        // Find min
        int v = -1;
        for (int j = 0; j < n; j++) {
            if (!is_used[j] && (v == -1 || paths[j] < paths[v])) {
                v = j;
            }
        }

        if (paths[v] == INFINITY) {
            break;
        }
        is_used[v] = TRUE;

        for (int to = 0; to < n; to++) {
            double weight;
            if ((weight = get_weight(v, to)) != NO_EDGE) {
                if (paths[v] + weight < paths[to]) {
                    parents[to] = v;
                    paths[to] = paths[v] + weight;
                }
            }
        }
    }
    *parents_pointer = parents;
    return paths;
}

int main() {
    int n, s, f, m;
    scanf_int(&n);
    my_assert(0 <= n && n <= 5000, "bad number of vertices");
    const int MAXS = n*n/2+5000;
    G = malloc(MAXS * sizeof(double));
    for (int i = 0; i < MAXS; i++) {
        G[i] = NO_EDGE;
    }

    scanf_int(&s);
    my_assert(1 <= s && s <= n, "bad vertex");

    scanf_int(&f);
    my_assert(1 <= f && f <= n, "bad vertex");
    s--; f--;

    scanf_int(&m);
    my_assert(0 <= m && m <= (n*n+n)/2, "bad number of edges");
    
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
    
    int* parents;
    double* paths = shortest_path(n, s, &parents);
    for (int i = 0; i < n; i++) {
        if (paths[i] == INFINITY) {
            printf("oo ");
        } else if (paths[i] > INT_MAX) {
            printf("INT_MAX+ ");
        } else {
            printf("%d ", (int)paths[i]);
        }
        
    }
    printf("\n");

    if (paths[f] == INFINITY) {
        printf("no path\n");
    } else {
        int bad_c = 0;
        for (int  v = 0; v < n; v++) {
            if (get_weight(v, f) != NO_EDGE) {
                if (paths[v] + get_weight(v, f) > INT_MAX) {
                    bad_c++;
                }
            }
        }
        if (bad_c >= 2 && paths[f] >= INT_MAX) {
            printf("overflow\n");
        } else {
            int path_length = 0;
            int path[n]; 
            for (int v = f; v != s; v = parents[v]) {
                path[path_length] = v;
                path_length++;
            }
            // BTW Path from F to S required, not S to F
            // for (int i = path_length - 1; i >= 0; i--) {
            //     printf("%d ", path[i] + 1);
            // }
            for (int i = 0; i <= path_length - 1; i++) {
                printf("%d ", path[i] + 1);
            }
            printf("%d ", s + 1);
            printf("\n");
        }
    }

    free(parents);
    free(paths);
    free(G);
    return 0;
}
