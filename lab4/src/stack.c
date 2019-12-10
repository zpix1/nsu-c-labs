#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

void stack_init(struct Stack* s) {
    s->size = 0;
    s->_maximum_size = MAX_STACK_SIZE;
	s->_data = (StackElement*) malloc(sizeof(StackElement) * s->_maximum_size);
	if (s->_data == NULL) {
		return;
	}
}

size_t stack_size(struct Stack* s) {
    if (s == NULL)
        return 0;
    return s->size;
}

StackElement stack_peek(struct Stack* s) {
	if (s == NULL || s->size == 0)
        return NULL;
	return s->_data[s->size - 1];
}

void stack_push(struct Stack* s, StackElement x) {

    if (s == NULL)
        return;
	
    s->_data[s->size] = x;
    s->size++;
    if (s->size >= s->_maximum_size) {
        s->_maximum_size *= 2;
        s->_data = realloc(s->_data, sizeof(StackElement) * s->_maximum_size);
		if (s->_data == NULL) {
			return;
		}
    }
}

StackElement stack_pop(struct Stack* s) {
    if (s == NULL || s->size == 0) {
        return NULL; // ?
    }
	s->size--;

    if (s->size * 2 == s->_maximum_size) {
        s->_maximum_size /= 2;
        s->_data = realloc(s->_data, sizeof(StackElement) * s->_maximum_size);
    }

    return s->_data[s->size];
}

void free_stack(struct Stack* s) {
	if (s == NULL) {
        return;
    }
	free(s->_data);
	free(s);
}
