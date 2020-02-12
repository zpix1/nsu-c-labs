#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "calculator.h"
#include "stack.h"

static void free_tokens(struct Token** tokens) {
    size_t i;
    for (i = 0; tokens[i]->type != END; i++) {
        free(tokens[i]);
    }
    free(tokens[i]);
    free(tokens);
}

struct Stack* transform_to_RPN(struct Token** tokens, enum ErrorCode* error_code) {
    struct Stack* output_stack = malloc(sizeof(struct Stack));
    stack_init(output_stack);

    // Maybe should be converted back to malloc style
    struct Stack op_stack;
    stack_init(&op_stack);
    int bad_bracket_pattern_flag = 0;
    size_t i;
    for (i = 0; tokens[i]->type != END; i++) {
        if (tokens[i]->type == Num || tokens[i]->type == END || tokens[i]->type == Prev) {
            bad_bracket_pattern_flag = 0;
            stack_push(output_stack, tokens[i]);
        } else if (tokens[i]->type == Op) {
            if (stack_size(&op_stack) != 0) {
                while (stack_size(&op_stack) != 0 && stack_peek(&op_stack)->type == Op && \
                    ((get_operation_associativity(stack_peek(&op_stack)->operation) == Left && \
                        get_operation_priority(stack_peek(&op_stack)->operation) >= get_operation_priority(tokens[i]->operation)) || \
                    (get_operation_associativity(stack_peek(&op_stack)->operation) == Right && \
                        get_operation_priority(stack_peek(&op_stack)->operation) > get_operation_priority(tokens[i]->operation)))
                    ) {
                    stack_push(output_stack, stack_pop(&op_stack));
                }
            }
            stack_push(&op_stack, tokens[i]);
        } else if (tokens[i]->type == BracketOn) {
            bad_bracket_pattern_flag = 1;
            stack_push(&op_stack, tokens[i]);
        } else if (tokens[i]->type == BracketOff) {
            if (bad_bracket_pattern_flag == 1 || stack_size(&op_stack) == 0) {
                free_tokens(tokens);
                free_stack(output_stack);
                free(op_stack._data);
                *error_code = SyntaxError;
                return NULL;
            }
            while (stack_peek(&op_stack)->type != BracketOn) {
                stack_push(output_stack, stack_pop(&op_stack));
                if (stack_size(&op_stack) == 0) {
                    free_tokens(tokens);
                    free_stack(output_stack);
                    free(op_stack._data);
                    *error_code = SyntaxError;
                    return NULL;
                }
            }
            free(stack_pop(&op_stack));
            free(tokens[i]);
        }
    }

    while (stack_size(&op_stack) != 0) {
        struct Token* temp = stack_pop(&op_stack);
        if (temp->type == BracketOn) {
            free_stack(output_stack);
            free(op_stack._data);
            free_tokens(tokens);
            *error_code = SyntaxError;
            return NULL;
        }
        stack_push(output_stack, temp);
    }
    
    free(tokens[i]);
    free(op_stack._data);
    return output_stack;
}

double calculate_from_RPN(struct Stack* RPN, const double previous_value, enum ErrorCode* error_code) {
    struct Stack op_stack;
    stack_init(&op_stack);

    for (size_t i = 0; i < stack_size(RPN); i++) {
        struct Token* cur = RPN->_data[i];
        if (cur->type == Num) {
            stack_push(&op_stack, cur);
        } else if (cur->type == Prev) {
            cur->type = Num;
            cur->operand = previous_value;
            stack_push(&op_stack, cur);
        } else if (cur->type == Op) {
            if (get_operation_arity(cur->operation) == Binary) {
                // Order is important
                struct Token* b_token = stack_pop(&op_stack);
                struct Token* a_token = stack_pop(&op_stack);
                if (a_token == NULL || b_token == NULL) {
                    for (size_t j = i; j < stack_size(RPN); j++) {
                        free(RPN->_data[j]);
                    }
                    free(RPN->_data);
                    free(RPN);
                    for (size_t j = 0; j < stack_size(&op_stack); j++) {
                        free(op_stack._data[j]);
                    }
                    free(op_stack._data);
                    free(a_token);
                    free(b_token);

                    *error_code = SyntaxError;
                    
                    return INFINITY;
                }
                double a = a_token->operand;
                double b = b_token->operand;

                struct Token* temp = malloc(sizeof(struct Token));
                enum ErrorCode t_error_code = Success;
                temp->operand = run_binary_operation(cur->operation, a, b, &t_error_code);
                if (t_error_code != Success) {
                    free(temp);
                    for (size_t j = i; j < stack_size(RPN); j++) {
                        free(RPN->_data[j]);
                    }
                    free(RPN->_data);
                    free(RPN);
                    for (size_t j = 0; j < stack_size(&op_stack); j++) {
                        free(op_stack._data[j]);
                    }
                    free(op_stack._data);
                    free(a_token);
                    free(b_token);

                    *error_code = t_error_code;
                    return INFINITY;
                }

                free(a_token);
                free(b_token);

                temp->type = Num;
                temp->operation = NOT_FOUND;

                stack_push(&op_stack, temp);
            }
            if (get_operation_arity(cur->operation) == Unary) {
                struct Token* a_token = stack_pop(&op_stack);

                if (a_token == NULL) {
                    
                    for (size_t j = i; j < stack_size(RPN); j++) {
                        free(RPN->_data[j]);
                    }
                    free(RPN->_data);
                    free(RPN);
                    for (size_t j = 0; j < stack_size(&op_stack); j++) {
                        free(op_stack._data[j]);
                    }
                    free(op_stack._data);
                    *error_code = SyntaxError;
                    return INFINITY;
                }

                double a = a_token->operand;

                free(a_token);

                struct Token* temp = malloc(sizeof(struct Token));
                
                enum ErrorCode t_error_code = Success;
                temp->operand = run_unary_operation(cur->operation, a, &t_error_code);
                if (t_error_code != Success) {
                    free(temp);
                    for (size_t j = i; j < stack_size(RPN); j++) {
                        free(RPN->_data[j]);
                    }
                    free(RPN->_data);
                    free(RPN);
                    for (size_t j = 0; j < stack_size(&op_stack); j++) {
                        free(op_stack._data[j]);
                    }
                    free(op_stack._data);
                    free(a_token);
                    *error_code = t_error_code;
                    return INFINITY;
                }
                temp->type = Num;
                temp->operation = NOT_FOUND;

                stack_push(&op_stack, temp);
            }
            free(cur);
        }
    }
    if (stack_size(&op_stack) != 1) {
        free_tokens(RPN->_data);
        free_stack(RPN);
        *error_code = SyntaxError;
        return INFINITY;
    }
    double ans = stack_peek(&op_stack)->operand;
    free(stack_peek(&op_stack));
    free(op_stack._data);
    free_stack(RPN);
    return ans;
}

double calculate_expression(const char* expr, const double previous_value, enum ErrorCode* error_code) {
    enum ErrorCode t_error_code = Success;
    struct Token** tokens = parse_string(expr, &t_error_code);
    if (t_error_code != Success) {
        free(tokens);
        *error_code = t_error_code;
        return INFINITY;
    }
    struct Stack* RPN = transform_to_RPN(tokens, &t_error_code);
    if (t_error_code != Success) {
        *error_code = t_error_code;
        return INFINITY;
    }
    double ans = calculate_from_RPN(RPN, previous_value, &t_error_code);
    if (t_error_code != Success) {
        free(tokens);
        *error_code = t_error_code;
        return INFINITY;
    }

    free(tokens);

    return ans;
}
