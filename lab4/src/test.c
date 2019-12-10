#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "calculator.h"
#include "operations.h"
#include "tokenizer.h"
#include "stack.h"

#define MAXS 100000

int main() {
    char input_str[MAXS];
    char* check = fgets(input_str, sizeof(input_str), stdin);
    if (check == NULL || strlen(check) == 1) {
        printf("syntax error\n");
        return 0;
    }
    input_str[strlen(input_str) - 1] = '\0';

    fprintf(stdout, "%d\n", ((int)calculate_expression(input_str)));
}
