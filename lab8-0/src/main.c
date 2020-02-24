#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include "dsu.h"

#define INT_MAX 2147483647
#define TRUE 1
#define FALSE 0

struct Edge {
    int weight;
    int start;
    int end;
};

struct Edge* edges = NULL;

void DIE() {
    if (edges != NULL) {
        free(edges);
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

int compare_edges(const void * a, const void * b) {
    return ( ((struct Edge*)a)->weight - ((struct Edge*)b)->weight ); 
}

// 0 - tree was not found, 1 - tree was found, 2 - an error was encountered
// mst_edges should be freed
int find_minimum_spanning_tree(struct Edge* edges, int n, int m, struct Edge** mst_edges_pointer) {
    
    qsort(edges, m, sizeof(struct Edge), compare_edges);

    int* DSU = (int*)malloc(sizeof(int) * n);
    if (DSU == NULL) {
        return 2;
    }

    for (int i = 0; i < n; i++) {
        create_set(DSU, i);
    }

    int current_v_i = 0;
    struct Edge* mst_edges = (struct Edge*)malloc(sizeof(struct Edge) * (n - 1));
    if (mst_edges == NULL) {
        free(DSU);
        return 2;
    }
    for (int i = 0; i < m; i++) {
        int s = edges[i].start;
        int f = edges[i].end;
        int weight = edges[i].weight;
        if (get_set(DSU, s) != get_set(DSU, f)) {
            connect_sets(DSU, s, f);
            mst_edges[current_v_i].start = s;
            mst_edges[current_v_i].end = f;
            mst_edges[current_v_i].weight = weight;
            current_v_i++;
        }
    }
    free(DSU);

    *mst_edges_pointer = mst_edges;

    return current_v_i == n - 1;
}

int main() {
    int n, m;
    scanf_int(&n);
    my_assert(0 <= n && n <= 5000, "bad number of vertices");

    scanf_int(&m);
    my_assert(0 <= m && m <= (n*n+n)/2, "bad number of edges");
    
    if (n == 0) {
        printf("no spanning tree\n");
        return 0;
    }

    if (m > 0) {
        edges = (struct Edge*)malloc(sizeof(struct Edge) * m);
        if (edges == NULL) {
            DIE();
        }
    }

    for (int i = 0; i < m; i++) {
        int v1, v2;
        long long weight;
        scanf_int(&v1); scanf_int(&v2); scanf_ll(&weight);
        my_assert(1 <= v1 && v1 <= n, "bad vertex");
        my_assert(1 <= v2 && v2 <= n, "bad vertex");
        my_assert(0 <= weight && weight <= INT_MAX, "bad length");
        v1--; v2--;
        edges[i].weight = weight;
        edges[i].start = v1;
        edges[i].end = v2;
    }

    struct Edge* ans;
    int status = find_minimum_spanning_tree(edges, n, m, &ans);
    if (status == 2) {
        free(edges);
        DIE();
    }
    if (!status) {
        printf("no spanning tree\n");
    } else {
        for (int i = 0; i < n - 1; i++) {
            printf("%d %d\n", ans[i].start + 1, ans[i].end + 1);
        }
    }

    free(ans);
    free(edges);
    
    return 0;
}
