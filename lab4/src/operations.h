#pragma once

#include "utils.h"

#define OP_COUNT 8

enum OperationType {
    Plus, Minus,
    Multiply, Divide,
    Sin, Cos, Factorial, Pow,
    NOT_FOUND
};

extern const char* OPERATIONS_LIST[OP_COUNT];

enum OperationArity {
    Unary, Binary
};

enum OperationAssociativity {
    Left, Right
};

int find_str_in_list(const char* str, const char** list, size_t length);

enum OperationArity get_operation_arity(enum OperationType operation_type);

enum OperationAssociativity get_operation_associativity(enum OperationType operation_type);

enum OperationType find_operation(const char* str);

int get_operation_priority(enum OperationType operation_type);

double run_unary_operation(enum OperationType operation_type, double a, enum ErrorCode* error_code);

double run_binary_operation(enum OperationType operation_type, double a, double b, enum ErrorCode* error_code);
