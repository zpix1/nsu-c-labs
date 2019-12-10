#include "operations.h"
#include "tokenizer.h"

struct Stack* transform_to_RPN(struct Token** tokens);

// WARNING: Destroys RPN stack
double calculate_from_RPN(struct Stack* RPN);

double calculate_expression(const char* expr);
