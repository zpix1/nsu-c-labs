#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_EX 3
#define MAX_POWER 1000

// MAX_NEEDLE_LENGTH < BUFFER_SIZE

#define BUFFER_SIZE 40000
#define MAX_NEEDLE_LENGTH 100

// Enable logs
#define DEBUG 0

#define debug_print(fmt, ...) //do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#define ABS(x) ((unsigned char)(x))

int POWERS[MAX_POWER];

// Copy given amount of bytes using given offset
// Offset > 0 : to right
// Offset < 0 : to left
// offset should be greater or equal to amount
void copy_bytes(char* src, size_t amount, size_t offset);

// Set POWERS array to format [i : <HASH_EX>**i]
void precalculate_powers();

// Generate hash for str
unsigned int generate_hash(const char* str, const size_t len);

// Find needle in stream
// Data structure:
//  haystack_data                    haystack_block
// [Old buffer x MAX_NEEDLE_LENGTH][Buffer x BUFFER_SIZE]
// Algo:
// 1. Read from stream to Buffer
// 2. Calculate hashes
// 3. Find needle in Buffer (starting from needle_length)
// 4. Copy needle_length chars from end of Buffer to Old Buffer
// 5. Read from stream to Buffer
// 6. Find needle in Buffer (starting from 0)
// 7. Goto n. 4 until EOF
void find_str_in_stream(FILE* stream, const char* needle);

// Utility function
void _find_str_in_stream(const char* haystack_block, size_t block_size, const char* needle, size_t needle_length, unsigned int needle_hash, unsigned int* current_hash, int offset);

int main() {
    char* needle = malloc(sizeof(char) * MAX_NEEDLE_LENGTH);

    needle = fgets(needle, sizeof(char) * MAX_NEEDLE_LENGTH, stdin);

    if (needle == NULL) {
        free(needle);
        return 1;
    }

    // Couse \n
    needle[strlen(needle) - 1] = '\0'; 

    precalculate_powers();

    find_str_in_stream(stdin, needle);

    free(needle);

    return 0;
}

void copy_bytes(char* src, size_t amount, size_t offset) {
    if (src == NULL || (amount >= offset)) {
        return;
    }

    for (size_t i = 0; i < amount; i++) {
        src[i + offset] = src[i];
    }
}

void precalculate_powers() {
    int current = 1;
    for (int i = 0; i < MAX_POWER; i++) {
        POWERS[i] = current;
        current *= HASH_EX;
    }
}

unsigned int generate_hash(const char* str, const size_t len) {
    unsigned int ans = 0;

    for (size_t i = 0; i < len; i++) {
        ans += POWERS[i] * (ABS(str[i]) % HASH_EX);
    }

    return ans;
}

void find_str_in_stream(FILE* stream, const char* needle) {
    if (stream == NULL || needle == NULL) {
        return;
    }

    char* haystack_data = malloc(sizeof(char) * (BUFFER_SIZE + MAX_NEEDLE_LENGTH));
    char* haystack_block = haystack_data + MAX_NEEDLE_LENGTH;

    size_t needle_length = strlen(needle);
    int offset = 0;
    unsigned int needle_hash = generate_hash(needle, needle_length);
    debug_print("Started: needle_length = %lu\nneedle_hash = %u\n",  needle_length, needle_hash);

    printf("%u ", needle_hash);


    size_t block_length = fread(haystack_block, sizeof(char), BUFFER_SIZE, stream);
    debug_print("First block: %d\n", block_length);

    unsigned int current_hash = generate_hash(haystack_block, needle_length);

    _find_str_in_stream(haystack_block + needle_length, block_length - needle_length, needle, needle_length, needle_hash, &current_hash, needle_length);

    while (block_length == BUFFER_SIZE) {
        offset += block_length;

        copy_bytes(haystack_block + block_length - needle_length, needle_length, -block_length);
        
        debug_print("Current block rest\n", "");
        for (int i = 0; i < (int)needle_length; i++) {
            debug_print("%c", haystack_block[i - needle_length]);
        }
        debug_print("\n","");

        block_length = fread(haystack_block, sizeof(char), BUFFER_SIZE, stream);
        debug_print("block: %d\n", block_length);


        _find_str_in_stream(haystack_block, block_length, needle, needle_length, needle_hash, &current_hash, offset);


        if (block_length != BUFFER_SIZE) {
            break;
        }
    }

    printf("\n");

    free(haystack_data);
}

void _find_str_in_stream(const char* haystack_block, size_t block_size, const char* needle, size_t needle_length, unsigned int needle_hash, unsigned int* current_hash_pointer, int offset) {
    for (int i = 0; i < (int)block_size + 1; i++) {
        debug_print("current hash = %u\n", *current_hash_pointer);
        if (needle_hash == *current_hash_pointer) {
            debug_print("FOUND i=%u j=%u\n", i, i - needle_length);
            for (int j = i - needle_length; j < i; j++) {
                debug_print("comp a=%d(%d) b=%d(%d)\n", haystack_block[j], j, needle[j - i + needle_length], j - i + needle_length);

                printf("%d ", offset + j + 1);

                if (haystack_block[j] != needle[j - i + needle_length]) {
                    break;
                }
            }
        }
        if (i != (int)block_size) {
            *current_hash_pointer = (*current_hash_pointer) / HASH_EX + (ABS(haystack_block[i]) % HASH_EX) * POWERS[needle_length - 1];
        }
    }
}
