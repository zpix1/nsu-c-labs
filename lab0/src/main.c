#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USE_UPPER_CASE 1
#define ALLOW_MIXED_CASE 1

#define MAX_INPUT_LENGTH 256
#define DELIMITER '.'
#define BAD_INPUT "bad input"

#if USE_UPPER_CASE == 1
    #define A_LETTER 'A'
    #define F_LETTER 'F'
#else
    #define A_LETTER 'a'
    #define F_LETTER 'f'
#endif

// Utility functions

// int max(int a, int b);
void error_exit(char* error_message);

// Reverse given string of size n
void reverse(char* s, size_t n);

// Try to allocate size bytes of heap memory
// Exit if a problem occured
void* errorcheck_malloc(size_t size);

// Try to scanf
// Exit if a problem occured
void errorcheck_scanf(const char *fmt, ...);

// Main functions

// Convert given char to int in given base
// Exit printing bad input if char is not in this base system range
int char2num(char c, int base);

// Convert given int to char in given base
// Exit printing bad input if n is not in this base system range
char num2char(int n, int base);

// Convert fractional portion of given double to string in given base
void from_10_to_base_float(char* s, double f, int base);

// Convert given int (long long) to string in given base
void from_10_to_base(char* s, long long n, int base);

// Convert given string to long long using given base
long long from_base_to_10(char* s, int base, char* delimiter_pos);

// Convert given string from base1 to base2
// Uses malloc; FREE IT after use;
char* convert_from_base1_to_base2(char *base1_str, int base1, int base2);

int main() {
    int base1, base2;
    errorcheck_scanf("%d %d", &base1, &base2);

    // I think program should assert in these cases, but tests think otherwise
    if ((base1 < 2 || base1 > 16) || (base2 < 2 || base2 > 16)) {
        error_exit(BAD_INPUT);
    }

    // Asserts in case 'if' line upper is commented out
    assert(base1 >= 2);
    assert(base1 <= 16);
    assert(base2 >= 2);
    assert(base2 <= 16);

    char base1_str[MAX_INPUT_LENGTH];

    // I am still not sure about safety of this operation
    errorcheck_scanf("%60s", base1_str);

    char* base2_str = convert_from_base1_to_base2(base1_str, base1, base2);

    printf("%s\n", base2_str);

    free(base2_str);

    return 0;
}

// Using long long is ok, becouse 
// FFFFFFFFFFFFF in 10ns is
// 4 503 599 627 370 495
// But long long max value is
// 9 223 372 036 854 775 807
// Double mantissa is not enough but I dont care
// It is 1.0000000000000002
// Whilst 0.FFFFFFFFFFFF is 
// 0.999999999999996447286321199499070644378662109375
// It still works


// int max(int a, int b) {
//     return a > b ? a : b;
// }

void error_exit(char* error_message) {
    printf("%s\n", error_message);
    exit(0);
}

void* errorcheck_malloc(size_t size) {
    void* result = malloc(size);

    if (result == NULL) {
        error_exit("Can not allocate memory");
    }

    return result;
}

void errorcheck_scanf(const char *fmt, ...) {
    int rc;

    va_list args;
    va_start(args, fmt);
    rc = vscanf(fmt, args);
    va_end(args);

    if (!rc) {
        error_exit("Scanf error");
    }
}

void reverse(char* s, size_t n) {
    for (size_t i = 0; i < n / 2; i++) {
        char t = s[i];
        s[i] = s[n - i - 1];
        s[n - i - 1] = t;
    }
}


int char2num(char c, int base) {
    if (ALLOW_MIXED_CASE && USE_UPPER_CASE)
        c = toupper(c);
    if (ALLOW_MIXED_CASE && !USE_UPPER_CASE)
        c = tolower(c);

    if (base <= 10) {
        if (!(('0' <= c) && (c <= '0' + base - 1)))
            error_exit(BAD_INPUT);
    } else {
        if (!((('0' <= c) && (c <= '9')) || ((A_LETTER <= c) && (c <= A_LETTER + base - 10))))
            error_exit(BAD_INPUT);
    } 
    if (('0' <= c) && (c <= '9')) {
        return c - '0';
    } else if ((A_LETTER <= c) && (c <= F_LETTER)) {
        return c - A_LETTER + 10;
    }
    return -1;
}

char num2char(int n, int base) {
    if (!((0 <= n) && (n < base))) {
        error_exit(BAD_INPUT);
    } else if ((0 <= n) && (n <= 9)) {
        return n + '0';
    } else if ((10 <= n) && (n <= 15)) {
        return n + A_LETTER - 10;
    }
    return -1;
}

void from_10_to_base_float(char* s, double f, int base) {
    double float_part = f;
    size_t index = 0;
    while (index < 12) {
        float_part *= base;
        int current_digit = floor(float_part);
        float_part -= current_digit;
        s[index] = num2char(current_digit, base);
        index++;
    }
    s[index] = '\0';
}

void from_10_to_base(char* s, long long n, int base) {
    long long integer_part = n;
    size_t index = 0;
    while (integer_part >= base) {
        int current_digit = integer_part % base;
        integer_part /= base;
        s[index] = num2char(current_digit, base);
        index++;
    }
    s[index] = num2char(integer_part, base);
    s[index + 1] = '\0';
    reverse(s, index + 1);
}

long long from_base_to_10(char* s, int base, char* delimiter_pos) {
    long long integer_part = 0;
    for (char* i = delimiter_pos; i >= s; i--) {
        integer_part += (long long)pow(base, delimiter_pos - i) * (char2num(*i, base));
    }
    return integer_part;
}

char* convert_from_base1_to_base2(char *base1_str, int base1, int base2) {
    size_t base1_str_length = strlen(base1_str);

    // Find '.' in input
    char* delimiter_pos = strchr(base1_str, DELIMITER);
    char* base2_str = (char *) errorcheck_malloc(MAX_INPUT_LENGTH * sizeof(char));

    // Integer scenario (delimiter does not exist)
    if (delimiter_pos == NULL) { 
        delimiter_pos = base1_str + base1_str_length - 1;

        long long integer_part = from_base_to_10(base1_str, base1, delimiter_pos);

        from_10_to_base(base2_str, integer_part, base2);

    // Float scenario
    } else {
        if (base1_str_length < 3) {
            error_exit(BAD_INPUT);
        }

        long long integer_part = from_base_to_10(base1_str, base1, delimiter_pos - 1);
        
        // Not normalized float part
        long long float_nn_part = from_base_to_10(delimiter_pos + 1, base1, base1_str + base1_str_length - 1);
        
        from_10_to_base(base2_str, integer_part, base2);
        size_t integer_part_len = strlen(base2_str);

        if (float_nn_part != 0) {
            double float_part = float_nn_part / pow(base1, ((int)base1_str_length - (int)(delimiter_pos - base1_str)) - 1);
            base2_str[integer_part_len] = DELIMITER;
            from_10_to_base_float(base2_str + integer_part_len + 1, float_part, base2);
        }
    }

    return base2_str;
}
