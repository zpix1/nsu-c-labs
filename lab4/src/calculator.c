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

struct Stack* transform_to_RPN(struct Token** tokens) {
    struct Stack* output_stack = malloc(sizeof(struct Stack));
    stack_init(output_stack);

    // Maybe should be converted back to malloc style
    struct Stack op_stack;
    stack_init(&op_stack);
    int bad_bracket_pattern_flag = 0;
    size_t i;
    for (i = 0; tokens[i]->type != END; i++) {
        if (tokens[i]->type == Num || tokens[i]->type == END) {
            bad_bracket_pattern_flag = 0;
            stack_push(output_stack, tokens[i]);
        } else if (tokens[i]->type == Op) {
            if (stack_size(&op_stack) != 0) {
                while (stack_size(&op_stack) != 0 && stack_peek(&op_stack)->type == Op && \
                    get_operation_priority(stack_peek(&op_stack)->operation) >= get_operation_priority(tokens[i]->operation)) {
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
                fprintf(stdout, "syntax error\n");
                exit(0);
            }
            while (stack_peek(&op_stack)->type != BracketOn) {
                stack_push(output_stack, stack_pop(&op_stack));
                if (stack_size(&op_stack) == 0) {
                    free_tokens(tokens);
                    free_stack(output_stack);
                    free(op_stack._data);
                    fprintf(stdout, "syntax error\n");
                    exit(0);
                }
            }
            free(stack_pop(&op_stack));
            free(tokens[i]);
        }
    }

    // Free end token
    free(tokens[i]);

    while (stack_size(&op_stack) != 0) {
        struct Token* temp = stack_pop(&op_stack);
        if (temp->type == BracketOn) {
            free_stack(output_stack);
            free(op_stack._data);
            fprintf(stdout, "syntax error\n");
            exit(0);
        }
        stack_push(output_stack, temp);
    }
    free(op_stack._data);
    return output_stack;
}

double calculate_from_RPN(struct Stack* RPN) {
    struct Stack op_stack;
    stack_init(&op_stack);

    for (size_t i = 0; i < stack_size(RPN); i++) {
        struct Token* cur = RPN->_data[i];
        if (cur->type == Num) {
            stack_push(&op_stack, cur);
        } else if (cur->type == Op) {
            if (get_operation_arity(cur->operation) == Binary) {
                // Order is important
                struct Token* b_token = stack_pop(&op_stack);
                struct Token* a_token = stack_pop(&op_stack);
                if (a_token == NULL || b_token == NULL) {
                    printf("syntax error\n");
                    for (size_t j = i; j < stack_size(RPN); j++) {
                        free(RPN->_data[j]);
                    }
                    free(RPN->_data);
                    free(RPN);
                    for (size_t j = 0; j < stack_size(&op_stack); j++) {
                        free(op_stack._data[j]);
                    }
                    free(op_stack._data);
                    exit(0);
                }
                double a = a_token->operand;
                double b = b_token->operand;

                

                struct Token* temp = malloc(sizeof(struct Token));
                temp->operand = run_binary_operation(cur->operation, a, b);
                if (temp->operand == INFINITY) {
                    free(temp);

                    printf("division by zero\n");
                    for (size_t j = i; j < stack_size(RPN); j++) {
                        free(RPN->_data[j]);
                    }
                    free(RPN->_data);
                    free(RPN);
                    for (size_t j = 0; j < stack_size(&op_stack); j++) {
                        free(op_stack._data[j]);
                    }
                    free(op_stack._data);
                    exit(0);
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
                    printf("syntax error\n");
                    for (size_t j = i; j < stack_size(RPN); j++) {
                        free(RPN->_data[j]);
                    }
                    free(RPN->_data);
                    free(RPN);
                    for (size_t j = 0; j < stack_size(&op_stack); j++) {
                        free(op_stack._data[j]);
                    }
                    free(op_stack._data);
                    exit(0);
                }

                double a = a_token->operand;

                free(a_token);

                struct Token* temp = malloc(sizeof(struct Token));
                temp->operand = run_unary_operation(cur->operation, a);
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
        fprintf(stdout, "syntax error\n");
        exit(0);
    }
    double ans = stack_peek(&op_stack)->operand;
    free(stack_peek(&op_stack));
    free(op_stack._data);
    free_stack(RPN);
    return ans;
}

double calculate_expression(const char* expr) {
    struct Token** tokens = parse_string(expr);
    struct Stack* RPN = transform_to_RPN(tokens);
    double ans = calculate_from_RPN(RPN);

    free(tokens);

    return ans;
}
