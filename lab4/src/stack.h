#include <stdlib.h>

#include "tokenizer.h"

#define MAX_STACK_SIZE 1000

typedef struct Token* StackElement;

struct Stack {
    size_t size;
    size_t _maximum_size;
    StackElement* _data;
};

void stack_init(struct Stack* s);

size_t stack_size(struct Stack* s);

StackElement stack_peek(struct Stack* s);

void stack_push(struct Stack* s, StackElement x);

StackElement stack_pop(struct Stack* s);

void free_stack(struct Stack* s);
