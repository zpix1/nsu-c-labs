#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 40000
#define MAX_NEEDLE_LENGTH 100

#define MAXCHAR 256
// Enable logs
#define DEBUG 0

#define debug_print(fmt, ...) //do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#define ABS(x) ((unsigned char)(x))

// Create an array of 256 ints and fill it according to BMH algorithm
// FREE IT after use
int* preprocess_str(const char* str);

// Copy given amount of bytes using given offset
// Offset > 0 : to right
// Offset < 0 : to left
// offset should be greater or equal to amount
void copy_bytes(char* src, size_t amount, size_t offset);

// Find needle in stream
// Data structure:
//  haystack_data                    haystack_block
// [Old buffer x MAX_NEEDLE_LENGTH][Buffer x BUFFER_SIZE]
// Algo:
// 1. Read from stream to Buffer
// 2. Find needle in Buffer (starting from needle_length - 1)
// 3. Copy needle_length chars from end of Buffer to Old Buffer
// 4. Read from stream to Buffer
// 5. Find needle in Buffer (starting from overdone saved at previous operation)
// 6. Goto n. 3 until EOF
void find_str_in_stream(FILE* stream, const char* needle);

// Utility function
int _find_str_in_stream(const char* block, size_t block_length, const char* needle, size_t needle_length, const int* needle_offsets, int offset, size_t start_from);

int main() {
    char* needle = malloc(sizeof(char) * MAX_NEEDLE_LENGTH);
    
    needle = fgets(needle, sizeof(char) * MAX_NEEDLE_LENGTH, stdin);

    if (needle == NULL) {
        free(needle);
        return 1;
    }

    // Couse \n
    needle[strlen(needle) - 1] = '\0';

    debug_print("Data find_str_in_stream started\n", "");
    find_str_in_stream(stdin, needle);
    debug_print("Data find_str_in_stream finished\n", "");

    free(needle);

    return 0;
}

int* preprocess_str(const char* str) {
    if (str == NULL) {
        return NULL;
    }

    int* offsets = (int*) malloc(sizeof(int) * MAXCHAR);

    if (offsets == NULL) {
        fprintf(stderr, "Cannot allocate memory\n");
        return NULL;
    }

    size_t len = strlen(str);
    for (size_t i = 0; i < MAXCHAR; i++) {
        offsets[i] = len;
    }

    for (size_t i = 0; i < len - 1; i++) {
        offsets[ABS(str[i])] = len - i - 1;
    }

    return offsets;
}


void copy_bytes(char* src, size_t amount, size_t offset) {
    if (src == NULL || (amount >= offset)) {
        return;
    }

    for (size_t i = 0; i < amount; i++) {
        src[i + offset] = src[i];
    }
}

void find_str_in_stream(FILE* stream, const char* needle) {
    char* haystack_data = (char*)malloc(sizeof(char) * (BUFFER_SIZE + MAX_NEEDLE_LENGTH));

    if (haystack_data == NULL) {
        fprintf(stderr, "Cannot allocate memory\n");
        return;
    }

    char* haystack_block = haystack_data + MAX_NEEDLE_LENGTH;

    size_t needle_length = strlen(needle);    
    int offset = 0;
    int* needle_offsets = preprocess_str(needle);

    size_t block_length = fread(haystack_block, sizeof(char), BUFFER_SIZE, stream);
    
    int overdone = _find_str_in_stream(haystack_block, block_length, needle, needle_length, needle_offsets, offset, needle_length - 1);
    while (block_length == BUFFER_SIZE) {
        offset += block_length;
        copy_bytes(haystack_block + block_length - needle_length, needle_length, -block_length);
        
        block_length = fread(haystack_block, sizeof(char), BUFFER_SIZE, stream);
        overdone = _find_str_in_stream(haystack_block, block_length, needle, needle_length, needle_offsets, offset, overdone);
        
        debug_print("BLOCK %d\n", block_length);
    }
    printf("\n");

    free(haystack_data);
    free(needle_offsets);
}

int _find_str_in_stream(const char* block, size_t block_length, const char* needle, size_t needle_length, const int* needle_offsets, int offset, size_t start_from) {
    debug_print("start_from=%d\n", start_from);

    size_t current_i = start_from;
    debug_print("current_i=%d, block_length=%d\n", current_i, block_length);

    while (current_i < block_length) {
        int flag = 1;
        for (size_t needle_i = 0; needle_i < needle_length; needle_i++) {
            debug_print("%c vs %c\n", block[current_i - needle_i], needle[needle_length - needle_i - 1]);

            printf("%d ", (int) (current_i - needle_i + 1 + offset));
            if (block[current_i - needle_i] != needle[needle_length - needle_i - 1]) {
                current_i += needle_offsets[ABS(block[current_i])];
                flag = 0;
                break;
            }
        }
        if (flag) {
            current_i += needle_length;
        }
    }

    return current_i - block_length;
}
