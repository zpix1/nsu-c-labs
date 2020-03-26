
#include <stdio.h>
#include <stdlib.h>

#include "coder.h"

typedef struct {
    int priority;
    FreqNode* data;
} node_t;

typedef struct {
    node_t* nodes;
    int len;
    int size;
} heap_t;


// PQ realisation was stolen and adapted for this project

void push(heap_t* h, int priority, FreqNode* data);

FreqNode* pop(heap_t* h);
