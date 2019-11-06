#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

#include "pq.h"

static void swap_ranges(void* a, void* b, size_t size) {
    for (size_t i = 0; i < size; i++) {
        char t = ((char*)a)[i];
        ((char*)a)[i] = ((char*)b)[i];
        ((char*)b)[i] = t;
    }
}

struct priority_queue* create_pq(size_t size, int (*compar)(const void *, const void*)) {
    struct priority_queue* pq = (struct priority_queue*) malloc(sizeof(struct priority_queue));
    pq->size = size;
    pq->compar = compar;
    pq->n_items = 0;
    pq->_allocated_size = 0;
    pq->base = NULL;
    return pq;
}

static void* get_element(struct priority_queue* pq, const size_t i) {
    return (void*)((char*)pq->base + i * pq->size);
}

void pq_add(struct priority_queue* pq, void* element) {
    if (pq == NULL || element == NULL) {
        return;
    }
    // Allocate base if not exists
    if (pq->base == NULL) {
        pq->base = malloc(INITIAL_SIZE * pq->size);
        if (pq->base == NULL) return; 
        pq->_allocated_size = INITIAL_SIZE * pq->size;
    }
    
    // Extend base if not enough
    if (pq->n_items * pq->size == pq->_allocated_size) {
        void* tmp = realloc(pq->base, pq->_allocated_size * GROWTH_RATE);
        if (tmp == NULL) return;
        pq->base = tmp;
        pq->_allocated_size = pq->_allocated_size * GROWTH_RATE;
    }
    
    // Copy element to the end of base
    memcpy(get_element(pq, pq->n_items), element, pq->size);
    pq->n_items++;
    size_t i = pq->n_items - 1;
    
    while(i > 0 && pq->compar(get_element(pq, (i-1)/2), get_element(pq, i)) > 0) {
        swap_ranges(get_element(pq, (i-1)/2), get_element(pq, i), pq->size);
        i = (i-1)/2;
    }
}

static void heapify(struct priority_queue* pq, size_t i) { 
    size_t current = i;
    size_t left = 2 * i + 1;
    size_t right = 2 * i + 2;
  
    if (left < pq->n_items && pq->compar(get_element(pq, left), get_element(pq, current)) < 0) {
        current = left; 
    }
    if (right < pq->n_items && pq->compar(get_element(pq, right), get_element(pq, current)) < 0) {
        current = right; 
    } 
    
    if (current != i) { 
        swap_ranges(get_element(pq, i), get_element(pq, current), pq->size);
        heapify(pq, current); 
    } 
} 

void* pq_pop(struct priority_queue* pq) {
    if (pq == NULL) {
        return NULL;
    }
    if (pq->n_items == 0) {
        return NULL;
    }
    if (pq->n_items * pq->size * GROWTH_RATE < pq->_allocated_size) {
        void* tmp = realloc(pq->base, (pq->n_items + 1) * pq->size);
        if (tmp == NULL) return NULL;
        pq->base = tmp;
        pq->_allocated_size = (pq->n_items + 1) * pq->size;
    }
    pq->n_items--;
    
    swap_ranges(get_element(pq, 0), get_element(pq, pq->n_items), pq->size);
    heapify(pq, 0);

    return get_element(pq, pq->n_items);
}

void delete_pq(struct priority_queue* pq) {
    free(pq->base);
    free(pq);
}

void myheapsort(void* base, size_t n_items, size_t size, int (*compar)(const void *, const void*)) {
    if (base == NULL) return;
    struct priority_queue* pq = create_pq(size, compar);
    if (pq == NULL) return;
    pq->base = base;
    pq->n_items = n_items;
    // Because size_t is never < 0
    for (int i = n_items / 2 - 1; i >= 0; i--) {
        heapify(pq, i);
    }
    for (size_t i = 0; i < n_items; i++) { 
        pq->n_items--;
        swap_ranges(get_element(pq, 0), get_element(pq, pq->n_items), pq->size);
        heapify(pq, 0);
    }

    free(pq);
}
