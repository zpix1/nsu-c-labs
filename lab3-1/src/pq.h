#pragma once
#include <stdlib.h>

#define INITIAL_SIZE 100
#define GROWTH_RATE 2

// Priority Queue
// n_items - the number of elements
// size - the size in bytes of each element
// compar - the function that compares 2 elements
// base - the pointer to the first element of array to be sorted
// (stdlib qsort name style)
struct priority_queue {
    size_t n_items;
    size_t size;
    size_t _allocated_size;
    int (*compar)(const void *, const void*);
    void* base;
};

// Create a pq
struct priority_queue* create_pq(size_t size, int (*compar)(const void *, const void*));

// Free given pq
void delete_pq(struct priority_queue* pq);

// Add an element to pq
void pq_add(struct priority_queue* pq, void* element);

// Remove element from pq, return pointer to it
void* pq_pop(struct priority_queue* pq);

// Sort given array using heapsort
void myheapsort(void* base, size_t n_items, size_t size, int (*compar)(const void *, const void*));
