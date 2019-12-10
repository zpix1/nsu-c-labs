#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "operations.h"

const char* OPERATIONS_LIST[OP_COUNT] = {
    "+",
    "-",
    "*",
    "/",
    "sin",
    "cos",
    "factorial"
};

enum OperationArity get_operation_arity(enum OperationType operation_type) {
    switch (operation_type) {
        case Plus:
        case Minus:
        case Multiply:
        case Divide:
            return Binary;
        default:
            return Unary;
    }
}


// Should be changed to: 1. Binary search
//                       2. Search tree?
int find_str_in_list(const char* str, const char** list, size_t length) {
    for (size_t i = 0; i < length; i++) {
        if (strcmp(str, list[i]) == 0) {
            return (int)i;
        }
    }
    return -1;
}

enum OperationType find_operation(const char* str) {
    int res = find_str_in_list(str, OPERATIONS_LIST, OP_COUNT);
    if (res == -1) {
        return NOT_FOUND;
    }
    return (enum OperationType)res;
}

int get_operation_priority(enum OperationType operation_type) {
    switch (operation_type) {
        case Plus:
        case Minus:
            return 1;
        case Multiply:
        case Divide:
            return 2;
        default:
            return 3;
    }
}

double run_unary_operation(enum OperationType operation_type, double a) {
    switch (operation_type) {
        case Sin:
            return sin(a);
        case Cos:
            return cos(a);
        case Factorial: {
            int fact = 1;
            for (int c = 1; c <= (int)a; c++)
                fact = fact * c;
            return (double)fact;
        }
        default:
            return -1;
    }
}

double run_binary_operation(enum OperationType operation_type, double a, double b) {
    switch (operation_type) {
        case Plus:
            return a+b;
        case Minus:
            return a-b;
        case Multiply:
            return a*b;
        case Divide: {
            if (fabs(b) < 0.00001) {
                return INFINITY;
            }
            return a/b;
        }
        default:
            return -1.0;
    }
}
