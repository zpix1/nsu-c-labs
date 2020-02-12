#pragma once

#include "tokenizer.h"
#include "operations.h"

struct Stack* transform_to_RPN(struct Token** tokens, enum ErrorCode* error_code);

// WARNING: Destroys RPN stack
double calculate_from_RPN(struct Stack* RPN, const double previous_value, enum ErrorCode* error_code);

double calculate_expression(const char* expr, const double previous_value, enum ErrorCode* error_code);
