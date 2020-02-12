#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#include "calculator.h"
#include "operations.h"
#include "tokenizer.h"
#include "stack.h"

#define MAXS 100000

// To remove warning
// implicit declaration of function 'fileno'; did you mean 'mblen'? [-Wimplicit-function-declaration]
int fileno(FILE *stream);

int main() {
    char input_str[MAXS];
    enum ErrorCode tec = Success;
    double pv = 0;
    assert((int)calculate_expression("1+1", 0, &tec) == 2);
    assert(tec == Success);
    assert((int)calculate_expression("2^3^2", 0, &tec) == 512);
    assert(tec == Success);
    assert((int)calculate_expression("1+$", 5.0, &tec) == 6);
    assert(tec == Success);
    assert((int)calculate_expression("2/3+5", 0, &tec) == 5);
    assert(tec == Success);
    // assert dont use variables (at least gcc thinks so)
    if (tec != Success || pv != 0.0) {
        fprintf(stderr, "nothing happened but i used these vars");
    }
    
    // Interactive mode
    if (isatty(fileno(stdin))) {
        double previous_value = 0.0;
        printf("simple calc\nTo exit type `exit`\n");
        while (1) {
            printf("> ");
            char* check = fgets(input_str, sizeof(input_str), stdin);
            if (check == NULL || strlen(check) == 1) {
                printf("syntax error\n");
                return 0;
            }

            if (strcmp(input_str, "exit\n") == 0) {
                return 0;
            }
            enum ErrorCode t_error_code = Success;
            double t_previous_value = calculate_expression(input_str, previous_value, &t_error_code);
            if (t_error_code == SyntaxError) {
                printf("syntax error\n");
            } else if (t_error_code == ZeroDivisionError) {
                printf("division by zero\n");
            } else if (t_error_code == Success) {
                previous_value = t_previous_value;
                fprintf(stdout, "%d\n", ((int)previous_value));
            }
        }
    // File/Pipe mode
    } else {
        char* check = fgets(input_str, sizeof(input_str), stdin);
        if (check == NULL || strlen(check) == 1) {
            printf("syntax error\n");
            return 0;
        }
        enum ErrorCode t_error_code = Success;
        int value = ((int)calculate_expression(input_str, 0.0, &t_error_code));
        
        if (t_error_code == SyntaxError) {
            printf("syntax error\n");
        } else if (t_error_code == ZeroDivisionError) {
            printf("division by zero\n");
        } else if (t_error_code == Success) {
            fprintf(stdout, "%d\n", value);
        }
    }
}
