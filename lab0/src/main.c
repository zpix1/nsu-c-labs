#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 64

static const char base16char[] = "0123456789abcdef";
static const char base16char_upper[] = "0123456789ABCDEF";

static unsigned to_val(const char* charset, const char* ch) {
    if (!ch) {
        return (unsigned) strlen(charset);
    }
    return (unsigned) (ch - charset);
}

static unsigned val(char ch) {
    const char* find = strchr(base16char, ch);
    if (find) {
        return to_val(base16char, find);
    }
    find = strchr(base16char_upper, ch);
    return to_val(base16char_upper, find);
}

static void bad_input(void) {
    puts("bad input");
    exit(EXIT_SUCCESS);
}

static bool valid_integer(const char *integer, unsigned base) {
    if (!*integer) {
        return false;
    }
    for (; *integer; ++integer) {
        if (val(*integer) >= base) {
            return false;
        }
    }
    return true;
}

static void split_fraction(char* integer, char** fraction) {
    if ((*fraction = strchr(integer, '.'))) {
        **fraction = '\0';
        ++*fraction;
    }
}

static bool input(unsigned* src_base, unsigned* dst_base, char* integer, char** fraction) {
    if (scanf("%u%u%13s", src_base, dst_base, integer) != 3) {
        return false;
    }
    if (*src_base < 2 || 16 < *src_base || *dst_base < 2 || 16 < *dst_base) {
        return false;
    }
    split_fraction(integer, fraction);
    if (!valid_integer(integer, *src_base)) {
        return false;
    }
    if (*fraction && !valid_integer(*fraction, *src_base)) {
        return false;
    }
    return true;
}

static char* reverse(char* str) {
    size_t len = strlen(str);
    for(size_t i = 0, k = len - 1 ; i < len / 2; ++i, --k) {
        char tmp = str[k];
        str[k] = str[i];
        str[i] = tmp;
    }
    return str;
}

static void print_integer(const char* integer, unsigned src_base, unsigned dst_base) {
    unsigned long long value = strtoull(integer, NULL, (int)src_base);
    char buffer[BUFFER_SIZE] = {0};
    for(char* it = buffer; value > 0; value /= dst_base, ++it) {
        *it = base16char[value % dst_base];
    }
    if (!*buffer) {
        *buffer = '0';
    }
    printf("%s", reverse(buffer));
}

static void print_fraction(const char* fraction, unsigned src_base, unsigned dst_base) {
    double value = 0.0;
    double multiplyer = 1.0;
    for (; *fraction; ++fraction) {
        multiplyer /= src_base;
        value += val(*fraction) * multiplyer;
    }

    char buffer[BUFFER_SIZE] = {0};
    for(char* it = buffer; value > 0 && it - buffer < 12; ++it) {
        value *= dst_base;
        int index = (int) value;
        value -= index;
        *it = base16char[index];
    }
    if (*buffer) {
        printf(".%s", buffer);
    }
}

int main(void) {
    unsigned src_base = 0, dst_base = 0;
    char integer[BUFFER_SIZE] = {0}, *fraction = NULL;

    if (!input(&src_base, &dst_base, integer, &fraction)) {
        bad_input();
    }

    print_integer(integer, src_base, dst_base);
    if (fraction) {
        print_fraction(fraction, src_base, dst_base);
    }
    putchar('\n');

    return EXIT_SUCCESS;
}
