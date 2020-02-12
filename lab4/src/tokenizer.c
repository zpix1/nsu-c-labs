#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tokenizer.h"

const char* next_token(const char* input, struct Token* out, enum ErrorCode* error_code) {
    if (input == NULL) {
        return NULL;
    }
    if (isspace(*input) && *input != '\n') {
        *error_code = SyntaxError;
        return NULL;
    }
    
    if (isdigit(*input)) {
        char* end;
        out->operand = strtol(input, &end, 10);
        out->type = Num;
        return end;
    } else if (*input == '(') {
        out->type = BracketOn;
        return input+1;
    } else if (*input == ')') {
        out->type = BracketOff;
        return input+1;
    } else if (*input == '$') {
        out->type = Prev;
        return input+1;
    } else if (*input == '\0' || *input == '\n') {
        out->type = END;
        return input + 1;
    } else {
        const char* end = input + 1;
        while ((*end != '\0' && *end != '\n' && !isdigit(*end)) && (*end != '(' && *end != ')' && *end != '$')) {
            end++;
        }
        size_t len = end - input;
        char op_str[MAX_OPERATION_LENGTH];
        memcpy(op_str, input, len);
        op_str[len] = '\0';
        out->type = Op;
        out->operation = find_operation(op_str);
        if (out->operation == NOT_FOUND) {
            *error_code = SyntaxError;
            return NULL;
        }
        return end;
    }
}

struct Token** parse_string(const char* str, enum ErrorCode* error_code) {
    if (str == NULL) {
        return NULL;
    }
    struct Token** ans = (struct Token**) malloc(MAX_TOKENS_COUNT * sizeof(struct Token*));
    size_t idx = 0;
    const char* current = str;
    struct Token* out;
    enum ErrorCode t_error_code = Success;
    do {
        out = (struct Token*)malloc(sizeof(struct Token));
        current = next_token(current, out, &t_error_code);
        if (t_error_code != Success) {
            *error_code = t_error_code;
            for (size_t j = 0; j < idx; j++) {
                free(ans[j]);
            }
            free(out);
            free(ans);
            return NULL;
        }
        ans[idx] = out;
        idx++;
    } while (out->type != END);


    return ans;
}
